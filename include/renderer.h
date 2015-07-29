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

    void toggleBloom(bool value) {_useBloom = value;}
    void toggleVLS(bool value) {_useLightShafts = value;}

    void render();
    void getModelViewProjectionMatrix(GLfloat matrix[16]);

protected:
    Shader* getShader(QString value) {return Shaders::getShader(value);}

private:
    float _width, _height;
    bool _initialized;
    bool _useLightShafts;
    bool _useBloom;

    qglviewer::Camera *_camera;
    Scene *_scene;
};

#endif // RENDERER_H
