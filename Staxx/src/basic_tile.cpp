#include "../include/basic_tile.h"
#include <iostream>
#include "../include/constants.h";

using namespace std;

basic_tile::basic_tile(int x_coord, int y_coord, unsigned width, unsigned height, SDL_Texture * off_texture, SDL_Texture * on_texture,
						SDL_Renderer * renderer)
	: x_coord(x_coord), y_coord(y_coord), width(width), height(height), off_texture(off_texture), on_texture(on_texture), renderer(renderer)
{
	set_outer_rect();
}

basic_tile::basic_tile(const basic_tile &other) 
	: basic_tile(other.x_coord, other.y_coord, other.width, other.height, other.off_texture, other.on_texture, other.renderer)
{
}

basic_tile& basic_tile::operator=(const basic_tile &other)
{
	x_coord = other.x_coord;
	y_coord = other.y_coord;
	outer = other.outer;
	width = other.width;
	height = other.height;
	renderer = other.renderer;

	return *this;
}


void basic_tile::set_outer_rect()
{
	auto coords_pair = get_coords();

	outer.x = coords_pair.first;
	outer.y = coords_pair.second;
	outer.h = height;
	outer.w = width;
}

pair<int, int> basic_tile::get_coords() const
{
	return{ x_coord, y_coord };
}

void basic_tile::set_coords(int x_coord, int y_coord)
{
	this->x_coord = x_coord;
	this->y_coord = y_coord;

	set_outer_rect();
}

void basic_tile::set_size(unsigned width, unsigned height)
{
	this->height = height;
	this->width = width;

	set_outer_rect();
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
	This 'target' parameter is for when the tile must be drawn to a rendertarget
	other than the default. This rendertarget gets specified by the caller of the
	object, and the caller is also responsible for restoring the original target
	after rendering.
*/
void basic_tile::draw(SDL_Texture * target)
{
	if (activation_level)
		SDL_RenderCopy(renderer, on_texture, NULL, &outer);
	else
		SDL_RenderCopy(renderer, off_texture, NULL, &outer);
}


/*
	derived classes will implement this function, since basic_tile fundamentally does not
	serve a specific purpose. This makes derived classes more flexible, since they don't need
	to deal with behaviours in the base class regarding the 'update' function.
*/
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