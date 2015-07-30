#include "include/scene.h"
#include <QCursor>
#include <QGLViewer/frame.h>

Scene::Scene(Camera *camera):
    _loaded(false),
    _moveSpeed(0.5f)
{
    _camera.reset(camera);
    _camera->setType(Camera::PERSPECTIVE);
    _camera->setZNearCoefficient(0.00001);
    _camera->setPivotPoint(Vec(0.0, 0.0, 0.0));
    _camera->setUpVector(Vec(0.0, 1.0, 0.0));
    _camera->setPosition(Vec(0.0, 0.0, 60.0));
}

Scene::~Scene()
{
}

void Scene::initialize(uint width, uint height)
{
    _width = width;
    _height = height;

    loadScene();

    foreach (Mesh *mesh, _meshes)
        mesh->initialize();

    _loaded = true;
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

void Scene::draw()
{
    glm::mat4 mvp = updateMVP();
    foreach (Mesh *mesh, _meshes)
    {
        mesh->preDraw();
        mesh->setMVP(mvp);
        mesh->draw();
    }
    //updateConstraints();
    _light->setMVP(mvp);
}

void Scene::updateConstraints()
{
    Quaternion orientation = _camera->orientation();
    float yaw = 2.0 * asin(orientation[0] * orientation[2] - orientation[3] * orientation[1]);

    _lookConstraints = glm::vec3(sin(yaw), -cos(yaw), 0.0);

    glm::vec3 camera = glm::vec3(_camera->position().x, _camera->position().y, _camera->position().z);
    reinterpret_cast<DirectionalLight*>(_light.get())->update(camera, _lookConstraints);
}

void Scene::resize(uint width, uint height)
{
    _camera->setAspectRatio(width/height);
}

void Scene::loadModel(std::string path)
{
    Texture::resetUnit();
    Texture* basicTexture = Texture::newFromNextUnit();
    Mesh* mesh = new Mesh(QString(path.c_str()));
    basicTexture->load("data/textures/white.jpg");
    basicTexture->setFilters(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
    basicTexture->initialize();

    ((MaterialDefault*)(mesh->getMaterial()))->setTexture(basicTexture);
    //mesh->translate(glm::vec3(0.0f, 5.0f, -5.0f));
    mesh->scale(glm::vec3(10.0f, 10.0f, 10.0f));
    addMesh(mesh);
}

void Scene::loadLight()
{
    glm::vec3 worldPosition = glm::vec3(0.0, 1.0, 0.0);
    glm::vec3 worldDirection = glm::vec3(-0.5, -0.5, -0.5);

    DirectionalLight *sun = new DirectionalLight(worldPosition, worldDirection);
    setLight(sun);
}

void Scene::loadScene()
{
    loadModel("data/models/tree.obj");
    loadLight();
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
