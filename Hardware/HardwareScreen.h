#ifndef HARDWARESCREEN_H
#define HARDWARESCREEN_H

#include <SDL2/SDL.h>
#include <chrono>
#include "Color.h"

/*
 * This class is NOT thread-safe. All calls to member functions of this class
 * must be made on the same thread that instantiated this class.
 */
class Screen {
    public:
        const int width;
        const int height;
    private:
        SDL_Window* window;
        SDL_Renderer* renderer;
        std::chrono::high_resolution_clock::time_point last_frame_time;
        std::chrono::high_resolution_clock::time_point current_frame_time;
    public:
        const bool vsync;
        const bool full_screen;
    public:
        Screen(int size_x, int size_y, const char * name, bool full_screen,
               bool vsync);
        ~Screen();

        void commit();
        bool on_screen(int x, int y);
        void set_color(Uint8 r, Uint8 g, Uint8 b);
        void set_color(SDL_Color c);
        void draw_pixel(int x, int y);
        void draw_pixel(int x, int y, SDL_Color c);
        void cls();
        void fill_screen();
        void fill_screen(SDL_Color c);
        void hor_line(int y, int x1, int x2);
        void hor_line(int y, int x1, int x2, SDL_Color c);
        void ver_line(int x, int y1, int y2);
        void ver_line(int x, int y1, int y2, SDL_Color c);
        void draw_rect(int x1, int y1, int x2, int y2);
        void draw_rect(int x1, int y1, int x2, int y2, SDL_Color c);
        void fill_rect(int x1, int y1, int x2, int y2);
        void fill_rect(int x1, int y1, int x2, int y2, SDL_Color c);
        void draw_line(int x1, int y1, int x2, int y2);
        void draw_line(int x1, int y1, int x2, int y2, SDL_Color c);
        void draw_circle(int x, int y, int r);
        void draw_circle(int x, int y, int r, SDL_Color c);
        void fill_circle(int x, int y, int r);
        void fill_circle(int x, int y, int r, SDL_Color c);
        float frame_time();
        float fps();
};

#endif
