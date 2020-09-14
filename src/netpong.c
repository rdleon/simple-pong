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

const Uint32 FRAMES_PER_SECOND = 60;
extern struct game_t Game;

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

int calculate_angle(SDL_Rect *paddle, SDL_Rect *ball)
{
    // calculate angle to return ball on paddle collision.
    // this breaks the paddle into 3 segments (up, middle, down)
    // and changes the angle depending on where the ball collisioned
    // if the ball hits the middle segment, it randomly selects the angle.
    int third = (paddle->h / 3);

    int middle_start = paddle->y + third;
    int middle_end = paddle->y + third*2;

    int paddle_middle = paddle->y + (paddle->h /2); // this breaks paddle in two

    if (ball->y >= middle_start && ball->y <= middle_end) {
        // if collision is in the "middle" segment
        // randomly pick one of the 3 values.
        return (rand() % 3) - 1;
    } else if (ball->y > paddle_middle) {
        // collision is in the upper segment
        return 1;
    } else {
        // collision is in the lower segment
        return -1;
    }

    return 0;
}

struct direction_vector
check_collisions(SDL_Rect *p1_rect, SDL_Rect *p2_rect, SDL_Rect *ball_rect, int *ball_speed)
{
    static float angle = 0;
    struct direction_vector direction;

    SDL_bool c1 = SDL_HasIntersection(ball_rect, p1_rect);
    SDL_bool c2 = SDL_HasIntersection(ball_rect, p2_rect);

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
        angle = calculate_angle(p1_rect, ball_rect);
    } else if (c2) {
        if (*ball_speed > 0) {
            *ball_speed *= -1;
        }
        angle = calculate_angle(p2_rect, ball_rect);
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
