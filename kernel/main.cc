#include <cstddef>
#include <cstdint>
#include <cstdio>

#include "console.h"
#include "font.h"
#include "graphics.h"
#include "pci.h"

// NOLINTNEXTLINE
void operator delete(void* obj) noexcept {}

const PixelColor kDesktopBgColor{45, 118, 237};
// const PixelColor kDesktopFgColor{255, 255, 255};

const int kMouseCursorWidth = 15;
const int kMouseCursorHeight = 24;

// clang-format off
const char mouse_cursor_shape[kMouseCursorHeight][kMouseCursorWidth + 1] = {
  "@              ",
  "@@             ",
  "@.@            ",
  "@..@           ",
  "@...@          ",
  "@....@         ",
  "@.....@        ",
  "@......@       ",
  "@.......@      ",
  "@........@     ",
  "@.........@    ",
  "@..........@   ",
  "@...........@  ",
  "@............@ ",
  "@......@@@@@@@@",
  "@......@       ",
  "@....@@.@      ",
  "@...@ @.@      ",
  "@..@   @.@     ",
  "@.@    @.@     ",
  "@@      @.@    ",
  "@       @.@    ",
  "         @.@   ",
  "         @@@   ",
};
// clang-format on

char pixel_writer_buf[sizeof(RGBResv8BitPerColorPixelWriter)];
PixelWriter* pixel_writer;

char console_buf[sizeof(Console)];
Console* console;

// NOLINTNEXTLINE(cert-dcl50-cpp)
int printk(const char* format, ...) {
  va_list ap;
  int result;
  char s[1024];

  // Following issues here
  // - cppcoreguidelines-pro-type-vararg
  // - cppcoreguidelines-pro-bounds-array-to-pointer-decay
  // - hicpp-vararg
  // - hicpp-no-array-decay
  // - clang-analyzer-valist.Uninitialized
  va_start(ap, format);                                  // NOLINT
  result = vsprintf(static_cast<char*>(s), format, ap);  // NOLINT
  va_end(ap);                                            // NOLINT

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

  const int kFrameWidth = frame_buffer_config.horizontal_resolution;
  const int kFrameHeight = frame_buffer_config.vertical_resolution;

  /**
   * Draw desktop
   */

  DrawFillRectangle(*pixel_writer, {0, 0}, {kFrameWidth, kFrameHeight - 50},
                    kDesktopBgColor);
  DrawFillRectangle(*pixel_writer, {0, kFrameHeight - 50}, {kFrameWidth, 50},
                    {1, 8, 17});
  DrawFillRectangle(*pixel_writer, {0, kFrameHeight - 50},
                    {kFrameWidth / 5, 50}, {80, 80, 80});
  DrawStrokeRectangle(*pixel_writer, {10, kFrameHeight - 40}, {30, 30},
                      {160, 160, 160});

  /**
   * Show console
   */

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast,cppcoreguidelines-owning-memory)
  console = new (reinterpret_cast<Console*>(console_buf))
      Console{*pixel_writer, {0, 0, 0}, {255, 255, 255}};

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg,hicpp-vararg)
  printk("Welcome to MikanOS!\n");

  /**
   * Draw mouse cursor
   */

  for (int dy = 0; dy < kMouseCursorHeight; ++dy) {
    for (int dx = 0; dx < kMouseCursorWidth; ++dx) {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
      if (mouse_cursor_shape[dy][dx] == '@') {
        pixel_writer->Write(200 + dx, 100 + dy, {0, 0, 0});
      }
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
      else if (mouse_cursor_shape[dy][dx] == '.') {
        pixel_writer->Write(200 + dx, 100 + dy, {255, 255, 255});
      }
    }
  }

  /**
   * Show devices
   */

  auto err = pci::ScanAllBus();
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg,hicpp-vararg)
  printk("ScanAllBus: %s\n", err.Name().data());

  for (int i = 0; i < pci::num_device; ++i) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    const auto& dev = pci::devices[i];
    auto vendor_id = pci::ReadVendorId(dev.bus, dev.device, dev.function);
    auto class_code = pci::ReadClassCode(dev.bus, dev.device, dev.function);
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg,hicpp-vararg)
    printk("%d.%d.%d: vend %04x, class %08x, head %02x\n", dev.bus, dev.device,
           dev.function, vendor_id, class_code, dev.header_type);
  }

  /**
   * Halt
   */

  while (true) {
    __asm__("hlt");  // NOLINT(hicpp-no-assembler)
  }
}
