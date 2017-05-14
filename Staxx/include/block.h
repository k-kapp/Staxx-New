#ifndef BLOCK_H
#define BLOCK_H

#include "shape.h"
#include "misc_types.h"

class shape;

class block
{
public:
	block(shape * design, color base_col, color border_col);
	block(shape * design, SDL_Texture * texture);
	orient get_curr_orient();
	bool_2d get_curr_state();


	int get_height();
	int get_width();
	int get_row();
	int get_col();
	void move_up(bool reverse=false);
	void move_down(bool reverse=false);
	void move_left(bool reverse=false);
	void move_right(bool reverse=false);
	void rotate_right(bool reverse=false);
	void reverse_previous();
	move_type get_last_move();
	bool active_pos(int row, int col);
	SDL_Texture * get_texture();

	bool operator () (int, int);

	tile_colors colors;

private:
	void init_vars();

	orient curr_orient;
	shape * design;
	move_type last_move;

	SDL_Texture * texture;

	int row, col;
};

#endif
