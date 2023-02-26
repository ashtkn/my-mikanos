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

void Console::PutString(std::string_view s) {
  for (int i = 0; s[i] != '\0'; ++i) {
    if (s[i] == '\n') {
      NewLine();
    } else if (cursor_column_ < kColumns - 1) {
      WriteAscii(writer_, 8 * cursor_column_, 16 * cursor_row_, s[i],
                 fg_color_);
      buffer_.at(cursor_row_).at(cursor_column_) = s[i];
      ++cursor_column_;
    }
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
      memcpy(buffer_.at(row).data(), buffer_.at(row + 1).data(), kColumns + 1);
      WriteString(writer_, 0, 16 * row,
                  static_cast<const char*>(buffer_.at(row).data()), fg_color_);
    }
    memset(static_cast<void*>(buffer_[kRows - 1].data()), 0, kColumns + 1);
  }
}
