#pragma once
#include <Arduino.h>

namespace LedMatrix {
  void Init();
  void DrawPixel(int8_t x, int8_t y, uint8_t color);
}
