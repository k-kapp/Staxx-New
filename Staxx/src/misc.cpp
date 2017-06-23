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
#include <SDL2/SDL_ttf.h>


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
			((Uint32*)surface->pixels)[row_idx * (surface->pitch / sizeof(Uint32)) + col_idx] = pixel_color;
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

SDL_Surface * make_up_arrow(unsigned width, unsigned height, color col)
{
	SDL_Surface * new_surface = SDL_CreateRGBSurface(0, width, height, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);

	for (int row_idx = 0; row_idx < new_surface->h; row_idx++)
	{
		for (int col_idx = 0; col_idx < new_surface->w; col_idx++)
		{
			Uint32 pixel_color;
			if (col_idx >= width / 2 - row_idx / 2 && col_idx <= width / 2 + row_idx / 2)
			{
				pixel_color = SDL_MapRGB(new_surface->format, col.red, col.green, col.blue);
			}
			else
			{
				pixel_color = SDL_MapRGB(new_surface->format, 0, 0, 0);
			}
			((Uint32*)new_surface->pixels)[row_idx * new_surface->pitch / sizeof(Uint32) + col_idx] = pixel_color;
		}
	}

	return new_surface;
}

void rotate_surface_clockwise(SDL_Surface * surface)
{
	for (int row_idx = 0; row_idx < surface->h / 2; row_idx++)
	{
		int up_left_edge_idx = row_idx * surface->pitch / sizeof(Uint32) + row_idx;
		int up_right_edge_idx = row_idx * surface->pitch / sizeof(Uint32) + surface->w - row_idx - 1;
		int low_right_edge_idx = (surface->h - row_idx - 1) * surface->pitch / sizeof(Uint32) + surface->w - row_idx - 1;
		int low_left_edge_idx = (surface->h - row_idx - 1) * surface->pitch / sizeof(Uint32) + row_idx;

		for (int col_idx = 0; col_idx < surface->w - 2*row_idx - 1; col_idx++)
		{
			int top_change_idx = up_left_edge_idx + col_idx;
			int right_change_idx = up_right_edge_idx + col_idx*surface->pitch / sizeof(Uint32);
			int bottom_change_idx = low_right_edge_idx - col_idx;
			int left_change_idx = low_left_edge_idx - col_idx * surface->pitch / sizeof(Uint32);

			Uint32 top_change = ((Uint32*)surface->pixels)[top_change_idx];
			Uint32 right_change = ((Uint32*)surface->pixels)[right_change_idx];
			Uint32 bottom_change = ((Uint32*)surface->pixels)[bottom_change_idx];
			Uint32 left_change = ((Uint32*)surface->pixels)[left_change_idx];

			((Uint32*)surface->pixels)[right_change_idx] = top_change;
			((Uint32*)surface->pixels)[bottom_change_idx] = right_change;
			((Uint32*)surface->pixels)[left_change_idx] = bottom_change;
			((Uint32*)surface->pixels)[top_change_idx] = left_change;

		}
	}
}

