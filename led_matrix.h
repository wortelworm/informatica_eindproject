#pragma once
#include <Arduino.h>

namespace LedMatrix {
  void Init();
  void DrawPixel(int8_t x, int8_t y, uint8_t color);
  void SwapPixels(int8_t x1, int8_t y1, int8_t x2, int8_t y2);
}
