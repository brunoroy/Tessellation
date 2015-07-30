#ifndef SCENEVIEWER_H
#define SCENEVIEWER_H

#include "renderer.h"
#include <QGLViewer/qglviewer.h>

#include <QVector3D>
#include <QFile>
#include <QTextStream>
#include "ui_mainWindow.h"
#include <glm/glm.hpp>

using namespace qglviewer;

class SceneViewer : public QGLViewer
{
public:
    SceneViewer(Ui_MainWindow *userInterface, QGLFormat glFormat);
    ~SceneViewer();

    void drawCube();
    void drawPlane();
    bool isReady();

    void toggleTessellation(bool value);
    void initializeTS();

protected:
    void init();
    void draw();
    void animate();
    void resizeGL(int width, int height);
    void keyPressEvent(QKeyEvent* event);

private:
    bool _isInitialized;

    std::shared_ptr<Scene> _scene;
    std::shared_ptr<Renderer> _renderer;

    qglviewer::Vec _cameraOrigin;
    qglviewer::Vec _cameraDirection;

    Ui_MainWindow *_userInterface;
};

#endif // SCENEVIEWER_H
