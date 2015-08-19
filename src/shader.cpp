#include "shader.h"

#include <fstream>
#include <iostream>
#include <vector>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

Shader::Shader(QString value, QString filename, bool doTessellation)
{
    _value = value;
    _doTessellation = doTessellation;

    _shaderFilenames.append(QString(filename).append(".vs"));
    if (doTessellation)
    {
        _shaderFilenames.append(QString(filename).append(".cs"));
        _shaderFilenames.append(QString(filename).append(".es"));
        //_shaderFilenames.append(QString(filename).append(".gs"));
    }
    _shaderFilenames.append(QString(filename).append(".fs"));
}

Shader::~Shader()
{
}

char* Shader::loadShaderFile(QString path)
{
    FILE* fp;
    char* content = NULL;
    long length;

    fp = fopen(path.toLatin1(), "rb");
    if (fp)
    {
        fseek( fp, 0, SEEK_END );
        length = ftell( fp );
        fseek( fp, 0, SEEK_SET );
        content = new char [length+1];
        fread( content, sizeof( char ), length, fp );
        fclose( fp );
        content[length] = '\0';
    }

    return content;
}

void Shader::load(QStringList attributes, QStringList uniforms)
{
    _shaderIds.push_back(glCreateShader(GL_VERTEX_SHADER));
    if (_doTessellation)
    {
        _shaderIds.push_back(glCreateShader(GL_TESS_CONTROL_SHADER));
        _shaderIds.push_back(glCreateShader(GL_TESS_EVALUATION_SHADER));
        //_shaderIds.push_back(glCreateShader(GL_GEOMETRY_SHADER));
    }
    _shaderIds.push_back(glCreateShader(GL_FRAGMENT_SHADER));

    GLint result = GL_FALSE;
    int infoLogLength;

    _programId = glCreateProgram();
    for (size_t i = 0; i < _shaderIds.size(); i++)
    {
        const char* shaderSourcePointer = loadShaderFile(_shaderFilenames.at(i).toLatin1());
        glShaderSource(_shaderIds[i], 1, &shaderSourcePointer, NULL);
        glCompileShader(_shaderIds[i]);

        glGetShaderiv(_shaderIds[i], GL_COMPILE_STATUS, &result);
        glGetShaderiv(_shaderIds[i], GL_INFO_LOG_LENGTH, &infoLogLength);
        std::vector<char> shaderErrorMessage(infoLogLength);
        glGetShaderInfoLog(_shaderIds[i], infoLogLength, NULL, &shaderErrorMessage[0]);

        if (shaderErrorMessage[0] != NULL)
        {
            std::vector<char> shaderErrorMessage(infoLogLength+1);
            glGetShaderInfoLog(_shaderIds[i], infoLogLength, NULL, &shaderErrorMessage[0]);
            std::cout << &shaderErrorMessage[0] << std::endl;
        }
        glAttachShader(_programId, _shaderIds[i]);
    }
    glLinkProgram(_programId);

    glGetProgramiv(_programId, GL_LINK_STATUS, &result);
    glGetProgramiv(_programId, GL_INFO_LOG_LENGTH, &infoLogLength);
    std::vector<char> programErrorMessage(std::max(infoLogLength, int(1)));
    glGetProgramInfoLog(_programId, infoLogLength, NULL, &programErrorMessage[0]);

    if (programErrorMessage[0] != NULL)
    {
        std::vector<char> programErrorMessage(infoLogLength+1);
        glGetShaderInfoLog(_programId, infoLogLength, NULL, &programErrorMessage[0]);
        std::cout << &programErrorMessage[0] << std::endl;
    }

    _matrixId = glGetUniformLocation(_programId, "mvp");

    for (size_t i = 0; i < _shaderIds.size(); i++)
        glDeleteShader(_shaderIds[i]);

    bind();
    foreach (QString attribute, attributes)
        _attributes.insert(attribute, glGetAttribLocation(_programId, attribute.toLatin1()));

    foreach (QString uniform, uniforms)
        _uniforms.insert(uniform, glGetUniformLocation(_programId, uniform.toLatin1()));
}

void Shader::enable()
{
    std::clog << __FUNCTION__ << std::endl;
    glUseProgram(_programId);
}

void Shader::disable()
{
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
}

GLint Shader::getVariable(std::string strVariable)
{
    if(!_programId)
        return -1;

    return glGetUniformLocation(_programId, strVariable.c_str());
}

void Shader::bind()
{
    glUseProgram(_programId);
}

uint Shader::getAttribute(QString name)
{
    return _attributes.value(name);
}

uint Shader::getUniform(QString name)
{
    return _uniforms.value(name);
}

void Shader::transmitUniform(QString name, const Texture *tex)
{
    glUniform1i(_uniforms.value(name, -1), tex->_index);
}

void Shader::transmitUniform(QString name, const TextureCubeMap *tex)
{
    glUniform1i(_uniforms.value(name, -1), tex->_index);
}

void Shader::transmitUniform(QString name, int i)
{
    glUniform1i(_uniforms.value(name, -1), i);
}

void Shader::transmitUniform(QString name, float f)
{
    glUniform1f(_uniforms.value(name, -1), f);
}

void Shader::transmitUniform(QString name, float f1, float f2)
{
    glUniform2f(_uniforms.value(name, -1), f1, f2);
}

void Shader::transmitUniform(QString name, float f1, float f2, float f3)
{
    glUniform3f(_uniforms.value(name, -1), f1, f2, f3);
}

void Shader::transmitUniform(QString name, const glm::vec3 &vec3)
{
    glUniform3f(_uniforms.value(name, -1), vec3.x, vec3.y, vec3.z);
}

void Shader::transmitUniform(QString name, const glm::mat3 &mat3)
{
    glUniformMatrix3fv(_uniforms.value(name, -1), 1, GL_TRUE, &mat3[0][0]);
}

void Shader::transmitUniform(QString name, const glm::mat4 &mat4)
{
    glUniformMatrix4fv(_uniforms.value(name, -1), 1, GL_FALSE, &mat4[0][0]);
}

void Shader::transmitUniform(QString name, bool b)
{
    glUniform1i(_uniforms.value(name, -1), b?1:0);
}

//Shaders database
QHash<QString, Shader*> Shaders::_shaders;

Shader* Shaders::getShader(QString value)
{
    if (_shaders.contains(value))
        return _shaders.value(value);
}
void Shaders::addShader(QString value, QStringList attributes, QStringList uniforms, bool doTessellation)
{
    QString path = QString("shaders/").append(value);
    Shader *shader = new Shader(value, path, doTessellation);
    shader->load(attributes, uniforms);
    _shaders.insert(value, shader);
}
