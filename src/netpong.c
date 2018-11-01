#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include "images.h"
#include "fonts.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define SCREEN_NAME "NetPong"

#define BASE_BALL_SPEED 6
#define BASE_PADDLE_SPEED 4
#define CENTER_X (SCREEN_WIDTH / 2)
#define CENTER_Y (SCREEN_HEIGHT / 2)

void game_init();
void game_quit();

const Uint32 FRAMES_PER_SECOND = 60;

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
    Uint32 score1;
    Uint32 score2;
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
    game_quit,
    0, 0
};

struct direction_vector {
    int x;
    int y;
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

void check_events(const Uint8 *keyboardState, int *moving)
{
    SDL_Event event;
    int paddle_speed = BASE_PADDLE_SPEED;

    if (keyboardState[SDL_SCANCODE_Q]) {
        Game.running = SDL_FALSE;
    }

    *moving = 0;
    if (keyboardState[SDL_SCANCODE_DOWN]) {
        *moving = paddle_speed;
    }
    else if (keyboardState[SDL_SCANCODE_UP]) {
        *moving = -paddle_speed;
    }

    while(SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                Game.running = SDL_FALSE;
                break;
        }
    }
}

void reset_ball(SDL_Rect* ball_rect, int* ball_speed, float* angle, int direction)
{
    *angle = 0;
    ball_rect->x = CENTER_X;
    ball_rect->y = CENTER_Y;
    *ball_speed = BASE_BALL_SPEED * direction;
}

struct direction_vector
check_collisions(SDL_Rect p1_rect, SDL_Rect p2_rect, SDL_Rect *ball_rect, int *ball_speed)
{
    static float angle = 0;
    struct direction_vector direction;

    SDL_bool c1 = SDL_HasIntersection(ball_rect, &p1_rect);
    SDL_bool c2 = SDL_HasIntersection(ball_rect, &p2_rect);

    if (ball_rect->x < 0){
        Game.score2++;
        reset_ball(ball_rect, ball_speed, &angle, 1);
    } else if(ball_rect->x > (int)(Game.screen.width - ball_rect->w)) {
        // Change for stop win game
        Game.score1++;
        reset_ball(ball_rect, ball_speed, &angle, -1);
    }

    if (c1){
        if (*ball_speed < 0) {
            *ball_speed *= -1;
        }
        angle = (rand() % 3) - 1;
    } else if (c2) {
        if (*ball_speed > 0) {
            *ball_speed *= -1;
        }
        angle = (rand() % 3) - 1;
    }

    if (ball_rect->y < 0 || ball_rect->y > (int)(Game.screen.height - ball_rect->h)) {
        angle *= -1;
    }

    direction.x = ball_rect->x + (int) *ball_speed;
    direction.y = ball_rect->y + (int) *ball_speed * angle;

    return direction;
}

void follow_ball(SDL_Rect *ball, SDL_Rect *paddle)
{
    int paddle_center = paddle->y + (paddle->h / 2);

    if (ball->y > (paddle_center + 2) && ball->y < (paddle_center - 2)) {
        // Avoid jitter
        return;
    }

    if (ball->y > paddle_center && ball->y > (paddle_center + 2)) {
        if (paddle->y + paddle->h > SCREEN_HEIGHT) {
            return;
        }
        paddle->y += BASE_PADDLE_SPEED;
    } else if (round(ball->y) < paddle_center) {
        if (paddle->y <= 0) {
            return;
        }
        paddle->y -= BASE_PADDLE_SPEED;
    }
}

Uint32 frame_limit(Uint32 last_tick, const Uint32 frame_limit) {
    Uint32 elapsed_ms = (SDL_GetTicks() - last_tick);

    if (elapsed_ms < (1000 / frame_limit))
    {
        SDL_Delay((1000 / frame_limit) - elapsed_ms);
    }

    return SDL_GetTicks();
}

int main()
{
    time_t t;
    int ball_speed = BASE_BALL_SPEED;
    srand((unsigned) time(&t));
    Uint32 last_tick = SDL_GetTicks();
    char buffer[100];

    Game.init();

    load_font(Game.screen.renderer);

    SDL_Texture *bg_texture = load_image(Game.screen.renderer, "images/court.png");

    SDL_Texture *ball_texture = load_image(Game.screen.renderer, "images/ball.png");
    SDL_Rect ball_rect = {
        CENTER_X,
        CENTER_Y,
        32,
        32
    };

    SDL_Texture *paddle_texture = load_image(Game.screen.renderer, "images/paddle.png");
    SDL_Rect p1_rect = {
        32,
        CENTER_Y - 64,
        32,
        128
    };
    SDL_Rect p2_rect = {
        Game.screen.width - 64,
        CENTER_Y - 64,
        32,
        128
    };

    int moving = 0;

    struct direction_vector direction;

    // randomize the start direction
    switch (rand() % 2) {
        case 0:
            ball_speed *= -1;
            break;
    }

    float angle = 0;

    reset_ball(&ball_rect, &ball_speed, &angle, 1);

    const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);

    while (Game.running) {
        // moving is the speed and direction of the paddle
        check_events(keyboardState, &moving);

        direction = check_collisions(p1_rect, p2_rect, &ball_rect, &ball_speed);

        ball_rect.x = direction.x;
        ball_rect.y = direction.y;

        p1_rect.y += moving;

        if (p1_rect.y < 0) {
            p1_rect.y = 0;
        } else if (p1_rect.y > (int)(Game.screen.height - p1_rect.h)) {
            p1_rect.y = (int)(Game.screen.height - p1_rect.h);
        }

        follow_ball(&ball_rect, &p2_rect);

        SDL_RenderClear(Game.screen.renderer);
        SDL_RenderCopy(Game.screen.renderer, bg_texture, NULL, NULL);
        SDL_RenderCopy(Game.screen.renderer, ball_texture, NULL, &ball_rect);
        SDL_RenderCopy(Game.screen.renderer, paddle_texture, NULL, &p1_rect);
        SDL_RenderCopy(Game.screen.renderer, paddle_texture, NULL, &p2_rect);

        for(int i = 0; i < 100; i++) buffer[i] = 0;
        sprintf(buffer, "%d", Game.score1);
        draw_text(Game.screen.renderer, buffer, SCREEN_WIDTH / 3, 20, 3);

        for(int i = 0; i < 100; i++) buffer[i] = 0;
        sprintf(buffer, "%d", Game.score2);
        draw_text(Game.screen.renderer, buffer, (SCREEN_WIDTH / 3) * 2, 20, 3);

        SDL_RenderPresent(Game.screen.renderer);

        // Add delay to match frame rate
        last_tick = frame_limit(last_tick, FRAMES_PER_SECOND);
    }

    SDL_DestroyTexture(bg_texture);

    Game.quit();

    return 0;
}
