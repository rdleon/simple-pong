#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define SCREEN_NAME "NetPong"

void game_init();
void game_quit();

static struct {
    SDL_bool running;
    struct {
        unsigned int width;
        unsigned int height;
        const char* name;
        SDL_Window* window;
        SDL_Renderer* renderer;
    } screen;

    void (*init)();
    void (*quit)();
} Game = {
    SDL_FALSE,
    {
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SCREEN_NAME,
        NULL,
        NULL
    },
    game_init,
    game_quit
};

void init_images()
{
    int flags = IMG_INIT_PNG;
    int rc = IMG_Init(flags);

    if ((rc&flags) != flags) {
        fprintf(stderr, "SDL error -> %s\n", IMG_GetError());
        exit(1);
    }
}

void game_init()
{
    int rc = SDL_Init(SDL_INIT_EVERYTHING);

    if (rc != 0) {
        fprintf(stderr, "SDL error -> %s\n", SDL_GetError());
        exit(1);
    }

    init_images();


    Game.screen.window = SDL_CreateWindow("NetPong",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            Game.screen.width,
            Game.screen.height,
            SDL_WINDOW_SHOWN);

    if (Game.screen.window == NULL) {
        fprintf(stderr, "Window couldn't be created");
        game_quit();
        exit(1);
    }

    Game.screen.renderer = SDL_CreateRenderer(
        Game.screen.window,
        -1,
        SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC
    );

    Game.running = SDL_TRUE;
}

void game_quit()
{
    if (Game.screen.renderer) {
        SDL_DestroyRenderer(Game.screen.renderer);
    }

    if (Game.screen.window) {
        SDL_DestroyWindow(Game.screen.window);
    }

    IMG_Quit();
    SDL_Quit();
    Game.running = SDL_FALSE;
}

SDL_Texture* load_image(const char* path)
{
    SDL_Surface *image_surface;
    SDL_Texture *texture;

   image_surface = IMG_Load(path);

    if (image_surface == NULL) {
        fprintf(stderr, "Failed to load image: %s\n", path);
        return NULL;
    }

    texture = SDL_CreateTextureFromSurface(Game.screen.renderer,
                                           image_surface);

    SDL_FreeSurface(image_surface);

    return texture;
}

void check_events(int *moving)
{
    SDL_Event event;
    int paddle_speed = 2;
    while(SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                Game.running = SDL_FALSE;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        *moving = -1 * (paddle_speed);
                        break;
                    case SDLK_DOWN:
                        *moving = 1 * (paddle_speed);
                        break;
                }
                break;
            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        *moving = 0;
                        break;
                    case SDLK_DOWN:
                        *moving = 0;
                        break;
                }
                break;
        }
    }
}

void check_collisions(SDL_Rect p1_rect, SDL_Rect p2_rect, SDL_Rect *ball_rect, int *ball_speed)
{
    int start = Game.screen.width / 2;
    static float angle = 0;

    SDL_bool c1 = SDL_HasIntersection(ball_rect, &p1_rect);
    SDL_bool c2 = SDL_HasIntersection(ball_rect, &p2_rect);



    if (ball_rect->x < 0){
        printf("POINT to player 2!\n");
        ball_rect->x = start;
    } else if(ball_rect->x > (int)(Game.screen.width - ball_rect->w)) {
        // Change for stop win game
        printf("POINT to player 1!\n");
        ball_rect->x = start;
    }

    if (c1 || c2) {
        angle = (rand() % 3) - 1;
        *ball_speed *= -1;
    }

    if (ball_rect->y < 0 || ball_rect->y > (int)(Game.screen.height - ball_rect->h)) {
        angle *= -1;
    }

    ball_rect->y += (int) *ball_speed * angle;
    ball_rect->x += (int) *ball_speed;
}

int main()
{
    time_t t;
    int ball_speed = 4;
    srand((unsigned) time(&t));

    Game.init();

    SDL_Texture *bg_texture = load_image("images/court.png");

    SDL_Texture *ball_texture = load_image("images/ball.png");
    SDL_Rect ball_rect = {
        Game.screen.width / 2,
        Game.screen.height / 2,
        32,
        32
    };

    SDL_Texture *paddle_texture = load_image("images/paddle.png");
    SDL_Rect p1_rect = {
        32,
        Game.screen.height / 2 - 64,
        32,
        128
    };
    SDL_Rect p2_rect = {
        Game.screen.width - 64,
        Game.screen.height / 2 - 64,
        32,
        128
    };

    int moving = 0;

    // randomize the start direction
    switch (rand() % 2) {
        case 0:
            ball_speed *= -1;
            break;
    }

    while (Game.running) {
        check_events(&moving);
        // Check for collision
        check_collisions(p1_rect, p2_rect, &ball_rect, &ball_speed);

        if (moving < 0 && p1_rect.y > 0) {
            p1_rect.y += moving;
        } else if (moving > 0 && p1_rect.y < (int)(Game.screen.height - p1_rect.h)) {
            p1_rect.y += moving;
        }

        SDL_RenderClear(Game.screen.renderer);
        SDL_RenderCopy(Game.screen.renderer, bg_texture, NULL, NULL);
        SDL_RenderCopy(Game.screen.renderer, ball_texture, NULL, &ball_rect);
        SDL_RenderCopy(Game.screen.renderer, paddle_texture, NULL, &p1_rect);
        SDL_RenderCopy(Game.screen.renderer, paddle_texture, NULL, &p2_rect);
        SDL_RenderPresent(Game.screen.renderer);
    }

    SDL_DestroyTexture(bg_texture);

    Game.quit();

    return 0;
}
