#ifndef SCREEN_H
#define SCREEN_H

#include <SDL2/SDL.h>
#include <chrono>

#include <string>

#define likely(x)   __builtin_expect((x),1)
#define unlikely(x) __builtin_expect((x),0)

struct Color {
    Uint8 r;
    Uint8 g;
    Uint8 b;

    Color operator+(const Color& other) { return {Uint8(r + other.r), Uint8(g + other.g), Uint8(b + other.b)}; }
    Color operator-(const Color& other) { return {Uint8(r - other.r), Uint8(g - other.g), Uint8(b - other.b)}; }
    Color operator*(float factor) { return {Uint8(r * factor), Uint8(g * factor), Uint8(b * factor)}; }
    Color operator/(float factor) { return {Uint8(r / factor), Uint8(g / factor), Uint8(b / factor)}; }
    Color& operator+=(const Color& other) { return *this = *this + other; }
    Color& operator-=(const Color& other) { return *this = *this - other; }
    Color& operator*=(float factor) { return *this = *this * factor; }
    Color& operator/=(float factor) { return *this = *this / factor; }
};

class Screen {
    private:
        Uint32* pixels;
        Uint32 default_color;
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Texture* texture;
        bool recording;
        int image_number;
        std::string image_dir;
        int z_fill;
        std::chrono::high_resolution_clock::time_point last_frame_time;
        std::chrono::high_resolution_clock::time_point current_frame_time;

        inline Uint32& pixel_at(int x, int y);
    public:
        const int width;
        const int height;
        const int rshift;
        const int gshift;
        const int bshift;
        const bool vsynced;
        const bool clipped;
        const bool direct_draw; /* Set to true for increased drawing
                                   performance. This may result in visual
                                   artifacts if drawing right after calling
                                   commit */

        Screen(int size_x, int size_y, bool full_screen, const char * name,
               bool vsync, bool clipped, bool direct = false);
        ~Screen();

        void commit();
        bool on_screen(int x, int y);
        int clip_x(int x);
        int clip_y(int y);
        Uint32 format_color(Uint8 r, Uint8 g, Uint8 b);
        Uint32 format_color(Color c);
        void set_color(Uint8 r, Uint8 g, Uint8 b);
        void set_color(Color c);
        void draw_pixel(int x, int y, Uint32 c);
        void draw_pixel(int x, int y);
        void draw_pixel(int x, int y, Color c);
        void cls();
        void fill_screen(Uint32 c);
        void fill_screen();
        void fill_screen(Color c);
        void hor_line(int y, int x1, int x2, Uint32 c);
        void hor_line(int y, int x1, int x2);
        void hor_line(int y, int x1, int x2, Color c);
        void ver_line(int x, int y1, int y2, Uint32 c);
        void ver_line(int x, int y1, int y2);
        void ver_line(int x, int y1, int y2, Color c);
        void draw_rect(int x1, int y1, int x2, int y2, Uint32 c);
        void draw_rect(int x1, int y1, int x2, int y2);
        void draw_rect(int x1, int y1, int x2, int y2, Color c);
        void fill_rect(int x1, int y1, int x2, int y2, Uint32 c);
        void fill_rect(int x1, int y1, int x2, int y2);
        void fill_rect(int x1, int y1, int x2, int y2, Color c);
        void draw_line(int x1, int y1, int x2, int y2, Uint32 c);
        void draw_line(int x1, int y1, int x2, int y2);
        void draw_line(int x1, int y1, int x2, int y2, Color c);
        void draw_circle(int x, int y, int r, Uint32 c);
        void draw_circle(int x, int y, int r);
        void draw_circle(int x, int y, int r, Color c);
        void fill_circle(int x, int y, int r, Uint32 c);
        void fill_circle(int x, int y, int r);
        void fill_circle(int x, int y, int r, Color c);
        void write_tga(const char * name);
        void toggle_recording();
        void set_recording_style(const char * image_dir, int z_fill);
        float frame_time();
        float fps();
};

#endif
