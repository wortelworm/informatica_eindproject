/**
 * this is code to simulate the game of atari breakout
 * see example: https://elgoog.im/breakout/
 * 
 * 
 */

#include "simon.h"
#include "utils.h"


namespace Breakout {
  void DrawBlock(uint8_t x, uint8_t y, uint8_t color) {
    Utils::FillRect(x*6+2, 16 + y*4, 5, 3, color);
  }

  void Play() {
    Utils::FillRect(0, 0, 64, 64, BLACK);
    Utils::DrawText(10, 1, BLUE, "Lives:1");
    
    // borders
    Utils::DrawLineHorizontal(0, 9, 63, GREY);
    Utils::DrawLineVertical(0, 10, 54, GREY);
    Utils::DrawLineVertical(62, 10, 54, GREY);

    // show field
    for (uint8_t i = 0; i < 5; i++) {
      for (uint8_t j = 0; j < 10; j++) {
        DrawBlock(j, i, RED);
      }
    }

    // show paddle
    Utils::DrawLineHorizontal(31-4, 61, 4 * 2 + 1, WHITE);

    delay(1000*60);
  }
}