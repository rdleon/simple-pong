#include "simplepong.h"

struct game Game = {
    Menu,
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
        NULL,
        NULL,
        NULL,
    },


    // Cursor
    {
        CENTER_X,
        CENTER_Y,
        BALL_TEXTURE_WIDTH,
        BALL_TEXTURE_WIDTH
    },

    // Ball
    {
        {
            CENTER_X,
            CENTER_Y,
            BALL_TEXTURE_WIDTH,
            BALL_TEXTURE_WIDTH
        },
        {0, 0},
        BASE_BALL_SPEED
    },

    // Player 1
    {
        {
            SPACING,
            CENTER_Y - (PADDLE_HEIGHT / 2),
            PADDLE_WIDTH,
            PADDLE_HEIGHT
        },
        0
    },

    // Player 2
    {
        {
            SCREEN_WIDTH - (SPACING + PADDLE_WIDTH),
            CENTER_Y - (PADDLE_HEIGHT / 2),
            PADDLE_WIDTH,
            PADDLE_HEIGHT
        },
        0
    },

    DEFAULT_MAX_SCORE,
    0,
};

double rand_range(double min, double max)
{
    return min + (rand() / ( RAND_MAX / (max - min) ) ) ;
}

void reset_ball(SDL_Rect* ball_rect, int8_t* ball_speed, double* angle, int direction)
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
    const double min_angle = M_PI/180;
    int third = (paddle->h / 3);

    int middle_start = paddle->y + third;
    int middle_end = paddle->y + third * 2;

    int paddle_middle = paddle->y + (paddle->h / 2); // this breaks paddle in two

    if (ball->y >= middle_start && ball->y <= middle_end) {
        // if collision is in the "middle" segment
        // return an angle in radians between 0 and Pi/2 or 3*Pi/2 and 2*Pi
        if (rand() % 2) {
            return rand_range(min_angle, M_PI_2);
        }

        return rand_range((3 * M_PI_2) + min_angle, 2 * M_PI);
    } else if (ball->y > paddle_middle) {
        // collision is in the upper segment
        return rand_range(min_angle, M_PI_2);
    } else {
        // collision is in the lower segment
        return rand_range((3 * M_PI_2) + min_angle, 2 * M_PI);
    }

    return 0;
}

void check_collisions(struct player* p1, struct player* p2, struct ball* ball)
{
    static double angle = 0;

    SDL_bool c1 = SDL_HasIntersection(&(ball->rect), &(p1->rect));
    SDL_bool c2 = SDL_HasIntersection(&(ball->rect), &(p2->rect));

    if (ball->rect.x < 0) {

        //play sound
        Mix_PlayChannel( -1, Game.sounds.scorepoint, 0 );

        p2->score++;
        reset_ball(&(ball->rect), &(ball->speed), &angle, 1);
    } else if(ball->rect.x > (int)(Game.screen.width - ball->rect.w)) {
        
        //play sound
        Mix_PlayChannel( -1, Game.sounds.scorepoint, 0 );

        // Change for stop win game
        p1->score++;
        reset_ball(&(ball->rect), &(ball->speed), &angle, -1);
    }

    if (c1){
        if (ball->speed < 0) {
            if (ball->speed > INT8_MIN + 1) {
                ball->speed -= 1;
                //play sound
                Mix_PlayChannel( -1, Game.sounds.paddlebounce, 0 );
            }
            ball->speed *= -1;
        }
        angle = calculate_angle(&(p1->rect), &(ball->rect));
    } else if (c2) {
        if (ball->speed > 0) {
            if (ball->speed < INT8_MAX - 1) {
                ball->speed += 1;   
                //play sound
                Mix_PlayChannel( -1, Game.sounds.paddlebounce, 0 );
            }
            ball->speed *= -1;
        }
        angle = calculate_angle(&(p2->rect), &(ball->rect));
    }

    if (ball->rect.y < 0 || ball->rect.y > (int)(Game.screen.height - ball->rect.h)) {
        angle *= -1;
        
        //play sound
        Mix_PlayChannel( -1, Game.sounds.wallbounce, 0 );

    }

    if (angle == 0) {
        ball->rect.x += ball->speed;
    } else {
        ball->rect.x += (int) round(ball->speed * cos(angle));
        ball->rect.y += (int) round(ball->speed * sin(angle));
    }
}

