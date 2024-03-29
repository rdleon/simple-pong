#ifndef FONTS_H
#define FONTS_H

#include "simplepong.h"

struct font
{
    SDL_Texture *texture;
    SDL_Rect rects[128]; // I will only going to support ASCII
};

void load_font(SDL_Renderer* renderer, const char *img_filename, const char *data_filename);
void draw_text(SDL_Renderer* renderer, char *text, int x, int y, double scaling);

#endif
