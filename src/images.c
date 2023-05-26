#include "simplepong.h"

SDL_Texture* load_image(SDL_Renderer* renderer, const char* path)
{
    SDL_Surface *image_surface;
    SDL_Texture *texture;

   image_surface = IMG_Load(path);

    if (image_surface == NULL) {
        fprintf(stderr, "Failed to load image: %s\n", path);
        return NULL;
    }

    texture = SDL_CreateTextureFromSurface(renderer, image_surface);

    SDL_FreeSurface(image_surface);

    return texture;
}
