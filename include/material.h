#ifndef MATERIAL_H
#define MATERIAL_H

#include "shader.h"
#include <memory>
//#include <mutex>
#include <iostream>

class Material
{
public:
    Material(QString value) {_shader.reset(Shaders::getShader(value));}
    ~Material() {}

    void initialize();
    void setShader(QString value)
    {
        std::clog << "shaders: " << Shaders::getCount() << std::endl;
        std::clog << "shader: " << value.toStdString().c_str() << std::endl;
        Shader *shader = Shaders::getShader(value);
        if (shader)
            std::clog << "shader's found.\n";
        else
            std::clog << "shader's not found.\n";
        //_mutex.lock();
        _shader.reset(Shaders::getShader(value));
        //_mutex.unlock();
    }
    Shader* getShader() {return _shader.get();}
    virtual void bind() {}

protected:
    std::shared_ptr<Shader> _shader;
    //mutable std::mutex _mutex;

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
