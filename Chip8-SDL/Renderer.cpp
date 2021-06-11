// Some rendering code from https://austinmorlan.com/posts/chip8_emulator/#the-platform-layer

#include "Chip8.h"
#include "Renderer.h"
#include <SDL.h>

#include <any>
#include <cstdint>
#include <string>

Renderer::Renderer(const std::string title, int textureWidth, int textureHeight, int videoScale)
{
	SDL_Init(SDL_INIT_VIDEO);

	m_window = SDL_CreateWindow
	(
		title.c_str(),
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		textureWidth * videoScale,
		textureHeight * videoScale,
		SDL_WINDOW_SHOWN
	);
	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
	m_texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, textureWidth, textureHeight);
}

Renderer::~Renderer()
{
	SDL_DestroyTexture(m_texture);
	m_texture = nullptr;

	SDL_DestroyRenderer(m_renderer);
	m_renderer = nullptr;

	SDL_DestroyWindow(m_window);
	m_window = nullptr;

	SDL_Quit();
}

void Renderer::update(const Chip8::display_type& video, int pitch)
{
	SDL_UpdateTexture(m_texture, nullptr, video.data(), pitch);
	SDL_RenderClear(m_renderer);
	SDL_RenderCopy(m_renderer, m_texture, nullptr, nullptr);
	SDL_RenderPresent(m_renderer);
}

bool Renderer::processInput(Chip8::keypad_type& keys)
{
	bool quit = false;

	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
		case SDL_QUIT:
			quit = true;
			break;

		case SDL_KEYDOWN:
		{
			switch (e.key.keysym.sym)
			{
			case SDLK_ESCAPE:	quit = true;	break;
			case SDLK_1:		keys[0x1] = 1;	break;
			case SDLK_2:		keys[0x2] = 1;	break;
			case SDLK_3:		keys[0x3] = 1;	break;
			case SDLK_4:		keys[0xC] = 1;	break;
			case SDLK_q:		keys[0x4] = 1;	break;
			case SDLK_w:		keys[0x5] = 1;	break;
			case SDLK_e:		keys[0x6] = 1;	break;
			case SDLK_r:		keys[0xD] = 1;	break;
			case SDLK_a:		keys[0x7] = 1;	break;
			case SDLK_s:		keys[0x8] = 1;	break;
			case SDLK_d:		keys[0x9] = 1;	break;
			case SDLK_f:		keys[0xE] = 1;	break;
			case SDLK_z:		keys[0xA] = 1;	break;
			case SDLK_x:		keys[0x0] = 1;	break;
			case SDLK_c:		keys[0xB] = 1;	break;
			case SDLK_v:		keys[0xF] = 1;	break;
			}
		}
		break;

		case SDL_KEYUP:
		{
			switch (e.key.keysym.sym)
			{
			case SDLK_1:	keys[0x1] = 0;	break;
			case SDLK_2:	keys[0x2] = 0;	break;
			case SDLK_3:	keys[0x3] = 0;	break;
			case SDLK_4:	keys[0xC] = 0;	break;
			case SDLK_q:	keys[0x4] = 0;	break;
			case SDLK_w:	keys[0x5] = 0;	break;
			case SDLK_e:	keys[0x6] = 0;	break;
			case SDLK_r:	keys[0xD] = 0;	break;
			case SDLK_a:	keys[0x7] = 0;	break;
			case SDLK_s:	keys[0x8] = 0;	break;
			case SDLK_d:	keys[0x9] = 0;	break;
			case SDLK_f:	keys[0xE] = 0;	break;
			case SDLK_z:	keys[0xA] = 0;	break;
			case SDLK_x:	keys[0x0] = 0;	break;
			case SDLK_c:	keys[0xB] = 0;	break;
			case SDLK_v:	keys[0xF] = 0;	break;
			}
		}
		break;
		}
	}

	return quit;
}
