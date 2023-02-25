#pragma once

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
  uint8_t* PixelAt(int x, int y) const {
    const int pixel_position = config_.pixels_per_scan_line * y + x;
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    return &config_.frame_buffer[4 * pixel_position];
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
