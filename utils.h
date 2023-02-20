#pragma once
/**
 * This file contails utility functions for the games.
 * Mainly to draw things on the display.
 * This could also become a replacement for the display library
 * 
 */

// colors used both in local testing as in arduino
#define BLACK    0    // 000000
#define WHITE   63    // 111111
#define GREY    21    // 010101

#define RED     48    // 110000
#define GREEN   12    // 001100
#define BLUE     3    // 000011

#define CYAN    15    // 001111
#define MAGENTA 51    // 110011
#define YELLOW  60    // 111100

#define PURPLE  17    // 010001
#define ORANGE  52    // 110100

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

#include "glcdfont.c"

namespace Utils {
  static void DrawText(uint8_t cursor_x, uint8_t cursor_y, uint8_t color, const char str[]) {
    for (uint8_t i = 0; i < strlen(str); i++) {
      if (str[i] == '\n' || str[i] == '\r') {
        // skip em
      } else {
        // drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor,);
        if((cursor_x >= 64)	|| (cursor_y >= 64) ||(cursor_x < 0) || (cursor_y < 0)) {
          // clips, dont draw
          continue;
        }

        // draw line by line
        for (int8_t j=0; j<6; j++ ) {
          uint8_t line;
          if (j == 5) {
            line = 0x0;
          } else {
            line = pgm_read_byte(font+(str[i]*5)+j);
          }
          

          for (int8_t k = 0; k<8; k++) {
            if (line & 0x1) {
              Utils::DrawPixel(cursor_x+j, cursor_y+k, color);
            }
            // for now, no background color
            // else if (bg != color) {
            //   Utils::DrawPixel(cursor_x+j, cursor_y+k, bg);
            // }
            line >>= 1;
          }
        }


        cursor_x += 6;
      }
    }
  }
}