#include <cstddef>  // ここで clang-diagnostic-error を回避できないので無効化している
#include <cstdint>

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

  virtual void Write(int x, int y, const PixelColor& c) = 0;

 protected:
  uint8_t* PixelAt(int x, int y) {
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
  void Write(int x, int y, const PixelColor& c) override {
    auto p = PixelAt(x, y);
    p[0] = c.r;  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    p[1] = c.g;  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    p[2] = c.b;  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  }
};

class BGRResv8BitPerColorPixelWriter : public PixelWriter {
 public:
  using PixelWriter::PixelWriter;
  void Write(int x, int y, const PixelColor& c) override {
    auto p = PixelAt(x, y);
    p[0] = c.b;  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    p[1] = c.g;  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    p[2] = c.r;  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  }
};

// NOLINTNEXTLINE(fuchsia-overloaded-operator,misc-unused-parameters)
void* operator new(size_t size, void* buf) { return buf; }

// NOLINTNEXTLINE(fuchsia-overloaded-operator,misc-unused-parameters,misc-new-delete-overloads,cert-dcl54-cpp,hicpp-new-delete-operators)
void operator delete(void* obj) noexcept {}

char pixel_writer_buf[sizeof(RGBResv8BitPerColorPixelWriter)];
PixelWriter* pixel_writer;

extern "C" void KernelMain(const FrameBufferConfig& frame_buffer_config) {
  switch (frame_buffer_config.pixel_format) {
    case kPixelRGBResv8BitPerColor:
      pixel_writer = new (pixel_writer_buf)
          RGBResv8BitPerColorPixelWriter{frame_buffer_config};
      break;
    case kPixelBGRResv8BitPerColor:
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
  while (true) {
    __asm__("hlt");  // NOLINT(hicpp-no-assembler)
  }
}
