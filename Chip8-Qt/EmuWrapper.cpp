#include "EmuWrapper.h"
#include <QImage>

EmuWrapper::EmuWrapper()
{
	originalScreen = QImage(Chip8::DISPLAY_WIDTH, Chip8::DISPLAY_HEIGHT, QImage::Format_RGBA8888);
}

void EmuWrapper::run()
{
	constexpr int INSTRUCTIONS_PER_SECOND{ 400 };
	const int cyclesPerFrame{ INSTRUCTIONS_PER_SECOND / 60 };

	restartEmu();

	//auto t_start = std::chrono::high_resolution_clock::now();

	while (true)
	{
		// Using chrono causes high CPU usage for some reason...
		// Sticking to sleeping thread every 16.67ms. Not entirely accurate but more performant.	
		//auto t_end = std::chrono::high_resolution_clock::now();
		//double elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end - t_start).count();

		for (int i{ 0 }; i < cyclesPerFrame; i++)
		{
			emu.cycle();
		}
		showFramebuffer();
		usleep(1000.0/60.0);
	}
}

void EmuWrapper::showFramebuffer()
{
	Chip8::display_type display{ emu.getDisplay() };
	auto displayChar{ reinterpret_cast<uchar*>(display.data())};
	originalScreen = QImage(displayChar, Chip8::DISPLAY_WIDTH, Chip8::DISPLAY_HEIGHT, QImage::Format_RGBA8888);

	transformedScreen = originalScreen.scaled(640, 320);
	emit screenUpdated(transformedScreen);
}

void EmuWrapper::handleInput(const int key, bool pressed)
{
	Chip8::keypad_type& keypad{ emu.getKeypad() };

	switch (key)
	{
	case Qt::Key_1: keypad[0x1] = pressed; break;
	case Qt::Key_2: keypad[0x2] = pressed; break;
	case Qt::Key_3: keypad[0x3] = pressed; break;
	case Qt::Key_4: keypad[0xC] = pressed; break;
	case Qt::Key_Q: keypad[0x4] = pressed; break;
	case Qt::Key_W: keypad[0x5] = pressed; break;
	case Qt::Key_E: keypad[0x6] = pressed; break;
	case Qt::Key_R: keypad[0xD] = pressed; break;
	case Qt::Key_A: keypad[0x7] = pressed; break;
	case Qt::Key_S: keypad[0x8] = pressed; break;
	case Qt::Key_D: keypad[0x9] = pressed; break;
	case Qt::Key_F: keypad[0xE] = pressed; break;
	case Qt::Key_Z: keypad[0xA] = pressed; break;
	case Qt::Key_X: keypad[0x0] = pressed; break;
	case Qt::Key_C: keypad[0xB] = pressed; break;
	case Qt::Key_V: keypad[0xF] = pressed; break;
	}
}

void EmuWrapper::openFile(const std::string& filename)
{
	if (!filename.empty())
	{
		romFile = filename;
		restartEmu();
	}
}

void EmuWrapper::restartEmu()
{
	emu.loadRom(romFile);
	showFramebuffer();
}
