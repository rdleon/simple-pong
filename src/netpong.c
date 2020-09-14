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
    srand((unsigned) time(&t));
    Uint32 last_tick = SDL_GetTicks();
    char buffer[MAX_TEXT_BUFF_SIZE];
    int moving = 0;

    game_init();

    load_font(Game.screen.renderer, "images/good_neighbors.png", "data/font.txt");

    // randomize the start direction
    if (rand() % 2 == 0) {
        Game.ball.speed *= -1;
    }

    float angle = 0;

    reset_ball(&Game.ball.rect, &Game.ball.speed, &angle, 1);

    const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);

    while (Game.running) {
        // moving is the speed and direction of the paddle
        check_events(keyboardState, &moving);

        check_collisions(&Game.player1, &Game.player2, &Game.ball);

        Game.player1.rect.y += moving;

        if (Game.player1.rect.y < 0) {
            Game.player1.rect.y = 0;
        } else if (Game.player1.rect.y > (int)(Game.screen.height - Game.player1.rect.h)) {
            Game.player1.rect.y = (int)(Game.screen.height - Game.player1.rect.h);
        }

        follow_ball(&Game.ball.rect, &Game.player2.rect);

        SDL_RenderClear(Game.screen.renderer);
        SDL_RenderCopy(Game.screen.renderer, Game.textures.background, NULL, NULL);
        SDL_RenderCopy(Game.screen.renderer, Game.textures.ball, NULL, &Game.ball.rect);
        SDL_RenderCopy(Game.screen.renderer, Game.textures.paddle, NULL, &Game.player1.rect);
        SDL_RenderCopy(Game.screen.renderer, Game.textures.paddle, NULL, &Game.player2.rect);

        memset(buffer, 0, MAX_TEXT_BUFF_SIZE);
        sprintf(buffer, "%d", Game.player1.score);
        draw_text(Game.screen.renderer, buffer, SCREEN_WIDTH / 3, 20, 3);

        for(int i = 0; i < MAX_TEXT_BUFF_SIZE; i++) buffer[i] = 0;
        sprintf(buffer, "%d", Game.player2.score);
        draw_text(Game.screen.renderer, buffer, (SCREEN_WIDTH / 3) * 2, 20, 3);

        SDL_RenderPresent(Game.screen.renderer);

        // Add delay to match frame rate
        last_tick = frame_limit(last_tick, FRAMES_PER_SECOND);
    }

    Game.quit();

    return 0;
}
