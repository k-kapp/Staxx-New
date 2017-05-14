#include "button.h"
#include "misc_types.h"
#include "game_state.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <tuple>

using namespace std;

using coord = pair<int, int>;

class menu : public game_state
{
public:
	menu(unsigned width, unsigned height, string heading, unsigned heading_width, unsigned heading_height, 
			unsigned buttons_width, unsigned buttons_height);
	//menu(const vector<tuple<string, function<void()>, tile_colors, tile_colors, tile_colors> > &buttons_info);
	//menu(const vector<tuple<string, tile_colors, tile_colors, tile_colors> > &buttons_info);
	menu(vector<button> buttons);

	~menu();

	void add_button(const button &new_button);
	void add_button(const string &text, const function<void()> &callback, 
					const SDL_Color &on_color, const SDL_Color &off_color);
	void align_buttons_left(int x_coord);
	void align_buttons_right(int x_coord);
	void align_buttons_top();
	void align_buttons_vertical(int vert_gap);
	void align_buttons_heading_horiz();
	void align_buttons_heading_vert(int vert_gap);
	void center_buttons(int x_coord);
	void resize_buttons(int width, int height);
	void space_buttons_vertical(int y_dist);
	void set_top_button(string button_text);
	void set_top_button(int idx);

	//void set_buttons(std::vector<button> buttons);

	void set_position(int x, int y);
	void set_size(int width, int height);

	void set_heading_pos(int x, int y);
	void set_heading_size(TTF_Font * font);
	void set_heading(string heading_str);
	void set_heading_rect(SDL_Rect rect);
	void draw_heading();
	void draw_buttons();
	void update_buttons();
	void draw_menu();
	void hide_menu();
	void show_menu();

	void add_button(string text, function<void()> callback);

	void mainloop() override;

	void exit();

	void set_buttons_colors(tile_colors off_color, tile_colors on_color, tile_colors clicked_color);
	void set_buttons_colors(std::string off_color, std::string on_color, std::string clicked_color);

	void set_button_callback(string button_title, function<void()> callback);

	void set_buttons_size(unsigned width, unsigned height);

	void reset_buttons_states();

protected:

	void draw() override;
	void update() override;

private:
	vector<button> buttons;
	menu(const vector<tuple<string, function<void()>, SDL_Color, SDL_Color> > &buttons_info);
	int x, y;
	unsigned width, height;
	SDL_Surface * heading_surface;
	SDL_Texture * heading_texture;
	SDL_Rect heading_rect;
	string heading_str;
	bool quit;

	unsigned buttons_width, buttons_height;

	unsigned heading_top_spacing = 10;
	unsigned button_vert_spacing = 10;

	tile_colors button_off_col;
	tile_colors button_hover_col;
	tile_colors button_on_col;

	void setup_heading();
};
