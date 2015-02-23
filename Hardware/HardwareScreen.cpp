#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include "HardwareScreen.h"

using namespace std;
using namespace chrono;

Screen::Screen(int size_x, int size_y, const char * name,
                               bool full_screen, bool vsync) :
    width(size_x),
    height(size_y),
    vsync(vsync),
    full_screen(full_screen) {
    if (full_screen) {
        window = SDL_CreateWindow(name, 0, 0, size_x, size_y,
                        SDL_WINDOW_FULLSCREEN);
    } else {
        window = SDL_CreateWindow(name,
                        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                        width, height, 0);
    }

    if (vsync) {
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC |
                                                  SDL_RENDERER_ACCELERATED);
    } else {
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    }
    current_frame_time = last_frame_time = high_resolution_clock::now();
}

Screen::~Screen() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

void Screen::commit() {
    SDL_RenderPresent(renderer);
    last_frame_time = current_frame_time;
    current_frame_time = high_resolution_clock::now();
}

bool Screen::on_screen(int x, int y) {
    return (x >= 0) && (x < width) && (y >= 0) && (y < height);
}

void Screen::set_color(Uint8 r, Uint8 g, Uint8 b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
}

void Screen::set_color(SDL_Color c) {
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
}

void Screen::draw_pixel(int x, int y) {
    SDL_RenderDrawPoint(renderer, x, y);
}

void Screen::draw_pixel(int x, int y, SDL_Color c) {
    set_color(c);
    draw_pixel(x, y);
}

void Screen::cls() {
    set_color(0, 0, 0);
    SDL_RenderClear(renderer);
}


void Screen::fill_screen() {
    SDL_RenderClear(renderer);
}

void Screen::fill_screen(SDL_Color c) {
    set_color(c);
    fill_screen();
}

// Prefer drawing horizontal lines to vertical ones
void Screen::hor_line(int y, int x1, int x2) {
    SDL_RenderDrawLine(renderer, x1, y, x2, y);
}

void Screen::hor_line(int y, int x1, int x2, SDL_Color c) {
    set_color(c);
    hor_line(y, x1, x2);
}

void Screen::ver_line(int x, int y1, int y2) {
    SDL_RenderDrawLine(renderer, x, y1, x, y2);
}

void Screen::ver_line(int x, int y1, int y2, SDL_Color c) {
    set_color(c);
    ver_line(x, y1, y2);
}

void Screen::draw_rect(int x1, int y1, int x2, int y2) {
    hor_line(y1, x1, x2);
    hor_line(y2, x1, x2);
    ver_line(x1, y1, y2);
    ver_line(x2, y1, y2);
}

void Screen::draw_rect(int x1, int y1, int x2, int y2, SDL_Color c) {
    set_color(c);
    draw_rect(x1, y1, x2, y2);
}

void Screen::fill_rect(int x1, int y1, int x2, int y2) {
    SDL_Rect r;
    r.x = x1;
    r.y = y1;
    r.w = x2 - x1 + 1;
    r.h = y2 - y1 + 1;
    SDL_RenderFillRect(renderer, &r);
}

void Screen::fill_rect(int x1, int y1, int x2, int y2, SDL_Color c) {
    set_color(c);
    fill_rect(x1, y1, x2, y2);
}

void Screen::draw_line(int x1, int y1, int x2, int y2) {
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

void Screen::draw_line(int x1, int y1, int x2, int y2, SDL_Color c) {
    set_color(c);
    draw_line(x1, y1, x2, y2);
}

// Code taken from Rosetta Code
void Screen::draw_circle(int x, int y, int r) {
    int f = 1 - r;
    int ddF_x = 0;
    int ddF_y = -2 * r;
    int iter_x = 0;
    int iter_y = r;

    draw_pixel(x, y + r);
    draw_pixel(x, y - r);
    draw_pixel(x + r, y);
    draw_pixel(x - r, y);

    while(iter_x < iter_y) {
        if(f >= 0) {
            iter_y--;
            ddF_y += 2;
            f += ddF_y;
        }
        iter_x++;
        ddF_x += 2;
        f += ddF_x + 1;    
        draw_pixel(x + iter_x, y + iter_y);
        draw_pixel(x - iter_x, y + iter_y);
        draw_pixel(x + iter_x, y - iter_y);
        draw_pixel(x - iter_x, y - iter_y);
        draw_pixel(x + iter_y, y + iter_x);
        draw_pixel(x - iter_y, y + iter_x);
        draw_pixel(x + iter_y, y - iter_x);
        draw_pixel(x - iter_y, y - iter_x);
    }
}

void Screen::draw_circle(int x, int y, int r, SDL_Color c) {
    set_color(c);
    draw_circle(x, y, r);
}

// Code taken from Rosetta Code
void Screen::fill_circle(int x, int y, int r) {
    int f = 1 - r;
    int ddF_x = 0;
    int ddF_y = -2 * r;
    int iter_x = 0;
    int iter_y = r;

    draw_pixel(x, y + r);
    draw_pixel(x, y - r);
    hor_line(y, x - r, x + r);

    while(iter_x < iter_y) {
        if(f >= 0) {
            iter_y--;
            ddF_y += 2;
            f += ddF_y;
        }
        iter_x++;
        ddF_x += 2;
        f += ddF_x + 1;    
        hor_line(y - iter_y, x - iter_x, x + iter_x);
        hor_line(y - iter_x, x - iter_y, x + iter_y);
        hor_line(y + iter_x, x - iter_y, x + iter_y);
        hor_line(y + iter_y, x - iter_x, x + iter_x);
    }
}

void Screen::fill_circle(int x, int y, int r, SDL_Color c) {
    set_color(c);
    fill_circle(x, y, r);
}

float Screen::frame_time() {
    return duration_cast<duration<float>>(current_frame_time -
                                          last_frame_time).count();
}

float Screen::fps() {
    return 1.0 / frame_time();
}

