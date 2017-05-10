#include "../include/shape.h"
#include "../include/block.h"
#include "../include/grid.h"
#include "../include/game.h"
#include "../include/clickable_tile.h"
#include "../include/constants.h"

#include <SDL2/SDL.h>
#include <vector>
#include <iostream>
#include <memory>
#include <string>
#include <cmath>
#include <algorithm>
#include <chrono>

using namespace std;

void move_down(void * void_game_grid_ptr)
{
	game * game_grid_ptr = (game *) void_game_grid_ptr;
	game_grid_ptr->move_active(DOWN_MV, true);
}

Uint32 callback_move_down(Uint32 interval, void * param)
{
	SDL_Event event;
	SDL_UserEvent user_event;

	user_event.type = SDL_USEREVENT;
	user_event.code = 0;
	user_event.data1 = (void *) &move_down;
	user_event.data2 = param;

	event.type = SDL_USEREVENT;
	event.user = user_event;

	SDL_PushEvent(&event);

	return interval;
}

game::game(tile_colors colors, tile_colors default_on_colors)
			: game_state(PLAY_SCREEN_TITLE, PLAY_SCREEN_X, PLAY_SCREEN_Y, PLAY_SCREEN_WIDTH, PLAY_SCREEN_HEIGHT), 
				main_grid(PLAY_ROWS + 2, PLAY_COLS + 2, 
					make_texture_from_colors(colors, PLAY_BLOCK_SIZE, PLAY_BLOCK_SIZE, renderer), 
					make_texture_from_colors(default_on_colors, PLAY_BLOCK_SIZE, PLAY_BLOCK_SIZE, renderer),
					PLAY_SCREEN_X, 
					PLAY_SCREEN_Y, 
					PLAY_BLOCK_SIZE, 
					PLAY_BORDER_SIZE, 
					0, 0,
					renderer), 
				delay(INIT_DELAY), 
				score(0), 
				active_block(nullptr), 
				next_texture(SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_TARGET, 8 * PLAY_BLOCK_SIZE / 2, 8 * PLAY_BLOCK_SIZE / 2)),
				gen(chrono::steady_clock::now().time_since_epoch().count())
{

	int block_size = PLAY_BLOCK_SIZE;

	this->game_over = false;

	int end_x = PLAY_COLS*block_size + PLAY_SCREEN_X;
	int end_y = PLAY_ROWS*block_size + PLAY_SCREEN_Y;

	//next_grid = grid<game_tile>(8, 8, main_grid.off_texture, main_grid.on_texture, NEXT_GRID_X, NEXT_GRID_Y, NEXT_GRID_BLOCK_SIZE, 1, renderer);
	next_grid = grid<game_tile>(8, 8, textures.at("black"), main_grid.on_texture, 0, 0, NEXT_GRID_BLOCK_SIZE, 1, 0, 0, renderer);

	next_block_rect.x = NEXT_GRID_X;
	next_block_rect.y = NEXT_GRID_Y;
	next_block_rect.w = NEXT_GRID_COLS*NEXT_GRID_BLOCK_SIZE;
	next_block_rect.h = NEXT_GRID_ROWS*NEXT_GRID_BLOCK_SIZE;

	assign_row_col_nums();
	set_borders();

	/*
	next_rect.w = 8 * block_size / 2;
	next_rect.h = 8 * block_size / 2;
	next_rect.x = end_x + 4 * block_size;
	next_rect.y = end_y + block_size - 8 * block_size / 2;
	*/

	//'next piece' text texture
	TTF_Font * font = TTF_OpenFont("fonts/ARCADECLASSIC.TTF", 20);
	next_text_surface = TTF_RenderText_Solid(font, "NEXT", {255, 255, 255});
	int w, h;
	TTF_SizeText(font, "NEXT", &w, &h);
	next_text_rect.w = 8*NEXT_GRID_BLOCK_SIZE;
	next_text_rect.h = 8*NEXT_GRID_BLOCK_SIZE/3;
	next_text_rect.x = NEXT_GRID_X + (NEXT_GRID_COLS * NEXT_GRID_BLOCK_SIZE - next_text_rect.w)/2;
	next_text_rect.y = NEXT_GRID_Y + (-NEXT_GRID_ROWS * NEXT_GRID_BLOCK_SIZE * 0.1 - next_text_rect.h);
	next_text_texture = SDL_CreateTextureFromSurface(renderer, next_text_surface);


	//'score' text texture
	int score_w, score_h;
	TTF_Font * score_heading_font = TTF_OpenFont("fonts/ARCADECLASSIC.TTF", 20);
	score_heading_surface = TTF_RenderText_Solid(score_heading_font, "SCORE", {255, 255, 255});
	TTF_SizeText(score_heading_font, "SCORE", &score_w, &score_h);
	score_heading_rect.w = next_text_rect.w*1.2;
	score_heading_rect.h = next_text_rect.h*1.2;
	score_heading_rect.x = next_text_rect.x - 0.1*next_text_rect.w;
	score_heading_rect.y = PLAY_SCREEN_Y + 0.1*PLAY_ROWS*block_size;
	score_heading_texture = SDL_CreateTextureFromSurface(renderer, score_heading_surface);

	SDL_RenderCopy(renderer, score_heading_texture, NULL, &score_heading_rect);

	score_font = score_heading_font;
	render_score_texture();
}

