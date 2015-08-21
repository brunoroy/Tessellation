#ifndef MATERIAL_H
#define MATERIAL_H

#include "shader.h"
#include <iostream>
#include <cassert>

namespace Tessellation
{

    class Material
    {
    public:
        Material(QString value) {_shader = Shaders::getShader(value);}
        ~Material() {}

        void initialize();
        void setShader(QString value) {_shader = Shaders::getShader(value);}
        bool equals(QString value) {return _shader->getValue() == value;}
        Shader* getShader() {return _shader;}
        virtual void bind() {}

    protected:
        Shader* _shader;

    private:
        virtual void configShader() {}
    };

    //Default mesh material
    class MaterialDefault : public Material
    {
    public:
        MaterialDefault(glm::vec4 color):
            Material("render"), _color(color) {}

        void bind()
        {
            glEnable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
            glDisable(GL_BLEND);

            configShader();
        }

        void configShader()
        {
            _shader->bind();
        }

        glm::vec4 getColor() {return _color;}

    private:
        glm::vec4 _color;
    };

}

#endif // MATERIAL_H
