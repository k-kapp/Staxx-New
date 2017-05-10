#include "../include/grid.h"
#include "../include/shape.h"
#include "../include/block.h"
#include "../include/misc_types.h"
#include "../include/game.h"
#include "../include/design_grid.h"
#include "../include/menu.h"
#include "../include/constants.h"
#include "../include/common_surfaces.h"
#include "../include/design_selection.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <memory>
#include <tuple>
#include <functional>
#include <fstream>

using namespace std;



void new_game(menu * parent)
{

	auto shapes = import_shapes();

	if (shapes.size() == 0)
		return;

	parent->hide_menu();

	int speed_inv = 700;
	game game_grid({ {0, 100, 0}, {0, 100, 0} }, { {255, 0, 0}, {200, 0, 0} });
	
	cout << "game address: " << &game_grid << endl;

	game_grid.add_shapes(shapes);
	game_grid.init_sample();

	game_grid.mainloop();

	parent->show_menu();
}

void new_design_grid(menu * parent)
{
	parent->hide_menu();

	auto grid = design_grid(8, { {0, 100, 0}, {100, 0, 100} }, { {0, 150, 0}, {150, 0, 150} }, { {0, 250, 0}, {250, 0, 250} },
		100, 100, DESIGN_BLOCK_SIZE, 1);
	grid.set_new_on_color("red");
	grid.mainloop();

	parent->show_menu();
}


void new_design_selection(menu * parent)
{
	parent->hide_menu();

	auto selection = design_selection(100, 100);
	selection.mainloop();

	parent->show_menu();
}

int main(int argc, char * argv [])
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER))
	{
		cout << "unable to initialize SDL" << endl;
		return 1;
	}

	if (TTF_Init() == -1)
	{
		cout << "TTF initialization failed" << endl;
	}

	init_surfaces(all_surfaces);

	SDL_Event event;

	bool exit_menu = false;

	menu main_menu(400, 600, "STAXX", 250, 100, 160, 80);

	/*
	tile_colors button_off = { {100, 0, 0}, {255, 255, 255} };
	tile_colors button_hover = { {225, 0, 0}, {255, 255, 255} };
	tile_colors button_on = { {255, 0, 0}, {255, 255, 255} };
	*/

	string button_off = "dark red";
	string button_hover = "red";
	string button_on = "red";


	main_menu.add_button("PLAY", bind(&new_game, &main_menu));
	main_menu.add_button("DESIGN", bind(&new_design_grid, &main_menu));
	main_menu.add_button("SELECT DESIGNS", bind(&new_design_selection, &main_menu));
	main_menu.add_button("QUIT", bind(&menu::exit, &main_menu));

	main_menu.set_buttons_colors(button_off, button_hover, button_on);

	main_menu.set_heading_rect({100, 100, 200, 200});
	main_menu.set_buttons_size(100, 50);
	main_menu.align_buttons_heading_vert(30);
	main_menu.align_buttons_heading_horiz();

	main_menu.mainloop();

	cout << "Quitting SDL..." << endl;
	SDL_Quit();

	return 0;
}


