#ifndef GAME_H
#define GAME_H

#include "HardwareScreen.h"

class Game {
    private:
        Screen* scr;
        bool running;

        void handle_input();
        void draw_game();
    public:
        Game(Screen* scr);
        ~Game();
        void run();
};

#endif
