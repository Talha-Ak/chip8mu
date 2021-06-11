#pragma once

#include <QtWidgets/QMainWindow>

#include "EmuWrapper.h"
#include "ui_MainWindow.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = Q_NULLPTR);

protected:
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);

private:
    Ui::MainWindowClass ui;

    EmuWrapper emu;

public slots:
    void menuOpenROM();
    void menuResetEmu();
    void showScreen(QImage const&);
    void closeEvent(QCloseEvent*);

signals:
    void inputReceived(const int, bool);
    void runFile(std::string const&);
    void resetEmu();
};