game::~game()
{
	/*
	 * remove the timer, in case program exited prematurely
	 */
	SDL_RemoveTimer(timer_id);
	//SDL_DestroyRenderer(renderer);
	//SDL_DestroyWindow(window);
}

void game::assign_row_col_nums()
{
	for (int row_idx = 0; row_idx < get_nrows(); row_idx++)
	{
		for (int col_idx = 0; col_idx < get_ncols(); col_idx++)
		{
			main_grid.tiles[row_idx][col_idx]->set_row_col(row_idx, col_idx);
			if (row_idx < next_grid.num_rows && col_idx < next_grid.num_cols)
			{
				next_grid.tiles[row_idx][col_idx]->set_row_col(row_idx, col_idx);
			}
		}
	}
}

void game::render_score_texture()
{
	//actual score texture
	string score_str = to_string(score);
	score_surface = TTF_RenderText_Solid(score_font, score_str.c_str(), {255, 255, 255});
	score_rect.w = (max((int)log10(score + 1e-1), 0) + 1) * 0.2 * score_heading_rect.w;
	score_rect.h = score_heading_rect.h * 0.9;
	score_rect.x = score_heading_rect.x + (score_heading_rect.w - score_rect.w)/2;
	score_rect.y = score_heading_rect.y + score_heading_rect.h * 1.2;
	score_texture = SDL_CreateTextureFromSurface(renderer, score_surface);
	SDL_RenderCopy(renderer, score_texture, NULL, &score_rect);
}

void game::move_active_up()
{
	active_block->move_up();
}

void game::move_active_down()
{
	active_block->move_down();
}

void game::move_active_left()
{
	active_block->move_left();
}

void game::move_active_right()
{
	active_block->move_right();
}

void game::rotate_active_right()
{
	active_block->rotate_right();
}

