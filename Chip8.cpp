#include "Chip8.h"

#include <stdexcept>
#include <fstream>
#include <iostream>
#include <vector>

Chip8::Chip8()
{
	reset();
}

void Chip8::reset()
{
	pc = MEM_START;
	for (int i = 0; i < fontChars.size(); i++)
	{
		memory[FONTCHAR_START + i] = fontChars[i];
	}
}

bool Chip8::loadRom(const std::string& filename)
{
	std::ifstream romFile{ filename, std::ios::binary};
	if (!romFile)
	{
		std::cout << "File not found.\n";
		return false;
	}

	// From https://stackoverflow.com/a/5420568
	int i = 0;
	for (unsigned char c : std::vector<std::uint8_t>(std::istreambuf_iterator<char>(romFile), {}))
	{
		memory[MEM_START + i] = c;
		++i;
	}
	std::cout << std::hex;

	for (int j = 0; j < memory.size(); ++j)
	{
		if (j == 0x200) std::cout << "\n\n";
		std::cout << static_cast<int>(memory[j]) << '\t';
		if (j % 15 == 0 && j != 0) std::cout << '\n';
	}
	return true;
}

std::uint16_t Chip8::fetch()
{
	std::uint8_t byteOne{ memory[pc] };
	std::uint8_t byteTwo{ memory[pc + 1] };
	pc += 2;

	return (byteOne << 8) | byteTwo;
}

void Chip8::cycle()
{
	auto currentTime = std::chrono::high_resolution_clock::now();
	double elapsedTime = std::chrono::duration<double, std::milli>(currentTime - delayLastTick).count();

	if (elapsedTime > (1 / DELAY_TIMER_HZ))
	{
		if (delayTimer > 0) --delayTimer;
		if (soundTimer > 0) --soundTimer;
	}

	opcode = fetch();

	int nibOne	{ (opcode & 0xF000) >> 12};
	int nibTwo	{ (opcode & 0x0F00) >> 8};
	int nibThree{ (opcode & 0x00F0) >> 4};
	int nibFour	{ opcode & 0x000F };

	switch (nibOne)
	{
	case 0x0:
		if (nibTwo == 0x0 && nibThree == 0xE)
		{
			if (nibFour == 0) opcode_00E0();
			else opcode_00EE();
		}
		break;
	case 0x1:
		opcode_1NNN();
		break;
	case 0x2:
		opcode_2NNN();
		break;
	case 0x3:
		opcode_3XNN();
		break;
	case 0x4:
		opcode_4XNN();
		break;
	case 0x5:
		opcode_5XY0();
		break;
	case 0x6:
		opcode_6XNN();
		break;
	case 0x7:
		opcode_7XNN();
		break;
	case 0x8:
		switch (nibFour)
		{
		case 0x0:
			opcode_8XY0();
			break;
		case 0x1:
			opcode_8XY1();
			break;
		case 0x2:
			opcode_8XY2();
			break;
		case 0x3:
			opcode_8XY3();
			break;
		case 0x4:
			opcode_8XY4();
			break;
		case 0x5:
			opcode_8XY5();
			break;
		case 0x6:
			opcode_8XY6();
			break;
		case 0x7:
			opcode_8XY7();
			break;
		case 0xE:
			opcode_8XYE();
			break;
		}
		break;
	case 0x9:
		opcode_9XY0();
		break;
	case 0xA:
		opcode_ANNN();
		break;
	case 0xB:
		opcode_BNNN();
		break;
	case 0xC:
		opcode_CXNN();
		break;
	case 0xD:
		opcode_DXYN();
		break;
	case 0xE:
		if (nibThree == 0x9 && nibFour == 0xE) opcode_EX9E();
		else opcode_EXA1();
		break;
	case 0xF:
		switch ((nibThree << 4) | nibFour)
		{
		case 0x07:
			opcode_FX07();
			break;
		case 0x0A:
			opcode_FX0A();
			break;	
		case 0x15:
			opcode_FX15();
			break;
		case 0x18:
			opcode_FX18();
			break;
		case 0x1E:
			opcode_FX1E();
			break;
		case 0x29:
			opcode_FX29();
			break;
		case 0x33:
			opcode_FX33();
			break;
		case 0x55:
			opcode_FX55();
			break;
		case 0x65:
			opcode_FX65();
			break;
		}
		break;
	default:
		std::cout << "\nMissing opcode instruction: 0x" << opcode << '\n';
	}

}

