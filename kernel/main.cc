#include <cstdint>

#include "frame_buffer_config.h"

struct PixelColor {
  uint8_t r, g, b;
};

int WritePixel(const FrameBufferConfig& config, int x, int y,
               const PixelColor& c) {
  const int pixel_position = config.pixels_per_scan_line * y + x;
  switch (config.pixel_format) {
    case kPixelRGBResv8BitPerColor: {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      uint8_t* p = &config.frame_buffer[4 * pixel_position];
      p[0] = c.r;  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      p[1] = c.g;  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      p[2] = c.b;  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      break;
    }
    case kPixelBGRResv8BitPerColor: {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      uint8_t* p = &config.frame_buffer[4 * pixel_position];
      p[0] = c.b;  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      p[1] = c.g;  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      p[2] = c.r;  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      break;
    }
    default:
      return 1;
  }
  return 0;
}

extern "C" void KernelMain(const FrameBufferConfig& frame_buffer_config) {
  for (int x = 0; x < frame_buffer_config.horizontal_resolution; ++x) {
    for (int y = 0; y < frame_buffer_config.vertical_resolution; ++y) {
      WritePixel(frame_buffer_config, x, y, {255, 255, 255});
    }
  }
  for (int x = 0; x < 200; ++x) {
    for (int y = 0; y < 100; ++y) {
      WritePixel(frame_buffer_config, 100 + x, 100 + y, {0, 255, 0});
    }
  }
  while (true) {
    __asm__("hlt");  // NOLINT(hicpp-no-assembler)
  }
}
