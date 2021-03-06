#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <QHash>
#include <QStringList>

#include <string>
#include <vector>

#include <glm/glm.hpp>

namespace Tessellation
{

    class Shader
    {
    public:
        Shader(QString value, QString filename, bool doTessellation = false);
        ~Shader();

        void initialize();
        void load(QStringList attributes, QStringList uniforms);
        void bind();

        GLint getVariable(std::string strVariable);
        GLuint getProgramId() {return _programId;}
        uint getAttribute(QString name);
        uint getUniform(QString name);
        void transmitUniform(QString name, int i);
        void transmitUniform(QString name, float f);
        void transmitUniform(QString name, float f1, float f2);
        void transmitUniform(QString name, float f1, float f2, float f3);
        void transmitUniform(QString name, const glm::vec3 &vec3);
        void transmitUniform(QString name, const glm::vec4 &vec4);
        void transmitUniform(QString name, const glm::mat3 &mat3);
        void transmitUniform(QString name, const glm::mat4 &mat4);
        void transmitUniform(QString name, bool b);

        void enable();
        void disable();
        QString getValue() {return _value;}

        void addDisplacement(bool value) {_doDisplacement = value;}
        bool doTessellation() {return _doTessellation;}
        bool doDisplacement() {return _doDisplacement;}
        size_t getShaderCount() {return _shaderIds.size();}

    private:
        QString _value;
        QStringList _shaderFilenames;
        bool _doTessellation;
        bool _doDisplacement;

        std::vector<GLuint> _shaderIds;
        GLuint _programId;
        GLuint _matrixId;

        QHash<QString, uint> _attributes;
        QHash<QString, uint> _uniforms;

        char* loadShaderFile(QString path);
    };

    class Shaders
    {
    public:
        static void clear() {_shaders.clear();}
        static Shader* getShader(QString value);
        static void addShader(QString value, QStringList attributes, QStringList uniforms, bool doTessellation = false);
        static size_t getCount() {return _shaders.size();}

    private:
        static QHash<QString, Shader*> _shaders;
    };

}

#endif // SHADER_H
