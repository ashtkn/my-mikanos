#include "font.h"

const uint8_t kFontA[16] = {
    0b00000000,  //
    0b00011000,  //    **
    0b00011000,  //    **
    0b00011000,  //    **
    0b00011000,  //    **
    0b00100100,  //   *  *
    0b00100100,  //   *  *
    0b00100100,  //   *  *
    0b00100100,  //   *  *
    0b01111110,  //  ******
    0b01000010,  //  *    *
    0b01000010,  //  *    *
    0b01000010,  //  *    *
    0b11100111,  // ***  ***
    0b00000000,  //
    0b00000000,  //
};

void WriteAscii(const PixelWriter& writer, int x, int y, char c,
                const PixelColor& color) {
  if (c != 'A') {
    return;
  }
  for (unsigned int dy = 0; dy < 16; ++dy) {
    for (unsigned int dx = 0; dx < 8; ++dx) {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
      if ((static_cast<uint8_t>(kFontA[dy] << dx) & 0x80u) != 0u) {
        writer.Write(x + dx, y + dy, color);
      }
    }
  }
}