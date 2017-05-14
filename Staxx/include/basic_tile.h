#pragma once

#include "misc_types.h"
#include <SDL2/SDL.h>
#include <utility>

class basic_tile
{
public:

	basic_tile(int x_coord, int y_coord, unsigned x_size, unsigned y_size, SDL_Texture * off_texture, SDL_Texture * on_texture,
				SDL_Renderer * renderer);
	//basic_tile(int x_coord, int y_coord, unsigned x_size, unsigned y_size, tile_colors back_colors, tile_colors fore_colors, SDL_Renderer * renderer);
	basic_tile(const basic_tile &other);
	basic_tile & operator = (const basic_tile &other);

	virtual void draw(SDL_Texture * target = nullptr);
	virtual void update();

	void set_coords(int x_coord, int y_coord);
	std::pair<int, int> get_coords() const;
	virtual void set_size(unsigned width, unsigned height);
	std::pair< unsigned, unsigned > get_size();

	virtual void set_off_colors(tile_colors back_colors);
	virtual void set_on_colors(tile_colors fore_colors);

	virtual void set_off_texture(SDL_Texture * off_texture);
	virtual void set_on_texture(SDL_Texture * on_texture);

	tile_colors get_on_colors();
	tile_colors get_off_colors();

	int x_coord, y_coord;

	void set_activation_level(unsigned level);
	void increase_activation_level();
	void decrease_activation_level();

	unsigned get_activation_level();

	virtual void reset_state();
protected:

	SDL_Renderer * renderer;
	unsigned width, height;
	tile_colors off_colors;
	tile_colors on_colors;

	SDL_Texture * off_texture, *on_texture;

	void set_inner_outer();

	SDL_Texture * make_color_texture(tile_colors colors);

	SDL_Rect inner, outer;


	unsigned activation_level = 0;

};