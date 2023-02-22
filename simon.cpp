/**
 * this is code to simulate the game of simon
 * see: https://en.wikipedia.org/wiki/Simon_(game)
 * 
 * the display is rotated 45 degrees to match the buttons of up down left and right
 */

#include "simon.h"
#include "utils.h"

namespace Simon {
  uint64_t sequence;

  // makes the light bright, plays tone and dims the light again
  // E (green, left, an octave lower than blue);
  // C♯ (yellow, lower);
  // E (blue, right);
  // A (red, upper).
  void showLight(uint8_t id) {
    uint8_t x;
    uint8_t y;
    uint8_t color;
    switch (id & 3) {
      case 0:
        x = 10;
        y = 22;
        color = GREEN;
        break;
      case 1:
        x = 22;
        y =  10;
        color = YELLOW;
        break;
      case 2:
        x = 34;
        y = 22;
        color = BLUE;
        break;
      case 3:
        x = 22;
        y = 34;
        color = RED;
        break;
    }

    // fill a rotated square lit up
    for (uint8_t i = 0; i < 10-1; i++) {
      for (uint8_t j = 0; j < 10; j++) {
        Utils::DrawPixel(x +     i + j, y - i + j, color);
      }
      for (uint8_t j = 0; j < 10-1; j++) {
        Utils::DrawPixel(x + 1 + i + j, y - i + j,  WHITE);
      }
    }
    for (uint8_t j = 0; j < 10; j++) {
      Utils::DrawPixel(x + 9 + j, y - 9 + j, color);
    }

    delay(500);

    // fill a rotated square dimmed
    for (uint8_t i = 0; i < 10-1; i++) {
      for (uint8_t j = 0; j < 10; j++) {
        // Utils::DrawPixel(x +     i + j, y - i + j, (((i+j)%2) == 0) ? color : BLACK);
        Utils::DrawPixel(x +     i + j, y - i + j, BLACK);
      }
      for (uint8_t j = 0; j < 10-1; j++) {
        // Utils::DrawPixel(x + 1 + i + j, y - i + j, (((i+j)%2) == 0) ? color : BLACK);
        Utils::DrawPixel(x + 1 + i + j, y - i + j, color);
      }
    }
    for (uint8_t j = 0; j < 10; j++) {
      // Utils::DrawPixel(x + 9 + j, y - 9 + j, (((9+j)%2) == 0) ? color : BLACK);
      Utils::DrawPixel(x + 9 + j, y - 9 + j, BLACK);
    }

    delay(200);

  }


  uint8_t readInput() {
    while(! (digitalRead(BUTTON_LEFT) || digitalRead(BUTTON_DOWN) || digitalRead(BUTTON_RIGHT) || digitalRead(BUTTON_UP))) {
      delay(10);
    }

    if (digitalRead(BUTTON_LEFT)) {
      return 0;
    }
    if (digitalRead(BUTTON_DOWN)) {
      return 1;
    }
    if (digitalRead(BUTTON_RIGHT)) {
      return 2;
    }
    return 3;
  }



  void Play() {
    Utils::FillRect(0, 0, 64, 64, BLACK);

    showLight(0);
    showLight(1);
    showLight(2);
    showLight(3);

    delay(1000);

    // create random sequence
    sequence = 0;
    for (uint8_t i = 0; i < 8; i++) {
      sequence += random(0, 1 << 8) << (8 * i);
    }

    // TESTING: show sequence
    while (true) {
      for (uint8_t i = 0; i < 32; i++) {
        uint8_t id = (sequence >> (2*i)) & 3;
        showLight(id);
      }
      delay(5 * 1000);
      return;
    }

    delay(5 * 60 * 1000);
  }
}
