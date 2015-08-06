#include "mediator.h"
#include <QMessageBox>
#include <QFileDialog>

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
    connect(_userInterface.actionImportModel, SIGNAL(triggered()), this, SLOT(importModel()));
    connect(_userInterface.actionImportAnimation, SIGNAL(triggered()), this, SLOT(importAnimation()));
    connect(_userInterface.actionSnapshot, SIGNAL(triggered()), this, SLOT(saveSnapshot()));
    connect(_userInterface.actionQuit, SIGNAL(triggered()), _mainWindow.get(), SLOT(close()));

    //help
    connect(_userInterface.actionAbout, SIGNAL(triggered()), this, SLOT(about()));

    //tools
    connect(_userInterface.actionShaders, SIGNAL(triggered(bool)), this, SLOT(showShaders(bool)));
    connect(_userInterface.actionDefaultValues, SIGNAL(triggered()), this, SLOT(defaultValues()));
    connect(_userInterface.actionPlayer, SIGNAL(triggered(bool)), this, SLOT(showPlayer(bool)));

    //tessellation
    connect(_userInterface.ckTessellation, SIGNAL(toggled(bool)), this, SLOT(toggleTessellation(bool)));
    connect(_userInterface.sInnerLevel, SIGNAL(valueChanged(int)), this, SLOT(setInnerLevel(int)));
    connect(_userInterface.sOuterLevel, SIGNAL(valueChanged(int)), this, SLOT(setOuterLevel(int)));
    connect(_userInterface.actionTessellation, SIGNAL(toggled(bool)), this, SLOT(toggleTessellation(bool)));

    //player
    connect(_userInterface.sFrames, SIGNAL(valueChanged(int)), this, SLOT(changeCurrentFrame(int)));
    connect(_userInterface.bPlayPause, SIGNAL(pressed()), this, SLOT(playPause()));
}

void Mediator::initUserInterface()
{
    defaultValues();
    showShaders(false);

    _userInterface.actionPlayer->setEnabled(false);
    showPlayer(false);

    _userInterface.progressBar->setVisible(false);
}

void Mediator::showShaders(bool value)
{
    _userInterface.widgetTools->setVisible(value);
}

void Mediator::showPlayer(bool value)
{
    _userInterface.widgetPlayer->setVisible(value);
}

void Mediator::defaultValues()
{
    //tessellation
    _userInterface.fTessellation->setEnabled(false);
    _userInterface.sInnerLevel->setValue(2);
    _userInterface.sOuterLevel->setValue(1);
}

void Mediator::toggleTessellation(bool value)
{
    if ((value && !_userInterface.ckTessellation->isChecked()) ||
        (!value && _userInterface.ckTessellation->isChecked()))
        _userInterface.ckTessellation->setChecked(value);

    _userInterface.fTessellation->setEnabled(value);
    _sceneViewer->setInnerTL(_userInterface.eInnerLevel->text().toInt());
    _sceneViewer->setOuterTL(_userInterface.eOuterLevel->text().toInt());
    _sceneViewer->toggleTessellation(value);
}

void Mediator::setInnerLevel(int level)
{
    _userInterface.eInnerLevel->setText(QString::number(level));
    if (_sceneViewer->isTessellated())
        _sceneViewer->setInnerTL(level);
}

void Mediator::setOuterLevel(int level)
{
    _userInterface.eOuterLevel->setText(QString::number(level));
    if (_sceneViewer->isTessellated())
        _sceneViewer->setOuterTL(level);
}

void Mediator::changeCurrentFrame(int currentFrame)
{
    QString currentFrameStr(std::to_string(static_cast<ll>(currentFrame)).c_str());
    _sceneViewer->setCurrentFrame((currentFrame));
    _userInterface.eCurrentFrame->setText(currentFrameStr);
}

void Mediator::importModel()
{
    QFileDialog dialog;
    if (dialog.exec())
    {
        QStringList files = dialog.selectedFiles();
        if (!files.isEmpty())
        {
            std::string path(std::string(files.at(0).toStdString()));
            _sceneViewer->loadModel(path);
        }
    }
}

void Mediator::importAnimation()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly);

    if (dialog.exec())
    {
        QDir folder(dialog.selectedFiles().at(0));
        QFileInfoList files = folder.entryInfoList(QStringList() << "*.ply");
        if (!files.isEmpty())
        {
            std::string path(std::string(files.at(0).filePath().toStdString()));
            _userInterface.progressBar->setVisible(true);
            _sceneViewer->loadAnimation(path, files.size());
            _userInterface.progressBar->setVisible(false);
            _userInterface.actionPlayer->setEnabled(true);
        }
    }
}

void Mediator::playPause()
{
    _sceneViewer->playPause();
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
