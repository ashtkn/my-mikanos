#include "font.h"

#include <optional>

#include "boost/core/bit.hpp"   // boost::core::bit_cast
#include "boost/core/span.hpp"  // boost::span

// $ man objcopy
// -B bfdarch
// --binary-architecture=bfdarch
//     Useful when transforming a architecture-less input file into an object
//     file.  In this case the output architecture can be set to bfdarch.  This
//     option will be ignored if the input file has a known bfdarch. You can
//     access this binary data inside a program by referencing the special
//     symbols that are created by the conversion process.  These symbols are
//     called _binary_objfile_start, _binary_objfile_end and
//     _binary_objfile_size.  e.g. you can transform a picture file into an
//     object file and then access it in your code using these symbols.

extern const uint8_t _binary_hankaku_bin_start;
extern const uint8_t _binary_hankaku_bin_end;
extern const uint8_t _binary_hankaku_bin_size;

namespace {

auto GetFont(char c) -> std::optional<boost::span<const uint8_t>> {
  const auto bin_start =
      boost::core::bit_cast<const uint8_t*>(&_binary_hankaku_bin_start);
  const auto bin_end =
      boost::core::bit_cast<const uint8_t*>(&_binary_hankaku_bin_end);
  const auto font_list_span = boost::span(bin_start, bin_end);

  const auto index = 16 * static_cast<uint32_t>(c);
  if (index >= font_list_span.size()) {
    return std::nullopt;
  }
  return font_list_span.subspan(index, 16);
}

}  // unnamed namespace

void WriteAscii(const PixelWriter& writer, int x, int y, char c,
                const PixelColor& color) {
  auto font = GetFont(c);
  if (!font) {
    return;
  }
  for (uint32_t dy = 0; dy < 16; ++dy) {
    for (uint32_t dx = 0; dx < 8; ++dx) {
      if ((static_cast<uint8_t>(font.value()[dy] << dx) & 0x80u) != 0u) {
        writer.Write(x + dx, y + dy, color);
      }
    }
  }
}

void WriteString(const PixelWriter& writer, int x, int y, std::string_view s,
                 const PixelColor& color) {
  for (int i = 0; s.at(i) != '\0'; ++i) {
    WriteAscii(writer, x + 8 * i, y, s.at(i), color);
  }
}
