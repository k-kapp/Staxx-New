#ifndef TILES_H
#define TILES_H

#include <SDL2/SDL.h>
#include <utility>
#include <memory>
#include <functional>
#include "misc_types.h"
#include "basic_tile.h"

using namespace std;

class clickable_tile;
class shape_phys;

class block;

/*
 * base class for all types of tiles that have some function when clicked. This includes
 * buttons and "drawing" tiles to design new shapes
*/

class clickable_tile : public basic_tile
{
public:
	clickable_tile(int x_coord, int y_coord, unsigned x_size, unsigned y_size, tile_colors back_colors, tile_colors fore_colors, tile_colors active_colors, 
		SDL_Renderer * renderer);
	clickable_tile(int x_coord, int y_coord, unsigned width, unsigned height, tile_colors back_colors, tile_colors active_colors, SDL_Renderer * renderer);
	clickable_tile(int x_coord, int y_coord, unsigned width, unsigned height, SDL_Texture * off_texture, SDL_Texture * on_texture,
					SDL_Renderer * renderer);
	clickable_tile(int x_coord, int y_coord, unsigned width, unsigned height, SDL_Texture * off_texture, SDL_Texture * hover_texture,
					SDL_Texture * on_texture, SDL_Renderer * renderer);
	clickable_tile(const clickable_tile &other);
	clickable_tile & operator = (const clickable_tile &other);

	virtual void draw(SDL_Texture * target = nullptr);
	virtual void update();

	void set_activate_on_release(bool setting);

	void set_hover_colors(tile_colors colors);
	void set_hover_texture(std::string colors);
	virtual void set_hover_texture(SDL_Texture * hover_texture);

	void set_sticky(bool setting);

	virtual void reset_state() override;

protected:
	SDL_Texture * hover_texture;   //NOTE: creator of clickable tile object (or its derivative classes) is 
											   // responsible for destroying off_texture and on_texture. i.e. clickable_tile
											   // and its derivative objects do not destroy off_texture and on_texture.
	SDL_Surface surface;

	tile_colors hover_colors;

	bool mouse_on_button();

	bool mousebutton_down = false;

	int mouse_x, mouse_y;

	bool activate_on_release = true;
	bool must_click_on_this = false;
	bool sticky = false;
};

#endif
