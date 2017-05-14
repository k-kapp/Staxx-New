#include "../include/block.h"
#include "../include/shape.h"
#include "../include/misc_types.h"


block::block(shape * design, color base_col, color border_col)
			: design(design), colors({base_col, border_col})
{
	tile_colors cols;
	cols.base = base_col;
	cols.border = border_col;
	init_vars();
}

block::block(shape * design, SDL_Texture * texture)
	: design(design), texture(texture)
{
	init_vars();
}

bool block::operator () (int row, int col)
{
	return design->get_coords(row, col, curr_orient);
}

void block::init_vars()
{
	curr_orient = UP;
	row = design->get_vert_padding().first;
	col = design->get_horiz_padding().first;
}

orient block::get_curr_orient()
{
	return curr_orient;
}

bool_2d block::get_curr_state()
{
	return design->get_shape(curr_orient);
}

int block::get_height()
{
	return design->get_height(curr_orient);
}

int block::get_width()
{
	return design->get_width(curr_orient);
}

int block::get_row()
{
	return row;
}

int block::get_col()
{
	return col;
}

bool block::active_pos(int row, int col)
{
	if (row >= get_row() && row < get_row() + get_height())
	{
		if (col >= get_col() && col < get_col() + get_width())
		{
			return design->get_coords(row - get_row(), col - get_col(), curr_orient);
		}
		return false;
	}
	return false;
}

void block::move_up(bool reverse)
{
	if (!reverse)
	{
		last_move = UP_MV;
		row--;
	}
	else
	{
		row++;
	}
}

void block::move_down(bool reverse)
{
	if (!reverse)
	{
		last_move = DOWN_MV;
		row++;
	}
	else
	{
		row--;
	}
}

void block::move_left(bool reverse)
{
	if (!reverse)
	{
		last_move = LEFT_MV;
		col--;
	}
	else
	{
		col++;
	}
}

void block::move_right(bool reverse)
{
	if (!reverse)
	{
		last_move = RIGHT_MV;
		col++;
	}
	else
	{
		col--;
	}
}

void block::rotate_right(bool reverse)
{
	if (!reverse)
	{
		last_move = ROTATE_MV;
		curr_orient = (orient)((curr_orient + 1) % 4);
	}
	else
	{
		curr_orient = (orient)((curr_orient + 3) % 4);
	}
}

move_type block::get_last_move()
{
	return last_move;
}

void block::reverse_previous()
{
	switch (last_move)
	{
		case (UP_MV):
		{
			move_up(true);
		} break;
		case (DOWN_MV):
		{
			move_down(true);
		} break;
		case (LEFT_MV):
		{
			move_left(true);
		} break;
		case (RIGHT_MV):
		{
			move_right(true);
		} break;
		case (ROTATE_MV):
		{
			rotate_right(true);
		} break;
	}
}

SDL_Texture * block::get_texture()
{
	return texture;
}