#include "Chip8.h"
#include "Renderer.h"

#include <chrono>
#include <iostream>
#include <memory>

void getRom(Chip8& chip8)
{
	std::string fileName{};
	do
	{
		std::cout << "\nEnter rom file name: ";
		std::cin >> fileName;
	} while (!chip8.loadRom(fileName));
}

int main(int argc, char* argv[])
{
	const static double INSTRUCTIONS_PER_SEC = 0.3;

	Renderer renderer{ "Chip8mu", Chip8::DISPLAY_WIDTH, Chip8::DISPLAY_HEIGHT, 10 };

	auto chip8{ std::make_unique<Chip8>() };
	if (argc > 1)
	{
		if (!chip8->loadRom(argv[1])) return 1;
	}
	else
	{
		getRom(*chip8);
	}
	

	int videoWidth{ sizeof(chip8->getDisplay()[0]) * Chip8::DISPLAY_WIDTH };

	auto t_start = std::chrono::high_resolution_clock::now();

	bool quit = false;
	while (!quit)
	{
		quit = renderer.processInput(chip8->getKeypad());

		auto t_end = std::chrono::high_resolution_clock::now();
		double elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end - t_start).count();

		if (elapsed_time_ms > (1 / INSTRUCTIONS_PER_SEC))
		{
			t_start = t_end;
			chip8->cycle();
			renderer.update(chip8->getDisplay(), videoWidth);
		}

	}

	return 0;
}
