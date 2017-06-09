#pragma once

#ifndef GAME_TILE_H
#define GAME_TILE_H

#include <memory>

//#include "../include/tile.h"
#include "../include/basic_tile.h"
#include "../include/block.h"

class game;

/*
 * The play board consists of the game_tile type, defined below. It differs from the clickable tile in that
 * it does not respond to mouse clicks and it has a few extra utility functions for determining e.g. clashes, etc.
 */

class game_tile : public basic_tile
{
public:
	game_tile(game * game_parent, int grid_x, int grid_y, int row_num, int col_num, unsigned size, SDL_Texture * off_texture,
		SDL_Texture * on_texture, SDL_Renderer * renderer);

	game_tile(const game_tile &other);

	game_tile(int x_coord, int y_coord, unsigned x_size, unsigned y_size, SDL_Texture * off_texture, SDL_Texture * on_texture, 
		SDL_Renderer * renderer);

	game_tile& operator = (const game_tile &other);

	bool has_clash();
	void set_active_block(std::shared_ptr<block> new_active);

	void set_parent(game * parent);

	void update() override;

	void set_occupied();

	bool is_occupied();

	void set_row_col(int row, int col);

private:
	std::shared_ptr<block> active_block;
	bool is_active();
	bool in_block();

	unsigned row_num, col_num;

	bool occupied = false;

	game * game_parent;
};

#endif