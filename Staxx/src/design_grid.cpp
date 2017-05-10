#include "../include/design_grid.h"
#include "../include/grid.h"
#include "../include/misc_types.h"
#include "../include/button.h"
#include "../include/shape.h"
#include "../include/constants.h"

#include <vector>
#include <iostream>
#include <functional>
#include <cstdlib>
#include <fstream>
#include <stdexcept>
#include <memory>

#include <SDL2/SDL.h>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/filesystem.hpp>
#include <boost/serialization/deque.hpp>
#include <boost/serialization/utility.hpp>


using namespace std;
using grid_pos = pair<int, int>;

pair<grid_pos, grid_pos> get_either_side_posses(pair<int, int> curr_pos, orient dir)
{
	switch (dir)
	{
		case (DOWN):
		{
			return {{curr_pos.first, curr_pos.second + 1}, {curr_pos.first, curr_pos.second - 1}};
		} break;
		case (UP):
		{
			return {{curr_pos.first, curr_pos.second - 1}, {curr_pos.first, curr_pos.second + 1}};
		} break;
		case (RIGHT):
		{
			return {{curr_pos.first - 1, curr_pos.second}, {curr_pos.first + 1, curr_pos.second}};
		} break;
		case (LEFT):
		{
			return {{curr_pos.first + 1, curr_pos.second}, {curr_pos.first - 1, curr_pos.second}};
		} break;
	}
}

void add_to_pos(grid_pos &pos, orient dir)
{
	switch (dir)
	{
		case (UP):
		{
			pos.first--;
		} break;
		case (RIGHT):
		{
			pos.second++;
		} break;
		case (DOWN):
		{
			pos.first++;
		} break;
		case (LEFT):
		{
			pos.second--;
		} break;
	}
}

void sub_from_pos(grid_pos &pos, orient dir)
{
	switch (dir)
	{
		case (UP):
		{
			pos.first++;
		} break;
		case (RIGHT):
		{
			pos.second--;
		} break;
		case (DOWN):
		{
			pos.first--;
		} break;
		case (LEFT):
		{
			pos.second++;
		} break;
	}
}



struct visit_tile
{
	visit_tile()
	{
		visited = false;
	}

	bool visited;
	vector<orient> dirs_not_taken;
};

/*

TODO:
	- apply a mouse_on_colors coloring to each tile in tiles variable

*/

design_grid::design_grid(int grid_size, tile_colors off_colors, tile_colors hover_colors, tile_colors on_colors,
	int x_offset, int y_offset, int block_size, int border_size)
	: game_state(DESIGN_SCREEN_TITLE, DESIGN_SCREEN_X, DESIGN_SCREEN_Y, DESIGN_SCREEN_WIDTH, DESIGN_SCREEN_HEIGHT),
	tiles(grid_size, grid_size,
		make_texture_from_colors(off_colors, block_size, block_size, renderer),
		make_texture_from_colors(hover_colors, block_size, block_size, renderer),
		x_offset, y_offset, block_size, border_size, 0, 0, renderer),
	off_texture(make_texture_from_colors(off_colors, block_size, block_size, renderer)),
	hover_texture(make_texture_from_colors(hover_colors, block_size, block_size, renderer)),
	on_texture(make_texture_from_colors(on_colors, block_size, block_size, renderer))
{
	num_rows = tiles.num_rows;
	num_cols = tiles.num_cols;

	tiles.apply_to_all([](clickable_tile &tile) {tile.set_sticky(true); });

	active_count = 0;
	this->grid_size = num_rows;
	occ_x = occ_y = 0;
	set_tile_colors();
	make_buttons();
}

design_grid::design_grid(int grid_size, SDL_Texture * off_texture, SDL_Texture * hover_texture, 
						 SDL_Texture * on_texture, int x_offset, int y_offset, int block_size, int border_size)
	: game_state(DESIGN_SCREEN_TITLE, DESIGN_SCREEN_X, DESIGN_SCREEN_Y, DESIGN_SCREEN_WIDTH, DESIGN_SCREEN_HEIGHT), 
		tiles(grid_size, grid_size, off_texture, on_texture, x_offset, y_offset, block_size, border_size, 5, 5, renderer), 
		off_texture(off_texture), hover_texture(hover_texture), on_texture(on_texture)
{
	
	num_rows = tiles.num_rows;
	num_cols = tiles.num_cols;

	tiles.apply_to_all([](clickable_tile &tile) {tile.set_sticky(true); });

	active_count = 0;
	this->grid_size = num_rows;
	occ_x = occ_y = 0;
	set_tile_colors();
	make_buttons();
}

/*
design_grid::~design_grid()
{
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
}
*/

