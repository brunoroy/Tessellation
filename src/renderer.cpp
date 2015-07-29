#include "renderer.h"

#include <GL/gl.h>
#include <iostream>
#include <random>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/ext.hpp"

#include "shader.h"

Renderer::Renderer():
    _initialized(false),
    _useBloom(true),
    _useLightShafts(true)
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

    _inputFBO = new FrameBuffer(QStringList()<< "textureBuffer" << "textureThreshold", _width, _height, true);
    _inputFBO->initialize(0);

    _width = _inputFBO->getWidth();
    _height = _inputFBO->getHeight();

    _readBuffer = new FrameBuffer(QStringList()<< "textureBuffer", _width, _height, false);
    _readBuffer->initialize(0);
    _writeBuffer = new FrameBuffer(QStringList()<< "textureBuffer", _width, _height, false);
    _writeBuffer->initialize(0);
    _readHalfBuffer = new FrameBuffer(QStringList()<< "textureBuffer", _width/2, _height/2, false);
    _readHalfBuffer->initialize(1);
    _writeHalfBuffer = new FrameBuffer(QStringList()<< "textureBuffer", _width/2, _height/2, false);
    _writeHalfBuffer->initialize(1);

    _screenSpaceImage.reset(new ScreenSpaceImage());
    _screenSpaceImage->initialize();

    _initialized = true;
}

void Renderer::resize(int width, int height)
{
    _width = width;
    _height = height;

    _inputFBO->resize(width, height);
    _readBuffer->resize(width, height);
    _writeBuffer->resize(width, height);
    _readHalfBuffer->resize(width/2.0f, height/2.0f);
    _writeHalfBuffer->resize(width/2.0f, height/2.0f);
}

void Renderer::loadShaders()
{
    Shaders::addShader("render", QStringList() << "position" << "uv" << "normal", QStringList() << "mvp" << "textureDiffuse" << "textureAlpha");
    Shaders::addShader("terrain", QStringList() << "position" << "normal", QStringList() << "mvp" << "normalMatrix" << "viewMatrix" << "lightDirection" << "tileSize" << "textureGrass");
    Shaders::addShader("sky", QStringList()<< "position", QStringList() << "mvp" << "textureCubeMap");

    //differed shaders
    Shaders::addShader("composition", QStringList() << "position", QStringList() << "textureBuffer");
    Shaders::addShader("linearBlurV", QStringList() << "position",QStringList() << "textureBuffer" << "resolution");
    Shaders::addShader("linearBlurH", QStringList() << "position",QStringList() << "textureBuffer" << "resolution");
    Shaders::addShader("final", QStringList() << "position", QStringList() << "textureBuffer" << "aspect");

    Shaders::addShader("bloomPass1", QStringList() << "position", QStringList() << "textureBuffer" << "threshold");
    Shaders::addShader("bloomPass2", QStringList() << "position", QStringList() << "textureBuffer" << "textureBloom");
    Shaders::addShader("lightShaftsPass1", QStringList() << "position", QStringList() << "textureBuffer" << "textureThreshold" << "screenSpaceLight" << "exposure" << "decay" << "density" << "weight" << "samples");
    Shaders::addShader("lightShaftsPass2", QStringList() << "position", QStringList() << "textureBuffer" << "textureLightShafts");
}

void Renderer::render()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);

    _inputFBO->bind();
    _inputFBO->resizeViewport();
    _inputFBO->clear();

    _scene->draw();

    _inputFBO->unbind();
    compose();
}

void Renderer::compose()
{
    preRenderBuffer();

    if (_useBloom)
        renderBloom();
    if (_useLightShafts)
        renderLightShafts();

    postRenderBuffer();
}

void Renderer::preRenderBuffer()
{
    _readBuffer->bind();
    glClear(GL_COLOR_BUFFER_BIT);
    getShader("composition")->bind();
    _inputFBO->bindAndTransmitTextures(getShader("composition"));
    _screenSpaceImage->draw(getShader("composition")->getAttribute("position"));
    _readBuffer->unbind();
}

void Renderer::postRenderBuffer()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, _width, _height);
    getShader("final")->bind();
    _readBuffer->bindAndTransmitTextures(getShader("final"));
    getShader("final")->transmitUniform("aspect", (float)_width/(float)_height);
    _screenSpaceImage->draw(getShader("final")->getAttribute("position"));
}

