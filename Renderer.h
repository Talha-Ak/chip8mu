#pragma once

#include "Chip8.h"
#include <SDL.h>

#include <array>
#include <cstdint>
#include <string>

class Renderer
{
	SDL_Window* m_window{};
	SDL_Renderer* m_renderer{};
	SDL_Texture* m_texture{};

public:
	Renderer(const std::string title, int textureWidth, int textureHeight, int videoScale);
	~Renderer();
	void update(const Chip8::display_type& video, int pitch);
	bool processInput(Chip8::keypad_type& keys);
};