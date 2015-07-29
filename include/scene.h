#ifndef SCENE_H
#define SCENE_H

#include "mesh.h"
#include <QGLViewer/qglviewer.h>
#include <vector>
#include "light.h"

using namespace qglviewer;

class Scene
{
public:
    Scene(Camera *camera);
    ~Scene();

    void initialize(uint width, uint height, bool validation = false);
    void draw();
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

    void loadSky(QString path);
    void loadTerrain();
    Mesh* loadTree(QString path);
    void loadModel(QString path);
    void loadLight();
    void loadForest(uint size);

    void loadScene();
    void loadValidationScene();

    uint getWidth() {return _width;}
    uint getHeight() {return _height;}

    void setLight(Light *light);
    Light* getLight() {return _light.get();}

private:
    std::shared_ptr<Camera> _camera;
    std::vector<Mesh*> _meshes;
    std::shared_ptr<Light> _light;

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
