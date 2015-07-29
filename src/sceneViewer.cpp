#include <GL/glew.h>
#include "sceneViewer.h"

#include <QKeyEvent>

#define USE_KEYBOARD 0

SceneViewer::SceneViewer(Ui_MainWindow *userInterface, QGLFormat glFormat):
    QGLViewer(glFormat),
    _isInitialized(false)
{
    _userInterface = userInterface;
    resize(1024, 768);
}

SceneViewer::~SceneViewer()
{
}

void SceneViewer::init()
{
    setSceneRadius(1000.0);
    setSceneCenter(Vec(0.0, 0.0, 0.0));
    setBackgroundColor(Qt::black);

    //setMouseBinding(Qt::NoModifier, Qt::LeftButton, CAMERA, LOOK_AROUND);
    //setMouseBinding(Qt::NoModifier, Qt::RightButton, NO_CLICK_ACTION);
    //setWheelBinding(Qt::NoModifier, CAMERA, NO_MOUSE_ACTION);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glewExperimental = GL_TRUE;
    glewInit();

    _renderer.reset(new Renderer());
    _scene.reset(new Scene(this->camera()));
    _scene->initialize(1024, 768);
    _renderer->initialize(_scene.get());

    //this->startAnimation();
    _isInitialized = true;
}

bool SceneViewer::isReady()
{
    if (_isInitialized)
        return _renderer->isInitialized();
    else
        return false;
}

void SceneViewer::resizeGL(int width, int height)
{
    _renderer->resize(width, height);
}

void SceneViewer::animate()
{
    //_userInterface->statusBar->showMessage(QString("camera: {").append(QString::number(_scene->getCamera()->position().x)).append(", ").append(QString::number(_scene->getCamera()->position().y)).append(", ").append(QString::number(_scene->getCamera()->position().z)).append("}"));
    //_userInterface->statusBar->showMessage(QString("FPS: ").append(QString::number(static_cast<int>(this->currentFPS()))));
}

void SceneViewer::draw()
{
    _renderer->render();
}

void SceneViewer::keyPressEvent(QKeyEvent* event)
{
#if USE_KEYBOARD
    switch (event->key())
    {
        case Qt::Key_W:
            _scene->moveForward();
        break;
        case Qt::Key_A:
            _scene->strafeLeft();
        break;
        case Qt::Key_S:
            _scene->moveBackward();
        break;
        case Qt::Key_D:
            _scene->strafeRight();
        break;
    }
    updateGL();
#endif
}
