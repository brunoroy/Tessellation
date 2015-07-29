#ifndef MATERIAL_H
#define MATERIAL_H

#include "shader.h"
#include <memory>

class Material
{
public:
    Material(QString value)
    {
        _shader.reset(Shaders::getShader(value));
    }

    ~Material() {}
    void initialize();

    Shader* getShader() {return _shader.get();}
    virtual void bind() {}

protected:
    std::shared_ptr<Shader> _shader;

private:
    virtual void configShader() {}
    virtual void configTexture() {}
};

//Default mesh material
class MaterialDefault : public Material
{
public:
    MaterialDefault(glm::vec4 color):
        Material("render"), _color(color) {}

    void setTexture(Texture *diffuse)
    {
        _diffuse = diffuse;
    }

    void bind()
    {
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glDisable(GL_BLEND);

        configShader();
        configTexture();
    }

    void configShader()
    {
        _shader->bind();
        _shader->transmitUniform("textureDiffuse", _diffuse);
    }

    void configTexture()
    {
        _diffuse->bind();
    }

private:
    glm::vec4 _color;
    Texture *_diffuse;
};

#endif // MATERIAL_H
