#pragma once

#include "graphics.h"

void WriteAscii(const PixelWriter& writer, int x, int y, char c,
                const PixelColor& color);

void WriteString(const PixelWriter& writer, int x, int y, const char* s,
                 const PixelColor& color);
