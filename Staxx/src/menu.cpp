#include "../include/menu.h"
#include "../include/button.h"
#include "../include/misc_types.h"
#include "../include/constants.h"

#include <SDL2/SDL_ttf.h>
#include <vector>
#include <tuple>

#include <iostream>

using namespace std;


menu::menu(unsigned width, unsigned height, string heading, unsigned heading_width, unsigned heading_height,
			unsigned buttons_width, unsigned buttons_height)
	: width(width), height(height), heading_str(heading), game_state("Main menu", 100, 100, width, height),
		buttons_width(buttons_width), buttons_height(buttons_height)
{
	heading_rect = { ((int)width - (int)heading_width) / 2, (int)heading_top_spacing, (int)heading_width, (int)heading_height };
	setup_heading();
}


menu::~menu()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}

void menu::set_buttons_size(unsigned width, unsigned height)
{
	buttons_width = width;
	buttons_height = height;

	for (auto &bt : buttons)
	{
		bt.set_size(width, height);
	}
}

void menu::add_button(string text, function<void()> callback)
{
	pair<int, int> coords, size;
	if (buttons.size() > 0)
	{
		coords = buttons.back().get_coords();
	}
	else
	{
		coords = { heading_rect.x, heading_rect.y + heading_rect.h + button_vert_spacing };
	}

	coords.second += size.second + button_vert_spacing;

	buttons.emplace_back(coords.first, coords.second, buttons_width, buttons_height, button_off_col, button_hover_col, button_on_col,
		callback, text, renderer);
}

void menu::set_buttons_colors(tile_colors off_color, tile_colors hover_color, tile_colors on_color)
{
	button_off_col = off_color;
	button_hover_col = hover_color;
	button_on_col = on_color;

	for (auto &bt : buttons)
	{
		bt.set_off_colors(button_off_col);
		bt.set_hover_colors(button_hover_col);
		bt.set_on_colors(button_on_col);
	}
}

void menu::set_buttons_colors(string off_color, string hover_color, string on_color)
{
	for (auto &bt : buttons)
	{
		bt.set_off_texture(textures.at(off_color));
		bt.set_hover_texture(textures.at(hover_color));
		bt.set_on_texture(textures.at(on_color));
		cout << endl;
	}
}

void menu::set_heading_pos(int x, int y)
{
	heading_rect.x = x;
	heading_rect.y = y;
}

void menu::set_heading_size(TTF_Font * font)
{
	int w, h;
	TTF_SizeText(font, heading_str.c_str(), &w, &h);
	heading_rect.w = w;
	heading_rect.h = h;
}

void menu::set_heading(string heading_str)
{
	this->heading_str = heading_str;
	TTF_Font * font = TTF_OpenFont("fonts/ARCADECLASSIC.TTF", 30);
	heading_surface = TTF_RenderText_Solid(font, heading_str.c_str(), {255, 255, 255});
	heading_texture = SDL_CreateTextureFromSurface(renderer, heading_surface);
	set_heading_size(font);
}

void menu::setup_heading()
{
	TTF_Font * font = TTF_OpenFont("fonts/ARCADECLASSIC.TTF", 30);
	heading_surface = TTF_RenderText_Solid(font, heading_str.c_str(), {255, 255, 255});
	heading_texture = SDL_CreateTextureFromSurface(renderer, heading_surface);
}

void menu::set_heading_rect(SDL_Rect rect)
{
	heading_rect = rect;
}


void menu::set_position(int x, int y)
{
	this->x = x;
	this->y = y;
}

void menu::set_size(int width, int height)
{
	this->width = width;
	this->height = height;
}

void menu::align_buttons_left(int x)
{
	for (auto & b : buttons)
	{
		b.set_coords(x, b.get_coords().second);
	}
}

void menu::align_buttons_vertical(int vert_gap)
{
	button &top_button = buttons.front();
	int top_y = top_button.get_coords().second;
	int top_height = top_button.get_size().second;
	int required_y = top_y + top_height + vert_gap;
	
	for (auto bt_iter = next(buttons.begin(), 1); bt_iter != buttons.end(); advance(bt_iter, 1))
	{
		if (required_y != bt_iter->get_coords().second)
		{
			bt_iter->set_coords(bt_iter->get_coords().first, required_y);
		}
		required_y = bt_iter->get_coords().second + bt_iter->get_size().second + vert_gap;
	}
}

void menu::align_buttons_heading_vert(int vert_gap)
{
	buttons.front().set_coords(buttons.front().get_coords().first, heading_rect.y + heading_rect.h + vert_gap);
	align_buttons_vertical(vert_gap);
}

void menu::center_buttons(int x)
{
	for (auto &bt : buttons)
	{
		bt.set_coords(x - width/2, bt.get_coords().second);
	}
}

void menu::align_buttons_heading_horiz()
{
	for (auto &bt : buttons)
	{
		bt.set_coords(heading_rect.x + heading_rect.w / 2 - bt.get_size().first / 2, bt.get_coords().second);
	}
}

void menu::resize_buttons(int width, int height)
{
	for (auto &bt : buttons)
	{
		bt.set_size(width, height);
	}
}

void menu::show_menu()
{
	reset_buttons_states();
	SDL_ShowWindow(window);
}

void menu::hide_menu()
{
	SDL_HideWindow(window);
}

void menu::draw_buttons()
{
	for (auto &bt : buttons)
	{
		bt.draw();
	}
}

void menu::update_buttons()
{
	for (auto &bt : buttons)
	{
		bt.update();
	}
}

void menu::draw_heading()
{
	SDL_RenderCopy(renderer, heading_texture, NULL, &heading_rect);
}

void menu::draw_menu()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	draw_buttons();
	draw_heading();
	SDL_RenderPresent(renderer);
}

void menu::exit()
{
	quit = true;
}


int menu::mainloop()
{
	SDL_Event event;

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
					switch (event.key.keysym.sym)
					{
						case (SDLK_ESCAPE):
						{
							quit = true;
						} break;
						default:
						{}
					}
				}break;
				default:
				{}
			}
		}
	}
	return 0;
}

void menu::draw()
{
	draw_menu();
	draw_heading();
	draw_buttons();
}

void menu::update()
{
	update_buttons();
}

void menu::reset_buttons_states()
{
	for (auto &bt : buttons)
	{
		bt.reset_state();
	}
}