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
	all_surfaces.add_surface("blue", { {0, 0, 255}, {0, 0, 155} });
	all_surfaces.add_surface("green", { {0, 255, 0}, {0, 155, 0} });
	all_surfaces.add_surface("medium green", { {0, 155, 0}, {0, 55, 0} });
	all_surfaces.add_surface("dark green", { {0, 100, 0}, {0, 50, 0} });
	all_surfaces.add_surface("black", { {0, 0, 0}, {0, 0, 0} });
	all_surfaces.add_surface("dark gray", { {50, 50, 50}, {50, 50, 50} });
	all_surfaces.add_surface("gray", { {125, 125, 125}, {125, 125, 125} });
	all_surfaces.add_surface("light gray", { {200, 200, 200}, {200, 200, 200} });
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
