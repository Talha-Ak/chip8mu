#include "MainWindow.h"
#include <QKeyEvent>
#include <QDebug>
#include <QFileDialog>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    connect(&emu, SIGNAL(screenUpdated(QImage const&)), this, SLOT(showScreen(QImage const&)));
    connect(ui.actionOpen_ROM, SIGNAL(triggered()), this, SLOT(menuOpenROM()));
    connect(ui.actionReset_Emulator, SIGNAL(triggered()), this, SLOT(menuResetEmu()));
    connect(this, SIGNAL(inputReceived(const int, bool)), &emu, SLOT(handleInput(const int, bool)));
    connect(this, SIGNAL(runFile(std::string const&)), &emu, SLOT(openFile(std::string const&)));
    connect(this, SIGNAL(resetEmu()), &emu, SLOT(restartEmu()));
}

void MainWindow::menuOpenROM()
{
    auto fileName{ QFileDialog::getOpenFileName(this, "Open Chip8 ROM file") };
    emit(runFile(fileName.toStdString()));

    if (!emu.isRunning() && !fileName.isNull()) emu.start();
}

void MainWindow::menuResetEmu()
{
    emit(resetEmu());
}

void MainWindow::showScreen(QImage const& image)
{
    ui.label->setPixmap(QPixmap::fromImage(image));
}

void MainWindow::closeEvent(QCloseEvent*)
{
    emu.terminate();
    emu.wait();
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    bool repeat{ event->isAutoRepeat() };
    if (repeat)
    {
        event->ignore();
    }
    else
    {
        emit inputReceived(event->key(), true);
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent* event)
{
    bool repeat{ event->isAutoRepeat() };
    if (repeat)
    {
        event->ignore();
    }
    else
    {
        emit inputReceived(event->key(), false);
    }
}
