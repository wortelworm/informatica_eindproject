/**
 * This is a replacement for utils.h for local testing
 * it will also replace a main.cpp
 * 
 */

#ifndef _LOCAL_SERVER_CPP
#define _LOCAL_SERVER_CPP


#include <stdint.h>
#include <fstream>
#include <synchapi.h>
#include <string>


std::ifstream input_file;
bool digitalRead(int pin) {
  input_file.open("input.txt", std::ios::binary | std::ios::in);
  input_file.seekg((pin - 1), std::ios::beg);
  char c;
  input_file >> c;
  input_file.close();
  return c == '1';
}


namespace Utils {
  std::ofstream output_file;

  static void Delay(unsigned int millis) {
    Sleep(millis);
  }

  static void Init() {

  }

  static uint8_t Color222(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 3) << 4) + ((g & 3) << 2) + (b & 3);
  }

  static void DrawPixel(int8_t x, int8_t y, uint8_t color) {
    if (x < 0 || x >= 64 || y < 0 || y >= 64) {
      return;
    }


    // write to file
    Utils::output_file.open("output.txt", std::ios::binary | std::ios::out | std::ios::in);
    Utils::output_file.seekp((x + y * 64), std::ios::beg);
    Utils::output_file.put(color);
    Utils::output_file.close();
  }

  static void FillRect(int8_t x, int8_t y, int8_t width, int8_t height, int16_t color) {
    for (int8_t i = 0; i < width; i++) {
      for (int8_t j = 0; j < height; j++) {
        DrawPixel(x + i, y + j, color);
      }
    }
  }
}

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
#define WHITE Utils::Color222(3, 3, 3)
#define RED   Utils::Color222(3, 0, 0)
#define GREEN Utils::Color222(0, 3, 0)
#define BLUE  Utils::Color222(0, 0, 3)

#include "informatica_eindproject.ino"
#include "snake.cpp"



int main() {
  // setup();
  // while (true) {
  //   loop();
  // }

  Snake::Play();
}


#endif