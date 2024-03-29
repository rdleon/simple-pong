#include "simplepong.h"

#define FONT_HEIGHT 16;

struct font Font;

void load_font(SDL_Renderer* renderer, const char *img_filename, const char *data_filename) {
    Font.texture = load_image(renderer, img_filename);
    for(Uint32 i = 0; i < 128; i++) {
        Font.rects[i].x = 0;
        Font.rects[i].y = 0;
        Font.rects[i].w = 0;
        Font.rects[i].h = FONT_HEIGHT;
    }
    FILE *f = fopen(data_filename, "r");
    char c;
    Uint32 x, w;
    while (fscanf(f, "%c %d %d\n", &c, &x, &w) != EOF) {
        Font.rects[(int)c].x = x;
        Font.rects[(int)c].w = w;
    }
    fclose(f);
}

void draw_text(SDL_Renderer* renderer, char *text, int x, int y, double scaling) {
    SDL_Rect dst_rect;
    dst_rect.x = x;
    dst_rect.y = y;
    dst_rect.h = scaling * FONT_HEIGHT;

    for(Uint32 i = 0; text[i] != '\0'; i++) {
        if(text[i] == ' ') {
            dst_rect.x += (Uint32)(scaling * 8.0f);
            continue;
        }
        dst_rect.w = (Font.rects[(int)text[i]].w * scaling);
        SDL_RenderCopy(renderer, Font.texture, &Font.rects[(int)text[i]], &dst_rect);
        dst_rect.x += dst_rect.w;
    }
}
