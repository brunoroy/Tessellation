#include "mediator.h"
#include <QMessageBox>

Mediator::Mediator()
{
    _mainWindow.reset(new QMainWindow());
    _userInterface.setupUi(_mainWindow.get());

    QGLFormat glFormat;
    //glFormat.setProfile(QGLFormat::CoreProfile);
    glFormat.setDepth(true);
    glFormat.setRgba(true);
    glFormat.setDoubleBuffer(true);
    QGLFormat::setDefaultFormat(glFormat);

    _sceneViewer.reset(new SceneViewer(&_userInterface, glFormat));

    initViewer();
    initSignalSlot();
    initUserInterface();

    _mainWindow->show();
}

Mediator::~Mediator()
{

}

void Mediator::initViewer()
{
    _viewerLayout.reset(new QGridLayout());
    _viewerLayout->setMargin(0);
    _viewerLayout->addWidget(_sceneViewer.get());
    _userInterface.widgetViewer->setLayout(_viewerLayout.get());
}

void Mediator::initSignalSlot()
{
    //file
    connect(_userInterface.actionSnapshot, SIGNAL(triggered()), this, SLOT(saveSnapshot()));
    connect(_userInterface.actionQuit, SIGNAL(triggered()), _mainWindow.get(), SLOT(close()));

    //help
    connect(_userInterface.actionAbout, SIGNAL(triggered()), this, SLOT(about()));

    //tools
    connect(_userInterface.actionShaders, SIGNAL(triggered(bool)), this, SLOT(showShaders(bool)));
    connect(_userInterface.actionDefaultValues, SIGNAL(triggered()), this, SLOT(defaultValues()));

    //shaders
    connect(_userInterface.checkVLS, SIGNAL(toggled(bool)), this, SLOT(toggleVLS(bool)));
    connect(_userInterface.checkBloom, SIGNAL(toggled(bool)), this, SLOT(toggleBloom(bool)));

    connect(_userInterface.sliderExposure, SIGNAL(valueChanged(int)), this, SLOT(setExposure(int)));
    connect(_userInterface.sliderDecay, SIGNAL(valueChanged(int)), this, SLOT(setDecay(int)));
    connect(_userInterface.sliderDensity, SIGNAL(valueChanged(int)), this, SLOT(setDensity(int)));
    connect(_userInterface.sliderWeight, SIGNAL(valueChanged(int)), this, SLOT(setWeight(int)));
    connect(_userInterface.sliderSamples, SIGNAL(valueChanged(int)), this, SLOT(setSamples(int)));
    connect(_userInterface.sliderThreshold, SIGNAL(valueChanged(int)), this, SLOT(setThreshold(int)));
}

void Mediator::initUserInterface()
{
    defaultValues();

    showShaders(false);
}

void Mediator::toggleVLS(bool value)
{
    _userInterface.groupVLS->setEnabled(value);
    _sceneViewer->toggleVLS(value);
    _sceneViewer->update();
}

void Mediator::toggleBloom(bool value)
{
    _userInterface.groupBloom->setEnabled(value);
    _sceneViewer->toggleBloom(value);
    _sceneViewer->update();
}

void Mediator::showShaders(bool value)
{
    _userInterface.widgetTools->setVisible(value);
}

void Mediator::defaultValues()
{
    //Initial values
    int exposure = 40;
    int decay = 93;
    int density = 90;
    int weight = 40;
    int samples = 100;
    int threshold = 30;

    //VLS
    _userInterface.sliderExposure->setValue(exposure);
    _userInterface.sliderDecay->setValue(decay);
    _userInterface.sliderDensity->setValue(density);
    _userInterface.sliderWeight->setValue(weight);
    _userInterface.sliderSamples->setValue(samples);

    //Bloom
    _userInterface.sliderThreshold->setValue(threshold);
}

void Mediator::setExposure(int value)
{
    if (_sceneViewer->isReady())
    {
        Shaders::getShader("lightShaftsPass1")->bind();
        Shaders::getShader("lightShaftsPass1")->transmitUniform("exposure", getValue(value));
        _sceneViewer->update();
    }
    _userInterface.eExposure->setText(QString::number(getValue(value)));
}

void Mediator::setDecay(int value)
{
    if (_sceneViewer->isReady())
    {
        Shaders::getShader("lightShaftsPass1")->bind();
        Shaders::getShader("lightShaftsPass1")->transmitUniform("decay", getValue(value));
        _sceneViewer->update();
    }
    _userInterface.eDecay->setText(QString::number(getValue(value)));
}

void Mediator::setDensity(int value)
{
    if (_sceneViewer->isReady())
    {
        Shaders::getShader("lightShaftsPass1")->bind();
        Shaders::getShader("lightShaftsPass1")->transmitUniform("density", getValue(value));
        _sceneViewer->update();
    }
    _userInterface.eDensity->setText(QString::number(getValue(value)));
}

void Mediator::setWeight(int value)
{
    if (_sceneViewer->isReady())
    {
        Shaders::getShader("lightShaftsPass1")->bind();
        Shaders::getShader("lightShaftsPass1")->transmitUniform("weight", getValue(value));
        _sceneViewer->update();
    }
    _userInterface.eWeight->setText(QString::number(getValue(value)));
}

void Mediator::setSamples(int value)
{
    if (_sceneViewer->isReady())
    {
        Shaders::getShader("lightShaftsPass1")->bind();
        Shaders::getShader("lightShaftsPass1")->transmitUniform("samples", value);
        _sceneViewer->update();
    }
    _userInterface.eSamples->setText(QString::number(value));
}

void Mediator::setThreshold(int value)
{
    if (_sceneViewer->isReady())
    {
        Shaders::getShader("bloomPass1")->bind();
        Shaders::getShader("bloomPass1")->transmitUniform("threshold", getValue(value));
        _sceneViewer->update();
    }
    _userInterface.eThreshold->setText(QString::number(getValue(value)));
}

float Mediator::getValue(int value)
{
    float floatValue = static_cast<float>(value) / 100.0f;
    return floatValue;
}

void Mediator::saveSnapshot()
{
    QDir savesFolder("data/saves/");
    uint id = savesFolder.entryList().size();
    _sceneViewer->saveSnapshot(QString("data/saves/image%1.jpg").arg(id));
}

void Mediator::about()
{
    QMessageBox msgBox;
    msgBox.setText("This project has been produced as part of IFT6095's course at the University of Montreal.\n\nCopyright (C) 2015 Bruno Roy\n\nThis program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.\n\nThis program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.\n\nYou should have received a copy of the GNU General Public License\nalong with this program; if not, write to the Free Software\nFoundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA");
    msgBox.exec();
}
