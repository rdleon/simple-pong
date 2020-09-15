#include "game.h"
#include "images.h"
#include "fonts.h"

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

    {
        {
            CENTER_X,
            CENTER_Y,
            32,
            32
        },
        {0, 0},
        BASE_BALL_SPEED
    },

    {
        {
            32,
            CENTER_Y - 64,
            32,
            128
        },
        0
    },

    {
        {
            SCREEN_WIDTH - 64,
            CENTER_Y - 64,
            32,
            128
        },
        0
    },

    game_init,
    game_loop,
    game_quit,
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

void
check_collisions(struct player_t* p1, struct player_t* p2, struct ball_t* ball)
{
    static float angle = 0;

    SDL_bool c1 = SDL_HasIntersection(&(ball->rect), &(p1->rect));
    SDL_bool c2 = SDL_HasIntersection(&(ball->rect), &(p2->rect));

    if (ball->rect.x < 0) {
        p2->score++;
        reset_ball(&(ball->rect), &(ball->speed), &angle, 1);
    } else if(ball->rect.x > (int)(Game.screen.width - ball->rect.w)) {
        // Change for stop win game
        p1->score++;
        reset_ball(&(ball->rect), &(ball->speed), &angle, -1);
    }

    if (c1){
        if (ball->speed < 0) {
            ball->speed *= -1;
        }
        angle = calculate_angle(&(p1->rect), &(ball->rect));
    } else if (c2) {
        if (ball->speed > 0) {
            ball->speed *= -1;
        }
        angle = calculate_angle(&(p2->rect), &(ball->rect));
    }

    if (ball->rect.y < 0 || ball->rect.y > (int)(Game.screen.height - ball->rect.h)) {
        angle *= -1;
    }

    ball->rect.x = ball->rect.x + (int) ball->speed;
    ball->rect.y = ball->rect.y + (int) ball->speed * angle;
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

void destroy_texture(SDL_Texture* texture)
{
    if (texture) {
        SDL_DestroyTexture(texture);
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

    // randomize the start direction
    if (rand() % 2 == 0) {
        Game.ball.speed *= -1;
    }

    float angle = 0;

    reset_ball(&Game.ball.rect, &Game.ball.speed, &angle, 1);


    Game.running = SDL_TRUE;
}

void game_loop(const Uint8 *keyboardState)
{
    int moving = 0;
    char buffer[MAX_TEXT_BUFF_SIZE];

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
}

void game_quit()
{
    destroy_texture(Game.textures.background);
    destroy_texture(Game.textures.ball);
    destroy_texture(Game.textures.paddle);

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
