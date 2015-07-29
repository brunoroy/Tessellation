#ifndef MEDIATOR_H
#define MEDIATOR_H

#include <QObject>
#include <QGridLayout>
#include <QFileDialog>
#include <QDir>

#include <memory>
#include "sceneViewer.h"

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
    void showShaders(bool value);
    void toggleVLS(bool value);
    void toggleBloom(bool value);
    void defaultValues();
    void saveSnapshot();

    void setExposure(int value);
    void setDecay(int value);
    void setDensity(int value);
    void setWeight(int value);
    void setSamples(int value);

    void setThreshold(int value);

    void about();

private:
    std::shared_ptr<QMainWindow> _mainWindow;
    std::shared_ptr<SceneViewer> _sceneViewer;
    std::shared_ptr<QGridLayout> _viewerLayout;
    Ui_MainWindow _userInterface;

    float getValue(int value);
    //void transmitUniform()
};

#endif // MEDIATOR_H
