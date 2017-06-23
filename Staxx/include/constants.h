#pragma once

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>
#include "SDL2/SDL.h"

extern const int INIT_DELAY;

extern const int BLOCK_SIZE;

extern const int PLAY_BLOCK_SIZE;
extern const int DESIGN_BLOCK_SIZE;

extern const int PLAY_BORDER_SIZE;
extern const int DESIGN_BORDER_SIZE;

extern const int DESIGN_SELECT_BLOCK_SIZE;
extern const int DESIGN_SELECT_SCROLLER_SIZE;
extern const int DESIGN_SELECT_SCREEN_WIDTH;
extern const int DESIGN_SELECT_SCREEN_HEIGHT;

extern const std::string PLAY_SCREEN_TITLE;
extern const int PLAY_SCREEN_X;
extern const int PLAY_SCREEN_Y;
extern const int PLAY_SCREEN_WIDTH;
extern const int PLAY_SCREEN_HEIGHT;
extern const int PLAY_GRID_X;
extern const int PLAY_GRID_Y;

extern const int NEXT_GRID_BLOCK_SIZE;
extern const int NEXT_GRID_ROWS;
extern const int NEXT_GRID_COLS;
extern const int NEXT_GRID_X;
extern const int NEXT_GRID_Y;

extern const int NEXT_ARROW_SIZE;

extern const int PLAY_ROWS;
extern const int PLAY_COLS;


extern const std::string DESIGN_SCREEN_TITLE;
extern const int DESIGN_SCREEN_X;
extern const int DESIGN_SCREEN_Y;
extern const int DESIGN_SCREEN_WIDTH;
extern const int DESIGN_SCREEN_HEIGHT;

extern const int DESIGN_ROWS;
extern const int DESIGN_COLS;

extern const Uint32 SDL_TEXTURE_FORMAT;

extern const SDL_Texture * GAME_OFF_TEXTURE;
extern const SDL_Texture * GAME_ON_TEXTURE;

extern const char * FONT_FILEPATH;

#endif CONSTANTS_H