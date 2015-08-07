#ifndef SCENE_H
#define SCENE_H

#include <QProgressBar>
#include <vector>

#include "geometry.h"
#include "light.h"
#include "spatialGrid.h"

#include <QGLViewer/qglviewer.h>

typedef long long ll;

using namespace qglviewer;

class Scene
{
public:
    Scene(Camera *camera);
    ~Scene();

    void initialize(uint width, uint height);
    void draw(const int currentFrame, const bool animation);
    void updateConstraints();
    glm::mat4 updateMVP();
    bool isLoaded() {return _loaded;}
    void resize(uint width, uint height);

    Camera* getCamera() {return _camera.get();}
    void addMesh(Mesh* mesh)
    {
        _meshes.push_back(mesh);
    }
    std::vector<Mesh*> getMeshes() {return _meshes;}
    Mesh* getMesh(uint index) {return _meshes.at(index);}

    void moveForward();
    void moveBackward();
    void strafeLeft();
    void strafeRight();

    glm::mat4 getCurrentMVP();
    void loadModel(std::string path);
    void loadLight();
    void loadScene(std::string path);
    void loadAnimation(std::string path, const int frameCount, QProgressBar &progress);

    uint getWidth() {return _width;}
    uint getHeight() {return _height;}

    void setLight(Light *light);
    Light* getLight() {return _light.get();}

    void updateObjectShaders();
    void updateGrid(Mesh *mesh);
    void updateGrid(InputPoints *points);

private:
    std::shared_ptr<Camera> _camera;
    std::vector<Mesh*> _meshes;
    std::shared_ptr<Light> _light;
    std::shared_ptr<SpatialGrid> _grid;

    glm::mat4 _modelView;
    glm::mat4 _projection;
    glm::mat4 _mvp;

    bool _loaded;
    float _moveSpeed;

    uint _width;
    uint _height;

    glm::vec3 _lookConstraints;
};

#endif // SCENE_H
