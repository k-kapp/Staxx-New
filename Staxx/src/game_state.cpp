#include "../include/game_state.h"
#include "../include/common_surfaces.h"

#include <string>
#include <iostream>

using namespace std;

game_state::game_state(string window_title, int window_x, int window_y, int window_width, int window_height)
	: window(SDL_CreateWindow(window_title.c_str(), window_x, window_y, window_width, window_height, 0)),
	renderer(init_renderer()), textures(all_surfaces.create_texture_map(renderer))
{
	cout << "Window address: " << window << endl;
	cout << "Renderer address: " << renderer << endl;
}

game_state::~game_state()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}

SDL_Renderer * game_state::init_renderer()
{
	SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	return renderer;
}