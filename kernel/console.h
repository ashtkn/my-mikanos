#pragma once

#include "graphics.h"

class Console {
 public:
  static const int kRows = 25;
  static const int kColumns = 80;

  Console(const PixelWriter& writer, const PixelColor& fg_color,
          const PixelColor& bg_color);
  void PutString(const char* s);

 private:
  void NewLine();

  const PixelWriter& writer_;
  const PixelColor fg_color_, bg_color_;

  char buffer_[kRows][kColumns + 1];  // 1 for null character
  int cursor_row_, cursor_column_;
};
