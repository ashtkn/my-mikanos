#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <string_view>

#include "boost/core/span.hpp"  // boost::span
#include "console.h"
#include "font.h"
#include "graphics.h"
#include "pci.h"

// NOLINTNEXTLINE
void operator delete(void* obj) noexcept {}

constexpr PixelColor kDesktopBgColor{std::byte{45}, std::byte{118},
                                     std::byte{237}};
// constexpr PixelColor kDesktopFgColor{ std::byte{255}, std::byte{255},
// std::byte{255} };

constexpr int kMouseCursorWidth = 15;
constexpr int kMouseCursorHeight = 24;

// clang-format off
constexpr std::array<std::string_view, kMouseCursorHeight> mouse_cursor_shape {
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

std::array<std::byte, sizeof(RGBResv8BitPerColorPixelWriter)> pixel_writer_buf;
PixelWriter* pixel_writer;

std::array<std::byte, sizeof(Console)> console_buf;
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
      pixel_writer =
          new (static_cast<void*>(boost::span(pixel_writer_buf).data()))
              RGBResv8BitPerColorPixelWriter{frame_buffer_config};
      break;
    case kPixelBGRResv8BitPerColor:
      pixel_writer =
          new (static_cast<void*>(boost::span(pixel_writer_buf).data()))
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
                    {std::byte{1}, std::byte{8}, std::byte{17}});
  DrawFillRectangle(*pixel_writer, {0, kFrameHeight - 50},
                    {kFrameWidth / 5, 50},
                    {std::byte{80}, std::byte{80}, std::byte{80}});
  DrawStrokeRectangle(*pixel_writer, {10, kFrameHeight - 40}, {30, 30},
                      {std::byte{160}, std::byte{160}, std::byte{160}});

  /**
   * Show console
   */

  // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
  console = new (static_cast<void*>(boost::span(console_buf).data()))
      Console{*pixel_writer,
              {std::byte{0}, std::byte{0}, std::byte{0}},
              {std::byte{255}, std::byte{255}, std::byte{255}}};

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg,hicpp-vararg)
  printk("Welcome to MikanOS!\n");

  /**
   * Draw mouse cursor
   */

  for (int dy = 0; dy < kMouseCursorHeight; ++dy) {
    for (int dx = 0; dx < kMouseCursorWidth; ++dx) {
      if (mouse_cursor_shape.at(dy).at(dx) == '@') {
        pixel_writer->Write(200 + dx, 100 + dy,
                            {std::byte{0}, std::byte{0}, std::byte{0}});
      } else if (mouse_cursor_shape.at(dy).at(dx) == '.') {
        pixel_writer->Write(200 + dx, 100 + dy,
                            {std::byte{255}, std::byte{255}, std::byte{255}});
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
    const auto& dev = pci::devices.at(i);
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