void game::set_borders()
{
	for (auto tile_iter = main_grid.tiles[0].begin(); tile_iter != main_grid.tiles[0].end();
			advance(tile_iter, 1))
	{
		(*tile_iter)->set_on_colors({{0, 150, 150}, {0, 150, 150}});
		(*tile_iter)->set_off_colors((*tile_iter)->get_on_colors());
		(*tile_iter)->set_occupied();
		//(*tile_iter)->set_activation_level(true);
	}

	for (auto tile_iter = main_grid.tiles[main_grid.num_rows - 1].begin(); tile_iter != main_grid.tiles[main_grid.num_rows - 1].end();
			advance(tile_iter, 1))
	{
		(*tile_iter)->set_on_colors({{0, 150, 150}, {0, 150, 150}});
		(*tile_iter)->set_off_colors((*tile_iter)->get_on_colors());
		(*tile_iter)->set_occupied();
		//(*tile_iter)->set_activation_level(true);
	}

	for (auto row_iter = main_grid.tiles.begin(); row_iter != main_grid.tiles.end(); advance(row_iter, 1))
	{
		row_iter->front()->set_on_colors({{0, 150, 150}, {0, 150, 150}});
		row_iter->front()->set_occupied();
		//row_iter->front()->set_activation_level(true);
		row_iter->back()->set_on_colors(row_iter->front()->get_on_colors());
		row_iter->back()->set_occupied();
		//row_iter->back()->set_activation_level(true);
	}
}

void game::draw()
{

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	main_grid.draw();

	SDL_RenderCopy(renderer, score_heading_texture, NULL, &score_heading_rect);
	render_score_texture();

	SDL_RenderCopy(renderer, next_block_texture, NULL, &next_block_rect);
	SDL_RenderCopy(renderer, next_text_texture, NULL, &next_text_rect);

	SDL_RenderPresent(renderer);
}

void game::update()
{
	//cout << "active_block row: " << active_block->get_row() << endl;

	main_grid.update();
	//next_grid.update();
}


bool game::has_clash()
{
	for (auto &row : main_grid.tiles)
	{
		for (auto &tile : row)
		{
			if (tile->has_clash())
			{
				active_block->reverse_previous();
				return true;
			}
		}
	}

	return false;
}

void game::move_active(move_type move, bool regular)
{
	draw();

	//print_main_occupied();

	void (game::*move_func) ();

	switch (move)
	{
		case (UP_MV):
		{
			move_func = &game::move_active_up;
		} break;
		case (DOWN_MV):
		{
			move_func = &game::move_active_down;
		} break;
		case (RIGHT_MV):
		{
			move_func = &game::move_active_right;
		} break;
		case (LEFT_MV):
		{
			move_func = &game::move_active_left;
		} break;
		case (ROTATE_MV):
		{
			move_func = &game::rotate_active_right;
		} break;
		default:
			move_func = nullptr;
	}

	(this->*move_func)();

	if (has_clash() && (move == curr_move))
	{
		set_active_tiles(main_grid, active_block);
		vector<int> rows_idxes = get_full_rows();
		active_block = nullptr;
		if (rows_idxes.size() > 0)
		{
			flash_rows(rows_idxes);

			score += main_grid.num_cols * rows_idxes.size() * rows_idxes.size();
			draw();
		}
		remove_rows(rows_idxes);
		sample_next();
		if (has_clash())
		{
			game_over = true;
		}
	}
}

void game::print_main_occupied()
{
	for (auto &row : main_grid.tiles)
	{
		for (auto &tile : row)
		{
			if (tile->is_occupied())
			{
				cout << "1";
			}
			else
				cout << "0";
		}
		cout << endl;
	}
	cout << endl;
}

void game::print_next_occupied()
{
	for (auto &row : next_grid.tiles)
	{
		for (auto &tile : row)
		{
			//if (tile->is_occupied())
			if (tile->get_activation_level())
			{
				cout << "1";
			}
			else
				cout << "0";
		}
		cout << endl;
	}
	cout << endl;
}

shared_ptr<shape> game::generate_next_shape_ptr()
{
	int idx = static_cast<int>(unif_real(gen)*shape_vec.size());

	return shape_vec.at(idx);
}

void game::init_sample()
{
	for (int i = 0; i < 2; i++)
	{
		auto next_block = shared_ptr<block>(new block(generate_next_shape_ptr().get(), {100, 0, 0}, {0, 0, 100}));
		next_blocks.push_back(next_block);
	}
	sample_next();
}