SDL_Surface * rotate_copy_surface_clockwise(SDL_Surface * surface)
{
	SDL_Surface * new_surface = SDL_CreateRGBSurface(0, surface->w, surface->h, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
	SDL_BlitSurface(surface, NULL, new_surface, NULL);

	rotate_surface_clockwise(new_surface);

	return new_surface;
}

void mult_surface(SDL_Surface * surface, color col)
{
	for (int row_idx = 0; row_idx < surface->w; row_idx++)
	{
		for (int col_idx = 0; col_idx < surface->h; col_idx++)
		{
			int idx = row_idx * surface->pitch / sizeof(Uint32) + col_idx;
			Uint32 pixel_val = ((Uint32*)surface->pixels)[idx];
			
			Uint8 r, g, b;
			SDL_GetRGB(pixel_val, surface->format, &r, &g, &b);
			unsigned red = r;
			unsigned green = g;
			unsigned blue = b;

			red = ((float)red / 255 * (float)col.red / 255) * 255;
			green = ((float)green / 255 * (float)col.green / 255) * 255;
			blue = ((float)blue / 255 * (float)col.blue / 255) * 255;

			Uint32 new_pixel_val = SDL_MapRGB(surface->format, red, green, blue);

			((Uint32*)surface->pixels)[idx] = new_pixel_val;
		}
	}
}

SDL_Surface *  mult_copy_surface(SDL_Surface * surface, color col)
{
	SDL_Surface * new_surface = SDL_CreateRGBSurface(0, surface->w, surface->h, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);

	SDL_BlitSurface(surface, NULL, new_surface, NULL);

	mult_surface(new_surface, col);

	return new_surface;
}

vector<SDL_Surface *> get_all_rotations(SDL_Surface * surface)
{
	vector<SDL_Surface *> surfaces = { surface };

	for (int i = 0; i < 3; i++)
		surfaces.push_back(rotate_copy_surface_clockwise(surfaces.back()));

	return surfaces;
}

vector<string> split_text(string text, bool ignore_empty)
{
	string curr_word;
	vector<string> words;
	for (char ch : text)
	{
		if (isspace(ch))
		{
			if (!(ignore_empty && (curr_word.size() == 0)))
				words.push_back(curr_word);
			curr_word.clear();
		}
		else
		{
			curr_word += ch;
		}
	}
	words.push_back(curr_word);
	return words;
}

string rebuild_string(vector<string> splitted, bool ignore_empty)
{
	string rebuilt = "";
	if (splitted.size() == 0)
		return rebuilt;
	for (int i = 0; i < splitted.size() - 1; i++)
	{
		if (ignore_empty && (splitted.at(i).size() == 0))
			continue;
		rebuilt += splitted.at(i) + " ";
	}

	rebuilt += splitted.back();

	return rebuilt;
}

SDL_Texture * copy_SDL_texture(SDL_Texture * src_texture, SDL_Renderer * renderer)
{
	if (!src_texture)
		return NULL;

	Uint32 src_format;
	int src_access;
	int src_w;
	int src_h;
	SDL_QueryTexture(src_texture, &src_format, &src_access, &src_w, &src_h);

	SDL_Texture * new_texture = SDL_CreateTexture(renderer, src_format, SDL_TEXTUREACCESS_TARGET, src_w, src_h);
	
	SDL_Texture * prev_rendertarget = SDL_GetRenderTarget(renderer);
	SDL_SetRenderTarget(renderer, new_texture);
	SDL_RenderCopy(renderer, src_texture, NULL, NULL);
	SDL_SetRenderTarget(renderer, prev_rendertarget);

	return new_texture;
}

SDL_Rect get_text_rect(string text, TTF_Font * font, int x, int y, int font_size)
{
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	double ratio;

	TTF_SizeText(font, text.c_str(), &rect.w, &rect.h);
	if (font_size >= 0)
		ratio = ((double)font_size) / rect.h;
	else
		ratio = 1.0;
	rect.h = font_size;
	rect.w *= ratio;

	return rect;
}

vector<pair<SDL_Rect, SDL_Texture *> > get_wrapped_text_rects_textures(string text, TTF_Font * font, 
																SDL_Renderer * renderer, int x, int y, int max_width, int font_size, int space_size)
{
	vector<string> splitted = split_text(text, true);
	vector<pair<SDL_Rect, SDL_Texture *> > rect_textures;

	if (splitted.size() == 0)
		return{};

	int mid_x = x + max_width / 2;

	string cand_line = splitted.front();
	string curr_line = cand_line;
	string prev_added = "";
	string next_word = "";

	int curr_y;
	int w, h;

	TTF_SizeText(font, "H", &w, &h);
	double ratio = font_size / ((double)h);

	TTF_SizeText(font, curr_line.c_str(), &w, &h);
	curr_y = y;

	vector<string>::iterator iter;

	auto add_new_pair = [&]() {
		pair<SDL_Rect, SDL_Texture *> new_pair;
		SDL_Rect &new_rect = new_pair.first;
		SDL_Texture * &new_texture = new_pair.second;

		/*
		TTF_SizeText(font, curr_line.c_str(), &new_rect.w, &new_rect.h);
		new_rect.x = mid_x - new_rect.w / 2;
		*/

		new_rect.y = curr_y;
		new_rect.x = 0;

		new_rect = get_text_rect(curr_line, font, new_rect.x, new_rect.y, font_size);
		new_rect.x = mid_x - new_rect.w / 2;

		SDL_Surface * text_surface = TTF_RenderText_Solid(font, curr_line.c_str(), { 255, 255, 255 });
		new_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
		rect_textures.push_back(new_pair);

		SDL_FreeSurface(text_surface);
		
		curr_y += font_size + space_size;

	};

	int word_count = 1;
	for (int i = 1; i < splitted.size(); i++)
	{
		iter = next(splitted.begin(), i);
		if (w > max_width)
		{
			add_new_pair();

			/*
			 * if we have only one word, add the candidate line to the finished
			 * lines and make the current and candidate lines equal to the next
			 * word
			 */
			if (word_count == 1)
			{
				curr_line = *iter;
				cand_line = *iter;
			}
			/*
			 * if more than one word, add the current line without the latest word,
			 * then make both current and candidate lines equal to the lastest word,
			 * which caused the line to be too large. Also decrement i, so that we may see
			 * the next word in the iteration again, since we ignore it in this case
			 */
			else
			{
				curr_line = prev_added;
				cand_line = prev_added;
				i--;
			}
			prev_added = "";
			word_count = 1;

			TTF_SizeText(font, cand_line.c_str(), &w, NULL);
			w *= ratio;
		}
		else
		{
			/*
			 * update lines, candidate and current
			 */
			if (prev_added.size() > 0)
				curr_line += " " + prev_added;
			cand_line += " " + *iter;
			prev_added = *iter;
			word_count++;

			/*
			 * update widths and x positions
			 */
			TTF_SizeText(font, cand_line.c_str(), &w, NULL);
			w *= ratio;
		}
	}
	if (w > max_width)
	{
		add_new_pair();
		curr_line = prev_added;
		if (prev_added.size() > 0)
			add_new_pair();
	}
	else
	{
		curr_line = cand_line;
		add_new_pair();
	}

	return rect_textures;
}