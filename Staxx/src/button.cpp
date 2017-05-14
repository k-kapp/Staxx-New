#include "../include/button.h"
#include "../include/misc_types.h"
#include "../include/constants.h"
#include "../include/common_surfaces.h"

#include <functional>
#include <string>

#include <iostream>

#include <sstream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

using namespace std;


SDL_Texture * make_button_texture(unsigned width, unsigned height, std::string text, SDL_Texture * background_texture,
									SDL_Renderer * renderer)
{
	if (text.size() == 0)
		return background_texture;

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	//where is SDL_PIXELFORMAT_RGBA32???
	SDL_Texture * texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
	TTF_Font * font = TTF_OpenFont(FONT_FILEPATH, 17);

	int font_width, font_height;
	TTF_SizeText(font, text.c_str(), &font_width, &font_height);

	font_height *= 3;

	if ((unsigned)font_width > 0.8 * width)
	{
		font_width = 0.8 * width;
	}
	if ((unsigned)font_height > height)
	{
		font_height = height;
	}

	SDL_Surface * text_surface = TTF_RenderText_Solid(font, text.c_str(), { 255, 255, 255 });
	SDL_Surface * scaled_surface = SDL_CreateRGBSurface(0, font_width, font_height, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);

	SDL_BlitScaled(text_surface, NULL, scaled_surface, NULL);
	//SDL_FreeSurface(text_surface);

	SDL_Texture * text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);

	SDL_SetRenderTarget(renderer, texture);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	SDL_Rect dest_rect;
	dest_rect.w = scaled_surface->w;
	dest_rect.h = scaled_surface->h;
	dest_rect.x = (width - scaled_surface->w)/2;
	dest_rect.y = (height - scaled_surface->h)/2;
	//SDL_FreeSurface(scaled_surface);

	SDL_RenderCopy(renderer, background_texture, NULL, NULL);
	SDL_RenderCopy(renderer, text_texture, NULL, &dest_rect);
	//SDL_RenderCopy(renderer, text_texture, NULL, NULL);
	SDL_SetRenderTarget(renderer, NULL);

	SDL_FreeSurface(scaled_surface);
	SDL_FreeSurface(text_surface);
	SDL_DestroyTexture(text_texture);

	return texture;
}

SDL_Texture * make_button_texture(unsigned width, unsigned height, std::string text, tile_colors background_colors,
									SDL_Renderer * renderer)
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	SDL_Texture * background_texture = make_texture_from_colors(background_colors, width, height, renderer);

	return make_button_texture(width, height, text, background_texture, renderer);

	SDL_DestroyTexture(background_texture);
}

button::button(int x_coord, int y_coord, unsigned width, unsigned height, SDL_Texture * off_texture, SDL_Texture * hover_texture,
	SDL_Texture * on_texture, std::function<void()> callback, std::string text, SDL_Renderer * renderer)
	: button(x_coord, y_coord, width, height,
		make_button_texture(width, height, text, off_texture, renderer),
		make_button_texture(width, height, text, hover_texture, renderer),
		make_button_texture(width, height, text, on_texture, renderer),
		callback, renderer)
{

}

button::button(int x_coord, int y_coord, unsigned width, unsigned height, SDL_Texture * off_texture,
	SDL_Texture * hover_texture, SDL_Texture * on_texture, function<void()> callback, SDL_Renderer * renderer)
	: clickable_tile(x_coord, y_coord, width, height, off_texture, hover_texture, on_texture, renderer), callback(callback)

{
	//must_click_on_this = true;
	activate_on_release = true;
}


button::button(int x_coord, int y_coord, unsigned width, unsigned height, tile_colors back_colors,
	tile_colors fore_colors, tile_colors active_colors, function<void()> callback, std::string text, SDL_Renderer * renderer)
	: button(x_coord, y_coord, width, height, 
		make_button_texture(width, height, text, back_colors, renderer), 
		make_button_texture(width, height, text, fore_colors, renderer), 
		make_button_texture(width, height, text, active_colors, renderer), 
		callback, renderer)
{
	std::cout << "text_buffer_x in ctor: " << text_buffer_x << endl;
	//set_text(text);
	this->text = text;
	std::cout << "exiting ctor" << endl;
}

