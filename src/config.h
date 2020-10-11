#ifndef CONFIG_H
#define CONFIG_H

#define SCREEN_NAME "NetPong"
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define FRAMES_PER_SECOND 60

/* Millisecons to wait for debouncing keypresses */
#define DEBOUNCE_WAIT 500

#define MAX_TEXT_BUFF_SIZE 100

#define BASE_BALL_SPEED 5
#define BASE_PADDLE_SPEED 4

#define BALL_TEXTURE_WIDTH 32
#define PADDLE_WIDTH 32
#define PADDLE_HEIGHT 128

#define SPACING 32

#define FUZZ_PIXELS 2

#define CENTER_X (SCREEN_WIDTH / 2)
#define CENTER_Y (SCREEN_HEIGHT / 2)

#define FINAL_TEXT_SCALE 7
#define FINAL_TEXT_X SCREEN_WIDTH / 4
#define FINAL_TEXT_Y CENTER_Y - 50

#define DEFAULT_MAX_SCORE 5

#endif
