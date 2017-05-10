#pragma once

#include "clickable_tile.h"
#include "misc_types.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <functional>
#include <string>

SDL_Texture * make_button_texture(unsigned width, unsigned height, std::string text, tile_colors background_colors,
									SDL_Renderer * renderer);

class button : public clickable_tile
{
public:
	//button(int x_coord, int y_coord, unsigned width, unsigned height, tile_colors back_colors, tile_colors fore_colors,
	//	tile_colors active_colors, std::function<void()> callback, SDL_Renderer * renderer);

	button(int x_coord, int y_coord, unsigned width, unsigned height, tile_colors back_colors,
		tile_colors fore_colors, tile_colors active_colors, function<void()> callback, std::string text, SDL_Renderer * renderer);

	button(int x_coord, int y_coord, unsigned width, unsigned height, SDL_Texture * off_texture, SDL_Texture * hover_texture,
		SDL_Texture * on_texture, std::function<void()> callback, SDL_Renderer * renderer);

	button(int x_coord, int y_coord, unsigned width, unsigned height, SDL_Texture * off_texture, SDL_Texture * hover_texture,
		SDL_Texture * on_texure, std::function<void()> callback, std::string text, SDL_Renderer * renderer);

	button(button &&other) noexcept;
	//button(button &other);

	button & operator=(button &&other) noexcept;
	//button &operator=(button &other);

	~button();

	virtual void update() override;
	//virtual void draw(SDL_Texture * target = nullptr) override;

	void set_text(std::string text);

	string get_text();

	void set_callback(function<void()> callback);

	void set_message_rect();

	void fix_text_size();

	void set_size(unsigned width, unsigned height) override;

	void set_off_texture(SDL_Texture * texture) override;
	void set_hover_texture(SDL_Texture * texture) override;
	void set_on_texture(SDL_Texture * texture) override;

protected:
	void init_text();

	std::function<void()> callback;
	std::string text;
	SDL_Texture * text_texture = NULL;
	TTF_Font * font = NULL;
	SDL_Rect message_rect;

	int text_w, text_h;

	int text_buffer_x = 5;
	int text_buffer_y = 5;
};