button::button(button &&other) noexcept
	: button(other.x_coord, other.y_coord, other.width, other.height, other.off_texture, other.hover_texture, other.on_texture, other.callback, other.renderer)
{
	other.text_texture = nullptr;
	text = other.text;
}

/*
button::button(button &other)
	: button(move(other))
{
	cout << "cannot call button copy constructor" << endl;
	throw new exception();
}
*/

button &button::operator=(button &&other) noexcept
{

	other.text_texture = nullptr;
	clickable_tile::operator=(other);
	callback = other.callback;
	text = other.text;
	hover_texture = other.hover_texture;

	return *this;
}

/*
button &button::operator=(button &other)
{
	cout << "cannot call button copy assignment operator" << endl;
	throw new exception();
	return *this;
}
*/

button::~button()
{
	std::cout << "calling button destructor" << endl;
	if (text_texture)
		SDL_DestroyTexture(text_texture);
}

void button::update()
{
	clickable_tile::update();

	if (activation_level == 2)
	{
		activation_level = 0;
		callback();
	}
}

void button::set_size(unsigned width, unsigned height)
{
	basic_tile::set_size(width, height);

	//fix_text_size();
	//set_message_rect();
}

void button::set_callback(function<void()> callback)
{
	this->callback = callback;
}


void button::set_off_texture(SDL_Texture * texture)
{
	off_texture = make_button_texture(width, height, text, texture, renderer);
}

void button::set_hover_texture(SDL_Texture * texture)
{
	hover_texture = make_button_texture(width, height, text, texture, renderer);
}

void button::set_on_texture(SDL_Texture * texture)
{
	on_texture = make_button_texture(width, height, text, texture, renderer);
}

/*
void button::draw(SDL_Texture * target)
{
	clickable_tile::draw(target);

	if (text_texture)
	{
		SDL_SetRenderTarget(renderer, target);

		set_message_rect();
		if (SDL_RenderCopy(renderer, text_texture, NULL, &message_rect))
		{
			ostringstream ostring;
			ostring << "Error rendering button text" << endl;
			ostring << "message_rect: x: " << to_string(message_rect.x) << ", y:" << to_string(message_rect.y) << ", w: " << to_string(message_rect.w)
				<< ", h: " << to_string(message_rect.h) << endl;
			if (!renderer)
			{
				cout << "renderer not initialized" << endl;
			}

			string err_string = ostring.str();

			cout << err_string << endl;

			throw new exception();
		}

		SDL_SetRenderTarget(renderer, nullptr);
	}
}
*/

/*
void button::init_text()
{	
	SDL_Surface * text_surface;

	font = TTF_OpenFont("fonts/ARCADECLASSIC.TTF", 30);
	TTF_SizeText(font, text.c_str(), &text_w, &text_h);

	int text_width = text_w + 10;
	int text_height = text_h + 10;

	fix_text_size();

	cout << "in init_texture" << endl;
	cout << "text buffer x: " << text_buffer_x << endl;
	cout << "text_w: " << text_w << endl;
	cout << "width: " << width << endl;

	text_surface = TTF_RenderText_Solid(font, text.c_str(), {255, 255, 255});
	text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
	set_message_rect();

	if (!text_texture)
	{
		cout << "Could not create text texture" << endl;
	}

	SDL_FreeSurface(text_surface);
	//SDL_DestroyTexture(text_texture);
}
*/

/*
void button::fix_text_size()
{
	TTF_SizeText(font, text.c_str(), &text_w, &text_h);

	if (text_w > (int)width - text_buffer_x*2)
	{
		text_w = (int)width - text_buffer_x*2;
	}
	else
	{
		text_buffer_x = ((int)width - text_w) / 2;
	}

	if (text_h > (int)height - text_buffer_y * 2)
	{
		text_h = (int)width - text_buffer_y * 2;
	}
	else
	{
		text_buffer_y = ((int)height - text_h) / 2;
	}

}
*/


/*
void button::set_message_rect()
{
	message_rect = {x_coord + text_buffer_x, y_coord + text_buffer_y, text_w, text_h};

	//cout << "message_rect: x: " << to_string(message_rect.x) << ", y:" << to_string(message_rect.y) << ", w: " << to_string(message_rect.w)
	//	<< ", h: " << to_string(message_rect.h) << endl;
}
*/

/*
void button::set_text(std::string text)
{
	this->text = text;
	init_text();
}
*/

/*
string button::get_text()
{
	return text;
}
*/

