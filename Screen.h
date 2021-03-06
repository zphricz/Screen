#ifndef SCREEN_H
#define SCREEN_H

#include "Color.h"
#include <SDL2/SDL.h>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>

#define likely(x) __builtin_expect((x), 1)
#define unlikely(x) __builtin_expect((x), 0)

constexpr int rshift = 16;
constexpr int gshift = 8;
constexpr int bshift = 0;

/*
 * When instantiated as a hardware renderer, all draw calls must be made on the
 * same thread as the thread that instantiated the Screen. If the only draw
 * calls being made are to draw_pixel(), it would likely be more efficient for
 * the Screen to be instantiated as a software renderer. When instantiated as
 * a hardware renderer, all draw calls are clipped. When instantiated as a
 * hardware renderer, some functionality is lost (such as the ability to draw
 * out the current Screen state and to record movies. The Screens are double
 * buffered. Because of this, you need to make sure that you draw to the whole
 * Screen before calling commit(), or else pixels from the frame before last
 * will appear on screen again.
 */

/*
 * SOFT sets whether or not the Screen uses a software or hardware renderer
 * for drawing. CLIPPED sets whether or not to clip all drawing functions.
 * Setting it to true will reduce performance.
 */
template <bool SOFT = true, bool CLIPPED = true> class Screen {
public:
  // It should be pretty obvious what this does
  Screen(int size_x, int size_y, const char *name, bool full_screen,
         bool vsync);
  ~Screen();
  // Makes all draw calls visible
  void commit();
  // returns true if (x, y) is within screen bounds
  bool on_screen(int x, int y);
  // Returns x clipped to within [0, width - 1]
  int clip_x(int x);
  // Returns y clipped to within [0, height - 1]
  int clip_y(int y);
  // Sets the default draw color to {r, g, b, 255}
  void set_color(Uint8 r, Uint8 g, Uint8 b);
  // Sets the default draw color to c
  void set_color(SDL_Color c);
  // Covers the screen with black
  void cls();
  // The followng functions return true if the Screen is configured as such
  bool is_direct_draw();
  bool is_vsync();
  bool is_full_screen();
  bool is_software();
  bool is_clipped();

  // The following draw functions can either be called with or without a
  // color. When called with a color, they will draw with that color. When
  // called without a color, they will draw with the last color set with
  // set_color().

  // Draws a pixel at (x, y)
  void draw_pixel(int x, int y);
  void draw_pixel(int x, int y, SDL_Color c);
  // Covers the screen with any color
  void fill_screen();
  void fill_screen(SDL_Color c);
  // Draws a line from (x1, y) to (x2, y) inclusive
  void hor_line(int y, int x1, int x2);
  void hor_line(int y, int x1, int x2, SDL_Color c);
  // Draws a line from (x, y1) to (x, y2) inclusive
  void ver_line(int x, int y1, int y2);
  void ver_line(int x, int y1, int y2, SDL_Color c);
  // Draws the borders of a rectangle from (x1, y1) to (x2, y2) inclusive
  void draw_rect(int x1, int y1, int x2, int y2);
  void draw_rect(int x1, int y1, int x2, int y2, SDL_Color c);
  // Fills a rectangle from (x1, y1) to (x2, y2) inclusive
  void fill_rect(int x1, int y1, int x2, int y2);
  void fill_rect(int x1, int y1, int x2, int y2, SDL_Color c);
  // Draws a line from (x1, y1) to (x2, y2) inclusive. The implementation for
  // this for the software Screen was taken from Rosetta code.
  void draw_line(int x1, int y1, int x2, int y2);
  void draw_line(int x1, int y1, int x2, int y2, SDL_Color c);
  // Draws the boundaries of a circle centered at (x, y) with 2 * r + 1 many
  // pixels for diameter. The implementation for this was taken from Rosetta
  // code.
  void draw_circle(int x, int y, int r);
  void draw_circle(int x, int y, int r, SDL_Color c);
  // Fills a circle centered at (x, y) with 2 * r + 1 many pixels for
  // diameter. The implementation for this was taken from Rosetta code.
  void fill_circle(int x, int y, int r);
  void fill_circle(int x, int y, int r, SDL_Color c);

  // Draws the current state of the screen into a .bmp file. Call this
  // directly
  // before a commit(). If instead you call it right after a commit, it will
  // draw the frame that was committed before the one that was just comitted.
  // This function does not work for hardware screens.
  void write_bmp(const char *name);
  // Draws the current state of the screen into a .tga file. Call this
  // directly
  // before a commit(). If instead you call it right after a commit, it will
  // draw the frame that was committed before the one that was just comitted.
  // This function does not work for hardware screens.
  void write_tga(const char *name);
  // Toggles whether or not to save each committed screen to a .tga file Does
  // not work for hardware Sceens.
  void toggle_recording();
  // Sets the directory to save recorded frames to and how many '0' characters
  // to pad their name to
  void set_recording_style(const char *image_dir, int z_fill);
  // Returns the time (in seconds) between the last two screen commits
  float frame_time();
  // Returns the number of frames committed per second (calculated only by
  // the time taken between the last two screen commits)
  float fps();
};

