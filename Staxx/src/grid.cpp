#include "../include/grid.h"
#include "../include/tiles.h"
#include "../include/block.h"

#include <SDL2/SDL.h>
#include <vector>
#include <iostream>
#include <memory>

using namespace std;


grid::grid(int num_rows, int num_cols, tile_colors colors, int x_offset, 
		int y_offset, int block_size, int border_size, SDL_Renderer * renderer)
		: num_rows(num_rows), num_cols(num_cols), colors(colors),
			x_offset(x_offset), y_offset(y_offset), block_size(block_size),
			border_size(border_size), renderer(renderer)
{
	fill_grid();
}

grid::grid()
{
}

grid::grid(const grid &other)
{
	num_rows = other.num_rows;
	num_cols = other.num_cols;
	colors = other.colors;
	x_offset = other.x_offset;
	y_offset = other.y_offset;
	block_size = other.block_size;
	border_size = other.border_size;
	renderer = other.renderer;

	//fill_grid();
	copy_grid(other);
}

grid &grid::operator = (const grid &other)
{
	num_rows = other.num_rows;
	num_cols = other.num_cols;
	colors = other.colors;
	x_offset = other.x_offset;
	y_offset = other.y_offset;
	block_size = other.block_size;
	border_size = other.border_size;
	renderer = other.renderer;

	//fill_grid();
	copy_grid(other);

	return *this;
}


bool grid::draw()
{
	draw_all(true);
	/*
	if (!draw_all(true))
	{
		active_block->reverse_previous();
		return false;
	}
	*/
	return true;
}

bool grid::update()
{

	return true;
}


bool grid::draw_all(bool render)
{
	for (auto &row_vec : tiles)
	{
		for (auto &tl : row_vec)
		{
			tl->draw();
			/*
			if (!tl.draw(active_block))
			{
				return false;
			}
			*/
		}
	}
	//if (render)
	//	SDL_RenderPresent(renderer);
	return true;
}

void grid::fix_active_tiles(shared_ptr<block> active_block)
{
	for (int block_row = 0; block_row < active_block->get_height(); block_row++)
	{
		for (int block_col = 0; block_col < active_block->get_width(); block_col++)
		{
			if ((*active_block)(block_row, block_col))
			{
				tiles[active_block->get_row() + block_row][active_block->get_col() + block_col]->occupied = true;
				tiles[active_block->get_row() + block_row][active_block->get_col() + block_col]->set_fore_colors(active_block->colors);
			}
		}
	}
}

void grid::print_grid(shared_ptr<block> active_block)
{
	for (auto row_iter = tiles.begin(); row_iter != tiles.end(); advance(row_iter, 1))
	{
		for (auto col_iter = row_iter->begin(); col_iter != row_iter->end(); advance(col_iter, 1))
		{
			bool make_one = (*col_iter)->occupied;
			if (active_block)
			{
				int row = distance(tiles.begin(), row_iter);
				int col = distance(row_iter->begin(), col_iter);
				make_one = make_one || active_block->active_pos(row, col);
			}
			cout << make_one << " ";
		}
		cout << endl;
	}
}

void grid::copy_grid(const grid &other)
{
	tiles.resize(other.num_rows);
	for (auto &row : tiles)
	{
		row.resize(other.num_cols);
	}

	for (unsigned row = 0; row < other.num_rows; row++)
	{
		for (unsigned col = 0; col < other.num_cols; col++)
		{
			tiles[row][col] = make_unique<tile>(*other.tiles[row][col]);
		}
	}
}

/*
bool grid::draw_no_render()
{
	int row_count = 0;
	for (auto &row_vec : tiles)
	{
		for (auto &tl : row_vec)
		{
			tl->draw(active_block);
		}
		row_count++;
	}
	return true;
}
*/

/*
bool grid::draw_render()
{
	if (draw_no_render())
	{
		SDL_RenderPresent(renderer);
		return true;
	}
	else
		return false;
}
*/


/*
void grid::set_active(shared_ptr<block> new_active)
{
	active_block = new_active;
}
*/

