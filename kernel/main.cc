#include <cstddef>
#include <cstdint>

#include "frame_buffer_config.h"

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
  void Write(int x, int y, const PixelColor& c) const override {
    auto p = PixelAt(x, y);
    p[0] = c.r;  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    p[1] = c.g;  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    p[2] = c.b;  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  }
};

class BGRResv8BitPerColorPixelWriter : public PixelWriter {
 public:
  using PixelWriter::PixelWriter;
  void Write(int x, int y, const PixelColor& c) const override {
    auto p = PixelAt(x, y);
    p[0] = c.b;  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    p[1] = c.g;  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    p[2] = c.r;  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  }
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

// NOLINTNEXTLINE(fuchsia-overloaded-operator,misc-unused-parameters)
void* operator new(size_t size, void* buf) { return buf; }

// NOLINTNEXTLINE(fuchsia-overloaded-operator,misc-unused-parameters,misc-new-delete-overloads,cert-dcl54-cpp,hicpp-new-delete-operators)
void operator delete(void* obj) noexcept {}

char pixel_writer_buf[sizeof(RGBResv8BitPerColorPixelWriter)];
PixelWriter* pixel_writer;

extern "C" void KernelMain(const FrameBufferConfig& frame_buffer_config) {
  switch (frame_buffer_config.pixel_format) {
    case kPixelRGBResv8BitPerColor:
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)
      pixel_writer = new (pixel_writer_buf)
          RGBResv8BitPerColorPixelWriter{frame_buffer_config};
      break;
    case kPixelBGRResv8BitPerColor:
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)
      pixel_writer = new (pixel_writer_buf)
          BGRResv8BitPerColorPixelWriter{frame_buffer_config};
      break;
  }

  for (int x = 0; x < frame_buffer_config.horizontal_resolution; ++x) {
    for (int y = 0; y < frame_buffer_config.vertical_resolution; ++y) {
      pixel_writer->Write(x, y, {255, 255, 255});
    }
  }
  for (int x = 0; x < 200; ++x) {
    for (int y = 0; y < 100; ++y) {
      pixel_writer->Write(100 + x, 100 + y, {0, 255, 0});
    }
  }

  WriteAscii(*pixel_writer, 50, 50, 'A', {0, 0, 0});
  WriteAscii(*pixel_writer, 58, 50, 'A', {0, 0, 0});

  while (true) {
    __asm__("hlt");  // NOLINT(hicpp-no-assembler)
  }
}