// A clipped software screen
typedef Screen<true, true> SoftScreen;
// A software screen that isn't clipped
typedef Screen<true, false> PerfSoftScreen;
// A clipped hardware screen
typedef Screen<false, true> HardScreen;

/* Implementation of software Screen */
template <bool CLIPPED> class Screen<true, CLIPPED> {
private:
  Uint32 *pixels;
  Uint32 default_color;

public:
  const int width;
  const int height;

private:
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *buffer_0; // First buffer
  SDL_Texture *buffer_1; // Second buffer
  std::chrono::high_resolution_clock::time_point last_frame_time;
  std::chrono::high_resolution_clock::time_point current_frame_time;
  bool buffer_flipper; // Controls which texture draw calls draw to
  bool recording;
  int image_number;
  std::string image_dir;
  int z_fill;

public:
  const bool vsynced;
  const bool full_screen;

private:
  Uint32 format_color(Uint8 r, Uint8 g, Uint8 b) {
    return (r << rshift) | (g << gshift) | (b << bshift);
  }

  Uint32 format_color(SDL_Color c) { return format_color(c.r, c.g, c.b); }

  inline Uint32 &pixel_at(int x, int y) { return pixels[y * width + x]; }

  void set_color(Uint32 c) { default_color = c; }

  void draw_pixel(int x, int y, Uint32 c) {
    if (CLIPPED) {
      if (unlikely(!on_screen(x, y))) {
        return;
      }
    }
    pixel_at(x, y) = c;
  }

  void fill_screen(Uint32 c) {
    for (int i = 0; i < width * height; ++i) {
      pixels[i] = c;
    }
  }

  void hor_line_fast(int y, int x1, int x2, Uint32 c) {
    if (CLIPPED) {
      if (unlikely(y >= height || y < 0 || (x1 >= width && x2 >= width) ||
                   (x1 < 0 && x2 < 0))) {
        return;
      }
      x1 = clip_x(x1);
      x2 = clip_x(x2);
    }
    int iter_i = y * width + x1;
    int end_i = iter_i + (x2 - x1);
    for (; iter_i <= end_i; ++iter_i) {
      pixels[iter_i] = c;
    }
  }

  void hor_line(int y, int x1, int x2, Uint32 c) {
    int iter_i;
    int end_i;
    if (CLIPPED) {
      if (unlikely(y >= height || y < 0 || (x1 >= width && x2 >= width) ||
                   (x1 < 0 && x2 < 0))) {
        return;
      }
      x1 = clip_x(x1);
      x2 = clip_x(x2);
    }
    if (x1 < x2) {
      iter_i = y * width + x1;
      end_i = iter_i + (x2 - x1);
    } else {
      iter_i = y * width + x2;
      end_i = iter_i + (x1 - x2);
    }
    for (; iter_i <= end_i; ++iter_i) {
      pixels[iter_i] = c;
    }
  }

  void ver_line(int x, int y1, int y2, Uint32 c) {
    int iter_i;
    int end_i;
    if (CLIPPED) {
      if (unlikely(x >= width || x < 0 || (y1 >= height && y2 >= height) ||
                   (y1 < 0 && y2 < 0))) {
        return;
      }
      y1 = clip_y(y1);
      y2 = clip_y(y2);
    }
    if (y1 < y2) {
      iter_i = y1 * width + x;
      end_i = y2 * width + x;
    } else {
      iter_i = y2 * width + x;
      end_i = y1 * width + x;
    }
    for (; iter_i <= end_i; iter_i += width) {
      pixels[iter_i] = c;
    }
  }

  void draw_rect(int x1, int y1, int x2, int y2, Uint32 c) {
    hor_line(y1, x1, x2, c);
    hor_line(y2, x1, x2, c);
    ver_line(x1, y1, y2, c);
    ver_line(x2, y1, y2, c);
  }

  void fill_rect(int x1, int y1, int x2, int y2, Uint32 c) {
    int iter_y;
    int end_y;
    int start_x;
    int dx;
    if (CLIPPED) {
      if (unlikely((x1 < 0 && x2 < 0) || (y1 < 0 && y2 < 0) ||
                   (x1 >= width && x2 >= width) ||
                   (y1 >= height && y2 >= height))) {
        return;
      }
      x1 = clip_x(x1);
      x2 = clip_x(x2);
      y1 = clip_y(y1);
      y2 = clip_y(y2);
    }
    if (y1 < y2) {
      iter_y = y1;
      end_y = y2;
    } else {
      iter_y = y2;
      end_y = y1;
    }
    if (x1 < x2) {
      start_x = x1;
      dx = x2 - x1;
    } else {
      start_x = x2;
      dx = x1 - x2;
    }

    for (; iter_y <= end_y; ++iter_y) {
      int iter_x = iter_y * width + start_x;
      int end_x = iter_x + dx;
      for (; iter_x <= end_x; ++iter_x) {
        pixels[iter_x] = c;
      }
    }
  }

  void draw_line(int x1, int y1, int x2, int y2, Uint32 c) {
    int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;
    while (true) {
      draw_pixel(x1, y1, c);
      if (x1 == x2 && y1 == y2) {
        break;
      }
      e2 = err;
      if (e2 > -dx) {
        err -= dy;
        x1 += sx;
      }
      if (e2 < dy) {
        err += dx;
        y1 += sy;
      }
    }
  }

  void draw_circle(int x, int y, int r, Uint32 c) {
    int f = 1 - r;
    int ddF_x = 0;
    int ddF_y = -2 * r;
    int iter_x = 0;
    int iter_y = r;

    draw_pixel(x, y + r, c);
    draw_pixel(x, y - r, c);
    draw_pixel(x + r, y, c);
    draw_pixel(x - r, y, c);

    while (iter_x < iter_y) {
      if (f >= 0) {
        iter_y--;
        ddF_y += 2;
        f += ddF_y;
      }
      iter_x++;
      ddF_x += 2;
      f += ddF_x + 1;
      draw_pixel(x + iter_x, y + iter_y, c);
      draw_pixel(x - iter_x, y + iter_y, c);
      draw_pixel(x + iter_x, y - iter_y, c);
      draw_pixel(x - iter_x, y - iter_y, c);
      draw_pixel(x + iter_y, y + iter_x, c);
      draw_pixel(x - iter_y, y + iter_x, c);
      draw_pixel(x + iter_y, y - iter_x, c);
      draw_pixel(x - iter_y, y - iter_x, c);
    }
  }

  void fill_circle(int x, int y, int r, Uint32 c) {
    int f = 1 - r;
    int ddF_x = 0;
    int ddF_y = -2 * r;
    int iter_x = 0;
    int iter_y = r;

    draw_pixel(x, y + r, c);
    draw_pixel(x, y - r, c);
    hor_line_fast(y, x - r, x + r, c);

    while (iter_x < iter_y) {
      if (f >= 0) {
        iter_y--;
        ddF_y += 2;
        f += ddF_y;
      }
      iter_x++;
      ddF_x += 2;
      f += ddF_x + 1;
      hor_line_fast(y - iter_y, x - iter_x, x + iter_x, c);
      hor_line_fast(y + iter_y, x - iter_x, x + iter_x, c);
      hor_line_fast(y - iter_x, x - iter_y, x + iter_y, c);
      hor_line_fast(y + iter_x, x - iter_y, x + iter_y, c);
    }
  }

public:
  Screen(int size_x, int size_y, const char *name, bool full_screen, bool vsync)
      : default_color(format_color(255, 255, 255)), width(size_x),
        height(size_y), buffer_flipper(false), recording(false),
        image_number(0), image_dir("."), z_fill(5), vsynced(vsync),
        full_screen(full_screen) {
    if (full_screen) {
      window = SDL_CreateWindow(name, 0, 0, width, height,
                                SDL_WINDOW_FULLSCREEN_DESKTOP);
    } else {
      window = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED, width, height, 0);
    }
    if (vsync) {
      renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
    } else {
      renderer = SDL_CreateRenderer(window, -1, 0);
    }
    buffer_0 = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                 SDL_TEXTUREACCESS_STREAMING, width, height);
    buffer_1 = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                 SDL_TEXTUREACCESS_STREAMING, width, height);
    int pitch;
    SDL_LockTexture(buffer_0, nullptr, reinterpret_cast<void **>(&pixels),
                    &pitch);
    current_frame_time = last_frame_time =
        std::chrono::high_resolution_clock::now();
  }

  ~Screen() {
    if (!buffer_flipper) {
      SDL_UnlockTexture(buffer_0);
    } else {
      SDL_UnlockTexture(buffer_1);
    }
    SDL_DestroyTexture(buffer_0);
    SDL_DestroyTexture(buffer_1);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
  }

  bool is_vsync() { return vsynced; }

  bool is_full_screen() { return full_screen; }

  bool is_software() { return true; }

  bool is_clipped() { return CLIPPED; }

  void commit() {
    if (recording) {
      std::ostringstream convert;
      convert << std::setw(z_fill) << std::setfill('0') << image_number;
      std::string name = image_dir + "/image_" + convert.str() + ".bmp";
      write_bmp(name.c_str());
      image_number++;
    }
    int pitch;
    if (!buffer_flipper) {
      SDL_UnlockTexture(buffer_0);
      SDL_RenderCopy(renderer, buffer_0, nullptr, nullptr);
      SDL_RenderPresent(renderer);
      SDL_LockTexture(buffer_1, nullptr, reinterpret_cast<void **>(&pixels),
                      &pitch);
    } else {
      SDL_UnlockTexture(buffer_1);
      SDL_RenderCopy(renderer, buffer_1, nullptr, nullptr);
      SDL_RenderPresent(renderer);
      SDL_LockTexture(buffer_0, nullptr, reinterpret_cast<void **>(&pixels),
                      &pitch);
    }
    last_frame_time = current_frame_time;
    current_frame_time = std::chrono::high_resolution_clock::now();
    buffer_flipper = !buffer_flipper;
  }

  bool on_screen(int x, int y) {
    return (x >= 0) && (x < width) && (y >= 0) && (y < height);
  }

  int clip_x(int x) {
    if (unlikely(x >= width)) {
      x = width - 1;
    }
    if (unlikely(x < 0)) {
      x = 0;
    }
    return x;
  }

  int clip_y(int y) {
    if (unlikely(y >= height)) {
      y = height - 1;
    }
    if (unlikely(y < 0)) {
      y = 0;
    }
    return y;
  }

  void set_color(Uint8 r, Uint8 g, Uint8 b) {
    default_color = format_color(r, g, b);
  }

  void set_color(SDL_Color c) { default_color = format_color(c); }

  void cls() { memset(pixels, 0, width * height * sizeof(Uint32)); }

  void draw_pixel(int x, int y) { draw_pixel(x, y, default_color); }

  void draw_pixel(int x, int y, SDL_Color c) {
    draw_pixel(x, y, format_color(c));
  }

  void fill_screen() { fill_screen(default_color); }

  void fill_screen(SDL_Color c) { fill_screen(format_color(c)); }
  void hor_line(int y, int x1, int x2) { hor_line(y, x1, x2, default_color); }

  void hor_line(int y, int x1, int x2, SDL_Color c) {
    hor_line(y, x1, x2, format_color(c));
  }
  void ver_line(int x, int y1, int y2) { ver_line(x, y1, y2, default_color); }

  void ver_line(int x, int y1, int y2, SDL_Color c) {
    ver_line(x, y1, y2, format_color(c));
  }
  void draw_rect(int x1, int y1, int x2, int y2) {
    draw_rect(x1, y1, x2, y2, default_color);
  }

  void draw_rect(int x1, int y1, int x2, int y2, SDL_Color c) {
    draw_rect(x1, y1, x2, y2, format_color(c));
  }

  void fill_rect(int x1, int y1, int x2, int y2) {
    fill_rect(x1, y1, x2, y2, default_color);
  }

  void fill_rect(int x1, int y1, int x2, int y2, SDL_Color c) {
    fill_rect(x1, y1, x2, y2, format_color(c));
  }

  void draw_line(int x1, int y1, int x2, int y2) {
    draw_line(x1, y1, x2, y2, default_color);
  }

  void draw_line(int x1, int y1, int x2, int y2, SDL_Color c) {
    draw_line(x1, y1, x2, y2, format_color(c));
  }

  void draw_circle(int x, int y, int r) { draw_circle(x, y, r, default_color); }

  void draw_circle(int x, int y, int r, SDL_Color c) {
    draw_circle(x, y, r, format_color(c));
  }

  void fill_circle(int x, int y, int r) { fill_circle(x, y, r, default_color); }

  void fill_circle(int x, int y, int r, SDL_Color c) {
    fill_circle(x, y, r, format_color(c));
  }

  /*
   * The code for this function was taken from:
   * http://stackoverflow.com/questions/2654480/writing-bmp-image-in-pure-c-c-without-other-libraries
   */
  void write_bmp(const char *name) {
    std::ofstream out_stream(name, std::ios_base::binary);
    Uint8 file[14] = {
        'B',     'M', // magic
        0,       0,   0,
        0,          // size in bytes
        0,       0, // app data
        0,       0, // app data
        40 + 14, 0,   0,
        0 // start of data offset
    };
    Uint8 info[40] = {
        40,   0,    0, 0, // info hd size
        0,    0,    0, 0, // width
        0,    0,    0, 0, // heigth
        1,    0,          // number color planes
        24,   0,          // bits per pixel
        0,    0,    0, 0, // compression is none
        0,    0,    0, 0, // image bits size
        0x13, 0x0B, 0, 0, // horz resolution in pixel / m
        0x13, 0x0B, 0, 0, // vert resolutions (0x03C3 = 96 dpi, 0x0B13 = 72 dpi)
        0,    0,    0, 0, // #colors in pallete
        0,    0,    0, 0, // #important colors
    };
    Uint32 pad_size = (4 - (width * 3) % 4) % 4;
    Uint32 size_data = width * height * 3 + height * pad_size;
    Uint32 size_all = size_data + sizeof(file) + sizeof(info);
    file[2] = size_all;
    file[3] = size_all >> 8;
    file[4] = size_all >> 16;
    file[5] = size_all >> 24;
    info[4] = width;
    info[5] = width >> 8;
    info[6] = width >> 16;
    info[7] = width >> 24;
    info[8] = height;
    info[9] = height >> 8;
    info[10] = height >> 16;
    info[11] = height >> 24;
    info[20] = size_data;
    info[21] = size_data >> 8;
    info[22] = size_data >> 16;
    info[23] = size_data >> 24;
    for (auto &ch : file) {
      out_stream.put(ch);
    }
    for (auto &ch : info) {
      out_stream.put(ch);
    }
    for (int y = height - 1; y >= 0; --y) {
      for (int x = 0; x < width; x++) {
        Uint32 c = pixel_at(x, y);
        Uint8 r = (c >> rshift) & 0xFF;
        Uint8 g = (c >> gshift) & 0xFF;
        Uint8 b = (c >> bshift) & 0xFF;
        out_stream.put(b).put(g).put(r);
      }
      for (Uint32 i = 0; i < pad_size; ++i) {
        out_stream.put(0);
      }
    }
  }

  void write_tga(const char *name) {
    std::ofstream file(name, std::ios_base::binary);
    file.put(0).put(0).put(2).put(0).put(0).put(0).put(0).put(0).put(0);
    file.put(0).put(0).put(0).put(width & 0xFF).put((width >> 8) & 0xFF);
    file.put(height & 0xFF).put((height >> 8) & 0xFF).put(24).put(0);
    for (int y = height - 1; y >= 0; --y) {
      for (int x = 0; x < width; ++x) {
        Uint32 c = pixel_at(x, y);
        Uint8 r = (c >> rshift) & 0xFF;
        Uint8 g = (c >> gshift) & 0xFF;
        Uint8 b = (c >> bshift) & 0xFF;
        file.put(b).put(g).put(r);
      }
    }
  }

  void toggle_recording() { recording = !recording; }

  void set_recording_style(const char *image_dir, int z_fill) {
    this->image_dir = image_dir;
    this->z_fill = z_fill;
    image_number = 0;
  }

  float frame_time() {
    return std::chrono::duration_cast<std::chrono::duration<float>>(
               current_frame_time - last_frame_time)
        .count();
  }

  float fps() { return 1.0 / frame_time(); }
};

