#include "../include/misc_types.h"
#include "../include/constants.h"
#include "../include/shape.h"
#include <string>
#include <vector>
#include <utility>
#include <memory>
#include <boost/filesystem.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <SDL2/SDL.h>


using namespace std;
using namespace boost::filesystem;

bool design_path_valid(path filepath)
{
	if (is_directory(filepath))
	{
		return false;
	}
	if (!(filepath.extension() == ".dsn"))
	{
		return false;
	}

	char * temp_filename = (char *)malloc((filepath.filename().size() + 1) * sizeof(char));
	wcstombs(temp_filename, filepath.filename().c_str(), filepath.filename().size() + 1);
	string file_str(temp_filename);
	free(temp_filename);

	string prefix_str(file_str.begin(), next(file_str.begin(), 6));
	if (!(prefix_str == "design"))
		return false;
	string num_str(next(file_str.begin(), 6), next(file_str.begin(), file_str.size() - 4));
	int curr_int;
	bool thrown = false;
	try
	{
		curr_int = stoi(num_str);
	}
	catch (invalid_argument &e)
	{
		thrown = true;
	}
	if (!thrown && curr_int > 0)
	{
		return true;
	}
	else
		return false;
}

int get_design_num(path filepath, bool check_valid)
{
	if (check_valid && !design_path_valid(filepath))
	{
		return 0;
	}

	char * temp_filename = (char *)malloc((filepath.filename().size() + 1) * sizeof(char));
	wcstombs(temp_filename, filepath.filename().c_str(), filepath.filename().size() + 1);
	string file_str(temp_filename);
	free(temp_filename);

	string num_str(next(file_str.begin(), 6), next(file_str.begin(), file_str.size() - 4));

	return stoi(num_str);
}



vector<path> get_design_paths()
{
	directory_iterator dir_iter("designs/"), end;

	if (!exists(dir_iter->path()))
	{
		return vector<path>();
	}

	vector<path> paths;

	for (; dir_iter != end; advance(dir_iter, 1))
	{
		if (design_path_valid(dir_iter->path()))
			paths.push_back(dir_iter->path());
	}

	return paths;
}

vector<shared_ptr<shape> > import_shapes()
{
	using namespace boost::filesystem;

	if (!exists(path("designs/")))
	{
		cout << "no designs exist. on the main menu, ";
		cout << "select the design option and design some shapes first" << endl;
		return vector<shared_ptr<shape> >();
	}

	vector<shared_ptr<shape> > shapes;

	directory_iterator curr_iter("designs/"), end;

	for (; curr_iter != end; advance(curr_iter, 1))
	{
		if (design_path_valid(curr_iter->path()))
		{
			std::ifstream in_file(curr_iter->path().c_str());
			boost::archive::text_iarchive ia(in_file);
			shapes.push_back(make_shared<shape>(ia));
		}
	}

	if (shapes.size() == 0)
	{
		cout << "no designs exist. on the main menu, ";
		cout << "select the design option and design some shapes first" << endl;
	}
	return shapes;
		
}

SDL_Surface * make_surface_from_colors(tile_colors colors, int width, int height)
{
	//SDL_Texture * new_texture = SDL_CreateTexture(renderer, SDL_TEXTURE_FORMAT, SDL_TEXTUREACCESS_STATIC, width, height);
	//SDL_Surface * surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 24, SDL_TEXTURE_FORMAT);
	SDL_Surface * surface = SDL_CreateRGBSurface(0, width, height, 24, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x00000000);

	for (int row_idx = 0; row_idx < height; row_idx++)
	{
		for (int col_idx = 0; col_idx < width; col_idx++)
		{
			Uint32 pixel_color;
			if (col_idx <= 0 || col_idx >= width - 1 || row_idx <= 0 || row_idx >= height - 1)
			{
				pixel_color = SDL_MapRGB(surface->format, colors.border.red, colors.border.green, colors.border.blue);
			}
			else
			{
				pixel_color = SDL_MapRGB(surface->format, colors.base.red, colors.base.green, colors.base.blue);
			}
			((Uint32*)surface->pixels)[row_idx * (surface->pitch / 4) + col_idx] = pixel_color;
		}
	}
	return surface;
}

SDL_Texture * make_texture_from_colors(tile_colors colors, int width, int height, SDL_Renderer *renderer)
{
	SDL_Surface * surface = make_surface_from_colors(colors, width, height);

	return SDL_CreateTextureFromSurface(renderer, surface);
}

SDL_Texture * make_default_on_texture(SDL_Renderer * renderer)
{
	tile_colors on_colors;
	on_colors.base.red = 175;
	on_colors.base.green = 0;
	on_colors.base.blue = 0;
	on_colors.border.red = 255;
	on_colors.border.green = 0;
	on_colors.border.blue = 0;

	return make_texture_from_colors(on_colors, PLAY_BLOCK_SIZE, PLAY_BLOCK_SIZE, renderer);
}

SDL_Texture * make_default_off_texture(SDL_Renderer * renderer)
{
	tile_colors off_colors;
	off_colors.base.red = off_colors.base.green = off_colors.base.red = 0;
	off_colors.border.red = off_colors.border.green = off_colors.border.blue = 0;

	return make_texture_from_colors(off_colors, PLAY_BLOCK_SIZE, PLAY_BLOCK_SIZE, renderer);
}