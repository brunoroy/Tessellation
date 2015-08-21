#ifndef MEDIATOR_H
#define MEDIATOR_H

#include <QObject>
#include <QGridLayout>
#include <QFileDialog>
#include <QDir>

#include <memory>
#include "sceneViewer.h"

namespace Tessellation
{

    class Mediator : public QObject
    {
    Q_OBJECT
    public:
        Mediator();
        ~Mediator();

        void initViewer();
        void initSignalSlot();
        void initUserInterface();

    public slots:
        //toolbar
        void importModel();
        void importAnimation();
        void showShaders(bool value);
        void showPlayer(bool value);
        void defaultValues();
        void resetScene();
        void saveSnapshot();
        void about();

        //tessellation
        void toggleTessellation(bool value);
        void setInnerLevel(int level);
        void setOuterLevel(int level);
        void setUniform(bool value);

        //displacement
        void toggleDisplacement(bool value);
        void setDensity(int value);
        void setDistanceEpsilon(int value);
        void browseInputPoints();
        void showInputPoints(bool value);

        //player
        void changeCurrentFrame(int currentFrame);
        void playPause();

    private:
        std::shared_ptr<QMainWindow> _mainWindow;
        std::shared_ptr<SceneViewer> _sceneViewer;
        std::shared_ptr<QGridLayout> _viewerLayout;
        Ui_MainWindow _userInterface;

        float getValue(int value);
    };

}

#endif // MEDIATOR_H
