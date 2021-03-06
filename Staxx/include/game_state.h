#pragma once

#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <SDL2/SDL.h>

#include <string>
#include <map>

/*
 * Base class for each game state. A new window gets created for each game state, with its own renderer.
 * I might change it in the future so that we use the same window and renderer for each state.
 */

class game_state
{
public:
	game_state(std::string window_title, int window_x, int window_y, int window_width, int window_height);

	virtual ~game_state();

	virtual int mainloop() = 0;

protected:
	SDL_Window * window;
	SDL_Renderer * renderer;

	std::map<std::string, SDL_Texture *> textures;

	virtual void draw() = 0;
	virtual void update() = 0;

	SDL_Renderer * init_renderer();

	SDL_Event sdl_event;

	bool quit = false;
};

#endif