// 00E0 - Clear display
void Chip8::opcode_00E0()
{
	display.fill(0);
}

// 00EE - Return to last address in stack
void Chip8::opcode_00EE()
{
	pc = stack.top();
	stack.pop();
}

// 1NNN - Jump
void Chip8::opcode_1NNN()
{
	pc = (opcode & BITMASK_NNN);
}

// 2NNN - Add to stack & Jump
void Chip8::opcode_2NNN()
{
	stack.push(pc);
	pc = (opcode & BITMASK_NNN);
}

// 3XNN - Skip if reg X == NN
void Chip8::opcode_3XNN()
{
	int regVal{ registers[(opcode & BITMASK_X) >> 8] };
	if (regVal == (opcode & BITMASK_NN)) pc += 2;
}

// 4XNN - Skip if reg X != NN
void Chip8::opcode_4XNN()
{
	int regVal{ registers[(opcode & BITMASK_X) >> 8] };
	if (regVal != (opcode & BITMASK_NN)) pc += 2;
}

// 5XY0 - Skip if reg X == reg Y
void Chip8::opcode_5XY0()
{
	int regValX{ registers[(opcode & BITMASK_X) >> 8] };
	int regValY{ registers[(opcode & BITMASK_Y) >> 4] };
	if (regValX == regValY) pc += 2;
}

// 6XNN - Set reg X to NN
void Chip8::opcode_6XNN()
{
	registers[(opcode & BITMASK_X) >> 8] = (opcode & BITMASK_NN);
}

// 7XNN - Add NN to reg X
void Chip8::opcode_7XNN()
{
	registers[(opcode & BITMASK_X) >> 8] += (opcode & BITMASK_NN);
}


// 8XY0 - Set reg X to reg Y
void Chip8::opcode_8XY0()
{
	registers[(opcode & BITMASK_X) >> 8] = registers[(opcode & BITMASK_Y) >> 4];
}

// 8XY1 - Logical OR
void Chip8::opcode_8XY1()
{
	registers[(opcode & BITMASK_X) >> 8] |= registers[(opcode & BITMASK_Y) >> 4];
}

// 8XY2 - Logical AND
void Chip8::opcode_8XY2()
{
	registers[(opcode & BITMASK_X) >> 8] &= registers[(opcode & BITMASK_Y) >> 4];
}

// 8XY3 - Logical XOR
void Chip8::opcode_8XY3()
{
	registers[(opcode & BITMASK_X) >> 8] ^= registers[(opcode & BITMASK_Y) >> 4];
}

// 8XY4 - Add
void Chip8::opcode_8XY4()
{
	int result{ registers[(opcode & BITMASK_X) >> 8] + registers[(opcode & BITMASK_Y) >> 4] };
	registers[0xF] = (result > 255);
	registers[(opcode & BITMASK_X) >> 8] = static_cast<std::uint8_t>(result);
}


// 8XY5 - X subtract Y
void Chip8::opcode_8XY5()
{
	int regX{ (opcode & BITMASK_X) >> 8 };
	int regY{ (opcode & BITMASK_Y) >> 4 };
	registers[0xF] = (registers[regX] > registers[regY]);
	registers[regX] -= registers[regY];
}

// 8XY6 - Shift Right
void Chip8::opcode_8XY6()
{
	int regX{ (opcode & BITMASK_X) >> 8 };

	if (altShrShl)
	{
		registers[regX] = registers[(opcode & BITMASK_Y) >> 4];
	}

	registers[0xF] = registers[regX] & 0x1;
	registers[regX] >>= 1;
}

// 8XY7 - Y subtract X
void Chip8::opcode_8XY7()
{
	int regX{ (opcode & BITMASK_X) >> 8 };
	int regY{ (opcode & BITMASK_Y) >> 4 };
	registers[0xF] = (registers[regY] > registers[regX]);
	registers[regY] -= registers[regX];
}

// 8XYE - Shift Left
void Chip8::opcode_8XYE()
{
	int regX{ (opcode & BITMASK_X) >> 8 };

	if (altShrShl)
	{
		registers[regX] = registers[(opcode & BITMASK_Y) >> 4];
	}

	registers[0xF] = (registers[regX] & 0x80) >> 7;
	registers[regX] <<= 1;
}


