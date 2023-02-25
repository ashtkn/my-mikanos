#include "console.h"

#include <cstring>

#include "font.h"

Console::Console(const PixelWriter& writer, const PixelColor& fg_color,
                 const PixelColor& bg_color)
    : writer_{writer},
      fg_color_{fg_color},
      bg_color_{bg_color},
      buffer_{},
      cursor_row_{0},
      cursor_column_{0} {}

void Console::PutString(const char* s) {
  while (*s != '\0') {
    if (*s == '\n') {
      NewLine();
    } else if (cursor_column_ < kColumns - 1) {
      WriteAscii(writer_, 8 * cursor_column_, 16 * cursor_row_, *s, fg_color_);
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
      buffer_[cursor_row_][cursor_column_] = *s;
      ++cursor_column_;
    }
    ++s;  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  }
}

void Console::NewLine() {
  cursor_column_ = 0;
  if (cursor_row_ < kRows - 1) {
    ++cursor_row_;
  } else {
    for (int y = 0; y < 16 * kRows; ++y) {
      for (int x = 0; x < 8 * kColumns; ++x) {
        writer_.Write(x, y, bg_color_);
      }
    }
    for (int row = 0; row < kRows - 1; ++row) {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
      auto dst = static_cast<void*>(buffer_[row]);
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
      auto src = static_cast<const void*>(buffer_[row + 1]);
      memcpy(dst, src, kColumns + 1);
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
      WriteString(writer_, 0, 16 * row, static_cast<const char*>(buffer_[row]),
                  fg_color_);
    }
    memset(static_cast<void*>(buffer_[kRows - 1]), 0, kColumns + 1);
  }
}