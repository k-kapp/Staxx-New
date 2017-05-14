#pragma once

#include <SDL2/SDL.h>
#include <map>
#include "../include/misc_types.h"

extern const SDL_Texture * black_texture;

class common_surfaces
{
public:
	common_surfaces();

	~common_surfaces();

	SDL_Surface * get_surface(std::string surface_name);
	void add_surface(std::string surface_name, SDL_Surface * surface);
	void add_surface(std::string surface_name, tile_colors colors);
	void remove_surface(std::string surface_name);

	std::map<std::string, SDL_Texture *> create_texture_map(SDL_Renderer * renderer);

private:
	static bool created;

	std::map<std::string, SDL_Surface *> surfaces;
	std::map<std::string, SDL_Surface *> new_surfaces;
	std::map<std::string, SDL_Surface *> deleted_surfaces;
};

void init_surfaces(common_surfaces &surfaces);

extern common_surfaces all_surfaces;

extern std::vector<std::string> block_texture_names;