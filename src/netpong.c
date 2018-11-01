#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define SCREEN_NAME "NetPong"

#define BASE_BALL_SPEED 6
#define BASE_PADDLE_SPEED 4
#define CENTER_X (SCREEN_WIDTH / 2)
#define CENTER_Y (SCREEN_HEIGHT / 2)

#define MAX_POINTS 5

const Uint32 FRAMES_PER_SECOND = 60;
const Uint32 FONT_HEIGHT = 16;

void game_init();
void game_quit();

static struct
{
    SDL_Texture *texture;
    SDL_Rect rects[128]; // I will only going to support ASCII
} Font;

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

void load_font() {
    Font.texture = load_image("images/good_neighbors.png");
    for(Uint32 i = 0; i < 128; i++) {
        Font.rects[i].x = 0;
        Font.rects[i].y = 0;
        Font.rects[i].w = 0;
        Font.rects[i].h = FONT_HEIGHT;
    }

    // This version only includes ASCII 33-127.
    Font.rects['!'].x  = 1;   Font.rects['!'].w  = 6;
    Font.rects['"'].x  = 8;   Font.rects['"'].w  = 7;
    Font.rects['#'].x  = 16;  Font.rects['#'].w  = 10;
    Font.rects['$'].x  = 27;  Font.rects['$'].w  = 10;
    Font.rects['%'].x  = 38;  Font.rects['%'].w  = 11;
    Font.rects['&'].x  = 50;  Font.rects['&'].w  = 11;
    Font.rects['\''].x = 62;  Font.rects['\''].w = 4;
    Font.rects['('].x  = 67;  Font.rects['('].w  = 6;
    Font.rects[')'].x  = 74;  Font.rects[')'].w  = 6;
    Font.rects['*'].x  = 81;  Font.rects['*'].w  = 10;
    Font.rects['+'].x  = 92;  Font.rects['+'].w  = 8;
    Font.rects[','].x  = 101; Font.rects[','].w  = 4;
    Font.rects['-'].x  = 106; Font.rects['-'].w  = 9;
    Font.rects['.'].x  = 116; Font.rects['.'].w  = 4;
    Font.rects['/'].x  = 121; Font.rects['/'].w  = 8;

    Font.rects['0'].x = 130; Font.rects['0'].w = 8;
    Font.rects['1'].x = 139; Font.rects['1'].w = 6;
    Font.rects['2'].x = 146; Font.rects['2'].w = 8;
    Font.rects['3'].x = 155; Font.rects['3'].w = 8;
    Font.rects['4'].x = 164; Font.rects['4'].w = 9;
    Font.rects['5'].x = 174; Font.rects['5'].w = 8;
    Font.rects['6'].x = 183; Font.rects['6'].w = 8;
    Font.rects['7'].x = 192; Font.rects['7'].w = 8;
    Font.rects['8'].x = 201; Font.rects['8'].w = 8;
    Font.rects['9'].x = 210; Font.rects['9'].w = 8;

    Font.rects[':'].x = 219; Font.rects[':'].w = 4;
    Font.rects[';'].x = 224; Font.rects[';'].w = 4;
    Font.rects['<'].x = 229; Font.rects['<'].w = 9;
    Font.rects['='].x = 239; Font.rects['='].w = 7;
    Font.rects['>'].x = 247; Font.rects['>'].w = 9;
    Font.rects['?'].x = 257; Font.rects['?'].w = 8;
    Font.rects['@'].x = 266; Font.rects['@'].w = 10;

    Font.rects['A'].x = 277; Font.rects['A'].w = 8;
    Font.rects['B'].x = 286; Font.rects['B'].w = 8;
    Font.rects['C'].x = 295; Font.rects['C'].w = 8;
    Font.rects['D'].x = 304; Font.rects['D'].w = 9;
    Font.rects['E'].x = 314; Font.rects['E'].w = 8;
    Font.rects['F'].x = 323; Font.rects['F'].w = 8;
    Font.rects['G'].x = 332; Font.rects['G'].w = 8;
    Font.rects['H'].x = 341; Font.rects['H'].w = 8;
    Font.rects['I'].x = 350; Font.rects['I'].w = 6;
    Font.rects['J'].x = 357; Font.rects['J'].w = 9;
    Font.rects['K'].x = 367; Font.rects['K'].w = 8;
    Font.rects['L'].x = 376; Font.rects['L'].w = 8;
    Font.rects['M'].x = 385; Font.rects['M'].w = 10;
    Font.rects['N'].x = 396; Font.rects['N'].w = 9;
    Font.rects['O'].x = 406; Font.rects['O'].w = 8;
    Font.rects['P'].x = 415; Font.rects['P'].w = 8;
    Font.rects['Q'].x = 424; Font.rects['Q'].w = 9;
    Font.rects['R'].x = 434; Font.rects['R'].w = 9;
    Font.rects['S'].x = 444; Font.rects['S'].w = 8;
    Font.rects['T'].x = 453; Font.rects['T'].w = 8;
    Font.rects['U'].x = 462; Font.rects['U'].w = 8;
    Font.rects['V'].x = 471; Font.rects['V'].w = 8;
    Font.rects['W'].x = 480; Font.rects['W'].w = 10;
    Font.rects['X'].x = 491; Font.rects['X'].w = 9;
    Font.rects['Y'].x = 501; Font.rects['Y'].w = 8;
    Font.rects['Z'].x = 510; Font.rects['Z'].w = 8;

    Font.rects['['].x = 519;  Font.rects['['].w = 6;
    Font.rects['\\'].x = 526; Font.rects['\\'].w = 8;
    Font.rects[']'].x = 535; Font.rects[']'].w = 6;
    Font.rects['^'].x = 542; Font.rects['^'].w = 11;
    Font.rects['_'].x = 554; Font.rects['_'].w = 8;
    Font.rects['`'].x = 563; Font.rects['`'].w = 6;

    Font.rects['a'].x = 570; Font.rects['a'].w = 8;
    Font.rects['b'].x = 579; Font.rects['b'].w = 8;
    Font.rects['c'].x = 588; Font.rects['c'].w = 8;
    Font.rects['d'].x = 597; Font.rects['d'].w = 8;
    Font.rects['e'].x = 606; Font.rects['e'].w = 8;
    Font.rects['f'].x = 615; Font.rects['f'].w = 7;
    Font.rects['g'].x = 623; Font.rects['g'].w = 8;
    Font.rects['h'].x = 632; Font.rects['h'].w = 8;
    Font.rects['i'].x = 641; Font.rects['i'].w = 6;
    Font.rects['j'].x = 648; Font.rects['j'].w = 6;
    Font.rects['k'].x = 655; Font.rects['k'].w = 8;
    Font.rects['l'].x = 664; Font.rects['l'].w = 5;
    Font.rects['m'].x = 670; Font.rects['m'].w = 10;
    Font.rects['n'].x = 681; Font.rects['n'].w = 8;
    Font.rects['o'].x = 690; Font.rects['o'].w = 8;
    Font.rects['p'].x = 699; Font.rects['p'].w = 8;
    Font.rects['q'].x = 708; Font.rects['q'].w = 9;
    Font.rects['r'].x = 718; Font.rects['r'].w = 8;
    Font.rects['s'].x = 727; Font.rects['s'].w = 8;
    Font.rects['t'].x = 736; Font.rects['t'].w = 8;
    Font.rects['u'].x = 745; Font.rects['u'].w = 8;
    Font.rects['v'].x = 754; Font.rects['v'].w = 9;
    Font.rects['w'].x = 763; Font.rects['w'].w = 10;
    Font.rects['x'].x = 774; Font.rects['x'].w = 8;
    Font.rects['y'].x = 783; Font.rects['y'].w = 8;
    Font.rects['z'].x = 792; Font.rects['z'].w = 8;

    Font.rects['{'].x = 801; Font.rects['{'].w = 7;
    Font.rects['|'].x = 809; Font.rects['|'].w = 4;
    Font.rects['}'].x = 814; Font.rects['}'].w = 7;
    Font.rects['~'].x = 822; Font.rects['~'].w = 9;
}

