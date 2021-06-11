#pragma once

#include <QObject>

#include <array>
#include <chrono>
#include <cstdint>
#include <ctime>
#include <random>
#include <stack>
#include <string>

class Chip8
{
public:

	static constexpr int DISPLAY_WIDTH{ 64 };
	static constexpr int DISPLAY_HEIGHT{ 32 };
	static constexpr std::uint8_t KEY_COUNT{ 16 };	// Number of input keys
	static constexpr std::size_t MEMORY_SIZE{ 4096 };

	using keypad_type = std::array<std::uint8_t, KEY_COUNT>;
	using memory_type = std::array<std::uint8_t, MEMORY_SIZE>;
	using display_type = std::array<std::uint32_t, DISPLAY_WIDTH * DISPLAY_HEIGHT>;

	Chip8();
	bool loadRom(const std::string& filename);
	bool cycle();

	keypad_type& getKeypad()
	{
		return keypad;
	}

	display_type& getDisplay()
	{
		return display;
	}

private:
	static constexpr std::size_t MEM_START{ 0x200 };		// Starting point for ROM memory
	static constexpr std::size_t FONTCHARS_LENGTH{ 80 };	// Each char 5 bytes, 5 * 16 chars = 80 bytes
	static constexpr std::uint8_t FONTCHAR_START{ 0x50 };	// Starting point for font memory
	static constexpr int DELAY_TIMER_HZ{ 60 };

	static constexpr std::uint16_t BITMASK_X{ 0x0F00 };
	static constexpr std::uint16_t BITMASK_Y{ 0x00F0 };
	static constexpr std::uint16_t BITMASK_N{ 0x000F };
	static constexpr std::uint16_t BITMASK_NN{ 0x00FF };
	static constexpr std::uint16_t BITMASK_NNN{ 0x0FFF };

	std::mt19937 rngEngine{ static_cast<std::mt19937::result_type>(std::time(nullptr)) };
	std::uniform_int_distribution<> intRng{ 0, 0xFF };

	memory_type memory{};						// 4kB 8-bit main memory
	std::array<std::uint8_t, 16> registers{};	// 16 8-bit registers

	std::uint16_t ir{};						// 16-bit index register
	std::uint16_t pc{};						// 16-bit program counter
	std::uint16_t opcode{};					// 16-bit opcode	

	std::stack<std::uint16_t> stack{};		// 16-bit address stack

	std::chrono::steady_clock::time_point delayLastTick{ std::chrono::high_resolution_clock::now() };
	std::uint8_t delayTimer{};				// 8-bit delay timer
	std::uint8_t soundTimer{};				// 8-bit sound timer

	keypad_type keypad{};					// Input keypad (Hex 0-F)

	display_type display{};					// 64px * 32px display

	std::array<std::uint8_t, FONTCHARS_LENGTH> fontChars
	{
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

	bool altJumpOffset{ true };	// Alt inst. flag for BNNN
	bool altShrShl{ false };	// Alt inst. flag for 8XY6, 8XYE
	bool altLoadStore{ false };	// Alt inst. flag for FX55, FX65

	void reset();
	std::uint16_t fetch();

	void opcode_00E0();
	void opcode_00EE();
	void opcode_1NNN();
	void opcode_2NNN();
	void opcode_3XNN();
	void opcode_4XNN();
	void opcode_5XY0();
	void opcode_6XNN();
	void opcode_7XNN();
	void opcode_8XY0();
	void opcode_8XY1();
	void opcode_8XY2();
	void opcode_8XY3();
	void opcode_8XY4();
	void opcode_8XY5();
	void opcode_8XY6();
	void opcode_8XY7();
	void opcode_8XYE();
	void opcode_9XY0();
	void opcode_ANNN();
	void opcode_BNNN();
	void opcode_CXNN();
	void opcode_DXYN();
	void opcode_EX9E();
	void opcode_EXA1();
	void opcode_FX07();
	void opcode_FX0A();
	void opcode_FX15();
	void opcode_FX18();
	void opcode_FX1E();
	void opcode_FX29();
	void opcode_FX33();
	void opcode_FX55();
	void opcode_FX65();
};
