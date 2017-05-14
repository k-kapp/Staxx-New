#include "../include/design_selection.h"
#include "../include/misc_types.h"
#include "../include/block.h"
#include "../include/grid.h"
#include "../include/scroll_grid.h"
#include "../include/usable_shapes.h"

#include <vector>
#include <functional>

using namespace std;

design_selection::design_selection(int grid_x, int grid_y)
	: game_state("Design selection", 100, 100, 800, 600), 
	scroller(2, 2, grid_x, grid_y, 20, 20, 120, textures.at("black"), renderer, textures),
	save_button(grid_x + (scroller.view_grid_width() - 100*2 - 50)/2, 
		scroller.view_grid_higher_y() + 50, 100, 50, textures.at("dark red"), textures.at("red"),
		textures.at("red"), 
		bind(&design_selection::save_selection, this),
		"Save", renderer),
	cancel_button(save_button.get_coords().first + save_button.get_size().first + 50, scroller.view_grid_higher_y() + 50, 100, 50, textures.at("dark red"), textures.at("red"),
		textures.at("red"), 
		bind(&design_selection::cancel, this), 
		"Cancel", renderer)
{
	init_selection();
}


void design_selection::init_selection()
{
	vector<shared_ptr<shape> > shapes = ::import_shapes();

	vector<block> blocks;

	for (auto &shape : shapes)
	{
		blocks.push_back(block(shape.get(), textures.at("red")));
		//blocks.push_back(block(shape.get(), { 200, 0, 0 }, { 50, 0, 0 }));
	}

	grid<clickable_tile> texture_generator(8, 8, textures.at("black"), textures.at("red"), 0, 0, 25, 25, 0, 0, renderer);

	for (int i = 0; i < blocks.size(); i++)
	{
		block &block = blocks.at(i);
		texture_generator.set_background(textures.at("black"));
		SDL_Texture * off_texture = texture_generator.create_texture(&block, true);
		texture_generator.set_background(textures.at("dark gray"));
		SDL_Texture * on_texture = texture_generator.create_texture(&block, true);
		//selectors.emplace_back(coords[i].first, coords[i].second, 300, 200, off_texture, off_texture, on_texture, renderer);
		selectors.push_back(make_shared<clickable_tile>(0, 0, 300, 200, off_texture, off_texture, on_texture, renderer));
		selectors.back()->set_sticky(true);
		if (i == 0)
			test_texture = on_texture;
	}
	scroller.add_tiles(selectors);
}

void design_selection::update()
{
	/*
	for (auto &selector_ptr : selectors)
	{
		selector_ptr->update();
	}
	*/

	save_button.update();
	cancel_button.update();

	scroller.update(pass_event);
}

void design_selection::draw()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	save_button.draw();
	cancel_button.draw();
	scroller.draw();

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