/**
 * this is code to simulate the game of atari breakout
 * see example: https://elgoog.im/breakout/
 * 
 * A special direction system is being used,
 * the direction is stored as a number from -3 to 3
 * up/down is stored in an additional boolean
 * 
 * a movement parity flag is used to allow for movement
 * to move horizontal then move diagonal
 * or to move vertical then move diagonal
 * 
 * so a total of 14 different directions are possible
 */

#include "breakout.h"
#include "utils.h"


namespace Breakout {
  /// top left of 4x4
  int8_t ballX, ballY;
  /// -3: -1/2
  /// -2: -1
  /// 1: -2
  /// 0: infinite
  /// 1: 2
  /// 2: 1
  /// 3: 1/2
  int8_t direction;
  bool going_up;
  bool moveParity;

  uint8_t paddleX;
  uint8_t lives;

  uint64_t field;
  uint16_t score;
  bool fieldCleared;

  void drawBlock(uint8_t x, uint8_t y, uint8_t color) {
    Utils::FillRect(x*6+2, 16 + y*4, 5, 3, color);
  }

  void redrawBall(int16_t newX, int16_t newY) {
    Utils::FillRect(ballX + 1, ballY + 10, 2, 2, BLACK);
    ballX = newX;
    ballY = newY;
    Utils::FillRect(ballX + 1, ballY + 10, 2, 2, YELLOW);
  }

  void drawHeart(uint8_t index, bool alive) {
    /*
      . # # . # # .
      # # # # # # #
      # # # # # # #
      # # # # # # #
      . # # # # # .
      . . # # # . .
      . . . # . . .
    */

    uint8_t color = alive ? RED : BLACK;
    uint8_t x = 1 + index * 8;
    Utils::DrawLineVertical(x+0, 2, 3, color);
    Utils::DrawLineVertical(x+1, 1, 5, color);
    Utils::DrawLineVertical(x+2, 1, 6, color);
    Utils::DrawLineVertical(x+3, 2, 6, color);
    Utils::DrawLineVertical(x+4, 1, 6, color);
    Utils::DrawLineVertical(x+5, 1, 5, color);
    Utils::DrawLineVertical(x+6, 2, 3, color);
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

    // update score
    score++;
    if (score > 999) {
      score = 999;
    }
    char buffer[3] = {};
    itoa(score, buffer, 10);
    Utils::FillRect(43, 1, 17, 7, BLACK);
    Utils::DrawText(score > 99 ? 43 : (score > 9 ? 49 : 55), 1, BLUE, buffer);

    if (score > 0 && score % 50 == 0) {
      fieldCleared = true;
    }
    
    return true;
  }

  bool moveBall() {
    if (direction < -3) {
      direction = -3;
    } else if (direction > 3) {
      direction = 3;
    }
    int8_t dx;
    int8_t dy;
    bool diagonal = false;
    if (moveParity && direction != 0) {
      // diagonal move
      dy = 1;
      dx = (direction > 0) ? 1 : -1;
      diagonal = true;
    } else {
      switch (direction) {
        case -3:
          dx = -1;
          dy = 0;
          break;
        case -2:
          dx = -1;
          dy = 1;
          diagonal = true;
          break;
        case -1:
          dx = 0;
          dy = 1;
          break;
        case 0:
          dx = 0;
          dy = 1;
          break;
        case 1:
          dx = 0;
          dy = 1;
          break;
        case 2:
          dx = 1;
          dy = 1;
          diagonal = true;
          break;
        case 3:
          dx = 1;
          dy = 0;
          break;
      }
    }

    if (going_up) {
      dy = -dy;
    }
    int8_t newX = ballX+dx;
    int8_t newY = ballY+dy;

    // collision detection
    // walls
    if (going_up) {
      if (newY < 0) {
        newY = 1;
        going_up = false;
      }
    } else {
      if (newY >= 49 && newX >= paddleX-1 && newX <= paddleX + 9) {
        newY = 49;
        going_up = true;

        direction = direction + (newX - (paddleX + 3));
      }
    }

    if (newX < 0) {
      newX = 1;
      direction = -direction;
    } else if (newX > 59) {
      newX = 59;
      direction = -direction;
    }

    // collision detection on field
    if (newY > 4 && newY < 25) {
      if (dy < 0) {
        // check above
        if ((newY) % 4 == 0) {
          // get the coords of this block
          uint8_t x = (newX) / 6;
          uint8_t y = (newY) / 4 - 2;

          // does it still exist?
          if (breakBlock(x, y)) {
            // yes, its broken now, bounce off
            going_up = false;
            newY += 2;
          }
        }
      } else if (dy > 0) {
        // check below
        if (newY % 4 == 1) {
          // get the coords of this block
          uint8_t x = (newX) / 6;
          uint8_t y = (newY) / 4 - 1;

          // does it still exist?
          if (breakBlock(x, y)) {
            // yes, its broken now, bounce off
            going_up = true;
            newY -= 2;
          }
        }
      }

      // check for blocks to the side
      if (dx < 0) {
        // check to the left
        if (newX%6 == 5) {
          // get the coords of this block
          uint8_t x = (newX) / 6;
          uint8_t y = (newY - 5) / 4;

          // does it still exist?
          if (breakBlock(x, y)) {
            // yes, its broken now, bounce off
            direction = -direction;
            newX += 2;
          }
        }
      } else
      if (dx > 0) {
        // check to the right
        if (newX%6 == 0) {
          // get the coords of this block
          uint8_t x = (newX) / 6;
          uint8_t y = (newY - 5) / 4;

          // does it still exist?
          if (breakBlock(x, y)) {
            // yes, its broken now, bounce off
            direction = -direction;
            newX -= 2;
          }
        }
      }
    }

    redrawBall(newX, newY);
    Utils::WriteBuffer();

    moveParity = ! moveParity;
    return diagonal;
  }

