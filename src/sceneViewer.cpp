#include <GL/glew.h>
#include "sceneViewer.h"

#include <QKeyEvent>

namespace Tessellation
{

    #define USE_KEYBOARD 0

    SceneViewer::SceneViewer(Ui_MainWindow *userInterface, QGLFormat glFormat):
        QGLViewer(glFormat),
        _isInitialized(false),
        _isWireframe(false),
        _currentFrame(1),
        _isTessellated(false)
    {
        _userInterface = userInterface;
        resize(1024, 768);
    }

    SceneViewer::~SceneViewer()
    {
    }

    void SceneViewer::init()
    {
        setSceneRadius(100.0);
        setSceneCenter(Vec(0.0, 0.0, 0.0));
        setBackgroundColor(Qt::black);

        //QSize screenSize = _userInterface->widgetViewer->size();
        //glViewport(0, 0, screenSize.width(), screenSize.height());

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glewExperimental = GL_TRUE;
        glewInit();

        _renderer.reset(new Renderer());
        _scene.reset(new Scene(this->camera()));
        _scene->initialize(1024, 768);
        _renderer->initialize(_scene.get());

        _isInitialized = true;
    }

    void SceneViewer::loadAnimation(std::string path, const int frameCount)
    {
        _animationPath = path.substr(0, path.length()-5);
        _player.reset(new ScenePlayer(frameCount));
        _userInterface->sFrames->setValue(1);
        _userInterface->sFrames->setMinimum(1);
        _userInterface->sFrames->setMaximum(frameCount);
        _scene->loadAnimation(_animationPath, frameCount, *_userInterface->progressBar);
    }

    void SceneViewer::loadModel(std::string path)
    {
        _scene->loadScene(path);
    }

    void SceneViewer::loadInputPoints(std::string path)
    {
        _scene->loadScene(path, false);
    }

    void SceneViewer::updateInputPoints()
    {
        _scene->updateInputPoints();
        update();
    }

    void SceneViewer::toggleTessellation(bool value)
    {
        _isTessellated = value;
        if (value)
            initializeTS();
        _scene->updateObjectShaders(value);
        update();
    }

    void SceneViewer::toggleDisplacement(bool value)
    {
        _scene->addDisplacement(value);
        update();
    }

    void SceneViewer::initializeTS()
    {
        GLint maxPatchVertices = 0;
        glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &maxPatchVertices);
        _userInterface->sInnerLevel->setMaximum(maxPatchVertices);
        _userInterface->sOuterLevel->setMaximum(maxPatchVertices);
    }

    bool SceneViewer::isReady()
    {
        if (_isInitialized)
            return _renderer->isInitialized();
        else
            return false;
    }

    void SceneViewer::playPause()
    {
        if (_player->isPaused())
            startAnimation();
        else
            stopAnimation();

        _player->playPause();
    }

    void SceneViewer::setInnerTL(int value)
    {
        _scene->getGeometry(_currentFrame-1)->setInnerTL(value);
        update();
    }

    void SceneViewer::setOuterTL(int value)
    {
        _scene->getGeometry(_currentFrame-1)->setOuterTL(value);
        update();
    }

    void SceneViewer::resizeGL(int width, int height)
    {
        _renderer->resize(width, height);
    }

    void SceneViewer::setCurrentFrame(const int currentFrame)
    {
        _currentFrame = currentFrame;
        update();
    }

    void SceneViewer::animate()
    {
        _currentFrame = _player->getNextFrame();
        _userInterface->sFrames->setValue(_currentFrame);
    }

    void SceneViewer::draw()
    {
        bool animation = (animationIsStarted() || _userInterface->widgetPlayer->isVisible() || _currentFrame != 1);
        _renderer->render(_currentFrame, animation);
    }

    void SceneViewer::showInputPoints(bool value)
    {
        _scene->showInputPoints(value);
        update();
    }

    void SceneViewer::keyPressEvent(QKeyEvent* event)
    {
        switch (event->key())
        {
    #if USE_KEYBOARD
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
    #endif
            case Qt::Key_Z:
            {
                if (_isWireframe)
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                else
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                _isWireframe = !_isWireframe;
            }
            break;
            case Qt::Key_1:
            {
                _scene->frontCameraView();
                _userInterface->statusBar->showMessage("Front view", 1000);
            }
            break;
            case Qt::Key_3:
            {
                _scene->rightCameraView();
                _userInterface->statusBar->showMessage("Right view", 1000);
            }
            break;
            case Qt::Key_5:
            {
                bool perspective = _scene->toggleCameraProjectionType();
                if (perspective)
                    _userInterface->statusBar->showMessage("Perspective projection", 1000);
                else
                    _userInterface->statusBar->showMessage("Orthographic projection", 1000);
            }
            break;
            case Qt::Key_7:
            {
                _scene->topCameraView();
                _userInterface->statusBar->showMessage("Top view", 1000);
            }
            break;
        }
        updateGL();
    }

}
