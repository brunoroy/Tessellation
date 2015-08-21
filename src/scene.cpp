#include "include/scene.h"
#include <QCursor>
#include <QGLViewer/frame.h>

namespace Tessellation
{

    Scene::Scene(Camera *camera):
        _loaded(false),
        _moveSpeed(0.5f),
        _showInputPoints(false)
    {
        _camera.reset(camera);
        _camera->setType(Camera::PERSPECTIVE);
        _camera->setZNearCoefficient(0.00001);
        _camera->setPivotPoint(Vec(0.0, 0.0, 0.0));
        _initialCameraUpVector = Vec(0.0, 1.0, 0.0);
        _camera->setUpVector(_initialCameraUpVector);
        _initialCameraDistance = 50.0;
        _initialCameraPosition = Vec(0.0, 0.0, _initialCameraDistance);
        _camera->setPosition(_initialCameraPosition);

        _grid.reset(new SpatialGrid(Volume(1.0f, 1.0f, 1.0f)));
    }

    Scene::~Scene()
    {
    }

    void Scene::initialize(uint width, uint height)
    {
        _width = width;
        _height = height;

        loadScene("data/models/triangle.obj");
    }

    glm::mat4 Scene::updateMVP()
    {
        GLfloat modelView[16], projection[16], mvp[16];

        _camera->getModelViewMatrix(modelView);
        _modelView = glm::make_mat4(modelView);

        _camera->getProjectionMatrix(projection);
        _projection = glm::make_mat4(projection);

        _camera->getModelViewProjectionMatrix(mvp);
        _mvp = glm::make_mat4(mvp);

        return _mvp;
    }

    void Scene::draw(const int currentFrame, const bool animation)
    {
        if (isLoaded() && !_geometries.empty())
        {
            glm::mat4 mvp = updateMVP();
            if (animation)
            {
                _geometries.at(currentFrame-1)->preDraw();
                _geometries.at(currentFrame-1)->setMVP(mvp);
                _geometries.at(currentFrame-1)->draw();
            }
            else
            {
                foreach (Geometry *geometry, _geometries)
                {
                    //std::clog << "geometry[" << geometry->getId() << "]: Shader[" << geometry->getShader()->getValue().toStdString().c_str() << "]" << std::endl;
                    if (geometry->getType() == GeometryType::Mesh ||
                       (geometry->getType() == GeometryType::Cloud && _showInputPoints))
                    {
                        geometry->preDraw();
                        geometry->setMVP(mvp);
                        geometry->draw();
                    }
                }
            }
            _light->setMVP(mvp);
        }
    }

    void Scene::addDisplacement(bool value)
    {
        foreach (Geometry *geometry, _geometries)
            geometry->addDisplacement(value);
    }

    void Scene::resize(uint width, uint height)
    {
        _camera->setAspectRatio(width/height);
    }

    void Scene::loadModel(std::string path, const bool isTessellable)
    {
        uint geometryId = _geometries.size() + 1;
        Geometry* geometry = new Geometry(QString(path.c_str()), geometryId, isTessellable);
        geometry->scale(glm::vec3(10.0f, 10.0f, 10.0f));
        addGeometry(geometry);
        updateGrid(geometry);
    }

    void Scene::updateGrid(Geometry *geometry)
    {
        if (geometry->getType() == GeometryType::Mesh)
        {
            for (int i = 0; i < geometry->getTriangleCount(); i++)
            {
                glm::vec3 vertices[3];
                for (int v = 0; v < 3; v++)
                {
                    uint vertexIndex = geometry->getIndices().at(i*3+v);
                    vertices[v] = geometry->getPositions().at(vertexIndex);
                }
                _grid->insertPolygon(i, vertices);
            }
            std::clog << __FUNCTION__ << ": " << geometry->getTriangleCount() << " triangles added.\n";
        }
        else if (geometry->getType() == GeometryType::Cloud)
        {
            for (int i = 0; i < geometry->getVertexCount(); i++)
            {
                glm::vec3 point = geometry->getPositions().at(i);
                _grid->insertPoint(i, point);
            }
            std::clog << __FUNCTION__ << ": " << geometry->getVertexCount() << " points added.\n";
        }
    }

