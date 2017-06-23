#include "../include/design_selection.h"
#include "../include/misc_types.h"
#include "../include/block.h"
#include "../include/grid.h"
#include "../include/scroll_grid.h"
#include "../include/usable_shapes.h"
#include "../include/constants.h"
#include "../include/misc_types.h"

#include <vector>
#include <functional>

using namespace std;

design_selection::design_selection(int grid_x, int grid_y)
	: game_state("Design selection", 100, 100, DESIGN_SELECT_SCREEN_WIDTH, DESIGN_SELECT_SCREEN_HEIGHT),
	scroller(2, 2, grid_x, grid_y, 20, 20, DESIGN_SELECT_BLOCK_SIZE, textures.at("black"), renderer,
		DESIGN_SELECT_SCROLLER_SIZE, textures),

	save_button(grid_x + (scroller.get_view_grid_width() - 100 * 2 - 50) / 2,
		scroller.get_view_grid_higher_y() + 50,
		100,
		50,
		textures.at("dark red"),
		textures.at("red"),
		textures.at("red"),
		bind(&design_selection::save_selection, this),
		"Save", renderer),

	cancel_button(save_button.get_coords().first + save_button.get_size().first + 50,
		scroller.get_view_grid_higher_y() + 50,
		100,
		50,
		textures.at("dark red"),
		textures.at("red"),
		textures.at("red"),
		bind(&design_selection::cancel, this),
		"Cancel", renderer),

	delete_button((DESIGN_SELECT_SCREEN_WIDTH - scroller.get_higher_x()) / 2 - 50 + scroller.get_higher_x(),
		DESIGN_SELECT_SCREEN_HEIGHT / 4,
		100,
		50,
		textures.at("dark red"),
		textures.at("red"),
		textures.at("red"),
		bind(&design_selection::delete_first_click, this),
		"Delete", renderer)
{
	init_selection();

	TTF_Font * arcade_font = TTF_OpenFont("fonts/ARCADECLASSIC.TTF", 20);

	int open_width = DESIGN_SELECT_SCREEN_WIDTH - scroller.get_higher_x();

	confirm_text_rects_textures = get_wrapped_text_rects_textures("DELETE THE SELECTED SHAPES?", arcade_font,
		renderer, delete_button.get_coords().first + delete_button.get_size().first / 2 - open_width / 3, 
		delete_button.get_coords().second + delete_button.get_size().second + 35,
		open_width * 3 / 4, 40, 2);
}


void design_selection::init_selection()
{
	vector<shared_ptr<shape> > shapes = ::import_shapes();

	vector<block> blocks;

	for (auto &shape : shapes)
	{
		blocks.push_back(block(shape.get(), textures.at("red")));
	}

	grid<clickable_tile> texture_generator(8, 8, textures.at("black"), textures.at("red"), 0, 0, 25, 25, 0, 0, renderer);

	for (int i = 0; i < blocks.size(); i++)
	{
		block &block = blocks.at(i);
		texture_generator.set_background(textures.at("black"));
		SDL_Texture * off_texture = texture_generator.create_texture(&block, true);
		texture_generator.set_background(textures.at("dark gray"));
		SDL_Texture * on_texture = texture_generator.create_texture(&block, true);
		selectors.push_back(make_shared<clickable_tile>(0, 0, 300, 200, off_texture, off_texture, on_texture, renderer));
		selectors.back()->set_sticky(true);
		if (i == 0)
			test_texture = on_texture;
	}
	scroller.add_tiles(selectors);
}

void design_selection::update()
{

	save_button.update();
	cancel_button.update();
	delete_button.update();

	scroller.update(pass_event);
}

void design_selection::draw()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	save_button.draw();
	cancel_button.draw();
	delete_button.draw();
	scroller.draw();

	if (draw_confirm_delete_text)
	{
		for (pair<SDL_Rect, SDL_Texture *> &rect_texture : confirm_text_rects_textures)
		{
			SDL_RenderCopy(renderer, rect_texture.second, NULL, &rect_texture.first);
		}
	}

	SDL_RenderPresent(renderer);
}

void design_selection::cancel()
{
	quit = true;
}

void design_selection::save_selection()
{
	usable_shapes.clear();

	for (int i = 0; i < selectors.size(); i++)
	{
		shared_ptr<clickable_tile> &tile_ptr = selectors.at(i);
		if (tile_ptr->get_activation_level() == 2)
		{
			usable_shapes.push_back(all_shapes.at(i));
		}
	}

	quit = true;
}

void design_selection::delete_first_click()
{
	draw_confirm_delete_text = true;
}

void design_selection::mainloop()
{
	quit = false;
	while (!quit)
	{

		if (SDL_PollEvent(&sdl_event))
		{
			pass_event = &sdl_event;
			if (sdl_event.type == SDL_KEYUP)
			{
				if (sdl_event.key.keysym.sym == SDLK_ESCAPE)
				{
					quit = true;
					break;
				}
				if (sdl_event.key.keysym.sym == SDLK_DOWN)
				{
					cout << "down key pressed in game state class" << endl;
				}
				if (sdl_event.key.keysym.sym == SDLK_UP)
				{
					cout << "up key pressed in game state class" << endl;
				}
			}
		}
		else
		{
			pass_event = nullptr;
		}


		update();
		draw();
	}
}