void draw_text(char *text, int x, int y, double scaling) {
    SDL_Rect dst_rect;
    dst_rect.x = x;
    dst_rect.y = y;
    dst_rect.h = FONT_HEIGHT * scaling;

    for(Uint32 i = 0; text[i] != '\0'; i++) {
        if(text[i] == ' ') {
            dst_rect.x += (8 * scaling);
            continue;
        }
        dst_rect.w = (Font.rects[(int)text[i]].w * scaling);
        SDL_RenderCopy(Game.screen.renderer, Font.texture, &Font.rects[(int)text[i]], &dst_rect);
        dst_rect.x += dst_rect.w;
    }
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

    load_font();

    SDL_Texture *bg_texture = load_image("images/court.png");

    SDL_Texture *ball_texture = load_image("images/ball.png");
    SDL_Rect ball_rect = {
        CENTER_X,
        CENTER_Y,
        32,
        32
    };

    SDL_Texture *paddle_texture = load_image("images/paddle.png");
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
        draw_text(buffer, SCREEN_WIDTH / 3, 20, 3);

        for(int i = 0; i < 100; i++) buffer[i] = 0;
        sprintf(buffer, "%d", Game.score2);
        draw_text(buffer, (SCREEN_WIDTH / 3) * 2, 20, 3);

        SDL_RenderPresent(Game.screen.renderer);

        // Add delay to match frame rate
        last_tick = frame_limit(last_tick, FRAMES_PER_SECOND);

        if (Game.score1 >= MAX_POINTS) {
            printf("Player WINS!\n");
            break;
        }

        if (Game.score2 >= MAX_POINTS) {
            printf("CPU Wins!\n");
            break;
        }
    }

    SDL_DestroyTexture(bg_texture);

    Game.quit();

    return 0;
}
