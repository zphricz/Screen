#ifndef COLOR_H
#define COLOR_H

#include <SDL2/SDL.h>

inline SDL_Color blend(const SDL_Color &fg, const SDL_Color &bg) {
  auto idiv = [](Uint32 num, Uint32 den) {
    return num / den + ((num % den) * 2 >= den);
  };
  Uint32 temp_a = fg.a * 255 + bg.a * (255 - fg.a);
  Uint32 out_a = fg.a + idiv(bg.a * (255 - fg.a), 255);
  if (temp_a == 0) {
    return {0, 0, 0, 0};
  }
  Uint32 out_r = idiv(fg.r * fg.a * 255 + bg.r * bg.a * (255 - fg.a), temp_a);
  Uint32 out_g = idiv(fg.g * fg.a * 255 + bg.g * bg.a * (255 - fg.a), temp_a);
  Uint32 out_b = idiv(fg.b * fg.a * 255 + bg.b * bg.a * (255 - fg.a), temp_a);
  return {Uint8(out_r), Uint8(out_g), Uint8(out_b), Uint8(out_a)};
}

inline SDL_Color operator*(const SDL_Color &lhs, float factor) {
  Uint8 out_r = std::min<int>(rint(lhs.r * factor), 255);
  Uint8 out_g = std::min<int>(rint(lhs.g * factor), 255);
  Uint8 out_b = std::min<int>(rint(lhs.b * factor), 255);
  return {out_r, out_g, out_b, lhs.a};
}

inline SDL_Color &operator*=(SDL_Color &lhs, float factor) {
  return lhs = lhs * factor;
}

inline SDL_Color operator/(const SDL_Color &lhs, float factor) {
  return {Uint8(rint(lhs.r / factor)), Uint8(rint(lhs.g / factor)),
          Uint8(rint(lhs.b / factor)), lhs.a};
}

inline SDL_Color &operator/=(SDL_Color &lhs, float factor) {
  return lhs = lhs / factor;
}

#endif
