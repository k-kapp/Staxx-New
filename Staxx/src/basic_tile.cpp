#include "../include/basic_tile.h"

#include <iostream>

#include "../include/constants.h";

using namespace std;


basic_tile::basic_tile(int x_coord, int y_coord, unsigned width, unsigned height, SDL_Texture * off_texture, SDL_Texture * on_texture,
						SDL_Renderer * renderer)
	: x_coord(x_coord), y_coord(y_coord), width(width), height(height), off_texture(off_texture), on_texture(on_texture), renderer(renderer)
{
	set_inner_outer();
}

/*
basic_tile::basic_tile(int x_coord, int y_coord, unsigned width, unsigned height, tile_colors back_colors, tile_colors fore_colors, SDL_Renderer * renderer) 
	: basic_tile(x_coord, y_coord, width, height, make_color_texture(back_colors), make_color_texture(fore_colors), renderer)
			//: x_coord(x_coord), y_coord(y_coord), width(width), height(height), off_colors(back_colors), on_colors(fore_colors), renderer(renderer)
{
}
*/

basic_tile::basic_tile(const basic_tile &other) 
	: basic_tile(other.x_coord, other.y_coord, other.width, other.height, other.off_texture, other.on_texture, other.renderer)
{
}

basic_tile& basic_tile::operator=(const basic_tile &other)
{
	x_coord = other.x_coord;
	y_coord = other.y_coord;
	inner = other.inner;
	outer = other.outer;
	width = other.width;
	height = other.height;
	renderer = other.renderer;

	return *this;
}


void basic_tile::set_inner_outer()
{
	auto coords_pair = get_coords();

	outer.x = coords_pair.first;
	outer.y = coords_pair.second;
	outer.h = height;
	outer.w = width;

	inner.x = outer.x + 1;
	inner.y = outer.y + 1;
	inner.w = outer.w - 2;
	inner.h = outer.h - 2;

}

pair<int, int> basic_tile::get_coords() const
{
	return{ x_coord, y_coord };
}

void basic_tile::set_coords(int x_coord, int y_coord)
{
	this->x_coord = x_coord;
	this->y_coord = y_coord;

	set_inner_outer();
}

void basic_tile::set_size(unsigned width, unsigned height)
{
	this->height = height;
	this->width = width;

	set_inner_outer();
}

std::pair<unsigned, unsigned> basic_tile::get_size()
{
	return{ width, height };
}

void basic_tile::set_off_texture(SDL_Texture * off_texture)
{
	this->off_texture = off_texture;
}

void basic_tile::set_on_texture(SDL_Texture * on_texture)
{
	this->on_texture = on_texture;
}

void basic_tile::set_off_colors(tile_colors colors)
{
	off_colors = colors;
}

void basic_tile::set_on_colors(tile_colors colors)
{
	on_colors = colors;
}

tile_colors basic_tile::get_on_colors()
{
	return on_colors;
}

tile_colors basic_tile::get_off_colors()
{
	return off_colors;
}

/*
void basic_tile::draw(SDL_Texture * target)
{
	SDL_SetRenderTarget(renderer, target);

	color * col;

	color base_col, border_col;

	if (activation_level)
	{
		base_col = on_colors.base;
		border_col = on_colors.border;
		//std::cout << "base col (activated): " << base_col.red << ", " << base_col.green << ", " << base_col.blue << endl;
	}
	else
	{
		base_col = off_colors.base;
		border_col = off_colors.border;

		//std::cout << "base col (inactive): " << base_col.red << ", " << base_col.green << ", " << base_col.blue << endl;
	}

	SDL_SetRenderDrawColor(renderer, border_col.red, border_col.green, border_col.blue, 255);
	SDL_RenderFillRect(renderer, &outer);
	SDL_SetRenderDrawColor(renderer, base_col.red, base_col.green, base_col.blue, 255);
	SDL_RenderFillRect(renderer, &inner);

	SDL_SetRenderTarget(renderer, nullptr);
}
*/

void basic_tile::draw(SDL_Texture * target)
{
	if (activation_level)
		SDL_RenderCopy(renderer, on_texture, NULL, &outer);
	else
		SDL_RenderCopy(renderer, off_texture, NULL, &outer);
}


void basic_tile::update()
{
	
}

void basic_tile::set_activation_level(unsigned level)
{
	activation_level = level;
}

void basic_tile::increase_activation_level()
{
	activation_level++;
}

void basic_tile::decrease_activation_level()
{
	activation_level--;
}

unsigned basic_tile::get_activation_level()
{
	return activation_level;
}

void basic_tile::reset_state()
{
	set_activation_level(0);
}