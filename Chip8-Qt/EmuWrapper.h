#pragma once

#include <QThread>
#include <QImage>

#include "Chip8.h"

class EmuWrapper : public QThread
{
	Q_OBJECT

public:
	EmuWrapper();

private:
	QImage originalScreen;
	QImage transformedScreen;
	Chip8 emu{};
	std::string romFile{};

private:
	void run();
	void showFramebuffer();

signals:
	void screenUpdated(QImage const&);
	void memoryUpdated(Chip8::memory_type const&);

public slots:
	void handleInput(const int, bool);
	void openFile(std::string const&);
	void restartEmu();
};

