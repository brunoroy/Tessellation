#ifndef RENDERER_H
#define RENDERER_H

#include <vector>

//#include "shader.h"
#include "scene.h"
#include "frameBuffer.h"
#include "screenSpaceImage.h"

class Renderer
{
public:
    Renderer();
    ~Renderer();

    void loadShaders();
    void initialize(Scene *scene);
    void resize(int width, int height);
    bool isInitialized() {return _initialized;}

    void toggleBloom(bool value) {_useBloom = value;}
    void toggleVLS(bool value) {_useLightShafts = value;}

    void render();
    void compose();
    void swapBuffers(bool half = false);
    void linearBlur(bool half);

    void preRenderBuffer();
    void postRenderBuffer();

    void renderBloom();
    void renderLightShafts();

    void getModelViewProjectionMatrix(GLfloat matrix[16]);

protected:
    Shader* getShader(QString value) {return Shaders::getShader(value);}

private:
    float _width, _height;
    bool _initialized;
    bool _useLightShafts;
    bool _useBloom;

    FrameBuffer *_inputFBO;
    FrameBuffer *_inputBuffer;
    FrameBuffer *_readBuffer;
    FrameBuffer *_writeBuffer;
    FrameBuffer *_readHalfBuffer;
    FrameBuffer *_writeHalfBuffer;

    std::shared_ptr<ScreenSpaceImage> _screenSpaceImage;

    qglviewer::Camera *_camera;
    Scene *_scene;
};

#endif // RENDERER_H
