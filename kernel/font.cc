#include "font.h"

#include <array>
#include <optional>

#include "boost/core/bit.hpp"
#include "boost/core/span.hpp"

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

auto GetFontListSpan() {
  auto bin_start =
      boost::core::bit_cast<const uint8_t*>(&_binary_hankaku_bin_start);
  auto bin_end =
      boost::core::bit_cast<const uint8_t*>(&_binary_hankaku_bin_end);
  return boost::span(bin_start, bin_end);
}

auto GetFont(char c) -> std::optional<std::array<uint8_t, 16>> {
  auto index = 16 * static_cast<unsigned int>(c);
  auto font_list_span = GetFontListSpan();
  if (index >= font_list_span.size()) {
    return std::nullopt;
  }
  auto font_span = font_list_span.subspan(index, 16);
  std::array<uint8_t, 16> font{};
  std::copy(font_span.begin(), font_span.end(), font.begin());
  return font;
}

void WriteAscii(const PixelWriter& writer, int x, int y, char c,
                const PixelColor& color) {
  auto font = GetFont(c);
  if (!font) {
    return;
  }
  for (unsigned int dy = 0; dy < 16; ++dy) {
    for (unsigned int dx = 0; dx < 8; ++dx) {
      if ((static_cast<uint8_t>(font->at(dy) << dx) & 0x80u) != 0u) {
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
