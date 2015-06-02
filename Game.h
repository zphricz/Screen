#ifndef GAME_H
#define GAME_H

#include "Screen.h"

class Game {
private:
  PerfSoftScreen *scr;
  bool running;

  void handle_input();
  void draw_game();

public:
  Game(PerfSoftScreen *scr);
  ~Game();
  void run();
};

#endif
