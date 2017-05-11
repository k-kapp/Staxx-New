#pragma once

#include "game_state.h"
#include "clickable_tile.h"
#include "scroll_grid.h"
#include "button.h"
#include <vector>

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

private:
	
	std::vector<shared_ptr<clickable_tile> > selectors;

	scroll_grid<clickable_tile> scroller;

	SDL_Texture * test_texture;

	SDL_Event * pass_event;

	button save_button, cancel_button;

};