void game::sample_next()
{
	set_main_active(next_blocks.front());
	next_blocks.pop_front();

	next_blocks.push_back(shared_ptr<block>(new block(generate_next_shape_ptr().get(), {100, 0, 0}, {0, 0, 100})));

	curr_move = static_cast<move_type>(static_cast<int>(unif_real(gen) * 4));

	//SDL_RenderCopy(renderer, next_text_texture, NULL, &next_text_rect);
}

void game::set_main_active(shared_ptr<block> next_active)
{
	active_block = next_active;
	for (auto &row : main_grid.tiles)
	{
		for (auto &tile : row)
		{
			tile->set_active_block(active_block);
		}
	}
	if (!func_ptr)
		init_timer(callback_move_down);
	else
		reset_timer();

	//shared_ptr<block> next_block = make_shared<block>(*active_block);
	shared_ptr<block> next_block = make_shared<block>(*next_blocks.back());
	set_next_active(next_block);

	SDL_SetRenderTarget(renderer, next_texture);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_SetRenderTarget(renderer, nullptr);
	next_grid.draw(next_texture);

	print_next_occupied();
}

/*
void game::set_next_active(shared_ptr<block> next_active)
{
	int col_diff = next_grid.num_cols - next_active->get_width();
	int row_diff = next_grid.num_rows - next_active->get_height();

	for (int i = next_active->get_row(); i < row_diff / 2; i++)
		next_active->move_down();
	for (int j = next_active->get_col(); j < col_diff / 2; j++)
		next_active->move_right();
	for (int j = next_active->get_col(); j > col_diff / 2; j--)
		next_active->move_left();

	for (auto &row : next_grid.tiles)
	{
		for (auto &tile : row)
		{
			tile->set_active_block(next_active);
		}
	}

	next_grid.update();
}
*/

void game::set_next_active(shared_ptr<block> next_active)
{
	SDL_DestroyTexture(next_block_texture);
	next_block_texture = next_grid.create_texture(next_active.get(), true);
}

void game::add_shape(const shared_ptr<shape> &shape_ptr)
{
	shape_ptr->set_padding(UP, get_ncols(), get_nrows());
	shape_vec.push_back(shape_ptr);
}

void game::add_shapes(const shape_vec_type &shape_vec)
{
	for (const shared_ptr<shape> &shape_ptr : shape_vec)
	{
		add_shape(shape_ptr);
	}
}

void game::remove_rows(const vector<int> rows_idxes)
{
	for (auto &row_idx : rows_idxes)
	{
		for (int curr_row = row_idx; curr_row > 0; curr_row--)
		{
			for (int curr_col = 1; curr_col < main_grid.num_cols - 1; curr_col++)
			{
				main_grid.tiles[curr_row][curr_col] = move(main_grid.tiles[curr_row - 1][curr_col]);
				//main_grid.tiles[curr_row - 1][curr_col].reset();
				//main_grid.tiles[curr_row][curr_col].reset(main_grid.tiles[curr_row - 1][curr_col].release());
				main_grid.tiles[curr_row][curr_col]->set_row_col(curr_row, curr_col);
				main_grid.tiles[curr_row][curr_col]->set_coords(main_grid.x_offset + curr_col * main_grid.block_size, 
					main_grid.y_offset + curr_row * main_grid.block_size);
			}
		}
		for (int curr_col = 1; curr_col < main_grid.num_cols - 1; curr_col++)
		{
			main_grid.tiles[0][curr_col] = make_shared<game_tile>(main_grid.x_offset + curr_col * main_grid.block_size,
				main_grid.y_offset, main_grid.block_size, main_grid.block_size, main_grid.off_texture, main_grid.on_texture, renderer);
			main_grid.tiles[0][curr_col]->set_row_col(0, curr_col);
		}
	}
}

