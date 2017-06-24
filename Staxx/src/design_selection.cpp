#include "../include/design_selection.h"
#include "../include/misc_types.h"
#include "../include/block.h"
#include "../include/grid.h"
#include "../include/scroll_grid.h"
#include "../include/usable_shapes.h"
#include "../include/constants.h"
#include "../include/misc_types.h"

#include <boost/range/combine.hpp>
#include <boost/filesystem.hpp>

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
		"Delete", renderer),

	confirm_button(delete_button.get_coords().first - 110,
		delete_button.get_coords().second + 50 + 250,
		100,
		50,
		textures.at("dark red"),
		textures.at("red"),
		textures.at("red"),
		bind(&design_selection::delete_selected_shapes, this),
		"Confirm", renderer),

	not_confirm_button(delete_button.get_coords().first + delete_button.get_size().first + 10,
		confirm_button.get_coords().second,
		100,
		50,
		textures.at("dark red"),
		textures.at("red"),
		textures.at("red"),
		bind(&design_selection::cancel_delete, this),
		"Cancel", renderer)
{
	init_selection();

	TTF_Font * arcade_font = TTF_OpenFont("fonts/ARCADECLASSIC.TTF", 20);

	int open_width = DESIGN_SELECT_SCREEN_WIDTH - scroller.get_higher_x();

	confirm_text_rects_textures = get_wrapped_text_rects_textures("DELETE THE SELECTED SHAPES?", arcade_font,
		renderer, delete_button.get_coords().first + delete_button.get_size().first / 2 - open_width / 3, 
		delete_button.get_coords().second + delete_button.get_size().second + 35,
		open_width * 3 / 4, 40, 2);

	noselect_text_rects_textures = get_wrapped_text_rects_textures("NO SHAPES SELECTED", arcade_font,
		renderer, delete_button.get_coords().first + delete_button.get_size().first / 2 - open_width / 3,
		delete_button.get_coords().second + delete_button.get_size().second + 35,
		open_width * 3 / 4, 40, 2);
}


void design_selection::init_selection()
{
	//vector<shared_ptr<shape> > shapes = ::imported_shapes();

	imported_shapes();

	vector<block> blocks;

	for (auto &shape : imported_shapes::shapes)
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

	if (show_confirm_delete_buttons)
	{
		confirm_button.update();
		not_confirm_button.update();
	}
}

