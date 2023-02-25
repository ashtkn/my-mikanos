#pragma once

#include "boost/core/span.hpp"
#include "frame_buffer_config.h"

struct PixelColor {
  uint8_t r, g, b;
};

class PixelWriter {
 public:
  explicit PixelWriter(const FrameBufferConfig& config) : config_{config} {}

  PixelWriter(const PixelWriter&) = delete;
  PixelWriter& operator=(const PixelWriter&) = delete;
  PixelWriter(PixelWriter&&) = delete;
  PixelWriter& operator=(PixelWriter&&) = delete;

  virtual ~PixelWriter() = default;

  virtual void Write(int x, int y, const PixelColor& c) const = 0;

 protected:
  boost::span<uint8_t, 4> PixelAt(int x, int y) const {
    const int pixel_position = config_.pixels_per_scan_line * y + x;
    return boost::span<uint8_t, 4>(&config_.frame_buffer[4 * pixel_position],
                                   4);
  }

 private:
  const FrameBufferConfig& config_;
};

class RGBResv8BitPerColorPixelWriter : public PixelWriter {
 public:
  using PixelWriter::PixelWriter;
  void Write(int x, int y, const PixelColor& c) const override;
};

class BGRResv8BitPerColorPixelWriter : public PixelWriter {
 public:
  using PixelWriter::PixelWriter;
  void Write(int x, int y, const PixelColor& c) const override;
};

template <typename T>
struct Vector2D {
  T x, y;
};

template <typename T>
struct Size {
  T w, h;
};

void DrawStrokeRectangle(const PixelWriter& writer,
                         const Vector2D<int>& position, const Size<int>& size,
                         const PixelColor& color);

void DrawFillRectangle(const PixelWriter& writer, const Vector2D<int>& position,
                       const Size<int>& size, const PixelColor& color);