void design_grid::make_buttons()
{
	function<void()> save_func = bind(&design_grid::save_design, this);
	function<void()> quit_func = bind(&design_grid::quit_design, this);
	save_button = make_unique<button>(move(button(tiles.x_offset, tiles.y_offset + tiles.num_rows*tiles.block_size + 100, 100, 50,
										{{150, 0, 0}, {180, 0, 0}}, {{200, 0, 0}, {230, 0, 0}}, {{255, 0, 0}, {255, 0, 0}},
										save_func, "Save", renderer)));
	quit_button = make_unique<button>(move(button(tiles.x_offset + tiles.num_cols*tiles.block_size - tiles.num_cols*tiles.block_size/3, tiles.y_offset + tiles.num_rows*tiles.block_size + 100, 100, 50,
											{{150, 0, 0}, {180, 0, 0}}, {{200, 0, 0}, {230, 0, 0}}, {{255, 0, 0}, {255, 0, 0}},
											quit_func, "Quit", renderer)));

	auto save_coords = save_button->get_coords();
	auto quit_coords = quit_button->get_coords();
}


void design_grid::get_mouse_in_grid()
{
	SDL_GetMouseState(&mouse_x, &mouse_y);

	int tile_on_x = (int)((mouse_x - tiles.x_offset)/tiles.block_size);
	if ((tile_on_x >= tiles.num_cols*tiles.block_size) || (tile_on_x < 0))
	{
		mouse_pos = {-1, -1};
		return;
	}
	
	int tile_on_y = (int)((mouse_y - tiles.y_offset)/tiles.block_size);
	if ((tile_on_y >= tiles.num_rows*tiles.block_size) || (tile_on_y < 0))
	{
		mouse_pos = {-1, -1};
		return;
	}
	mouse_pos = {tile_on_y, tile_on_x};
}


/*
 * Memory leak below!!!! Textures are created for the grid in constructor, but here, those pointers simply get
	reassigned to textures created by design_grid class, leading to loss of memory. Fix this!
*/
void design_grid::set_tile_colors()
{
	for (auto &row : tiles.tiles)
	{
		for (auto &tl : row)
		{
			tl->set_off_texture(off_texture);
			tl->set_hover_texture(hover_texture);
			tl->set_on_texture(on_texture);
			
			/*
			tl->set_off_colors(button_off_colors);
			tl->set_hover_colors(button_hover_colors);
			tl->set_on_colors(button_on_colors);
			*/
		}
	}
}

void design_grid::mainloop()
{
	quit = false;
	while (!quit)
	{
		update();
		draw();
		if (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case (SDL_KEYUP):
				{
					if (event.key.keysym.sym == SDLK_ESCAPE)
					{
						quit = true;
					}
				} break;
				default:
				{
				}
			}
		}
	}
}

bool design_grid::design_exists(const shape &new_design)
{
	auto existing = import_shapes();

	for (auto &exist_shape : existing)
	{
		if (new_design == *exist_shape)
		{
			return true;
		}
	}
	return false;
}

unsigned design_grid::count_active()
{
	unsigned count = 0;
	for (auto &row : tiles.tiles)
	{
		for (auto &tile : row)
		{
			if (tile->get_activation_level())
			{
				count++;
			}
		}
	}
	return count;
}

