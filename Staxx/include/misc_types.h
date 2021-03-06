#ifndef MISC_TYPES
#define MISC_TYPES

#include <boost/filesystem.hpp>
#include <vector>
#include <string>
#include <memory>
#include <utility>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

struct color
{
	int red;
	int green;
	int blue;

	//color();
};

struct tile_colors
{
	color base;
	color border;
};


typedef enum occupant_type_
{
	EMPTY,
	BLOCKED,
	ACTIVE
} occupant_type;

typedef enum move_type
{
	UP_MV,
	LEFT_MV,
	DOWN_MV,
	RIGHT_MV,
	ROTATE_MV
} move_type;

class shape;

class imported_shapes
{
public:
	imported_shapes();

	static std::vector<std::shared_ptr<shape> > shapes;
	static std::vector<std::string> filenames;
private:
	void do_import_shapes();
};


//functions below are implemented in src/misc.cpp

bool design_path_valid(boost::filesystem::path filepath);

int get_design_num(boost::filesystem::path filepath, bool check_valid = true);

std::vector<boost::filesystem::path> get_design_paths();

void set_pixel(SDL_Surface * surface, int x, int y, Uint32 color);

SDL_Surface * make_surface_from_colors(tile_colors colors, int width, int height);

SDL_Texture * make_texture_from_colors(tile_colors colors, int width, int height, SDL_Renderer *renderer);

SDL_Texture * make_default_off_texture(SDL_Renderer * renderer);

SDL_Texture * make_default_on_texture(SDL_Renderer * renderer);

SDL_Surface * make_up_arrow(unsigned width, unsigned height, color col);

void rotate_surface_clockwise(SDL_Surface * surface);

SDL_Surface * rotate_copy_surface_clockwise(SDL_Surface * surface);

void mult_surface(SDL_Surface * surface, color col);

SDL_Surface * mult_copy_surface(SDL_Surface * surface, color col);

std::vector<SDL_Surface *> get_all_rotations(SDL_Surface * surface);

SDL_Texture * copy_SDL_texture(SDL_Texture * texture, SDL_Renderer * renderer);

std::string remove_consec_spaces(std::string text);

SDL_Rect get_text_rect(std::string text, TTF_Font * font, int x, int y, int font_size = -1);

std::vector<std::pair<SDL_Rect, SDL_Texture *> > get_wrapped_text_rects_textures(std::string text, TTF_Font * font,
	SDL_Renderer * renderer, int x, int y, int max_width, int font_size, int space_size);

#endif
