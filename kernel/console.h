#pragma once

#include <array>
#include <string_view>

#include "graphics.h"

class Console {
 public:
  static const int kRows = 25;
  static const int kColumns = 80;

  Console(const PixelWriter& writer, const PixelColor& fg_color,
          const PixelColor& bg_color);
  void PutString(std::string_view s);

 private:
  void NewLine();

  const PixelWriter& writer_;
  const PixelColor fg_color_, bg_color_;

  std::array<std::array<char, kColumns + 1>, kRows>
      buffer_;  // 1 for null character
  int cursor_row_, cursor_column_;
};
