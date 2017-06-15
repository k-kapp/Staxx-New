#ifndef GRID_H
#define GRID_H

#include "misc_types.h"
#include <SDL2/SDL.h>
#include <vector>
#include <memory>
#include <random>

using namespace std;

class shape;
class block;

/*
 * The grid type is useful wherever a grid of tiles, each with the same type, is to be used. For this reason, the grid type is
 * templatised. It also contains a few basic utility functions, such as drawing the grid, applying a function to all grid tiles,
 * etc. This class could probably be further improved, still, since it contains certain functions that might not be useful for all
 * grids (such as the set_to_block function, for example).
 */

template<typename T>
class grid
{
public:
	std::vector< std::vector<std::shared_ptr<T> > > tiles;
	tile_colors off_colors, on_colors;
	SDL_Texture * off_texture, *on_texture;
	int x_offset, y_offset;
	int block_size;
	int border_size;
	int num_cols, num_rows;
	SDL_Renderer * renderer;
	bool game_over;

	grid()
	{
	}

	grid(int num_rows, int num_cols, SDL_Texture * off_texture, SDL_Texture * on_texture, int x_offset, 
			int y_offset, int block_size, int border_size, unsigned padding_horiz, unsigned padding_vert, SDL_Renderer * renderer)
			: num_rows(num_rows), num_cols(num_cols), x_offset(x_offset), y_offset(y_offset), block_size(block_size),
				border_size(border_size), renderer(renderer), off_texture(off_texture), on_texture(on_texture), 
				padding_horiz(padding_horiz), padding_vert(padding_vert)
	{
		/*
			see comment at grid object destructor below
		*/
		if (on_texture)
		{
			this->on_texture = copy_SDL_texture(on_texture, renderer);
		}
		else
		{
			this->on_texture = NULL;
		}

		if (off_texture)
		{
			this->off_texture = copy_SDL_texture(off_texture, renderer);
		}
		else
		{
			this->off_texture = NULL;
		}
		fill_grid();
	}

	grid(const grid &other)
	{
		num_rows = other.num_rows;
		num_cols = other.num_cols;
		on_colors = other.on_colors;
		off_colors = other.off_colors;
		x_offset = other.x_offset;
		y_offset = other.y_offset;
		block_size = other.block_size;
		border_size = other.border_size;
		renderer = other.renderer;

		SDL_Texture * prev_rendertarget = SDL_GetRenderTarget(renderer);
		SDL_SetRenderTarget(renderer, on_texture);
		SDL_RenderCopy(renderer, other.on_texture, NULL, NULL);
		SDL_SetRenderTarget(renderer, off_texture);
		SDL_RenderCopy(renderer, other.off_texture, NULL, NULL);
		SDL_SetRenderTarget(renderer, prev_rendertarget);
		copy_grid(other);
	}

	~grid()
	{
		/*
			this destruction of textures is slightly debatable. Another option would be
			to have all necessary textures in one central location, then simply do DestroyRenderer
			when the renderer is not necessary anymore. That is, the grid object does not "own" the textures
			and is thus not responsible for destroying them. On the other hand, this might possibly introduce
			memory leaks if the program quits unexpectedly.
		*/
		if (on_texture)
			SDL_DestroyTexture(on_texture);
		if (off_texture)
			SDL_DestroyTexture(off_texture);
	}

	grid &operator = (const grid &other)
	{
		num_rows = other.num_rows;
		num_cols = other.num_cols;
		off_colors = other.off_colors;
		on_colors = other.on_colors;
		x_offset = other.x_offset;
		y_offset = other.y_offset;
		block_size = other.block_size;
		border_size = other.border_size;
		renderer = other.renderer;

		on_texture = other.on_texture;
		off_texture = other.off_texture;

		copy_grid(other);

		return *this;
	}

	shared_ptr<T> & operator() (unsigned row_idx, unsigned col_idx)
	{
		return tiles[row_idx][col_idx];
	}

	bool update()
	{
		for (auto &row : tiles)
		{
			for (auto &tile_ptr : row)
			{
				tile_ptr->update();
			}
		}

		return true;
	}

	bool draw(SDL_Texture * target = nullptr)
	{
		SDL_Texture * prev_target = SDL_GetRenderTarget(renderer);
		SDL_SetRenderTarget(renderer, target);
		for (auto &row_vec : tiles)
		{
			for (auto &tl : row_vec)
			{
				tl->draw(target);
			}
		}
		SDL_SetRenderTarget(renderer, prev_target);
		return true;
	}

