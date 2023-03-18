/**
 * this is code to simulate the game of atari breakout
 * see example: https://elgoog.im/breakout/
 * 
 * TODO: REMOVE SUBPIXEL SYSTEM ITS QUITE BUGGY
 * A special subpixel and velocity direction system is being used
 */

#include "breakout.h"
#include "utils.h"

#include <iostream>


namespace Breakout {
  /// top left of 4x4 AND subpixeld
  int16_t ballX, ballY;
  /// -3: -1/2
  /// -2: -1
  /// 1: -2
  /// 0: infinite
  /// 1: 2
  /// 2: 1
  /// 3: 1/2
  int8_t direction;
  bool going_up;

  uint8_t paddleX;
  uint8_t lives;

  uint64_t field;

  void drawBlock(uint8_t x, uint8_t y, uint8_t color) {
    Utils::FillRect(x*6+2, 16 + y*4, 5, 3, color);
  }

  void redrawBall(int16_t newX, int16_t newY) {
    Utils::FillRect(ballX/2 + 1, ballY/2 + 10, 2, 2, BLACK);
    ballX = newX;
    ballY = newY;
    Utils::FillRect(ballX/2 + 1, ballY/2 + 10, 2, 2, YELLOW);
  }

  bool breakBlock(uint8_t x, uint8_t y) {
    if (x < 0 || x >= 10 || y < 0 || y >= 5) {
      return false;
    }

    uint64_t field_mask = (uint64_t) 1 << (x + y * 10);
    if ((field & field_mask) == 0) {
      // no block to break
      return false;
    }

    // remove block and draw removed block
    field &= ~field_mask;
    drawBlock(x, y, BLACK);
    return true;
  }

  void moveBall() {
    if (direction < -3) {
      direction = -3;
    } else if (direction > 3) {
      direction = 3;
    }
    int8_t dx;
    int8_t dy;
    switch (direction) {
      case -3:
        dx = -2;
        dy = 1;
        break;
      case -2:
        dx = -1;
        dy = 1;
        break;
      case -1:
        dx = -1;
        dy = 2;
        break;
      case 0:
        dx = 0;
        dy = 2;
        break;
      case 1:
        dx = 1;
        dy = 2;
        break;
      case 2:
        dx = 1;
        dy = 1;
        break;
      case 3:
        dx = 2;
        dy = 1;
        break;
    }

    if (going_up) {
      dy = -dy;
    }
    int16_t newX = ballX+dx;
    int16_t newY = ballY+dy;

    // collision detection
    // walls
    if (going_up) {
      if (newY < 0) {
        newY = 0;
        going_up = false;
      }
    } else {
      if (newY >= 98 && newX/2 >= paddleX-2 && newX/2 <= paddleX + 8) {
        newY = 98;
        going_up = true;

        direction = direction + (newX/2 - (paddleX + 3));
      }
    }

    if (newX < 0) {
      newX = 0;
      direction = -direction;
    } else if (newX >= 118) {
      newX = 118;
      direction = -direction;
    }

    // collision detection on field
    if (newY > 8 && newY < 50) {
      // check if above or below
      if (going_up) {
        // check for above
        if ((newY/2) % 4 == 0) {
          // get the coords of this block
          uint8_t x = (newX / 2) / 6;
          uint8_t y = (newY / 2) / 4 - 2;

          // does it still exist?
          if (breakBlock(x, y)) {
            // yes, its broken now, bounce off
            going_up = false;
            newY = ((newY & ~1) + 3);
          }
        }
      } else {
        // TODO
      }

      // check for blocks to the side
      if (direction < 0) {
        // check to the left
        if ((newX/2)%6 == 5) {
          // get the coords of this block
          uint8_t x = (newX / 2) / 6;
          uint8_t y = (newY / 2 + 4) / 4 - 2;

          // does it still exist?
          if (breakBlock(x, y)) {
            // yes, its broken now, bounce off
            direction = -direction;
            newX = ((newX & ~1) - 3);
          }
        }
      } else
      if (direction > 0) {
        // check to the right
        if ((newX/2)%6 == 0) {
          // get the coords of this block
          uint8_t x = (newX / 2) / 6;
          uint8_t y = (newY / 2 + 4) / 4 - 2;

          // does it still exist?
          if (breakBlock(x, y)) {
            // yes, its broken now, bounce off
            direction = -direction;
            newX = ((newX & ~1) + 3);
          }
        }
      }
    }

    redrawBall(newX, newY);
  }

  void Play() {
    Utils::FillRect(0, 0, 64, 64, BLACK);
    Utils::DrawText(10, 1, BLUE, "Lives:3");
    
    // borders
    Utils::DrawLineHorizontal(0, 9, 63, GREY);
    Utils::DrawLineVertical(0, 10, 54, GREY);
    Utils::DrawLineVertical(62, 10, 54, GREY);

    // show field
    for (uint8_t j = 0; j < 10; j++) {
      drawBlock(j, 0, RED);
    }
    for (uint8_t j = 0; j < 10; j++) {
      drawBlock(j, 1, ORANGE);
    }
    for (uint8_t j = 0; j < 10; j++) {
      drawBlock(j, 2, YELLOW);
    }
    for (uint8_t j = 0; j < 10; j++) {
      drawBlock(j, 3, GREEN);
    }
    for (uint8_t j = 0; j < 10; j++) {
      drawBlock(j, 4, BLUE);
    }

    lives = 3;
    direction = 0;
    field = ((uint64_t) 1 << 51) - 1;

    // main game loop
    while (lives > 0) {
      // remove any previous paddle or ball on the screen
      Utils::FillRect(1, 61, 61, 3, BLACK);

      // show paddle
      Utils::DrawLineHorizontal(31-5, 61, 5 * 2, WHITE);

      // show first frame of the ball
      ballX = 58;
      ballY = 98;
      redrawBall(58, 98);
      
      do {
        direction = random(-2, 2 + 1);
      } while (direction == 0);

      going_up = true;
      paddleX = 25;

      delay(2000);

      while (ballY < 106) {
        moveBall();

        // read inputs
        for (uint8_t i = 0; i < 3; i++) {
          if (digitalRead(BUTTON_LEFT)) {
            if (paddleX > 1) {
              paddleX--;
              Utils::DrawPixel(paddleX+1 , 61, WHITE);
              Utils::DrawPixel(paddleX+11, 61, BLACK);
            }
          } else if (digitalRead(BUTTON_RIGHT)) {
            if (paddleX < 51) {
              paddleX++;
              Utils::DrawPixel(paddleX+9, 61, WHITE);
              Utils::DrawPixel(paddleX-1, 61, BLACK);
            }
          }

          delay(12);
          // delay(30);
        }
      }
      lives--;
      Utils::FillRect(46, 1, 5, 7, BLACK);
      char buffer[1] = {};
      itoa(lives, buffer, 10);
      Utils::DrawText(46, 1, BLUE, buffer);

      delay(2000);
    }

    delay(1000*5);
  }
}