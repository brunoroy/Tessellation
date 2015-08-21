#ifndef MATERIAL_H
#define MATERIAL_H

#include "shader.h"
#include <iostream>
#include <cassert>

class Material
{
public:
    Material(QString value) {_shader = Shaders::getShader(value);}//_shader.reset(Shaders::getShader(value));}
    ~Material() {}

    void initialize();
    void setShader(QString value)
    {
        //std::clog << "changing shader from " << _shader->getValue().toStdString().c_str() << " to " << value.toStdString().c_str() << std::endl;
        _shader = Shaders::getShader(value);
    }
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

private:
    glm::vec4 _color;
};

#endif // MATERIAL_H
