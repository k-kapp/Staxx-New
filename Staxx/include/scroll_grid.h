#pragma once

#include "grid.h"

#include <SDL2/SDL.h>

#include <vector>
#include <memory>
#include <cmath>
#include <iostream>

template<typename T>
class scroll_grid
{
public:
	scroll_grid(unsigned view_cols, unsigned view_rows, int x_offset, int y_offset,
		unsigned padding_horiz, unsigned padding_vert, unsigned block_size, SDL_Texture * empty_texture, SDL_Renderer * renderer)
		: view_cols(view_cols), view_rows(view_rows), x_offset(x_offset), y_offset(y_offset),
		padding_horiz(padding_horiz), padding_vert(padding_vert), block_size(block_size), renderer(renderer),
		view_grid(view_rows, view_cols, nullptr, nullptr, x_offset, y_offset, block_size, 1, padding_horiz,
			padding_vert, renderer), empty_texture(empty_texture)
	{
		higher_x = x_offset + view_grid.num_cols * (view_grid.block_size + 2*padding_horiz);
		higher_y = y_offset + view_grid.num_rows * (view_grid.block_size + 2 * padding_vert);
	}

	void add_tiles(const vector<shared_ptr<T> > &new_tiles)
	{
		all_tiles.insert(all_tiles.end(), new_tiles.begin(), new_tiles.end());
		fill_view_grid();
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
	}
	
	void update(SDL_Event * sdl_event)
	{
		view_grid.update();

		bool grid_changed = false;

		if (sdl_event)
		{
			if (sdl_event->type == SDL_KEYDOWN)
			{
				if (sdl_event->key.keysym.sym == SDLK_DOWN)
				{
					cout << "down pressed" << endl;
					unsigned div_val = (unsigned)std::ceil( (double)all_tiles.size() / (double)(view_rows*view_cols));
					if ((int)scroll_pos < div_val - 1)
					{
						scroll_pos++;
						grid_changed = true;
					}

				}
				if (sdl_event->key.keysym.sym == SDLK_UP)
				{
					cout << "up pressed" << endl;
					if (scroll_pos > 0)
					{
						scroll_pos--;
						grid_changed = true;
					}
				}
			}
		}

		if (grid_changed)
		{
			fill_view_grid();
		}
	}

	int view_grid_higher_x()
	{
		return view_grid.get_width() + view_grid.get_x();
	}

	int view_grid_higher_y()
	{
		return view_grid.get_height() + view_grid.get_y();
	}

	unsigned view_grid_width()
	{
		return view_grid.get_width();
	}

	unsigned view_grid_height()
	{
		return view_grid.get_height();
	}

private:
	grid<T> view_grid;

	std::vector<std::shared_ptr<T> > all_tiles;
	unsigned scroll_pos = 0;
	unsigned view_cols, view_rows, padding_horiz, padding_vert, block_size;
	int x_offset, y_offset;
	SDL_Renderer * renderer;

	SDL_Texture * empty_texture;

	int higher_x, higher_y;
};