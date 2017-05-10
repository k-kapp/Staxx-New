#ifndef BUTTON_H
#define BUTTON_H

#include "misc_types.h"
#include <functional>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string>

using namespace std;



class button
{
public:
	button()
	{}

	button(function<void()> callback) : callback(callback) 
	{}

	button(int x_pos, int y_pos, tile_colors off_colors, 
			function<void()> callback, string text, SDL_Renderer * renderer)
			: x_coord(x_pos), y_coord(y_pos), height(0), width(0), off_colors(off_colors),
				callback(callback), text(text), renderer(renderer), surface_message(nullptr),
				message(nullptr), mousebutton_down(false)
	{
		on_colors = off_colors;
		init_text();
		set_button_rects();
	}

	button(int x_pos, int y_pos, tile_colors off_colors, 
			tile_colors on_colors, function<void()> callback, string text, SDL_Renderer * renderer)
			: x_coord(x_pos), y_coord(y_pos), height(0), width(0), off_colors(off_colors),
				on_colors(on_colors), callback(callback), text(text), renderer(renderer), 
				surface_message(nullptr), message(nullptr), mousebutton_down(false)
	{
		init_text();
		set_button_rects();
	}
	
	button(int x_pos, int y_pos, tile_colors off_colors, 
			tile_colors on_colors, string text, SDL_Renderer * renderer)
			: x_coord(x_pos), y_coord(y_pos), height(0), width(0), off_colors(off_colors),
				on_colors(on_colors), callback(nullptr), text(text), renderer(renderer), 
				surface_message(nullptr), message(nullptr), mousebutton_down(false)
	{
		init_text();
		set_button_rects();
	}

	button &operator = (const button &other)
	{
		this->x_coord = other.x_coord;
		this->y_coord = other.y_coord;
		this->width = other.width;
		this->height = other.height;
		this->off_colors = other.off_colors;
		this->on_colors = other.on_colors;
		//this->callback(move(other.callback));
		this->callback = other.callback;
		this->text = other.text;
		this->renderer = other.renderer;
		this->button_border_rect = other.button_border_rect;
		this->button_base_rect = other.button_base_rect;
		this->surface_message = nullptr;
		this->message = nullptr;
		this->mousebutton_down = other.mousebutton_down;
		init_text();

		return *this;
	}

	button(const button &other)
	{
		this->x_coord = other.x_coord;
		this->y_coord = other.y_coord;
		this->width = other.width;
		this->height = other.height;
		this->off_colors = other.off_colors;
		this->on_colors = other.on_colors;
		//this->callback(move(other.callback));
		this->callback = other.callback;
		this->text = other.text;
		this->renderer = other.renderer;
		this->button_border_rect = other.button_border_rect;
		this->button_base_rect = other.button_base_rect;
		this->surface_message = nullptr;
		this->message = nullptr;
		this->mousebutton_down = other.mousebutton_down;
		init_text();
	}

	~button()
	{
		if (surface_message)
			SDL_FreeSurface(surface_message);
		if (message)
			SDL_DestroyTexture(message);
	}

	void init_text()
	{	
		font = TTF_OpenFont("fonts/ARCADECLASSIC.TTF", 30);
		TTF_SizeText(font, text.c_str(), &text_w, &text_h);
		width = text_w + 10;
		height = text_h + 10;
		surface_message = TTF_RenderText_Solid(font, text.c_str(), {255, 255, 255});
		message = SDL_CreateTextureFromSurface(renderer, surface_message);
		set_message_rect();
	}

	void set_message_rect()
	{
		message_rect = {x_coord + 5, y_coord + 5, text_w, text_h};
	}

	void set_button_rects()
	{
		button_border_rect = {x_coord, y_coord, width, height};
		button_base_rect = {x_coord + 1, y_coord + 1, width - 2, height - 2};
	}

	void set_position(int x, int y)
	{
		x_coord = x;
		y_coord = y;
		set_message_rect();
		set_button_rects();
	}

	void set_x(int x)
	{
		x_coord = x;
		set_message_rect();
		set_button_rects();
	}

	void set_y(int y)
	{
		y_coord = y;
		set_message_rect();
		set_button_rects();
	}

	void set_size(int width, int height)
	{
		this->width = width;
		this->height = height;
		set_message_rect();
		set_button_rects();
	}

	int get_x()
	{
		return x_coord;
	}

	int get_y()
	{
		return y_coord;
	}

	int get_width()
	{
		return width;
	}

	int get_height()
	{
		return height;
	}

	string get_text()
	{
		return text;
	}

	void set_callback(function<void()> callb)
	{
		callback = callb;
	}

	bool mouse_on_button()
	{
		SDL_GetMouseState(&mouse_x, &mouse_y);

		if ((mouse_x > x_coord) && (mouse_x < x_coord + width))
			if ((mouse_y > y_coord) && (mouse_y < y_coord + height))
				return true;
		return false;
	}

	void draw_button()
	{
		tile_colors * draw_colors;
		if (mouse_on_button())
		{
			draw_colors = &on_colors;
		}
		else
		{
			draw_colors = &off_colors;
		}

		SDL_SetRenderDrawColor(renderer, draw_colors->border.red, draw_colors->border.green, draw_colors->border.blue, 255);
		SDL_RenderFillRect(renderer, &button_border_rect);
		SDL_SetRenderDrawColor(renderer, draw_colors->base.red, draw_colors->base.green, draw_colors->base.blue, 255);
		SDL_RenderFillRect(renderer, &button_base_rect);
		
		//init_text();
		SDL_RenderCopy(renderer, message, NULL, &message_rect);

		//leave this to caller?
		//SDL_RenderPresent(renderer);
	}

	void draw()
	{
		draw_button();
	}

	void update()
	{
		int x, y;
		bool left_button_down = SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT);
		if (!left_button_down && mousebutton_down)
		{
			mousebutton_up();
		}
		mousebutton_down = left_button_down;
	}

		
	bool mousebutton_up()
	{
		if (mouse_on_button())
		{
			if (callback)
				callback();
			return true;
		}
	}

private:
	int x_coord, y_coord, width, height;
	int mouse_x, mouse_y;
	int text_w, text_h;
	tile_colors off_colors, on_colors;
	SDL_Rect button_border_rect, button_base_rect;
	function<void()> callback;
	SDL_Renderer * renderer;
	string text;
	TTF_Font * font;
	SDL_Surface * surface_message;
	SDL_Texture * message;
	SDL_Rect message_rect;
	bool mousebutton_down;
};

	
#endif
