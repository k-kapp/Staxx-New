#include "../include/constants.h"
#include "../include/common_surfaces.h"
#include "../include/misc_types.h"

#include <map>
#include <utility>

using namespace std;

void init_surfaces(common_surfaces &surfaces)
{
	all_surfaces.add_surface("dark red", { {100, 0, 0}, {50, 0, 0} });
	all_surfaces.add_surface("red", { {255, 0, 0}, {155, 0, 0} });
	all_surfaces.add_surface("sky_blue:lighter sky_blue", { { 0, 154, 205 }, { 0, 191, 255 } });
	all_surfaces.add_surface("blue", { {0, 0, 255}, {0, 0, 155} });
	all_surfaces.add_surface("green", { {0, 255, 0}, {0, 155, 0} });
	all_surfaces.add_surface("medium green", { {0, 155, 0}, {0, 55, 0} });
	all_surfaces.add_surface("dark green", { {0, 100, 0}, {0, 50, 0} });
	all_surfaces.add_surface("black", { {0, 0, 0}, {0, 0, 0} });
	all_surfaces.add_surface("black:gray", { {0, 0, 0}, {100, 100, 100} });
	all_surfaces.add_surface("dark gray", { {50, 50, 50}, {50, 50, 50} });
	all_surfaces.add_surface("gray", { {125, 125, 125}, {125, 125, 125} });
	all_surfaces.add_surface("light gray", { {200, 200, 200}, {200, 200, 200} });

	SDL_Surface * up_white_arrow = make_up_arrow(200, 200, { 255, 255, 255 });
	SDL_Surface * up_red_arrow = mult_copy_surface(up_white_arrow, { 255, 0, 0 });
	SDL_Surface * up_dark_red_arrow = mult_copy_surface(up_white_arrow, { 155, 0, 0 });

	auto red_arrow_rotations = get_all_rotations(up_red_arrow);
	auto dark_red_arrow_rotations = get_all_rotations(up_dark_red_arrow);

	for (int i = 0; i < red_arrow_rotations.size(); i++)
	{
		string prefix;
		switch (i)
		{
		case (0):
			prefix = "up ";
			break;
		case (1):
			prefix = "right ";
			break;
		case (2):
			prefix = "down ";
			break;
		case (3):
			prefix = "left ";
			break;
		default:
			break;
		}
		all_surfaces.add_surface(prefix + "arrow:red", red_arrow_rotations.at(i));
		all_surfaces.add_surface(prefix + "arrow:dark red", dark_red_arrow_rotations.at(i));
	}
}

bool common_surfaces::created = false;

common_surfaces::common_surfaces()
{
	if (created)
	{
		throw new exception("Common surfaces object already exists");
	}

	created = true;
}

common_surfaces::~common_surfaces()
{
	for (auto &key_val : surfaces)
	{
		SDL_FreeSurface(key_val.second);
	}
}

void common_surfaces::add_surface(string surface_name, SDL_Surface * new_surface)
{
	surfaces.insert(pair<string, SDL_Surface *>(surface_name, new_surface));
}

void common_surfaces::add_surface(string surface_name, tile_colors colors)
{
	surfaces.insert(pair<string, SDL_Surface *>(surface_name,
		make_surface_from_colors(colors, PLAY_BLOCK_SIZE, PLAY_BLOCK_SIZE)));
}

SDL_Surface * common_surfaces::get_surface(string surface_name)
{
	return surfaces.at(surface_name);
}

void common_surfaces::remove_surface(string surface_name)
{
	surfaces.erase(surface_name);
}

std::map<std::string, SDL_Texture *> common_surfaces::create_texture_map(SDL_Renderer * renderer)
{
	std::map<std::string, SDL_Texture *> texture_map;
	for (auto &key_val : surfaces)
	{
		SDL_Texture * new_texture = SDL_CreateTextureFromSurface(renderer, key_val.second);
		texture_map.insert(pair<string, SDL_Texture *>(key_val.first, new_texture));
	}

	return texture_map;
}

common_surfaces all_surfaces = common_surfaces();

vector<string> block_texture_names
= { "red", "sky_blue:lighter sky_blue", "green" };