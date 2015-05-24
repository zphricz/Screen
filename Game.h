#ifndef GAME_H
#define GAME_H

#include "Screen.h"

class Game {
private:
  SoftScreen *scr;
  bool running;

  void handle_input();
  void draw_game();

public:
  Game(SoftScreen *scr);
  ~Game();
  void run();
};

#endif
