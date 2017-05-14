#include <string>
#include "../include/constants.h"
#include "../include/misc_types.h"

const int INIT_DELAY = 500;

const int BLOCK_SIZE = 20;

const int PLAY_BLOCK_SIZE = 30;
const int DESIGN_BLOCK_SIZE = 40;

const int PLAY_BORDER_SIZE = 1;
const int DESIGN_BORDER_SIZE = 1;


const std::string PLAY_SCREEN_TITLE = "STAXX";
const int PLAY_SCREEN_X = 100;
const int PLAY_SCREEN_Y = 100;
const int PLAY_SCREEN_WIDTH = 1300;
const int PLAY_SCREEN_HEIGHT = 800;
const int PLAY_GRID_X = 100;
const int PLAY_GRID_Y = 100;
const int PLAY_ROWS = 14;
const int PLAY_COLS = 14;

const int NEXT_GRID_BLOCK_SIZE = PLAY_BLOCK_SIZE / 1.7;
const int NEXT_GRID_ROWS = 8;
const int NEXT_GRID_COLS = 8;
const int NEXT_GRID_X = PLAY_GRID_X + (PLAY_COLS + 2) * PLAY_BLOCK_SIZE + NEXT_GRID_BLOCK_SIZE * NEXT_GRID_COLS / 3;
const int NEXT_GRID_Y = PLAY_GRID_Y + PLAY_BLOCK_SIZE * PLAY_ROWS - NEXT_GRID_BLOCK_SIZE * NEXT_GRID_ROWS;

const int NEXT_ARROW_SIZE = NEXT_GRID_ROWS * NEXT_GRID_BLOCK_SIZE / 5;



const std::string DESIGN_SCREEN_TITLE = "Design grid";
const int DESIGN_SCREEN_X = 100;
const int DESIGN_SCREEN_Y = 100;
const int DESIGN_SCREEN_WIDTH = 800;
const int DESIGN_SCREEN_HEIGHT = 600;

const int DESIGN_ROWS = 8;
const int DESIGN_COLS = 8;

const Uint32 SDL_TEXTURE_FORMAT = SDL_PIXELFORMAT_RGB24;

const char * FONT_FILEPATH = "fonts/ARCADECLASSIC.TTF";

//const SDL_Texture * GAME_OFF_TEXTURE = make_default_off_texture();
//const SDL_Texture * GAME_ON_TEXTURE = make_default_on_texture();