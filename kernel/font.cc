#include "font.h"

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

const uint8_t* GetFont(char c) {
  auto index = 16 * static_cast<unsigned int>(c);
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  if (index >= reinterpret_cast<uintptr_t>(&_binary_hankaku_bin_size)) {
    return nullptr;
  }
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  return &_binary_hankaku_bin_start + index;
}

void WriteAscii(const PixelWriter& writer, int x, int y, char c,
                const PixelColor& color) {
  auto font = GetFont(c);
  if (font == nullptr) {
    return;
  }
  for (unsigned int dy = 0; dy < 16; ++dy) {
    for (unsigned int dx = 0; dx < 8; ++dx) {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
      if ((static_cast<uint8_t>(font[dy] << dx) & 0x80u) != 0u) {
        writer.Write(x + dx, y + dy, color);
      }
    }
  }
}

void WriteString(const PixelWriter& writer, int x, int y, const char* s,
                 const PixelColor& color) {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  for (int i = 0; s[i] != '\0'; ++i) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    WriteAscii(writer, x + 8 * i, y, s[i], color);
  }
}
