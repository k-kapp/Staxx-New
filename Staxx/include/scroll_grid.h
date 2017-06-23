#pragma once

#include "grid.h"
#include "button.h"

#include <SDL2/SDL.h>

#include <vector>
#include <memory>
#include <cmath>
#include <iostream>
#include <map>

/*
 * Has similarities to the grid type, but here we can also display only a part of the grid on the screen,
 * then we scroll up and down to reveal other parts of the grid (like a normal scroller, more or less).
*/

template<typename T>
class scroll_grid
{
public:
	scroll_grid(unsigned view_cols, unsigned view_rows, int x_offset, int y_offset,
		unsigned padding_horiz, unsigned padding_vert, unsigned block_size, SDL_Texture * empty_texture, SDL_Renderer * renderer,
		unsigned scroll_buttons_size, std::map<std::string, SDL_Texture *> textures)
		: view_cols(view_cols), view_rows(view_rows), x_offset(x_offset), y_offset(y_offset),

		padding_horiz(padding_horiz), padding_vert(padding_vert), block_size(block_size), renderer(renderer),

		view_grid(view_rows, view_cols, nullptr, nullptr, x_offset, y_offset, block_size, 1, padding_horiz,
			padding_vert, renderer), empty_texture(empty_texture),

		up_button(view_grid.get_x() + view_grid.get_width(), view_grid.get_y(), scroll_buttons_size, scroll_buttons_size, 
			textures.at("up arrow:dark red"), textures.at("up arrow:red"), textures.at("up arrow:red"), 
			bind(&scroll_grid::scroll_up, this), renderer),

		down_button(view_grid.get_x() + view_grid.get_width(), view_grid.get_y() + view_grid.get_height() - block_size/3, 
			scroll_buttons_size, scroll_buttons_size, textures.at("down arrow:dark red"), textures.at("down arrow:red"), 
			textures.at("down arrow:red"), bind(&scroll_grid::scroll_down, this), renderer), num_screens(0),

		scroll_buttons_width(scroll_buttons_size)
	{
		higher_x = get_x() + get_width();
		higher_y = get_y() + get_height();
	}

	void add_tiles(const vector<shared_ptr<T> > &new_tiles)
	{
		all_tiles.insert(all_tiles.end(), new_tiles.begin(), new_tiles.end());
		fill_view_grid();

		num_screens = (unsigned)ceil((float)all_tiles.size() / (float)(view_cols * view_rows));
	}

	void fill_view_grid()
	{
		using namespace std;

		unsigned start_idx = scroll_pos * view_cols * view_rows;
		unsigned end_idx = start_idx + view_rows * view_cols;
		end_idx = end_idx < all_tiles.size() ? end_idx : all_tiles.size();
		vector<shared_ptr<T> > temp_vec(next(all_tiles.begin(), start_idx), next(all_tiles.begin(), end_idx));

		while (temp_vec.size() < view_rows * view_cols)
		{
			temp_vec.push_back(make_shared<T>(0, 0, block_size, block_size, empty_texture, empty_texture, renderer));
			temp_vec.back()->set_hover_texture(empty_texture);
		}

		view_grid.fill_grid(temp_vec);
	}

	void draw()
	{
		view_grid.draw();
		if (!is_at_top())
			up_button.draw();
		if (!is_at_bottom())
			down_button.draw();
	}
	
	void update(SDL_Event * sdl_event)
	{
		view_grid.update();
		if (!is_at_top())
			up_button.update();
		if (!is_at_bottom())
			down_button.update();
	}

	int get_view_grid_higher_x()
	{
		return view_grid.get_width() + view_grid.get_x();
	}

	int get_view_grid_higher_y()
	{
		return view_grid.get_height() + view_grid.get_y();
	}

	unsigned get_view_grid_width()
	{
		return view_grid.get_width();
	}

	unsigned get_view_grid_height()
	{
		return view_grid.get_height();
	}

	/*
	 * the condition in this function below will anyway be satisfied if it is called through 
	 * the button, since the button cannot be clicked if this condition is false. However, this ensures 
	 * that we cannot scroll down by means other than the button, if they get added. The same goes
	 * for scroll_down()
	*/
	void scroll_up()
	{
		if (!is_at_top())
		{
			scroll_pos--;
			fill_view_grid();
		}
	}

	void scroll_down()
	{
		if (!is_at_bottom())
		{
			scroll_pos++;
			fill_view_grid();
		}
	}

	bool is_at_top()
	{
		return scroll_pos <= 0;
	}

	bool is_at_bottom()
	{
		return ((int)scroll_pos >= num_screens - 1);
	}

	unsigned get_width()
	{
		return get_view_grid_width() + scroll_buttons_width;
	}

	unsigned get_height()
	{
		return get_view_grid_height() + scroll_buttons_height;
	}

	int get_x()
	{
		return x_offset;
	}

	int get_y()
	{
		return y_offset;
	}

	int get_higher_x()
	{
		return higher_x;
	}

	int get_higher_y()
	{
		return higher_y;
	}

private:
	grid<T> view_grid;

	std::vector<std::shared_ptr<T> > all_tiles;
	unsigned scroll_pos = 0;
	unsigned view_cols, view_rows, padding_horiz, padding_vert, block_size;
	int x_offset, y_offset;
	SDL_Renderer * renderer;

	button up_button, down_button;

	SDL_Texture * empty_texture;

	int higher_x, higher_y;

	unsigned num_screens;

	unsigned scroll_buttons_width;
	unsigned scroll_buttons_height;
};