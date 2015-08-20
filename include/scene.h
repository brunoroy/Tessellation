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
    void draw(const int currentFrame, const bool animation = false);
    void updateConstraints();
    glm::mat4 updateMVP();
    bool isLoaded() {return _loaded;}
    void setLoaded(bool loaded) {_loaded = loaded;}
    void resize(uint width, uint height);

    Camera* getCamera() {return _camera.get();}
    void addGeometry(Geometry* geometry)
    {
        _geometries.push_back(geometry);
    }
    std::vector<Geometry*> getGeometries() {return _geometries;}
    Geometry* getGeometry(uint index) {return _geometries.at(index);}

    void moveForward();
    void moveBackward();
    void strafeLeft();
    void strafeRight();

    glm::mat4 getCurrentMVP();
    void loadModel(std::string path, const bool isCloud = false);
    void loadLight();
    void loadScene(std::string path, const bool isCloud = false);
    void loadAnimation(std::string path, const int frameCount, QProgressBar &progress);
    void reset()
    {
        _loaded = false;
        _geometries.clear();
    }

    uint getWidth() {return _width;}
    uint getHeight() {return _height;}

    void setLight(Light *light);
    Light* getLight() {return _light.get();}

    void updateObjectShaders();
    void updateGrid(Geometry *geometry);
    void updateGrid(std::vector<glm::vec3> points);

    void frontCameraView();
    void rightCameraView();
    void topCameraView();
    bool toggleCameraProjectionType();

private:
    std::shared_ptr<Camera> _camera;
    std::vector<Geometry*> _geometries;
    std::shared_ptr<Light> _light;
    std::shared_ptr<SpatialGrid> _grid;

    glm::mat4 _modelView;
    glm::mat4 _projection;
    glm::mat4 _mvp;

    qreal _initialCameraDistance;
    Vec _initialCameraPosition;
    Vec _initialCameraUpVector;

    bool _loaded;
    float _moveSpeed;

    uint _width;
    uint _height;

    glm::vec3 _lookConstraints;
};

#endif // SCENE_H
