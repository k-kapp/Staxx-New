#pragma once

#include "game_state.h"
#include "clickable_tile.h"
#include "scroll_grid.h"
#include "button.h"
#include <vector>

/*
 * Game state for the design selection. Each shape is a clickble_tile object with a texture showing each
 * shape. Selected shapes are then used in the next game.
 */

class design_selection : game_state
{
public:
	design_selection(int grid_x, int grid_y);

	void mainloop() override;
private:

	void update() override;
	void draw() override;

	void init_selection();

	void cancel();
	void save_selection();
	void delete_first_click();

private:
	
	std::vector<shared_ptr<clickable_tile> > selectors;

	scroll_grid<clickable_tile> scroller;

	SDL_Texture * test_texture;

	SDL_Event * pass_event;

	button save_button, cancel_button, delete_button;

	std::vector<SDL_Texture *> confirm_text_textures;

	std::vector<std::pair<SDL_Rect, SDL_Texture *> > confirm_text_rects_textures;

	bool draw_confirm_delete_text = false;
};