#pragma once
/**
 * This is a replacement for utils.h for local testing
 * it will also replace a main.cpp
 * 
 */

// TODO: define digitalWrite for the buttons
// Definitions of arduino functions absend in native c++
#define delay Utils::Delay
#define random(low, high) (rand() % (high - low) + low)


#define BUTTON_LEFT  1
#define BUTTON_DOWN  2
#define BUTTON_RIGHT 3
#define BUTTON_UP    4
#define BUTTON_START 5
#define BUTTON_MENU  6

#define BLACK 0
#define WHITE Utils::Color333(7, 7, 7)
#define RED   Utils::Color333(7, 0, 0)
#define GREEN Utils::Color333(0, 7, 0)
#define BLUE  Utils::Color333(0, 0, 7)

#include <stdint.h>
#include "informatica_eindproject.ino"
#include "snake.cpp"
#include <fstream>
#include <synchapi.h>


bool digitalRead(int) {

  // TODO read file
  return false;
}


namespace Utils {
  std::ofstream myfile;

  static void Delay(unsigned int millis) {
    Sleep(millis);
  }

  static void Init() {

  }

  static uint16_t Color333(uint8_t r, uint8_t g, uint8_t b) {
    return (r & 0b0111 << 6) + (g & 0b0111 << 3) + (b & 0b0111);
  }

  static void DrawPixel(int8_t x, int8_t y, uint16_t color) {
    // write to file
    Utils::myfile.open("output.txt", std::ios_base::app);
    Utils::myfile << x << y << color << '\n';
    Utils::myfile.close();
  }

  static void FillRect(int8_t x, int8_t y, int8_t width, int8_t height, int16_t color) {
    for (int8_t i = 0; i < width; i++) {
      for (int8_t j = 0; j < height; j++) {
        DrawPixel(x + i, y + j, color);
      }
    }
  }
}

int main() {
  // setup();
  // while (true) {
  //   loop();
  // }

  Snake::reset();
}