// 9XY0 = Skip if reg X != reg Y
void Chip8::opcode_9XY0()
{
	int regValX{ registers[(opcode & BITMASK_X) >> 8] };
	int regValY{ registers[(opcode & BITMASK_Y) >> 4] };
	if (regValX != regValY) pc += 2;
}

// ANNN - Set index reg to NNN
void Chip8::opcode_ANNN()
{
	ir = (opcode & BITMASK_NNN);
}

// BNNN - Jump with offset
void Chip8::opcode_BNNN()
{
	if (altJumpOffset)
	{
		pc = (opcode & BITMASK_NNN) + registers[(opcode & BITMASK_X) >> 8];
	}
	else
	{
		pc = (opcode & BITMASK_NNN) + registers[0];
	}
}

// CXNN - Generate random number
void Chip8::opcode_CXNN()
{
	registers[(opcode & BITMASK_X) >> 8] = (opcode & BITMASK_NN) & intRng(rngEngine);
}

// DXYN - Display to screen
void Chip8::opcode_DXYN()
{
	int xCoord{ registers[(opcode & BITMASK_X) >> 8] % DISPLAY_WIDTH };
	int yCoord{ registers[(opcode & BITMASK_Y) >> 4] % DISPLAY_HEIGHT };
	registers[0xF] = 0;

	for (int row{ 0 }; row < (opcode & BITMASK_N); ++row)
	{
		std::uint8_t spriteData{ memory[ir + row] };

		for (int col{ 0 }; col < 8; ++col)
		{
			if (spriteData & (0x80 >> col)) // Check each pixel (bit) and see if on (1)
			{
				// Get specific display pixel for this sprite pixel
				int displayIndex{ ((yCoord + row) * DISPLAY_WIDTH) + (xCoord + col) };

				if (display[displayIndex] == 0xFFFFFFFF)
				{
					registers[0xF] = 1;
				}

				display[displayIndex] ^= 0xFFFFFFFF;
			}
		}
	}
}

// EX9E - Skip on key press
void Chip8::opcode_EX9E()
{
	if (keypad[registers[(opcode & BITMASK_X) >> 8]])
	{
		pc += 2;
	}
}

// EXA1 - Skip on no key press
void Chip8::opcode_EXA1()
{
	if (!keypad[registers[(opcode & BITMASK_X) >> 8]])
	{
		pc += 2;
	}
}

// FX07 - Get delay timer
void Chip8::opcode_FX07()
{
	registers[(opcode & BITMASK_X) >> 8] = delayTimer;
}

// FX0A - Get key input
void Chip8::opcode_FX0A()
{
	auto keyPressed{ std::find(keypad.begin(), keypad.end(), 1) };
	if (keyPressed == keypad.end())
	{
		pc -= 2;
	}
	else
	{
		registers[(opcode & BITMASK_X) >> 8] = static_cast<std::uint8_t>(std::distance(keypad.begin(), keyPressed));
	}
}

// FX15 - Set delay timer
void Chip8::opcode_FX15()
{
	delayTimer = registers[(opcode & BITMASK_X) >> 8];
}

// FX18 - Set sound timer
void Chip8::opcode_FX18()
{
	soundTimer = registers[(opcode & BITMASK_X) >> 8];
}

// FX1E - Add to index
void Chip8::opcode_FX1E()
{
	int result{ ir + registers[(opcode & BITMASK_X) >> 8] };
	registers[0xF] = (result > 0xFFF);

	ir = static_cast<std::uint16_t>(result);
}

// FX29 - Get font char
void Chip8::opcode_FX29()
{
	std::uint8_t fontChar = registers[(opcode & BITMASK_X) >> 8];
	ir = FONTCHAR_START + (5 * fontChar);
}

// FX33 - Bin to Dec conversion
void Chip8::opcode_FX33()
{
	int number = registers[(opcode & BITMASK_X) >> 8];

	memory[ir + 2] = number % 10;
	number /= 10;

	memory[ir + 1] = number % 10;
	number /= 10;

	memory[ir] = number % 10;
}

// FX55 - Store mem
void Chip8::opcode_FX55()
{
	int regX{ (opcode & BITMASK_X) >> 8 };

	for (int i{ 0 }; i <= regX; ++i)
	{
		memory[ir + i] = registers[i];
	}

}

// FX65 - Load mem
void Chip8::opcode_FX65()
{
	int regX{ (opcode & BITMASK_X) >> 8 };

	for (int i{ 0 }; i <= regX; ++i)
	{
		registers[i] = memory[ir + i];
	}
}