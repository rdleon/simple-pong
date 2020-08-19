# Development requirements

You will need a C compiler SDL2 and SDL2 image extension.

##  To install dependencies in Debian:

`$ apt install build-essential libsdl2-dev libsdl2-image-dev`


# Netpong Roadmap

## Version 1

1. Limit frame rate to 60FPS (Done.)
2. IA Player
    Predict where it must be to bounce the ball
3. Show the game score on the screen.
    [font](https://opengameart.org/content/good-neighbors-pixel-font)
4. End game when a max score is achieved
5. At the start of the game the ball is randomly served to one side, after
this it is sent to the one who just scored
6. Add a menu
    a. Start Game
    b. View Controls
    c. Exit
7. Update README

## Version 2

1. Add Options to the menu: Configure Controls and Maximum score
2. Set the angle of the ball depending of the movement and angle of the paddle
3. Add sound when hitting and scoring
4. Add a second player
    a. Set Menu option for 2 player vs.
    b. Set Options for player 2 controls
5. Update README

## Version 3

1. Online Server
2. Server lobby
3. Online multiplayer
