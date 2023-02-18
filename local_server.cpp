/**
 * This is a replacement for utils.h for local testing
 * it will also replace a main.cpp
 * 
 */

#ifndef ARDUINO
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

uint8_t buffer[4096];


namespace Utils {
  std::ofstream output_file;

  static void Delay(unsigned int millis) {
    Sleep(millis);
  }

  static void Init() {
    memset(buffer, 0, 4096);
  }

  static void DrawPixel(int8_t x, int8_t y, uint8_t color) {
    if (x < 0 || x >= 64 || y < 0 || y >= 64) {
      return;
    }

    buffer[x + y*64] = color;
    // write to file
    Utils::output_file.open("output.txt", std::ios::binary | std::ios::out | std::ios::in);
    Utils::output_file.seekp((x + y * 64), std::ios::beg);
    Utils::output_file.put(color);
    Utils::output_file.close();
  }

  static void writeBuffer() {
    Utils::output_file.open("output.txt", std::ios::binary | std::ios::out | std::ios::in);
    Utils::output_file.write((char*) buffer, 4096);
    Utils::output_file.close();
  }

  static void FillRect(int8_t x, int8_t y, int8_t width, int8_t height, int16_t color) {
    for (int8_t i = 0; i < width; i++) {
      for (int8_t j = 0; j < height; j++) {
        buffer[(x+i) + (y+j)*64] = color;
      }
    }
    Utils::writeBuffer();
    Utils::Delay(width * height / 20);
  }
}

// TODO: define digitalWrite for the buttons
// Definitions of arduino functions absend in native c++
#define delay Utils::Delay
#define random(low, high) (rand() % (high - low) + low)
#define pgm_read_byte(x) *((uint8_t*) (x))


#define BUTTON_LEFT  1
#define BUTTON_DOWN  2
#define BUTTON_RIGHT 3
#define BUTTON_UP    4
#define BUTTON_START 5
#define BUTTON_MENU  6

#include "informatica_eindproject.ino"
#include "snake.cpp"
#include "tetris.cpp"



int main() {
  // setup();
  // while (true) {
  //   loop();
  // }

  Tetris::Play();
}


#endif
#endif