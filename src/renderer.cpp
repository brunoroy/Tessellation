#include "renderer.h"

#include <GL/gl.h>
#include <iostream>
#include <random>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/ext.hpp"

#include "shader.h"

namespace Tessellation
{

    Renderer::Renderer():
        _initialized(false),
        _doTessellation(false)
    {
        loadShaders();
    }

    Renderer::~Renderer()
    {
    }

    void Renderer::initialize(Scene *scene)
    {
        _scene = scene;
        _camera = scene->getCamera();
        _width = _scene->getWidth();
        _height = _scene->getHeight();

        _initialized = true;
    }

    void Renderer::resize(int width, int height)
    {
        _width = width;
        _height = height;
    }

    void Renderer::loadShaders()
    {
        Shaders::addShader("render", QStringList() << "position" << "uv" << "normal" << "delta",
                           QStringList() << "mvp" << "doTessellation" << "doDisplacement" << "color", false);
        Shaders::addShader("render", QStringList() << "position" << "uv" << "normal" << "delta",
                           QStringList() << "mvp" << "doTessellation" << "doDisplacement" << "color" << "innerTL" << "outerTL",
                           true);
    }

    void Renderer::render(const int currentFrame, const bool animation)
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClearDepth(1.0f);

        _scene->draw(currentFrame, animation);
    }

    void Renderer::getModelViewProjectionMatrix(GLfloat matrix[16])
    {
        _scene->getCamera()->getModelViewProjectionMatrix(matrix);
    }

}
