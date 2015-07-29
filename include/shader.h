#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <string>
#include <glm/glm.hpp>
#include <QHash>
#include <QStringList>
#include "texture.h"

class Shader
{
public:
    Shader(QString value, QString filename);
    ~Shader();

    void initialize();
    void load(QStringList attributes, QStringList uniforms);
    //void load();
    void bind();

    GLint getVariable(std::string strVariable);
    GLuint getProgramId() {return _programId;}
    uint getAttribute(QString name);
    uint getUniform(QString name);
    void transmitUniform(QString name, const Texture* tex);
    void transmitUniform(QString name, const TextureCubeMap* tex);
    void transmitUniform(QString name, int i);
    void transmitUniform(QString name, float f);
    void transmitUniform(QString name, float f1, float f2);
    void transmitUniform(QString name, float f1, float f2, float f3);
    void transmitUniform(QString name, const glm::vec3 &vec3);
    void transmitUniform(QString name, const glm::mat3 &mat3);
    void transmitUniform(QString name, const glm::mat4 &mat4);
    void transmitUniform(QString name, bool b);

    void enable();
    void disable();

private:
    QString _value;
    QString _vertexFilename;
    QString _fragmentFilename;

    GLuint _shaderId[2];
    GLint _shaderLocation[2];
    GLuint _programId;
    GLuint _matrixId;

    QHash<QString, uint> _attributes;
    QHash<QString, uint> _uniforms;

    char* loadShaderFile(QString path);
};

class Shaders
{
public:
    static Shader* getShader(QString value);
    static void addShader(QString value, QStringList attributes, QStringList uniforms);

private:
    static QHash<QString, Shader*> _shaders;
};

#endif // SHADER_H
