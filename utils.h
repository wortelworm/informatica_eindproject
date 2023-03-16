#pragma once
/**
 * This file contails utility functions for the games.
 * Mainly to draw things on the display.
 * This could also become a replacement for the display library
 * 
 */

// colors used both in local testing as in arduino
// Format: GBRbgr, used to be RrGgBb
// first the upper bits for all colors, then the lower bits for all colors
#define BLACK    0    // 000000 -> 000000
#define WHITE   63    // 111111 -> 111111
#define GREY     7    // 010101 -> 000111

#define GREEN   36    // 110000 -> 100100
#define BLUE    18    // 001100 -> 010010
#define RED      9    // 000011 -> 001001

#define YELLOW  27    // 001111 -> 011011
#define MAGENTA 45    // 110011 -> 101101
#define CYAN    54    // 111100 -> 110110

#define PURPLE   3    // 010001 -> 000011
#define ORANGE  11    // 110100 -> 001011

// the original DFRobot_RGBMatrix library is used right now
//#define ORIGINAL_LIB true

#ifndef ARDUINO
// this is local testing
#include "local_server.cpp"

#else
// arduino
#include <Arduino.h>
#if ORIGINAL_LIB
#include "DFRobot_RGBMatrix.h"
#define PIN_OUTPUT_ENABLED 9
#define PIN_LATCH 10
#define PIN_CLOCK 11
#define PIN_LA    A0
#define PIN_LB    A1
#define PIN_LC    A2
#define PIN_LD    A3
#define PIN_LE    A4
#else
#include "led_matrix.h"
#endif


#define BUTTON_LEFT  42
#define BUTTON_DOWN  41
#define BUTTON_RIGHT 43
#define BUTTON_UP    40
#define BUTTON_START 38
#define BUTTON_MENU  39


namespace Utils {

#ifdef ORIGINAL_LIB
  static DFRobot_RGBMatrix LedMatrix(PIN_LA, PIN_LB, PIN_LC, PIN_LD, PIN_LE, PIN_CLOCK, PIN_LATCH, PIN_OUTPUT_ENABLED, false, 64, 64);

  static void Init() {
    LedMatrix.begin();
#else
  static void Init() {
    LedMatrix::Init();

#endif

    pinMode(BUTTON_LEFT,  INPUT);
    pinMode(BUTTON_DOWN,  INPUT);
    pinMode(BUTTON_RIGHT, INPUT);
    pinMode(BUTTON_UP,    INPUT);
    pinMode(BUTTON_START, INPUT);
    pinMode(BUTTON_MENU,  INPUT);
  }

  static void DrawPixel(int8_t x, int8_t y, uint8_t color) {
#ifdef ORIGINAL_LIB
	  // convert RGBrgb to format library uses
	  uint16_t encoded = LedMatrix.Color333((color >> 4) & 3, (color >> 2) & 3, color & 3);
	  LedMatrix.drawPixel(x, y, encoded);
#else
    LedMatrix::DrawPixel(x, y, color);
#endif
  }

  static void FillRect(int8_t x, int8_t y, int8_t width, int8_t height, int8_t color) {
    for (int8_t i = 0; i < width; i++) {
      for (int8_t j = 0; j < height; j++) {
	      Utils::DrawPixel(x + i, y + j, color);
      }
    }
  }

  static void SwapPixels(int8_t x1, int8_t y1, int8_t x2, int8_t y2) {
#ifdef ORIGINAL_LIB
	// The original library does not support swapping of pixels
	// so tetris will look very weird
#else
	LedMatrix::SwapPixels(x1, y1, x2, y2);
#endif
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
            // no background color is needed, it will be handeld by drawing a rect in the code of the game
            line >>= 1;
          }
        }


        cursor_x += 6;
      }
    }
  }
}