	void print_grid(shared_ptr<block> active_block)
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

	void copy_grid(const grid &other)
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
				tiles[row][col] = make_shared<T>(*other.tiles[row][col]);
			}
		}
	}

	void fill_grid()
	{
		tiles.reserve(num_rows + 1);
		for (int i = 0; i < num_rows; i++)
		{
			tiles.push_back(vector<shared_ptr<T> >());
			tiles.back().reserve(num_cols + 2);
		}

		int i = 0;
		for (auto row_iter = tiles.begin(); row_iter != tiles.end(); advance(row_iter, 1), i++)
		{
			int j = 0;
			for (int j = 0; j < num_cols; j++)
			{
				row_iter->push_back(make_shared<T>(x_offset + padding_horiz*(1 + 2*j) + j * block_size, 
					y_offset + padding_vert*(1 + 2*i) + i * block_size, block_size, block_size, off_texture, on_texture,
					renderer));
			}
		}
	}

	void fill_grid(const vector<shared_ptr<T> > &new_tiles)
	{
		auto new_tile_iter = new_tiles.begin();
		for (auto &row : tiles)
		{
			for (auto &tile_ptr : row)
			{
				auto coords = tile_ptr->get_coords();
				auto size = tile_ptr->get_size();
				tile_ptr = *new_tile_iter;
				tile_ptr->set_coords(coords.first, coords.second);
				tile_ptr->set_size(size.first, size.second);
				std::advance(new_tile_iter, 1);
			}
		}
	}

	void set_all_inactive()
	{
		for (auto &row : tiles)
		{
			for (auto &tile : row)
			{
				tile->set_activation_level(0);
			}
		}
	}

	void set_to_block(block * block_ptr, unsigned row_num, unsigned col_num)
	{
		unsigned block_width = block_ptr->get_width();
		unsigned block_height = block_ptr->get_height();

		for (unsigned block_row = 0; block_row < block_height; block_row++)
		{
			for (unsigned block_col = 0; block_col < block_width; block_col++)
			{
				bool active = (*block_ptr)(block_row, block_col);
				unsigned short activ_level;
				if (active)
					activ_level = 2;
				else
					activ_level = 0;
				tiles.at(block_row + row_num).at(block_col + col_num)->set_activation_level(activ_level);
				tiles.at(block_row + row_num).at(block_col + col_num)->set_on_texture(block_ptr->get_texture());
			}
		}
	}

	void apply_to_all(function<void(T &)> func)
	{
		for (auto &row : tiles)
		{
			for (auto &tile : row)
			{
				func(*tile);
			}
		}
	}

	SDL_Texture * create_texture(block * block_ptr, bool centered)
	{

		SDL_Texture * texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, num_cols * block_size, num_rows * block_size);

		set_all_inactive();

		if (centered)
			center_shape(block_ptr);

		draw(texture);

		return texture;
	}

	void set_background(SDL_Texture * color_texture)
	{
		off_texture = color_texture;

		apply_to_all([this](T &tile) {tile.set_off_texture(off_texture); });
	}

	void set_background(tile_colors color)
	{
		set_background(make_texture_from_colors(color, block_size, block_size, renderer));
	}

	int get_x()
	{
		return x_offset;
	}

	int get_y()
	{
		return y_offset;
	}

	unsigned get_width()
	{
		return num_cols * (2 * padding_horiz + block_size);
	}

	unsigned get_height()
	{
		return num_rows * (2 * padding_vert + block_size);
	}

	void set_coords(int x_coord, int y_coord)
	{
		int i = 0;
		for (auto row_iter = tiles.begin(); row_iter != tiles.end(); advance(row_iter, 1), i++)
		{
			int j = 0;
			for (int j = 0; j < num_cols; j++)
			{
				row_iter->set_coords(x_offset + padding_horiz*(1 + 2 * j) + j * block_size,
					y_offset + padding_vert * (1 + 2 * i) + i * block_size);
			}
		}
	}

protected:

	void center_shape(block * block_ptr)
	{
		unsigned block_width = block_ptr->get_width();
		unsigned block_height = block_ptr->get_height();

		if (block_width > num_cols || block_height > num_rows)
		{
			return;
		}

		unsigned start_col = (num_cols - block_width) / 2;
		unsigned start_row = (num_rows - block_height) / 2;

		set_to_block(block_ptr, start_row, start_col);
	}


private:

	unsigned padding_horiz;
	unsigned padding_vert;
};









#endif
