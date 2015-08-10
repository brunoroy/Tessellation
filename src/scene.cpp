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
    if (isLoaded())
    {
        glm::mat4 mvp = updateMVP();
        if (animation)
        {
            _meshes.at(currentFrame-1)->preDraw();
            _meshes.at(currentFrame-1)->setMVP(mvp);
            _meshes.at(currentFrame-1)->draw();
        }
        else
        {
            foreach (Mesh *mesh, _meshes)
            {
                mesh->preDraw();
                mesh->setMVP(mvp);
                mesh->draw();
            }
        }
        _light->setMVP(mvp);
    }
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
    mesh->scale(glm::vec3(10.0f, 10.0f, 10.0f));
    addMesh(mesh);
    //updateGrid(mesh);
}

void Scene::updateGrid(Mesh *mesh)
{
    for (int i = 0; i < mesh->getTriangleCount(); i++)
    {
        //std::clog << "processing polygon " << i << "." << std::endl;
        glm::vec3 vertices[3];
        for (int v = 0; v < 3; v++)
        {
            uint vertexIndex = mesh->getIndices().at(i*3+v);
            //std::clog << "index: " << vertexIndex << std::endl;
            vertices[v] = mesh->getPositions().at(vertexIndex);
            //std::clog << "v[" << v << "]: { " << vertices[v].x << "," << vertices[v].y << "," << vertices[v].z << "}" << std::endl;
        }
        _grid->insertPolygon(i, vertices);
    }
}

void Scene::updateGrid(InputPoints *points)
{
    for (int i = 0; i < points->getPointCount(); i++)
        _grid->insertPoint(i, points->getPoint(i));
}

void Scene::loadLight()
{
    glm::vec3 worldPosition = glm::vec3(0.0, 1.0, 0.0);
    glm::vec3 worldDirection = glm::vec3(-0.5, -0.5, -0.5);

    DirectionalLight *sun = new DirectionalLight(worldPosition, worldDirection);
    setLight(sun);
}

void Scene::loadScene(std::string path)
{
    _meshes.clear();
    loadModel(path);
    loadLight();

    foreach (Mesh *mesh, _meshes)
        mesh->initialize();

    _loaded = true;
}

void Scene::loadAnimation(std::string path, const int frameCount, QProgressBar &progress)
{
    _meshes.clear();
    progress.setValue(0);
    progress.setMaximum(frameCount);
    for (size_t i = 0; i < frameCount; i++)
    {
        std::string filename(std::string(path).append(std::to_string(static_cast<ll>(i))).append(".ply"));
        loadModel(filename);
        progress.setValue(progress.value()+1);
    }
    loadLight();

    foreach (Mesh *mesh, _meshes)
        mesh->initialize();

    _loaded = true;
}

void Scene::updateObjectShaders()
{
    foreach (Mesh *mesh, _meshes)
        mesh->getMaterial()->setShader("render");
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
