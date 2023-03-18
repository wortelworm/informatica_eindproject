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

// for local use colors are defined like an enum
#define BLACK    0
#define WHITE    1
#define GREY     2

#define RED      3
#define GREEN    4 
#define BLUE     5

#define CYAN     6
#define MAGENTA  7
#define YELLOW   8

#define PURPLE   9
#define ORANGE  10


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

  static void PlayTone(uint16_t frequency, uint16_t duration) {
    Utils::output_file.open("tone_output.txt", std::ios::trunc | std::ios::out | std::ios::in);
    Utils::output_file << frequency;
    Utils::output_file.close();

    Utils::Delay(duration);

    Utils::output_file.open("tone_output.txt", std::ios::trunc | std::ios::out | std::ios::in);
    Utils::output_file << 0;
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
        int16_t index = (x+i) + (y+j)*64;
        if (index >= 0 && index < 4096) {
          buffer[index] = color;
        }
      }
    }
    Utils::writeBuffer();
    Utils::Delay(width * height / 20);
  }

  static void SwapPixels(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
    uint8_t temp = buffer[x1 + y1*64];
    buffer[x1 + y1*64] = buffer[x2 + y2*64];
    buffer[x2 + y2*64] = temp;
  }
}

// Definitions of arduino functions absent in native c++
#define delay(duration) Utils::Delay(duration)
#define random(low, high) (rand() % ((high) - (low)) + (low))
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
#include "game2048.cpp"
#include "simon.cpp"
#include "breakout.cpp"


int main() {
  setup();
  while (true) {
    loop();
  }
}


#endif
#endif