bool design_grid::design_valid()
{
	active_count = count_active();
	if (active_count < 2)
		return false;

	vector<vector<visit_tile> > visited(num_rows, vector<visit_tile>(num_cols));

	auto valid_idx = [this](int row_num, int col_num) 
	{
		if ((row_num < 0) || (col_num < 0))
		{
			return false;
		}
		if ((row_num >= this->num_rows) || (col_num >= this->num_rows))
		{
			return false;
		}
		return true;
	};

	orient dir = RIGHT;
	pair<int, int> curr_pos;
	bool pos_found = false;
	for (int row_num = num_rows - 1; row_num >= 0; row_num--)
	{
		for (int col_num = 0; col_num < num_cols; col_num++)
		{
			if (tiles.tiles[row_num][col_num]->get_activation_level() == 2)
			{
				curr_pos = {row_num, col_num};
				pos_found = true;
				break;
			}
		}
		if (pos_found)
			break;
	}

	bool cycle_finished = false;
	bool finished = false;

	vector<grid_pos> unfinished;

	int visit_count = 0;

	while (!finished)
	{
		if (cycle_finished)
		{
			if (unfinished.size() == 0)
			{
				finished = true;
				break;
			}
			curr_pos = unfinished.back();
			dir = visited[curr_pos.first][curr_pos.second].dirs_not_taken.back();
			visited[curr_pos.first][curr_pos.second].dirs_not_taken.pop_back();
			if (visited[curr_pos.first][curr_pos.second].dirs_not_taken.size() == 0)
			{
				unfinished.pop_back();
			}
			cycle_finished = false;
			add_to_pos(curr_pos, dir);
		}
		orient start_dir = dir;
		while (!cycle_finished)
		{
			if (valid_idx(curr_pos.first, curr_pos.second) && tiles.tiles[curr_pos.first][curr_pos.second]->get_activation_level() == 2)
			{
				if (!visited[curr_pos.first][curr_pos.second].visited)
				{
					visit_count++;
					visited[curr_pos.first][curr_pos.second].visited = true;
					pair<grid_pos, grid_pos> either_pos = get_either_side_posses(curr_pos, dir);
					for (int count = 0; count < 2; count++)
					{
						grid_pos side_pos;
						if (count == 0)
							side_pos = either_pos.first;
						else
							side_pos = either_pos.second;
						if (valid_idx(side_pos.first, side_pos.second) &&
							(tiles.tiles[side_pos.first][side_pos.second]->get_activation_level() == 2) &&
							!visited[side_pos.first][side_pos.second].visited)
						{
							orient new_dir;
							if ((count == 0))
							{
								if (dir == RIGHT)
									new_dir = UP;
								else
									new_dir = DOWN;
							}
							else if (count == 1)
							{
								if (dir == UP)
									new_dir = LEFT;
								else
									new_dir = RIGHT;
							}
							visited[curr_pos.first][curr_pos.second].dirs_not_taken.push_back(new_dir);
						}
					}
					if (visited[curr_pos.first][curr_pos.second].dirs_not_taken.size() > 0)
					{
						unfinished.push_back(curr_pos);
					}
				}
				else
				{
					sub_from_pos(curr_pos, dir);
					int temp_dir = (int)dir - 1;
					if (temp_dir == -1)
					{
						temp_dir = 3;
					}
					dir = (orient)temp_dir;
					if (dir == start_dir)
					{
						cycle_finished = true;
					}
				}
			}
			else
			{
				sub_from_pos(curr_pos, dir);
				int temp_dir = (int)dir - 1;
				if (temp_dir == -1)
				{
					temp_dir = 3;
				}
				dir = (orient)temp_dir;
				if (dir == start_dir)
				{
					cycle_finished = true;
				}
			}
			add_to_pos(curr_pos, dir);
		}
	}
	if (visit_count < active_count)
	{
		return false;
	}
	else
	{
		return true;
	}
}


bool_2d design_grid::get_bool_arr()
{
	bool_2d bool_arr = deque< deque<bool> >(num_rows, deque<bool>(num_cols));

	for (int row_num = 0; row_num < num_rows; row_num++)
	{
		for (int col_num = 0; col_num < num_cols; col_num++)
		{
			if (tiles.tiles[row_num][col_num]->get_activation_level() == 2)
			{
				bool_arr[row_num][col_num] = true;
			}
			else
			{
				bool_arr[row_num][col_num] = false;
			}
		}
	}
	return bool_arr;
}


/*
 * move this function to misc_types.h and misc.cpp for implementation?
 */
string design_grid::next_design_string()
{
	boost::filesystem::path design_path("designs/");

	if (!exists(design_path))
	{
		if(!boost::filesystem::create_directory("designs/"))
		{
			cout << "error creating designs directory. design not saved." << endl;
			return string();
		}
		return "design1.dsn";
	}

	boost::filesystem::directory_iterator dir_iter(design_path), end;

	vector<int> file_numbers;

	for (; dir_iter != end; advance(dir_iter, 1))
	{
		int num = get_design_num(dir_iter->path());
		if (num > 0)
			file_numbers.push_back(num);
	}

	sort(file_numbers.begin(), file_numbers.end());

	int last_num = 0;

	if (file_numbers.size() > 0)
	{
		last_num = *next(file_numbers.end(), -1);
		for (auto iter = next(file_numbers.begin(), 1); iter != file_numbers.end(); 
					advance(iter, 1))
		{
			if (*iter != *next(iter, -1) + 1)
			{
				last_num = *next(iter, -1);
				break;
			}
		}
	}
	return string("design" + to_string(last_num + 1) + ".dsn");
}


void design_grid::save_design()
{
	if (!design_valid())
	{
		cout << "Design invalid" << endl;
		return;
	}
	shape shape_obj(get_bool_arr());
	if (design_exists(shape_obj))
	{
		cout << "Design already exists" << endl;
		return;
	}
	ofstream out_file("designs/" + next_design_string());
	
	boost::archive::text_oarchive oa(out_file);
	oa << shape_obj;
}

void design_grid::quit_design()
{
	quit = true;
}


void design_grid::draw()
{
	tiles.draw();

	save_button->draw();
	quit_button->draw();

	SDL_RenderPresent(renderer);
}

void design_grid::update()
{
	tiles.update();
	save_button->update();
	quit_button->update();
}

void design_grid::set_new_on_color(SDL_Texture * color)
{
	tiles.apply_to_all([&color](clickable_tile &tile) {tile.set_on_texture(color); });
}

void design_grid::set_new_on_color(tile_colors colors)
{
	SDL_Texture * texture = make_texture_from_colors(colors, tiles.block_size, tiles.block_size, renderer);
	set_new_on_color(texture);
}

void design_grid::set_new_on_color(string color)
{
	set_new_on_color(textures.at(color));
}