void game::flash_rows(vector<int> row_idxes)
{
	SDL_RemoveTimer(timer_id);
	lock = true;
	for (int i = 0; i < 4; i++)
	{
		for (auto &row_idx : row_idxes)
		{
			for (int col_idx = 1; col_idx < main_grid.num_cols - 1; col_idx++)
			{
				main_grid.tiles[row_idx][col_idx]->set_activation_level(false);
			}
		}
		draw();
		SDL_Delay(100);
		for (auto &row_idx : row_idxes)
		{
			for (int col_idx = 1; col_idx < get_ncols() - 1; col_idx++)
			{
				main_grid.tiles[row_idx][col_idx]->set_activation_level(true);
			}
		}
		draw();
		SDL_Delay(100);
	}
	lock = false;
	reset_timer();
}



vector<int> game::get_full_rows()
{
	int top_row = active_block->get_row();
	int bottom_row = active_block->get_row() + active_block->get_height() - 1;

	vector<int> remove_rows;

	for (int curr_row = top_row; curr_row <= bottom_row; curr_row++)
	{
		bool row_full = true;
		for (int curr_col = 1; curr_col < get_ncols() - 1; curr_col++)
		{
			if (!main_grid.tiles[curr_row][curr_col]->get_activation_level())
			{
				row_full = false;
				break;
			}
		}
		if (row_full)
		{
			remove_rows.push_back(curr_row);
		}
	}
	return remove_rows;
}

void game::init_timer(Uint32 (* func_ptr)(Uint32, void *))
{
	this->func_ptr = func_ptr;
	timer_id = SDL_AddTimer(delay, func_ptr, (void *)this);
}

void game::reset_timer()
{
	SDL_RemoveTimer(timer_id);
	timer_id = SDL_AddTimer(delay, func_ptr, (void *)this);
}

void game::mainloop()
{
	bool quit = false;

	int prev_row, prev_col;
	prev_row = 0;
	prev_col = active_block->get_col();

	bool deactivate = false;
	bool paused = false;

	SDL_Event event;

	while (true)
	{
		deactivate = false;
		
		if (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case (SDL_KEYUP):
				{
					if (event.key.keysym.sym == SDLK_p)
						paused = !paused;
					if (event.key.keysym.sym == SDLK_ESCAPE)
						quit = true;
					if (paused)
					{
						cout << "continuing" << endl;
						continue;
					}


					switch (event.key.keysym.sym)
					{
						case (SDLK_SPACE):
						{
							move_active(ROTATE_MV, false);
						} break;
						case (SDLK_RIGHT):
						{
							if (curr_move == LEFT_MV)
								break;
							move_active(RIGHT_MV, false);
						} break;
						case (SDLK_LEFT):
						{
							if (curr_move == RIGHT_MV)
								break;
							move_active(LEFT_MV, false);
						} break;
						case (SDLK_DOWN):
						{
							if (curr_move == UP_MV)
								break;
							reset_timer();
							SDL_FlushEvent(SDL_USEREVENT);
							move_active(DOWN_MV, false);
						} break;
						case (SDLK_UP):
						{
							if (curr_move == DOWN_MV)
								break;
							move_active(UP_MV, false);
						} break;
						default:
						{}
					}
				} break;
				case (SDL_USEREVENT):
				{
					if (!paused)
					{
						//void (*move_down_ptr)(void *) = (void(*)(void *)) event.user.data1;
						//void * param = event.user.data2;
						//move_down(param);
						move_active(curr_move, true);
					}
				} break;
				default:
				{}
			}


			quit = quit || game_over;
		}
		if (active_block)
		{
			//cout << "drawing and updating..." << endl;
			update();
			draw();
		}
		if (quit)
		{
			SDL_RemoveTimer(timer_id);
			break;
		}
	}
}


unsigned game::get_ncols()
{
	return main_grid.num_cols;
}

unsigned game::get_nrows()
{
	return main_grid.num_rows;
}