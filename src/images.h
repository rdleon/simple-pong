#ifndef IMAGES_H
#define IMAGES_H

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

SDL_Texture* load_image(SDL_Renderer* renderer, const char* path);
#endif
