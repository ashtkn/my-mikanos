#include "graphics.h"

void RGBResv8BitPerColorPixelWriter::Write(int x, int y,
                                           const PixelColor& c) const {
  auto p = PixelAt(x, y);
  p[0] = std::byte{c.r};
  p[1] = std::byte{c.g};
  p[2] = std::byte{c.b};
}

void BGRResv8BitPerColorPixelWriter::Write(int x, int y,
                                           const PixelColor& c) const {
  auto p = PixelAt(x, y);
  p[0] = std::byte{c.b};
  p[1] = std::byte{c.g};
  p[2] = std::byte{c.r};
}

void DrawStrokeRectangle(const PixelWriter& writer,
                         const Vector2D<int>& position, const Size<int>& size,
                         const PixelColor& color) {
  for (int dx = 0; dx < size.w; ++dx) {
    writer.Write(position.x + dx, position.y, color);               // 上辺
    writer.Write(position.x + dx, position.y + size.h - 1, color);  // 下辺
  }
  for (int dy = 0; dy < size.h; ++dy) {
    writer.Write(position.x, position.y + dy, color);               // 左辺
    writer.Write(position.x + size.w - 1, position.y + dy, color);  // 右辺
  }
}

void DrawFillRectangle(const PixelWriter& writer, const Vector2D<int>& position,
                       const Size<int>& size, const PixelColor& color) {
  for (int dy = 0; dy < size.h; ++dy) {
    for (int dx = 0; dx < size.w; ++dx) {
      writer.Write(position.x + dx, position.y + dy, color);
    }
  }
}
