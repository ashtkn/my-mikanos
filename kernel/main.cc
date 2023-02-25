#include <cstddef>
#include <cstdint>
#include <cstdio>

#include "console.h"
#include "font.h"
#include "graphics.h"

// NOLINTNEXTLINE(fuchsia-overloaded-operator,misc-unused-parameters)
void* operator new(size_t size, void* buf) { return buf; }

// NOLINTNEXTLINE(fuchsia-overloaded-operator,misc-unused-parameters,misc-new-delete-overloads,cert-dcl54-cpp,hicpp-new-delete-operators)
void operator delete(void* obj) noexcept {}

char pixel_writer_buf[sizeof(RGBResv8BitPerColorPixelWriter)];
PixelWriter* pixel_writer;

char console_buf[sizeof(Console)];
Console* console;

// NOLINTNEXTLINE(cert-dcl50-cpp)
int printk(const char* format, ...) {
  va_list ap;
  int result;
  char s[1024];

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg,hicpp-vararg,cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)
  va_start(ap, format);
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)
  result = vsprintf(static_cast<char*>(s), format, ap);
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)
  va_end(ap);

  console->PutString(static_cast<const char*>(s));
  return result;
}

extern "C" void KernelMain(const FrameBufferConfig& frame_buffer_config) {
  switch (frame_buffer_config.pixel_format) {
    case kPixelRGBResv8BitPerColor:
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      pixel_writer = new (reinterpret_cast<PixelWriter*>(pixel_writer_buf))
          RGBResv8BitPerColorPixelWriter{frame_buffer_config};
      break;
    case kPixelBGRResv8BitPerColor:
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      pixel_writer = new (reinterpret_cast<PixelWriter*>(pixel_writer_buf))
          BGRResv8BitPerColorPixelWriter{frame_buffer_config};
      break;
  }

  for (int x = 0; x < frame_buffer_config.horizontal_resolution; ++x) {
    for (int y = 0; y < frame_buffer_config.vertical_resolution; ++y) {
      pixel_writer->Write(x, y, {255, 255, 255});
    }
  }

  /**
   * Show console
   */

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast,cppcoreguidelines-owning-memory)
  console = new (reinterpret_cast<Console*>(console_buf))
      Console{*pixel_writer, {0, 0, 0}, {255, 255, 255}};

  for (int i = 0; i < 27; ++i) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg,hicpp-vararg)
    printk("printk; %d\n", i);
  }

  /**
   * Halt
   */

  while (true) {
    __asm__("hlt");  // NOLINT(hicpp-no-assembler)
  }
}