void follow_ball(SDL_Rect *ball, SDL_Rect *paddle)
{
    int paddle_center = paddle->y + (paddle->h / 2);
    int paddle_speed = BASE_PADDLE_SPEED;

    // If ball is going away, try to go to the middle of the screen
    if (Game.ball.speed < 0) {
        if (paddle_center > CENTER_Y) {
            paddle->y -= paddle_speed / 2;
        } else if (paddle_center < CENTER_Y) {
            paddle->y += paddle_speed / 2;
        }
        return;
    }

    if (ball->x < CENTER_X) {
        return;
    }

    if (ball->y > (paddle_center + FUZZ_PIXELS) && ball->y < (paddle_center - FUZZ_PIXELS)) {
        // Avoid jitter
        return;
    }

    if (ball->y > paddle_center && ball->y > (paddle_center + FUZZ_PIXELS)) {
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

void check_events(const Uint8 *keyboard_state, int *player1_moving, int *player2_moving)
{
    int paddle_speed = BASE_PADDLE_SPEED;

    if (keyboard_state[SDL_SCANCODE_Q]) {
        Game.state = Menu;
        SDL_Delay(DEBOUNCE_WAIT);
    
    //reset game when quitting to menu
    game_reset();
    }

    *player1_moving = 0;
    *player2_moving = 0;

    //check player 1 input
    if (keyboard_state[SDL_SCANCODE_DOWN]) {
        *player1_moving = paddle_speed;
    }
    else if (keyboard_state[SDL_SCANCODE_UP]) {
        *player1_moving = -paddle_speed;
    }
    
    //check player 2 input
    if (keyboard_state[SDL_SCANCODE_L]) {
        *player2_moving = paddle_speed;
    }
    else if (keyboard_state[SDL_SCANCODE_O]) {
        *player2_moving = -paddle_speed;
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

void init_sounds()
{
   if ( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 1024 ) < 0 ) {
        fprintf(stderr, "Error initializing SDL_mixer: -> %s\n", Mix_GetError());
        exit(1);
    }
}

void game_reset()
{
    Game.player1.rect.y = Game.player2.rect.y = CENTER_Y - (PADDLE_HEIGHT / 2);
    Game.player1.score = Game.player2.score = 0;
    // randomize the start direction
    if (rand() % 2 == 0) {
        Game.ball.speed *= -1;
    }

    double angle = 0;

    reset_ball(&Game.ball.rect, &Game.ball.speed, &angle, 1);
}

void game_init()
{
    int rc = SDL_Init(SDL_INIT_EVERYTHING);

    if (rc != 0) {
        fprintf(stderr, "SDL error -> %s\n", SDL_GetError());
        exit(1);
    }

    init_images();
    init_sounds();

    Game.screen.window = SDL_CreateWindow("SimplePong",
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

    Game.sounds.wallbounce = Mix_LoadWAV("sounds/wall_bounce.wav");
    Game.sounds.paddlebounce = Mix_LoadWAV("sounds/paddle_bounce.wav");
    Game.sounds.scorepoint = Mix_LoadWAV("sounds/score_point.wav");

    Game.textures.background = load_image(Game.screen.renderer, "images/court.png");
    Game.textures.ball = load_image(Game.screen.renderer, "images/ball.png");
    Game.textures.paddle = load_image(Game.screen.renderer, "images/paddle.png");

    game_reset();

    Game.state = Menu;
}

void end_game_screen(const Uint8 *keyboard_state)
{
    char end_message[MAX_TEXT_BUFF_SIZE];

    SDL_RenderClear(Game.screen.renderer);
    SDL_RenderCopy(Game.screen.renderer, Game.textures.background, NULL, NULL);

    if (Game.player1.score > Game.player2.score) {
        sprintf(end_message, "You Win!");
    } else {
        sprintf(end_message, "You Lose!");
    }

    draw_text(
        Game.screen.renderer,
        end_message,
        FINAL_TEXT_X,
        FINAL_TEXT_Y,
        FINAL_TEXT_SCALE
    );

    if (keyboard_state[SDL_SCANCODE_RETURN]) {
        game_reset();
        Game.state = Menu;
        SDL_Delay(DEBOUNCE_WAIT);
    }
}

void game_loop(const Uint8 *keyboard_state)
{
    int player1_moving = 0;
    int player2_moving = 0;
    char buffer[MAX_TEXT_BUFF_SIZE];

    check_events(keyboard_state, &player1_moving, &player2_moving);

    if (Game.player1.score >= Game.max_score || Game.player2.score >= Game.max_score) {
        end_game_screen(keyboard_state);
        return;
    }

    check_collisions(&Game.player1, &Game.player2, &Game.ball);

    //player 1
    Game.player1.rect.y += player1_moving;

    if (Game.player1.rect.y < 0) {
        Game.player1.rect.y = 0;
    } else if (Game.player1.rect.y > (int)(Game.screen.height - Game.player1.rect.h)) {
        Game.player1.rect.y = (int)(Game.screen.height - Game.player1.rect.h);
    }
    
    //player 2
    if (Game.two_players) {     
        Game.player2.rect.y += player2_moving;

        if (Game.player2.rect.y < 0) {
            Game.player2.rect.y = 0;
        } else if (Game.player2.rect.y > (int)(Game.screen.height - Game.player2.rect.h)) {
            Game.player2.rect.y = (int)(Game.screen.height - Game.player2.rect.h);
        }   
    //bot player
    } else {
        follow_ball(&Game.ball.rect, &Game.player2.rect);
    }
    
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
    SDL_DestroyTexture(Game.textures.background);
    SDL_DestroyTexture(Game.textures.ball);
    SDL_DestroyTexture(Game.textures.paddle);

    Mix_FreeChunk(Game.sounds.wallbounce);
    Mix_FreeChunk(Game.sounds.paddlebounce);
    Mix_FreeChunk(Game.sounds.scorepoint);

    if (Game.screen.renderer) {
        SDL_DestroyRenderer(Game.screen.renderer);
    }

    if (Game.screen.window) {
        SDL_DestroyWindow(Game.screen.window);
    }

    Mix_Quit();
    IMG_Quit();
    SDL_Quit();

    Game.state = Quit;
}