  void resetField() {
    
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

    field = ((uint64_t) 1 << 51) - 1;
  }

  void Play() {
    Utils::FillRect(0, 0, 64, 64, BLACK);
    for (uint8_t i = 0; i < 3; i++) {
      drawHeart(i, true);
    }
    
    // borders
    Utils::DrawLineHorizontal(0, 9, 63, GREY);
    Utils::DrawLineVertical(0, 10, 54, GREY);
    Utils::DrawLineVertical(62, 10, 54, GREY);

    for (uint8_t i = 0; i < 9; i += 2) {
      Utils::DrawPixel(25, i, GREY);
    }

    lives = 3;
    direction = 0;
    score = 0;

    resetField();

    // main game loop
    while (lives > 0) {
      
      // remove any previous paddle or ball on the screen
      Utils::FillRect(1, 61, 61, 3, BLACK);

      // show paddle
      Utils::DrawLineHorizontal(31-5, 61, 5 * 2, WHITE);

      // show first frame of the ball
      ballX = 29;
      ballY = 49;
      redrawBall(29, 49);
      
      do {
        direction = random(-2, 2 + 1);
      } while (direction == 0);

      going_up = true;
      moveParity = true;
      paddleX = 25;
      fieldCleared = false;

      Utils::WriteBuffer();

      delay(2000);

      while (ballY < 53) {
        bool diagonalMove = moveBall();

        if (fieldCleared) {
          // remove the ball drawing
          Utils::FillRect(ballX + 1, ballY + 10, 2, 2, BLACK);
          resetField();
          break;
        }

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

          if (digitalRead(BUTTON_MENU)) {
            // pause
            Utils::FillRect(13, 39, 37, 9, BLACK);
            Utils::DrawLineHorizontal(13, 38, 37, WHITE);
            Utils::DrawLineHorizontal(13, 48, 37, WHITE);
            Utils::DrawLineVertical(12, 39, 9, WHITE);
            Utils::DrawLineVertical(50, 39, 9, WHITE);
            Utils::DrawText(14, 40, RED, "Paused");
            Utils::WriteBuffer();

            // wait for button unpress and press
            while (digitalRead(BUTTON_MENU)) {
              delay(10);
            }
            while (! (digitalRead(BUTTON_MENU) || digitalRead(BUTTON_START))) {
              delay(10);
            }

            if (digitalRead(BUTTON_MENU)) {
              // stop the game
              return;
            }

            // resume the game
            Utils::FillRect(12, 38, 39, 11, BLACK);
            redrawBall(ballX, ballY);
            Utils::WriteBuffer();
          }

          // diagonal moves move further, so add more delay
          if (diagonalMove) {
            // multiplied by sqrt(2)
            delay(17);
            // delay(42);
          } else {
            delay(12);
            // delay(30);
          }
        }
      }

      if (! fieldCleared) {
        // remove a heart
        lives--;
        drawHeart(lives, false);
        delay(2000); 
      }
    }

    Utils::FillRect(5, 40, 53, 7, BLACK);
    Utils::DrawText(5, 40, RED, "Game Over");

    Utils::WriteBuffer();

    // wait till button press before returning
    while (! digitalRead(BUTTON_MENU)) {
      delay(10);
    }
  }
}
