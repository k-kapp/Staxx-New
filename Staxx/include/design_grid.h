#ifndef DESIGN_GRID_H
#define DESIGN_GRID_H

#include "grid.h"
#include "misc_types.h"
#include "game_state.h"
#include "button.h"
#include <SDL2/SDL.h>
#include <utility>
#include <functional>
#include <deque>

extern const int BLOCK_SIZE;

/*
void a_func(int a1, int a2)
{
	cout << "Function called" << endl;
	cout << "args: " << a1 << ", " << a2 << endl;
}

void func2()
{}
*/

using bool_2d = deque< deque<bool> >;

class design_grid : public game_state
{
public:
	design_grid(int grid_size, tile_colors colors, tile_colors mouse_on_colors, tile_colors on_colors,
				int x_offset, int y_offset, int block_size,
				int border_size);
	design_grid(int grid_size, SDL_Texture * off_texture, SDL_Texture * hover_texture,
		SDL_Texture * on_texture, int x_offset, int y_offset, int block_size, int border_size);
	//~design_grid();

	void mainloop();
	void get_mouse_in_grid();
	void set_tile_colors();
	void make_buttons();
	void save_design();
	void quit_design();
	bool design_valid();
	bool design_exists(const shape &new_design);
	bool_2d get_bool_arr();

	void set_new_on_color(SDL_Texture * new_color);
	void set_new_on_color(tile_colors colors);
	void set_new_on_color(std::string color);

	//void make_save_button();
	//void draw_save_button();
	//bool mouse_on_save_button();

protected:
	void draw() override;
	void update() override;

private:
	tile_colors button_off_colors;
	tile_colors button_hover_colors;
	tile_colors button_on_colors;
	SDL_Event event;
	int mouse_x, mouse_y;
	int occ_x, occ_y;
	pair<int, int> mouse_pos;
	int grid_size;
	bool quit;
	int active_count;

	//static constexpr auto bound_func = bind(a_func, init1, init2);

	SDL_Rect save_rect_inner, save_rect_outer;
	//button<decltype(function<void()>(bind(a_func, 1, 1)))> save_button;
	//button<decltype(function<void()>(bind(a_func, 1, 1)))> discard_button;
	//button<void(*)()> save_button;
	//button<void(*)()> discard_button;
	std::unique_ptr<button> save_button;
	std::unique_ptr<button> quit_button;

	string next_design_string();
	unsigned count_active();

	grid<clickable_tile> tiles;

	unsigned num_rows, num_cols;

	SDL_Texture * off_texture, *hover_texture, *on_texture;
};

#endif
