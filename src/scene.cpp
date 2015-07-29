#include "include/scene.h"
#include "sky.h"
#include "terrain.h"

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
    _camera->setPosition(Vec(0.0, 5.0, 0.0));
}

Scene::~Scene()
{
}

void Scene::initialize(uint width, uint height, bool validation)
{
    _width = width;
    _height = height;

    if (validation)
        loadValidationScene();
    else
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
    updateConstraints();
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

void Scene::loadSky(QString path)
{
    TextureCubeMap *cubeMap = new TextureCubeMap(path);
    MaterialSky *skyMaterial = new MaterialSky(cubeMap);

    Sky *sky = new Sky(skyMaterial, "data/models/cube.obj");
    sky->scale(glm::vec3(1000.0, 1000.0, 1000.0));
    addMesh(sky);
}

void Scene::loadTerrain()
{
    Texture::resetUnit();
    Texture *terrainGrass = Texture::newFromNextUnit();
    terrainGrass->load("data/textures/grass.jpg");
    terrainGrass->setFilters(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
    terrainGrass->initialize();

    MaterialTerrain *terrainMaterial = new MaterialTerrain(terrainGrass);
    Terrain *terrain = new Terrain("data/textures/heightMap.png", terrainMaterial, 2000.0f, 10.0f);
    addMesh(terrain);
}

void Scene::loadForest(uint size)
{
    srand (time(NULL));
    for (uint i = 0; i < size; i++)
    {
        Mesh *tree = loadTree("data/models/tree.obj");

        glm::vec3 randomTranslate = glm::vec3(static_cast<float>(rand() % 50 - 25), 0.0, static_cast<float>(rand() % 50 - 50));
        float randomAngle = static_cast<float>(rand() % 360 + 1);
        float randomScale = static_cast<float>(rand() % 3 + 1);

        tree->translate(glm::vec3(randomTranslate.x, randomTranslate.y, randomTranslate.z));
        tree->rotate(randomAngle, glm::vec3(0.0, 1.0, 0.0));
        tree->scale(glm::vec3(randomScale, randomScale, randomScale));
    }
}

Mesh* Scene::loadTree(QString path)
{
    Texture::resetUnit();
    Texture* barkTexture = Texture::newFromNextUnit();
    Texture* alpha = Texture::newFromNextUnit();
    Mesh* mesh = new Mesh(path);
    barkTexture->load("data/textures/bark.jpg");
    barkTexture->setFilters(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
    barkTexture->initialize();
    alpha->load("data/textures/alpha.jpg");
    alpha->setFilters(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
    alpha->initialize();

    ((MaterialDefault*)(mesh->getMaterial()))->setTextures(barkTexture, alpha);
    addMesh(mesh);

    return mesh;
}

void Scene::loadModel(QString path)
{
    Texture::resetUnit();
    Texture* basicTexture = Texture::newFromNextUnit();
    Texture* alphaTexture = Texture::newFromNextUnit();
    Mesh* mesh = new Mesh(path);
    basicTexture->load("data/textures/red.jpg");
    basicTexture->setFilters(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
    basicTexture->initialize();
    alphaTexture->load("data/textures/alpha.jpg");
    alphaTexture->setFilters(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
    alphaTexture->initialize();

    ((MaterialDefault*)(mesh->getMaterial()))->setTextures(basicTexture, alphaTexture);
    mesh->translate(glm::vec3(0.0f, 10.0f, 0.0f));
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
    loadSky("data/textures/sky");
    loadTerrain();
    loadForest(20);
    loadLight();
}

void Scene::loadValidationScene()
{
    /*loadSky("data/textures/sun2");
    loadModel("data/models/window.obj");
    loadLight();*/
    loadSky("data/textures/sun");
    loadModel("data/models/cube.obj");
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
