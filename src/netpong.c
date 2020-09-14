#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include "config.h"
#include "game.h"
#include "images.h"
#include "fonts.h"

extern struct game_t Game;

int main()
{
    time_t t;
    int ball_speed = BASE_BALL_SPEED;
    srand((unsigned) time(&t));
    Uint32 last_tick = SDL_GetTicks();
    char buffer[MAX_TEXT_BUFF_SIZE];

    game_init();

    load_font(Game.screen.renderer, "images/good_neighbors.png", "data/font.txt");

    SDL_Rect ball_rect = {
        CENTER_X,
        CENTER_Y,
        32,
        32
    };

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

        direction = check_collisions(&p1_rect, &p2_rect, &ball_rect, &ball_speed);

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
        SDL_RenderCopy(Game.screen.renderer, Game.textures.background, NULL, NULL);
        SDL_RenderCopy(Game.screen.renderer, Game.textures.ball, NULL, &ball_rect);
        SDL_RenderCopy(Game.screen.renderer, Game.textures.paddle, NULL, &p1_rect);
        SDL_RenderCopy(Game.screen.renderer, Game.textures.paddle, NULL, &p2_rect);

        memset(buffer, 0, MAX_TEXT_BUFF_SIZE);
        sprintf(buffer, "%d", Game.score1);
        draw_text(Game.screen.renderer, buffer, SCREEN_WIDTH / 3, 20, 3);

        for(int i = 0; i < 100; i++) buffer[i] = 0;
        sprintf(buffer, "%d", Game.score2);
        draw_text(Game.screen.renderer, buffer, (SCREEN_WIDTH / 3) * 2, 20, 3);

        SDL_RenderPresent(Game.screen.renderer);

        // Add delay to match frame rate
        last_tick = frame_limit(last_tick, FRAMES_PER_SECOND);
    }

    Game.quit();

    return 0;
}
