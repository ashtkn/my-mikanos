#include "graphics.h"

void RGBResv8BitPerColorPixelWriter::Write(int x, int y,
                                           const PixelColor& c) const {
  auto p = PixelAt(x, y);
  p[0] = c.r;  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  p[1] = c.g;  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  p[2] = c.b;  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
}

void BGRResv8BitPerColorPixelWriter::Write(int x, int y,
                                           const PixelColor& c) const {
  auto p = PixelAt(x, y);
  p[0] = c.b;  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  p[1] = c.g;  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  p[2] = c.r;  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
}
