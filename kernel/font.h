#pragma once

#include <string_view>

#include "graphics.h"

void WriteAscii(const PixelWriter& writer, int x, int y, char c,
                const PixelColor& color);

void WriteString(const PixelWriter& writer, int x, int y, std::string_view s,
                 const PixelColor& color);
