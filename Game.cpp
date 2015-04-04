#include <iostream>
#include <cmath>
#include <thread>
#include <future>
#include <vector>
#include "Game.h"

using namespace std;

int x1;
int y_1;
int x2;
int y2;
int chosen;
int rate = 6;
enum class ShapeState {
  RECT,
  LINE,
  VERLINE,
  HORLINE,
  CIRCLE
} state = ShapeState::RECT;

void Game::handle_input() {
  static bool up = false;
  static bool down = false;
  static bool left = false;
  static bool right = false;
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT: {
      running = false;
      break;
    }
    case SDL_KEYUP: {
      switch (event.key.keysym.sym) {
      case SDLK_LEFT: {
        left = false;
        break;
      }
      case SDLK_RIGHT: {
        right = false;
        break;
      }
      case SDLK_UP: {
        up = false;
        break;
      }
      case SDLK_DOWN: {
        down = false;
        break;
      }
      }
      break;
    }
    case SDL_KEYDOWN: {
      switch (event.key.keysym.sym) {
      case SDLK_ESCAPE: {
        running = false;
        break;
      }
      case SDLK_LEFT: {
        left = true;
        break;
      }
      case SDLK_RIGHT: {
        right = true;
        break;
      }
      case SDLK_UP: {
        up = true;
        break;
      }
      case SDLK_DOWN: {
        down = true;
        break;
      }
      case SDLK_RETURN: {
        chosen++;
        chosen %= 2;
        break;
      }
      case SDLK_SPACE: {
        switch (state) {
        case ShapeState::RECT:
          state = ShapeState::LINE;
          break;
        case ShapeState::LINE:
          state = ShapeState::VERLINE;
          break;
        case ShapeState::VERLINE:
          state = ShapeState::HORLINE;
          break;
        case ShapeState::HORLINE:
          state = ShapeState::CIRCLE;
          break;
        case ShapeState::CIRCLE:
          state = ShapeState::RECT;
          break;
        }
        break;
      }
      }
    }
    }
  }
  switch (chosen) {
  case 0:
    if (up) {
      y_1 -= rate;
    }
    if (down) {
      y_1 += rate;
    }
    if (left) {
      x1 -= rate;
    }
    if (right) {
      x1 += rate;
    }
    break;
  case 1:
    if (up) {
      y2 -= rate;
    }
    if (down) {
      y2 += rate;
    }
    if (left) {
      x2 -= rate;
    }
    if (right) {
      x2 += rate;
    }
    break;
  }
}

Game::Game(Screen<> *scr) : scr(scr), running(true) {
  x1 = 0;
  y_1 = 0;
  x2 = scr->width - 1;
  y2 = scr->height - 1;
  chosen = 0;
}

Game::~Game() {}

void Game::draw_game() {
  scr->cls();
  switch (state) {
  case ShapeState::RECT:
    scr->fill_rect(x1, y_1, x2, y2, {255, 0, 0});
    break;
  case ShapeState::LINE:
    scr->draw_line(x1, y_1, x2, y2, {255, 0, 0});
    break;
  case ShapeState::VERLINE:
    scr->ver_line(x1, y_1, y2, {255, 0, 0});
    break;
  case ShapeState::HORLINE:
    scr->hor_line(y_1, x1, x2, {255, 0, 0});
    break;
  case ShapeState::CIRCLE:
    scr->fill_circle(x1, y_1, abs(x1 - x2), {255, 0, 0});
    break;
  }
}

void Game::run() {
  while (running) {
    handle_input();
    draw_game();
    scr->commit();
    cout << "FPS: " << scr->fps() << endl;
  }
}