/* Implementation of hardware Screen */
template <bool CLIPPED> class Screen<false, CLIPPED> {
private:
  Uint32 *pixels;
  SDL_Color default_color;

public:
  const int width;
  const int height;

private:
  SDL_Window *window;
  SDL_Renderer *renderer;
  std::chrono::high_resolution_clock::time_point last_frame_time;
  std::chrono::high_resolution_clock::time_point current_frame_time;

public:
  const bool vsynced;
  const bool full_screen;

private:
  Uint32 format_color(Uint8 r, Uint8 g, Uint8 b) {
    return (r << rshift) | (g << gshift) | (b << bshift);
  }

  Uint32 format_color(SDL_Color c) { return format_color(c.r, c.g, c.b); }

  void set_default_color() {
    SDL_SetRenderDrawColor(renderer, default_color.r, default_color.g,
                           default_color.b, default_color.a);
  }

public:
  Screen(int size_x, int size_y, const char *name, bool full_screen, bool vsync)
      : default_color({255, 255, 255}), width(size_x), height(size_y),
        vsynced(vsync), full_screen(full_screen) {
    static_assert(CLIPPED == true, "Hardware Screens can only be clipped");
    if (full_screen) {
      window =
          SDL_CreateWindow(name, 0, 0, width, height, SDL_WINDOW_FULLSCREEN);
    } else {
      window = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED, width, height, 0);
    }
    if (vsync) {
      renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC |
                                                    SDL_RENDERER_ACCELERATED);
    } else {
      renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    }
    current_frame_time = last_frame_time =
        std::chrono::high_resolution_clock::now();
  }

  ~Screen() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
  }

  bool is_direct_draw() { return false; }

  bool is_vsync() { return vsynced; }

  bool is_full_screen() { return full_screen; }

  bool is_software() { return false; }

  bool is_clipped() { return true; }

  void commit() {
    SDL_RenderPresent(renderer);
    last_frame_time = current_frame_time;
    current_frame_time = std::chrono::high_resolution_clock::now();
  }

  bool on_screen(int x, int y) {
    return (x >= 0) && (x < width) && (y >= 0) && (y < height);
  }

  int clip_x(int x) {
    if (unlikely(x >= width)) {
      x = width - 1;
    }
    if (unlikely(x < 0)) {
      x = 0;
    }
    return x;
  }

  int clip_y(int y) {
    if (unlikely(y >= height)) {
      y = height - 1;
    }
    if (unlikely(y < 0)) {
      y = 0;
    }
    return y;
  }

  void set_color(Uint8 r, Uint8 g, Uint8 b) {
    default_color = {r, g, b, 255};
    set_default_color();
  }

  void set_color(SDL_Color c) {
    default_color = c;
    set_default_color();
  }

  void cls() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    set_default_color();
  }

  void draw_pixel(int x, int y) { SDL_RenderDrawPoint(renderer, x, y); }

  void draw_pixel(int x, int y, SDL_Color c) {
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    SDL_RenderDrawPoint(renderer, x, y);
    set_default_color();
  }

  void fill_screen() { SDL_RenderClear(renderer); }

  void fill_screen(SDL_Color c) {
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    SDL_RenderClear(renderer);
    set_default_color();
  }

  void hor_line(int y, int x1, int x2) {
    SDL_RenderDrawLine(renderer, x1, y, x2, y);
  }

  void hor_line(int y, int x1, int x2, SDL_Color c) {
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    SDL_RenderDrawLine(renderer, x1, y, x2, y);
    set_default_color();
  }

  void ver_line(int x, int y1, int y2) {
    SDL_RenderDrawLine(renderer, x, y1, x, y2);
  }

  void ver_line(int x, int y1, int y2, SDL_Color c) {
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    SDL_RenderDrawLine(renderer, x, y1, x, y2);
    set_default_color();
  }

  void draw_rect(int x1, int y1, int x2, int y2) {
    SDL_Rect r;
    r.x = x1;
    r.y = y1;
    r.w = x2 - x1 + 1;
    r.h = y2 - y1 + 1;
    SDL_RenderDrawRect(renderer, &r);
  }

  void draw_rect(int x1, int y1, int x2, int y2, SDL_Color c) {
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    SDL_Rect r;
    r.x = x1;
    r.y = y1;
    r.w = x2 - x1 + 1;
    r.h = y2 - y1 + 1;
    SDL_RenderDrawRect(renderer, &r);
    set_default_color();
  }

  void fill_rect(int x1, int y1, int x2, int y2) {
    SDL_Rect r;
    r.x = x1;
    r.y = y1;
    r.w = x2 - x1 + 1;
    r.h = y2 - y1 + 1;
    SDL_RenderFillRect(renderer, &r);
  }

  void fill_rect(int x1, int y1, int x2, int y2, SDL_Color c) {
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    SDL_Rect r;
    r.x = x1;
    r.y = y1;
    r.w = x2 - x1 + 1;
    r.h = y2 - y1 + 1;
    SDL_RenderFillRect(renderer, &r);
    set_default_color();
  }

  void draw_line(int x1, int y1, int x2, int y2) {
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
  }

  void draw_line(int x1, int y1, int x2, int y2, SDL_Color c) {
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    set_default_color();
  }

  void draw_circle(int x, int y, int r) {
    int f = 1 - r;
    int ddF_x = 0;
    int ddF_y = -2 * r;
    int iter_x = 0;
    int iter_y = r;

    SDL_Point outer_points[4] = {{.x = x, .y = y + r},
                                 {.x = x, .y = y - r},
                                 {.x = x + r, .y = y},
                                 {.x = x - r, .y = y}};
    SDL_RenderDrawPoints(renderer, outer_points, 4);
    while (iter_x < iter_y) {
      if (f >= 0) {
        iter_y--;
        ddF_y += 2;
        f += ddF_y;
      }
      iter_x++;
      ddF_x += 2;
      f += ddF_x + 1;
      SDL_Point inner_points[8] = {{.x = x + iter_x, .y = y + iter_y},
                                   {.x = x - iter_x, .y = y + iter_y},
                                   {.x = x + iter_x, .y = y - iter_y},
                                   {.x = x - iter_x, .y = y - iter_y},
                                   {.x = x + iter_y, .y = y + iter_x},
                                   {.x = x - iter_y, .y = y + iter_x},
                                   {.x = x + iter_y, .y = y - iter_x},
                                   {.x = x - iter_y, .y = y - iter_x}};
      SDL_RenderDrawPoints(renderer, inner_points, 8);
    }
  }

  void draw_circle(int x, int y, int r, SDL_Color c) {
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    draw_circle(x, y, r);
    set_default_color();
  }

  void fill_circle(int x, int y, int r) {
    int f = 1 - r;
    int ddF_x = 0;
    int ddF_y = -2 * r;
    int iter_x = 0;
    int iter_y = r;

    SDL_Point points[2] = {{.x = x, .y = y + r}, {.x = x, .y = y - r}};
    SDL_RenderDrawPoints(renderer, points, 2);
    hor_line(y, x - r, x + r);
    while (iter_x < iter_y) {
      if (f >= 0) {
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

  void fill_circle(int x, int y, int r, SDL_Color c) {
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    fill_circle(x, y, r);
    set_default_color();
  }

  void write_bmp(const char *name) { return; }

  void write_tga(const char *name) { return; }

  void toggle_recording() { return; }

  void set_recording_style(const char *image_dir, int z_fill) { return; }

  float frame_time() {
    return std::chrono::duration_cast<std::chrono::duration<float>>(
               current_frame_time - last_frame_time)
        .count();
  }

  float fps() { return 1.0 / frame_time(); }
};

#endif
