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

    void setTextures(Texture *diffuse, Texture *alpha)
    {
        _diffuse = diffuse;
        _alpha = alpha;
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
        _shader->transmitUniform("textureAlpha", _alpha);
    }

    void configTexture()
    {
        _diffuse->bind();
        _alpha->bind();
    }

private:
    glm::vec4 _color;
    Texture *_diffuse;
    Texture *_alpha;
};

//Terrain
class MaterialTerrain : public Material
{
public:
    MaterialTerrain(Texture *textureGrass):
        Material("terrain"), _textureGrass(textureGrass)
    {
        _tileSize = static_cast<float>(_textureGrass->getWidth());
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

        _shader->transmitUniform("tileSize", _tileSize);
        _shader->transmitUniform("textureGrass", _textureGrass);
    }

    void configTexture()
    {
        _textureGrass->bind();
    }


private:
    float _tileSize;
    Texture *_textureGrass;
};

//Sky
class MaterialSky : public Material
{
public:
    MaterialSky(TextureCubeMap *cubeMap):
        Material("sky"), _cubeMap(cubeMap)
    {
        _cubeMap->initialize();
    }

    void bind()
    {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glDisable(GL_BLEND);

        configShader();
        configTexture();
    }

    void configShader()
    {
        _shader->bind();
        _shader->transmitUniform("textureCubeMap", _cubeMap);
    }

    void configTexture()
    {
        _cubeMap->bind();
    }

private:
    TextureCubeMap *_cubeMap;
};

#endif // MATERIAL_H
