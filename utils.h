#pragma once
/**
 * This file contails utility functions for the games.
 * Mainly to draw things on the display.
 * This could also become a replacement for the display library
 * 
 */

#ifndef ARDUINO
// this is local testing
#include "local_server.cpp"
#else
// arduino

#include <Arduino.h>
#include "DFRobot_RGBMatrix.h"


#define BUTTON_LEFT  1
#define BUTTON_DOWN  2
#define BUTTON_RIGHT 3
#define BUTTON_UP    4
#define BUTTON_START 5
#define BUTTON_MENU  6

#define PIN_OUTPUT_ENABLED 9
#define PIN_LATCH 10
#define PIN_CLOCK 11
#define PIN_LA   	A0
#define PIN_LB   	A1
#define PIN_LC   	A2
#define PIN_LD   	A3
#define PIN_LE   	A4
#define WIDTH     64
#define _HIGH	    64

// TODO: define colors
#define BLACK 0
#define WHITE Utils::Color333(7, 7, 7)
#define RED   Utils::Color333(7, 0, 0)
#define GREEN Utils::Color333(0, 7, 0)
#define BLUE  Utils::Color333(0, 0, 7)

namespace Utils {
  static DFRobot_RGBMatrix matrix(PIN_LA, PIN_LB, PIN_LC, PIN_LD, PIN_LE, PIN_CLOCK, PIN_LATCH, PIN_OUTPUT_ENABLED, false, WIDTH, _HIGH);

  static void Init() {
    matrix.begin();
    pinMode(BUTTON_LEFT,  INPUT);
    pinMode(BUTTON_DOWN,  INPUT);
    pinMode(BUTTON_RIGHT, INPUT);
    pinMode(BUTTON_UP,    INPUT);
    pinMode(BUTTON_START, INPUT);
    pinMode(BUTTON_MENU,  INPUT);
  }

  static uint16_t Color222(uint8_t r, uint8_t g, uint8_t b) {
    return matrix.Color222(r, g, b);
  }

  static void DrawPixel(int8_t x, int8_t y, uint16_t color) {
    matrix.drawPixel(x, y, color);
  }

  static void FillRect(int8_t x, int8_t y, int8_t width, int8_t height, int16_t color) {
    for (int8_t i = 0; i < width; i++) {
      for (int8_t j = 0; j < height; j++) {
        matrix.drawPixel(x + i, y + j, color);
      }
    }
  }
}

#endif