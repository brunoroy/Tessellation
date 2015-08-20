#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include "scene.h"

class Renderer
{
public:
    Renderer();
    ~Renderer();

    void loadShaders();
    void initialize(Scene *scene);
    void resize(int width, int height);
    bool isInitialized() {return _initialized;}

    void render(const int currentFrame, const bool animation = false);

    void getModelViewProjectionMatrix(GLfloat matrix[16]);

protected:
    Shader* getShader(QString value) {return Shaders::getShader(value);}

private:
    float _width, _height;
    bool _initialized;
    bool _doTessellation;

    qglviewer::Camera *_camera;
    Scene *_scene;
};

#endif // RENDERER_H
