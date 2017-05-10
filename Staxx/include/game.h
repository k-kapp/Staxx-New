#ifndef GAME_H
#define GAME_H

#include <memory>
#include <vector>
#include <random>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "misc_types.h"
#include "game_state.h"
#include "game_tile.h"

using namespace std;

class block;
class shape;
using block_vec_type = vector<shared_ptr<block> >;
using shape_vec_type = vector<shared_ptr<shape> >;
using next_type = list<shared_ptr<block> >;

class clickable_tile;

class game : game_state
{
public:
	game(tile_colors colors, tile_colors default_on_colors);

	~game();
	void add_shape(const shared_ptr<shape> &shape_ptr);
	void add_shapes(const shape_vec_type &shape_vec);
	void init_sample();
	void sample_next();
	void move_active_up();
	void move_active_down();
	void move_active_left();
	void move_active_right();
	void rotate_active_right();
	void move_active(move_type move, bool regular);
	void remove_rows(const vector<int> rows_idxes);
	void flash_rows(vector<int> row_idxes);
	vector<int> get_full_rows();
	shared_ptr<shape> generate_next_shape_ptr();
	void set_borders();
	void mainloop();
	void set_main_active(shared_ptr<block>);
	void set_next_active(shared_ptr<block>);
	void reset_timer();

	void init_timer(Uint32 (*)(Uint32, void *));

	void assign_row_col_nums();

	void print_main_occupied();
	void print_next_occupied();

	unsigned get_ncols();
	unsigned get_nrows();

protected:
	void draw();
	void update();

private:
	void render_score_texture();

	bool has_clash();

	template<typename T>
	void set_active_tiles(grid<T> &grid, shared_ptr<block> set_block)
	{
		for (int block_row = 0; block_row < set_block->get_height(); block_row++)
		{
			for (int block_col = 0; block_col < set_block->get_width(); block_col++)
			{
				if ((*set_block)(block_row, block_col))
				{
					grid(set_block->get_row() + block_row, set_block->get_col() + block_col)->set_occupied();
					grid(set_block->get_row() + block_row, set_block->get_col() + block_col)->set_on_colors(set_block->colors);
				}
			}
		}
	}

private:
	bool lock; 
	shape_vec_type shape_vec;
	default_random_engine gen;
	uniform_real_distribution<double> unif_real;
	int delay;
	SDL_TimerID timer_id;
	Uint32 (*func_ptr)(Uint32, void *);

	next_type next_blocks;

	grid<game_tile> main_grid;
	grid<game_tile> next_grid;

	SDL_Surface * next_text_surface;
	SDL_Texture * next_text_texture;
	SDL_Surface * score_heading_surface;
	SDL_Texture * score_heading_texture;
	SDL_Surface * score_surface;
	SDL_Texture * score_texture;
	TTF_Font * score_font;

	SDL_Texture * next_texture;
	SDL_Rect next_block_rect;

	SDL_Rect next_text_rect;
	SDL_Rect score_heading_rect;
	SDL_Rect score_rect;

	unsigned score;

	bool game_over;

	shared_ptr<block> active_block;

	SDL_Texture * next_block_texture;

	move_type curr_move;

};

#endif