void Renderer::renderBloom()
{
    _writeHalfBuffer->resizeViewport();
    _writeHalfBuffer->bind();
    glClear(GL_COLOR_BUFFER_BIT);
    getShader("bloomPass1")->bind();
    _readBuffer->bindAndTransmitTextures(getShader("bloomPass1"));
    _screenSpaceImage->draw(getShader("bloomPass1")->getAttribute("position"));
    _writeHalfBuffer->unbind();
    swapBuffers(true);
    linearBlur(true);
    _writeBuffer->resizeViewport();
    _writeBuffer->bind();
    glClear(GL_COLOR_BUFFER_BIT);
    getShader("bloomPass2")->bind();
    Texture* base = _readBuffer->getTexture("textureBuffer");
    base->bind();
    getShader("bloomPass2")->transmitUniform("textureBuffer", base);
    Texture* bloom = _readHalfBuffer->getTexture("textureBuffer");
    bloom->bind();
    getShader("bloomPass2")->transmitUniform("textureBloom", bloom);
    _screenSpaceImage->draw(getShader("bloomPass2")->getAttribute("position"));
    _writeBuffer->unbind();
    swapBuffers();
}

void Renderer::renderLightShafts()
{
    // Pass 1 : downsample
    _writeHalfBuffer->resizeViewport();
    _writeHalfBuffer->bind();
    glClear(GL_COLOR_BUFFER_BIT);
    //shaderCompose->bind();
    Texture* threshold = _inputFBO->getTexture("textureThreshold");
    threshold->bind();
    getShader("composition")->transmitUniform("textureBuffer", threshold);
    _screenSpaceImage->draw(getShader("lightShaftsPass1")->getAttribute("position"));
    _writeHalfBuffer->unbind();
    linearBlur(true);
    // Pass 2 : rays
    _writeHalfBuffer->resizeViewport();
    _writeHalfBuffer->bind();
    glClear(GL_COLOR_BUFFER_BIT);
    //glm::vec3 lightPosScreen(0.0, 10.0, 0.0);
    getShader("lightShaftsPass1")->bind();
    getShader("lightShaftsPass1")->transmitUniform("screenSpaceLight", reinterpret_cast<DirectionalLight*>(_scene->getLight())->getScreenSpacePosition());
    _readHalfBuffer->bindAndTransmitTextures(getShader("lightShaftsPass1"));
    _screenSpaceImage->draw(getShader("lightShaftsPass1")->getAttribute("position"));
    _writeHalfBuffer->unbind();
    swapBuffers(true);
    _writeBuffer->resizeViewport();
    _writeBuffer->bind();
    glClear(GL_COLOR_BUFFER_BIT);
    getShader("lightShaftsPass2")->bind();
    Texture* base = _readBuffer->getTexture("textureBuffer");
    base->bind();
    getShader("lightShaftsPass2")->transmitUniform("textureBuffer", base);
    Texture* lightShafts = _readHalfBuffer->getTexture("textureBuffer");
    lightShafts->bind();
    getShader("lightShaftsPass2")->transmitUniform("textureLightShafts", lightShafts);
    _screenSpaceImage->draw(getShader("lightShaftsPass2")->getAttribute("position"));
    _writeBuffer->unbind();
    swapBuffers();
}

void Renderer::linearBlur(bool half)
{
    float res = half ? 2.0 : 1.0;

    FrameBuffer* inputBuffer = half ? _readHalfBuffer : _readBuffer;
    FrameBuffer* outputBuffer = half ? _writeHalfBuffer : _writeBuffer;

    outputBuffer->bind();
    glClear(GL_COLOR_BUFFER_BIT);

    getShader("linearBlurV")->bind();
    getShader("linearBlurV")->transmitUniform("resolution", res/_width, res/_height);

    inputBuffer->bindAndTransmitTextures(getShader("linearBlurV"));

    _screenSpaceImage->draw(getShader("linearBlurV")->getAttribute("position"));

    outputBuffer->unbind();

    FrameBuffer* tmp = inputBuffer;
    inputBuffer = outputBuffer;
    outputBuffer = tmp;

    outputBuffer->bind();
    glClear(GL_COLOR_BUFFER_BIT);

    getShader("linearBlurH")->bind();
    getShader("linearBlurH")->transmitUniform("resolution", res/_width, res/_height);

    inputBuffer->bindAndTransmitTextures(getShader("linearBlurH"));

    _screenSpaceImage->draw(getShader("linearBlurH")->getAttribute("position"));

    outputBuffer->unbind();
}

void Renderer::getModelViewProjectionMatrix(GLfloat matrix[16])
{
    _scene->getCamera()->getModelViewProjectionMatrix(matrix);
}

void Renderer::swapBuffers(bool haft)
{
    if (!haft)
    {
        FrameBuffer* tmp = _readBuffer;
        _readBuffer = _writeBuffer;
        _writeBuffer = tmp;
    }
    else
    {
        FrameBuffer* tmp = _readHalfBuffer;
        _readHalfBuffer = _writeHalfBuffer;
        _writeHalfBuffer = tmp;
    }
}
