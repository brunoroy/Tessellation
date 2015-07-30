#include "mediator.h"
#include <QMessageBox>

Mediator::Mediator()
{
    _mainWindow.reset(new QMainWindow());
    _userInterface.setupUi(_mainWindow.get());

    QGLFormat glFormat;
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

    //tessellation
    connect(_userInterface.ckTessellation, SIGNAL(toggled(bool)), this, SLOT(toggleTessellation(bool)));
    connect(_userInterface.sInnerLevel, SIGNAL(valueChanged(int)), this, SLOT(setInnerLevel(int)));
    connect(_userInterface.sOuterLevel, SIGNAL(valueChanged(int)), this, SLOT(setOuterLevel(int)));
    connect(_userInterface.actionTessellation, SIGNAL(toggled(bool)), this, SLOT(toggleTessellation(bool)));
}

void Mediator::initUserInterface()
{
    defaultValues();
    showShaders(false);
}

void Mediator::showShaders(bool value)
{
    _userInterface.widgetTools->setVisible(value);
}

void Mediator::defaultValues()
{
    //tessellation
    _userInterface.fTessellation->setEnabled(false);
    _userInterface.sInnerLevel->setValue(1);
    _userInterface.sOuterLevel->setValue(3);
}

void Mediator::toggleTessellation(bool value)
{
    if (value && !_userInterface.ckTessellation->isChecked())
        _userInterface.ckTessellation->setChecked(value);

    _userInterface.fTessellation->setEnabled(value);
    _sceneViewer->toggleTessellation(value);
}

void Mediator::setInnerLevel(int level)
{
    _userInterface.eInnerLevel->setText(QString::number(level));
}

void Mediator::setOuterLevel(int level)
{
    _userInterface.eOuterLevel->setText(QString::number(level));
}

void Mediator::saveSnapshot()
{
    QDir savesFolder("data/saves/");
    uint id = savesFolder.entryList().size()-1;
    _sceneViewer->saveSnapshot(QString("data/saves/image%1.jpg").arg(id));
}

void Mediator::about()
{
    QMessageBox msgBox;
    msgBox.setText("This project has been produced as part of Posture-Vision project at the Société des arts technologiques of Montréal.\n\nCopyright (C) 2015 Bruno Roy\n\nThis program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.\n\nThis program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.\n\nYou should have received a copy of the GNU General Public License\nalong with this program; if not, write to the Free Software\nFoundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA");
    msgBox.exec();
}