void design_selection::draw()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	save_button.draw();
	cancel_button.draw();
	delete_button.draw();
	scroller.draw();

	if (show_confirm_delete_buttons)
	{
		confirm_button.draw();
		not_confirm_button.draw();
	}

	if (draw_confirm_delete_text)
	{
		for (pair<SDL_Rect, SDL_Texture *> &rect_texture : confirm_text_rects_textures)
		{
			SDL_RenderCopy(renderer, rect_texture.second, NULL, &rect_texture.first);
		}
	}

	if (draw_noselect_text)
	{
		for (pair<SDL_Rect, SDL_Texture *> &rect_texture : noselect_text_rects_textures)
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

void design_selection::update_selected_shapes()
{
	selected_shapes.clear();

	for (int i = 0; i < selectors.size(); i++)
	{
		shared_ptr<clickable_tile> &tile_ptr = selectors.at(i);
		if (tile_ptr->get_activation_level() == 2)
		{
			//selected_shapes.push_back(all_shapes.at(i));
			selected_shapes.push_back(imported_shapes::shapes.at(i));
		}
	}
}

void design_selection::save_selection()
{
	turn_all_text_off();
	if (noselect_notify_timer)
	{
		/*
		*  TODO: check possible threading issues here
		*/
		SDL_bool remove_result = SDL_RemoveTimer(noselect_notify_timer);
		noselect_notify_timer = 0;
	}

	update_selected_shapes();

	if (selected_shapes.size() == 0)
	{
		show_noselect_text(5000);
		return;
	}

	quit = true;
}

Uint32 design_selection::sdl_timer_textoff_callback(Uint32 interval, void * args)
{
	void ** args_arr = (void **)args;
	design_selection * this_ptr = (design_selection *)args_arr[0];
	notify_text_type * notify_text = (notify_text_type *)args_arr[1];

	this_ptr->turn_text_off(*notify_text);

	delete args_arr[1];
	free(args);

	return 0;
}

void design_selection::turn_text_off(notify_text_type notify_text)
{

	switch (notify_text)
	{
		case (CONFIRM_DELETE):
		{
			draw_confirm_delete_text = false;
		} break;
		case (NO_SELECTION):
		{
			draw_noselect_text = false;
		} break;
	}
}

void design_selection::turn_all_text_off()
{
	for (int i = CONFIRM_DELETE; i != NO_SELECTION; i++)
	{
		turn_text_off((notify_text_type)i);
	}
	turn_text_off(NO_SELECTION);
}

void design_selection::show_noselect_text(int delay)
{

	draw_noselect_text = true;

	notify_text_type * notify_text = new notify_text_type;
	*notify_text = NO_SELECTION;

	void ** arg_ptrs = (void **)malloc(sizeof(void *) * 2);
	arg_ptrs[0] = (void *)this;
	arg_ptrs[1] = (void *)notify_text;

	noselect_notify_timer = SDL_AddTimer(delay, design_selection::sdl_timer_textoff_callback, (void *)arg_ptrs);
}

int design_selection::count_selected()
{
	int num_selected = 0;
	for (int i = 0; i < selectors.size(); i++)
	{
		shared_ptr<clickable_tile> &tile_ptr = selectors.at(i);
		if (tile_ptr->get_activation_level() == 2)
		{
			num_selected++;
		}
	}
	return num_selected;
}

void design_selection::delete_first_click()
{
	turn_all_text_off();
	if (noselect_notify_timer)
	{
		/*
		*  TODO: check possible threading issues here
		*/
		SDL_bool remove_result = SDL_RemoveTimer(noselect_notify_timer);
		noselect_notify_timer = 0;
	}

	if (count_selected() == 0)
	{
		show_noselect_text(5000);
		return;
	}

	draw_confirm_delete_text = true;
	show_confirm_delete_buttons = true;
	delete_button_clicked = true;
}

void design_selection::delete_selected_shapes()
{
	cout << "in delete_selected_shapes callback" << endl;

	imported_shapes();
	update_selected_shapes();

	vector<string> delete_filepaths;

	cout << "size of imported_shapes::shapes: " << imported_shapes::shapes.size() << endl;
	cout << "size of imported_shapes::filenames: " << imported_shapes::filenames.size() << endl;

	cout << "number of selected shapes: " << selected_shapes.size() << endl;

	for (auto shape_ptr_selected : selected_shapes)
	{
		for (auto tup : boost::combine(imported_shapes::shapes, imported_shapes::filenames))
		{
			shared_ptr<shape> shape_ptr;
			string filepath;
			boost::tie(shape_ptr, filepath) = tup;
			if (*shape_ptr_selected == *shape_ptr)
			{
				delete_filepaths.push_back(filepath);
			}
		}
	}

	cout << "size of delete_filepaths: " << delete_filepaths.size() << endl;

	for (auto del_str : delete_filepaths)
	{
		cout << "filepath to be deleted: " << del_str << endl;
		boost::filesystem::path del_path(del_str);
		if (boost::filesystem::remove(del_path))
		{
			cout << "shape data at " << del_str << " removed successfully" << endl;
		}
		else
		{
			cout << "Error: shape data at " << del_str << " does not exist. No deletions done" << endl;
		}
	}

	show_confirm_delete_buttons = false;

	reload = true;
}

void design_selection::cancel_delete()
{
	draw_confirm_delete_text = false;
	show_confirm_delete_buttons = false;
}

int design_selection::mainloop()
{
	quit = false;
	while (!quit)
	{
		bool left_button_pressed = false;
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
			else if (sdl_event.type == SDL_MOUSEBUTTONUP)
			{
				/*
				* solution to remove dialog text when activity is detected.
				* very hackish solution. Hope to have something more solid in the future
				*/
				if (sdl_event.button.button == SDL_BUTTON_LEFT)
				{
					turn_all_text_off();
					if (noselect_notify_timer)
					{
						
						// TODO: check possible threading issues here
						
						SDL_bool remove_result = SDL_RemoveTimer(noselect_notify_timer);
						noselect_notify_timer = 0;
					}
					left_button_pressed = true;
				}
			}
		}
		else
		{
			pass_event = nullptr;
		}

		update();
		draw();

		/*
	    * part of solution to remove dialog text when activity is detected.
		* Very hackish solution. Hope to have something more solid in the future
		*/
		if (!delete_button_clicked && left_button_pressed)
			show_confirm_delete_buttons = false;

		delete_button_clicked = false;

		if (reload)
		{
			return 1;
		}
	}
	return 0;
}