    void Scene::updateInputPoints()
    {
        foreach (Geometry *geometry, _geometries)
        {
            if (geometry->getType() == GeometryType::Cloud)
            {
                for (int i = 0; i < _grid->getSize(); i++)
                {
                    GridCell &cell = _grid->getCell(i);
                    PointList points = cell.getPoints();
                    PolygonList polygons = cell.getPolygons();
                    for (int j = 0; j < points.size(); j++)
                    {
                        Point point = points.at(j);
                        for (int k = 0; k < polygons.size(); k++)
                        {
                            GridPolygon polygon = polygons.at(k);
                            glm::vec3 displacement = GeometryTools::getDisplacement(polygon.getVertices(), point.getPosition());
                            geometry->setDisplacement(point.getId(), displacement);
                        }
                    }
                }
            }
            geometry->initialize();
        }
    }

    void Scene::loadLight()
    {
        glm::vec3 worldPosition = glm::vec3(0.0, 1.0, 0.0);
        glm::vec3 worldDirection = glm::vec3(-0.5, -0.5, -0.5);

        DirectionalLight *sun = new DirectionalLight(worldPosition, worldDirection);
        setLight(sun);
    }

    void Scene::loadScene(std::string path, const bool isTessellable)
    {
        loadModel(path, isTessellable);
        loadLight();

        foreach (Geometry *geometry, _geometries)
            geometry->initialize();

        _loaded = true;
    }

    void Scene::loadAnimation(std::string path, const int frameCount, QProgressBar &progress)
    {
        _geometries.clear();
        progress.setValue(0);
        progress.setMaximum(frameCount);
        for (size_t i = 0; i < frameCount; i++)
        {
            std::string filename(std::string(path).append(std::to_string(static_cast<ll>(i))).append(".ply"));
            loadModel(filename);
            progress.setValue(progress.value()+1);
        }
        loadLight();

        foreach (Geometry *geometry, _geometries)
            geometry->initialize();

        _loaded = true;
    }

    void Scene::updateObjectShaders(const bool doTessellation)
    {
        foreach (Geometry *geometry, _geometries)
            if (geometry->isTessellable() && doTessellation)
                geometry->getMaterial()->setShader("renderTL");
            else
                geometry->getMaterial()->setShader("render");
    }

    void Scene::setLight(Light *light)
    {
        _light.reset(light);
    }

    void Scene::moveForward()
    {
        Vec position = _camera.get()->position();
        Vec direction = _camera.get()->viewDirection()*_moveSpeed;
        Vec newPosition(position.x+direction.x, position.y, position.z+direction.z);
        _camera.get()->setPosition(newPosition);
    }

    void Scene::moveBackward()
    {
        Vec position = _camera.get()->position();
        Vec direction = _camera.get()->viewDirection()*_moveSpeed;
        Vec newPosition(position.x-direction.x, position.y, position.z-direction.z);
        _camera.get()->setPosition(newPosition);
    }

    void Scene::strafeLeft()
    {
        Vec position = _camera.get()->position();
        Vec direction = _camera.get()->rightVector()*_moveSpeed;
        Vec newPosition(position.x-direction.x, position.y, position.z-direction.z);
        _camera.get()->setPosition(newPosition);
    }

    void Scene::strafeRight()
    {
        Vec position = _camera.get()->position();
        Vec direction = _camera.get()->rightVector()*_moveSpeed;
        Vec newPosition(position.x+direction.x, position.y, position.z+direction.z);
        _camera.get()->setPosition(newPosition);
    }

    void Scene::frontCameraView()
    {
        _camera->setPosition(_initialCameraPosition);
        _camera->setUpVector(_initialCameraUpVector);
        _camera.get()->setOrientation(0.0, 0.0);
    }

    void Scene::rightCameraView()
    {
        _camera->setPosition(Vec(_initialCameraDistance, 0.0, 0.0));
        _camera->setUpVector(_initialCameraUpVector);
        _camera.get()->setOrientation(M_PI/2.0, 0.0);
    }

    void Scene::topCameraView()
    {
        _camera->setPosition(Vec(0.0, _initialCameraDistance, 0.0));
        _camera->setUpVector(Vec(0.0, 0.0, -1.0));
        _camera.get()->setOrientation(0.0, M_PI/2.0);
    }

    bool Scene::toggleCameraProjectionType()
    {
        if (_camera.get()->type() == Camera::PERSPECTIVE)
            _camera.get()->setType(Camera::ORTHOGRAPHIC);
        else
            _camera.get()->setType(Camera::PERSPECTIVE);

        return (_camera.get()->type() == Camera::PERSPECTIVE);
    }

}
