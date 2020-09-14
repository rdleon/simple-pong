#include "game.h"
#include "images.h"

struct game_t Game = {
    SDL_FALSE,
    {
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SCREEN_NAME,
        NULL,
        NULL
    },
    {
        NULL,
        NULL,
        NULL,
    },
    game_init,
    game_quit,
    0, 0
};

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

    Game.textures.background = load_image(Game.screen.renderer, "images/court.png");
    Game.textures.ball = load_image(Game.screen.renderer, "images/ball.png");
    Game.textures.paddle = load_image(Game.screen.renderer, "images/paddle.png");

    Game.running = SDL_TRUE;
}

void game_quit()
{
    SDL_DestroyTexture(Game.textures.background);
    SDL_DestroyTexture(Game.textures.ball);
    SDL_DestroyTexture(Game.textures.paddle);

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


