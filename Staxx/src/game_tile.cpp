
#include "../include/game_tile.h"
#include "../include/game.h"
#include "../include/block.h"

game_tile::game_tile(game * game_parent, int grid_x, int grid_y, int row_num, int col_num, unsigned size, SDL_Texture * off_texture,
	SDL_Texture * on_texture, SDL_Renderer * renderer)
	: basic_tile(col_num * size + grid_x, row_num * size + grid_y, size, size, off_texture, on_texture, renderer)
{
}

game_tile::game_tile(const game_tile &other) 
	: basic_tile(other.x_coord, other.y_coord, other.width, other.height, other.off_texture, other.on_texture, other.renderer), game_parent(game_parent)
{
	
}

game_tile::game_tile(int x_coord, int y_coord, unsigned x_size, unsigned y_size, SDL_Texture * off_texture, SDL_Texture * on_texture, SDL_Renderer * renderer)
	: basic_tile(x_coord, y_coord, x_size, y_size, off_texture, on_texture, renderer)
{
}

game_tile & game_tile::operator = (const game_tile &other)
{
	basic_tile::operator=(other);
	active_block = other.active_block;
	game_parent = other.game_parent;

	return *this;
}

void game_tile::set_row_col(int row, int col)
{
	if (row < 0)
	{
		row = 0;
	}
	if (col < 0)
	{
		col = 0;
	}

	row_num = row;
	col_num = col;
}

void game_tile::set_parent(game * parent)
{
	game_parent = parent;
}

bool game_tile::has_clash()
{
	if (active_block && in_block())
	{
		if (is_active())
		{
			if (occupied)
				return true;
		}
	}
	return false;
}

bool game_tile::is_active()
{
	int arr_row = row_num - active_block->get_row();
	int arr_col = col_num - active_block->get_col();

	return (*active_block)(arr_row, arr_col);
}

bool game_tile::in_block()
{
	if (active_block->get_row() > row_num)
	{
		return false;
	}
	if (active_block->get_row() + active_block->get_height() <= row_num)
	{
		return false;
	}
	if (active_block->get_col() > col_num)
	{
		return false;
	}
	if (active_block->get_col() + active_block->get_width() <= col_num)
	{
		return false;
	}
	return true;
}

void game_tile::set_active_block(shared_ptr<block> active_block)
{
	this->active_block = active_block;
}

void game_tile::update()
{
	basic_tile::update();

	if (is_occupied())
	{
		return;
	}

	if (in_block())
	{
		if (is_active())
		{
			set_activation_level(true);
			set_on_texture(active_block->get_texture());
			return;
		}
	}

	set_activation_level(false);

}

void game_tile::set_occupied()
{
	occupied = true;
	set_activation_level(true);
}

bool game_tile::is_occupied()
{
	return occupied;
}

