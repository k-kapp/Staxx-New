#include "../include/clickable_tile.h"
#include "../include/grid.h"
#include "../include/block.h"
#include "../include/common_surfaces.h"
#include <SDL2/SDL.h>
#include <iostream>


clickable_tile::clickable_tile(int x_coord, int y_coord, unsigned width, unsigned height, SDL_Texture * off_texture,
	SDL_Texture * on_texture, SDL_Renderer * renderer)
	: basic_tile(x_coord, y_coord, width, height, off_texture, on_texture, renderer)
{

}

clickable_tile::clickable_tile(int x_coord, int y_coord, unsigned width, unsigned height, SDL_Texture * off_texture,
	SDL_Texture * hover_texture, SDL_Texture * on_texture, SDL_Renderer * renderer)
	: clickable_tile(x_coord, y_coord, width, height, off_texture, on_texture, renderer)
{
	this->hover_texture = hover_texture;
}

clickable_tile::clickable_tile(const clickable_tile &other)
	: clickable_tile(other.x_coord, other.y_coord, other.width, other.height, other.off_texture, other.hover_texture, other.on_texture, other.renderer)
{
	sticky = other.sticky;
}

clickable_tile & clickable_tile::operator = (const clickable_tile &other)
{

	basic_tile::operator=(other);

	off_colors = other.off_colors;
	hover_colors = other.hover_colors;
	on_colors = other.on_colors;

	// state variables...perhaps not assign these to current object?
	mousebutton_down = other.mousebutton_down;
	mouse_x = other.mouse_x;
	mouse_y = other.mouse_y;
	activate_on_release = other.activate_on_release;
	sticky = other.sticky;

	return *this;
}

bool clickable_tile::mouse_on_button()
{
	SDL_GetMouseState(&mouse_x, &mouse_y);

	if ((mouse_x > x_coord) && (mouse_x < x_coord + width))
		if ((mouse_y > y_coord) && (mouse_y < y_coord + height))
			return true;
	return false;
}


void clickable_tile::draw(SDL_Texture * target)
{

	if (get_activation_level() == 1)
	{
		SDL_RenderCopy(renderer, hover_texture, NULL, &outer);
	}
	else
	{
		basic_tile::draw(target);
	}
}

void clickable_tile::update()
{
	int x, y;
	bool left_button_down = SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT);

	bool mouse_on_this = mouse_on_button();

	if (get_activation_level() != 2)
		set_activation_level(0);
	else if (!sticky)
		set_activation_level(0);

	if (activate_on_release && !left_button_down && mousebutton_down)
	{
		if (mouse_on_this)
		{
			if (sticky)
			{
				if (activation_level != 2)
				{
					set_activation_level(2);
				}
				else
				{
					set_activation_level(0);
				}
			}
			else
				set_activation_level(2);
		}
	}
	else if (!activate_on_release && left_button_down && !mousebutton_down)
	{
		if (mouse_on_this)
		{
			set_activation_level(2);
		}
	}

	if (!get_activation_level() && mouse_on_this)
	{
		set_activation_level(1);
	}


	mousebutton_down = left_button_down;
}

void clickable_tile::set_activate_on_release(bool setting)
{
	activate_on_release = setting;
}

void clickable_tile::set_hover_texture(SDL_Texture * hover_texture)
{
	this->hover_texture = hover_texture;
}

void clickable_tile::set_hover_colors(tile_colors colors)
{
	hover_colors = colors;
}

void clickable_tile::set_sticky(bool setting)
{
	sticky = setting;
}

void clickable_tile::reset_state()
{
	basic_tile::reset_state();
	mousebutton_down = false;
}