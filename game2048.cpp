/**
 * this is code to simulate the game of 2048
 * for the used rules, see: https://en.wikipedia.org/wiki/2048_(video_game)
 */


#include "game2048.h"
#include "utils.h"

namespace Game2048 {
  uint8_t field[16];
  bool gameOver;
  uint32_t score;
  bool win;
  bool stoppedByButton;

  void drawTile(uint8_t xField, uint8_t yField) {
    uint8_t x = xField*16, y = yField*16;
    Utils::FillRect(x + 1, y + 1, 14, 14, BLACK);
    switch (field[yField * 4 + xField]) {
      case 0:
        break;
      case 1:
        Utils::DrawText(x + 5, y + 5, RED, "2");
        break;
      case 2:
        Utils::DrawText(x + 5, y + 5, RED, "4");
        break;
      case 3:
        Utils::DrawText(x + 5, y + 5, RED, "8");
        break;

      case 4:
        Utils::DrawText(x + 2, y + 5, RED, "16");
        break;
      case 5:
        Utils::DrawText(x + 2, y + 5, RED, "32");
        break;
      case 6:
        Utils::DrawText(x + 2, y + 5, RED, "64");
        break;

      case 7:
        // From here on out the default font wont fit
        // so a smaller, custom one is used
        // this thing has an offset of (2, 5)
        // . # . # # # # . # # # #
        // # # . . . . # . # . . #
        // . # . . . . # . # . . #
        // . # . # # # # . # # # #
        // . # . # . . . . # . . #
        // . # . # . . . . # . . #
        // . # . # # # # . # # # #

        // 1
        Utils::DrawPixel(x+2, y+6, BLUE);
        Utils::DrawLineVertical(x+3, y+5, 7, BLUE);

        // 2
        Utils::DrawLineHorizontal(x+5, y+5, 4, BLUE);
        Utils::DrawLineHorizontal(x+5, y+8, 4, BLUE);
        Utils::DrawLineHorizontal(x+5, y+11, 4, BLUE);
        Utils::DrawLineVertical(x+5, y+9, 2, BLUE);
        Utils::DrawLineVertical(x+8, y+6, 2, BLUE);

        // 8
        Utils::DrawLineVertical(x+10, y+5, 7, BLUE);
        Utils::DrawLineVertical(x+13, y+5, 7, BLUE);
        Utils::DrawLineHorizontal(x+11, y+5, 2, BLUE);
        Utils::DrawLineHorizontal(x+11, y+8, 2, BLUE);
        Utils::DrawLineHorizontal(x+11, y+11, 2, BLUE);

        // Utils::DrawText(x*16 + 5, y * 16 + 1, RED,  "1");
        // Utils::DrawText(x*16 + 2, y * 16 + 8, RED, "28");
        break;
      case 8:
        // this thing has an offset of (3, 5)
        // # # # . # # # . # # #
        // . . # . # . . . # . .
        // . . # . # . . . # . .
        // # # # . # # # . # # #
        // # . . . . . # . # . #
        // # . . . . . # . # . #
        // # # # . # # # . # # #

        // 2
        Utils::DrawLineHorizontal(x+3, y+5, 3, BLUE);
        Utils::DrawLineHorizontal(x+3, y+8, 3, BLUE);
        Utils::DrawLineHorizontal(x+3, y+11, 3, BLUE);
        Utils::DrawLineVertical(x+3, y+9, 2, BLUE);
        Utils::DrawLineVertical(x+5, y+6, 2, BLUE);

        // 5
        Utils::DrawLineHorizontal(x+7, y+5, 3, BLUE);
        Utils::DrawLineHorizontal(x+7, y+8, 3, BLUE);
        Utils::DrawLineHorizontal(x+7, y+11, 3, BLUE);
        Utils::DrawLineVertical(x+7, y+6, 2, BLUE);
        Utils::DrawLineVertical(x+9, y+9, 2, BLUE);

        // 6
        Utils::DrawLineHorizontal(x+12, y+5, 2, BLUE);
        Utils::DrawLineHorizontal(x+12, y+8, 2, BLUE);
        Utils::DrawLineHorizontal(x+12, y+11, 2, BLUE);
        Utils::DrawLineVertical(x+11, y+5, 7, BLUE);
        Utils::DrawLineVertical(x+13, y+9, 2, BLUE);

        // Utils::DrawText(x + 5, y + 1, RED,  "2");
        // Utils::DrawText(x + 2, y + 8, RED, "56");
        break;
      case 9:
        // this thing has an offset of (3, 5)
        // # # # . . # . . # # #
        // # . . . # # . . . . #
        // # . . . . # . . . . #
        // # # # . . # . . # # #
        // . . # . . # . . # . .
        // . . # . . # . . # . .
        // # # # . # # # . # # #

        // 5
        Utils::DrawLineHorizontal(x+3, y+5, 3, BLUE);
        Utils::DrawLineHorizontal(x+3, y+8, 3, BLUE);
        Utils::DrawLineHorizontal(x+3, y+11, 3, BLUE);
        Utils::DrawLineVertical(x+3, y+6, 2, BLUE);
        Utils::DrawLineVertical(x+5, y+9, 2, BLUE);

        // 1
        Utils::DrawPixel(x+7, y+6, BLUE);
        Utils::DrawLineVertical(x+8, y+5, 6, BLUE);
        Utils::DrawLineHorizontal(x+7, y+11, 3, BLUE);

        // 2
        Utils::DrawLineHorizontal(x+11, y+5, 3, BLUE);
        Utils::DrawLineHorizontal(x+11, y+8, 3, BLUE);
        Utils::DrawLineHorizontal(x+11, y+11, 3, BLUE);
        Utils::DrawLineVertical(x+11, y+9, 2, BLUE);
        Utils::DrawLineVertical(x+13, y+6, 2, BLUE);

        // Utils::DrawText(x + 5, y + 1, RED,  "5");
        // Utils::DrawText(x + 2, y + 8, RED, "12");
        break;

      case 10:
        // this thing has an offset of (1, 5)
        // . # . # # # . # # # . # . #
        // # # . # . # . . . # . # . #
        // . # . # . # . . . # . # . #
        // . # . # . # . # # # . # # #
        // . # . # . # . # . . . . . #
        // . # . # . # . # . . . . . #
        // . # . # # # . # # # . . . #

        // 1
        Utils::DrawPixel(x+1, y+6, BLUE);
        Utils::DrawLineVertical(x+2, y+5, 7, BLUE);

        // 0
        Utils::DrawLineVertical(x+4, y+5, 7, BLUE);
        Utils::DrawLineVertical(x+6, y+5, 7, BLUE);
        Utils::DrawPixel(x+5, y+5, BLUE);
        Utils::DrawPixel(x+5, y+11, BLUE);

        // 2
        Utils::DrawLineHorizontal(x+8, y+5, 3, BLUE);
        Utils::DrawLineHorizontal(x+8, y+8, 3, BLUE);
        Utils::DrawLineHorizontal(x+8, y+11, 3, BLUE);
        Utils::DrawLineVertical(x+8, y+9, 2, BLUE);
        Utils::DrawLineVertical(x+10, y+6, 2, BLUE);

        // 4
        Utils::DrawLineVertical(x+12, y+5, 4, BLUE);
        Utils::DrawLineVertical(x+14, y+5, 7, BLUE);
        Utils::DrawPixel(x+13, y+8, BLUE);


        // Utils::DrawText(x + 2, y + 1, RED, "10");
        // Utils::DrawText(x + 2, y + 8, RED, "24");
        break;
      case 11:
        // this thing has an offset of (3, 2)
        // # # # # . . # # # #
        // . . . # . . # . . #
        // # # # # . . # . . #
        // # . . . . . # . . #
        // # # # # . . # # # #
        // . . . . . . . . . .
        // . . . . . . . . . .
        // # . . # . . # # # #
        // # . . # . . # . . #
        // # # # # . . # # # #
        // . . . # . . # . . #
        // . . . # . . # # # #

        // 2
        Utils::DrawLineHorizontal(x+3, y+2, 4, YELLOW);
        Utils::DrawLineHorizontal(x+3, y+4, 4, YELLOW);
        Utils::DrawLineHorizontal(x+3, y+6, 4, YELLOW);
        Utils::DrawPixel(x+6, y+3, YELLOW);
        Utils::DrawPixel(x+3, y+5, YELLOW);

        // 0
        Utils::DrawLineHorizontal(x+9, y+2, 4, YELLOW);
        Utils::DrawLineHorizontal(x+9, y+6, 4, YELLOW);
        Utils::DrawLineVertical(x+9, y+3, 3, YELLOW);
        Utils::DrawLineVertical(x+12, y+3, 3, YELLOW);

        // 4
        Utils::DrawLineVertical(x+3, y+9, 3, YELLOW);
        Utils::DrawLineVertical(x+6, y+9, 5, YELLOW);
        Utils::DrawLineHorizontal(x+4, y+11, 2, YELLOW);

        // 8
        Utils::DrawLineVertical(x+9, y+9, 5, YELLOW);
        Utils::DrawLineVertical(x+12, y+9, 5, YELLOW);
        Utils::DrawLineHorizontal(x+10, y+9, 2, YELLOW);
        Utils::DrawLineHorizontal(x+10, y+11, 2, YELLOW);
        Utils::DrawLineHorizontal(x+10, y+13, 2, YELLOW);
        

        // Utils::DrawText(x + 2, y + 1, RED, "20");
        // Utils::DrawText(x + 2, y + 8, RED, "48");
        gameOver = true;
        win = true;
        break;
      
      default:
        Utils::DrawText(x + 5, y + 5, RED, "?");
        break;
    }
  }

  /// helper function for spawnNextBlock to check if any move is possible
  bool positionIsNumber(int8_t x, int8_t y, uint8_t num) {
    if (x < 0 || x >= 4 || y < 0 || y >= 4) {
      // out of bounds
      return false;
    }

    return field[y*4 + x] == num;
  }


  void spawnNextBlock() {
    // 90%: 2 = 2**1
    // 10%: 4 = 2**2
    uint8_t number = (random(1, 10 +1) == 10) ? 2 : 1;
    // Cheat for testing the game
    // number = 6;
    
    bool good = false;
    uint8_t pos;
    while (! good) {
      pos = random(0, 16);
      good = (field[pos] == 0);
    }

    field[pos] = number;
    drawTile(pos%4, pos/4);

    // check for gameOver properly
    gameOver = true;
    for (int8_t y = 0; y < 4; y++) {
      for (int8_t x = 0; x < 4; x++) {
        uint8_t num = field[y*4 + x];
        if (num == 0) {
          // empty field, always possible move
          gameOver = false;
          break;
        }
        // check if surrounding tiles have the same value
        if (positionIsNumber(x-1, y, num) || positionIsNumber(x+1, y, num) || positionIsNumber(x, y-1, num) || positionIsNumber(x+1, y, num)) {
          gameOver = false;
          break;
        }
      }
      if (! gameOver) {
        break;
      }
    }
    
  }


  /**
   * this function is quite large but most of it
   * is copy pasta with a few tiny changes
   * 
   * it is quite annoying that different directions
   * require slightly different code
   */
  void waitDoMovement() {
    if (gameOver) {
      return;
    }

    bool anyChange = false;
    while (! anyChange) {
      // wait until new button press
      while (digitalRead(BUTTON_LEFT) || digitalRead(BUTTON_DOWN) || digitalRead(BUTTON_RIGHT) || digitalRead(BUTTON_UP)) {
        delay(10);
      }
      while (! (digitalRead(BUTTON_LEFT) || digitalRead(BUTTON_DOWN) || digitalRead(BUTTON_RIGHT) || digitalRead(BUTTON_UP) || digitalRead(BUTTON_MENU))) {
        delay(10);
      }

      if (digitalRead(BUTTON_MENU)) {
        // suicide :(
        gameOver = true;
        stoppedByButton = true;
        return;
      }

      // what button?
      if (digitalRead(BUTTON_LEFT)) {
        // high X goes to low X
        for (int8_t y = 0; y < 4; y++) {
          int8_t minX = 0;
          for (int8_t originalX = 1; originalX < 4; originalX++) {
            uint8_t value = field[y*4 + originalX];
            if (value == 0) {
              // no tile to be moved over
              continue;
            }

            // move this tile over?
            int8_t newX = originalX-1;
            bool merge = false;
            for (; newX >= minX; newX--) {
              if (field[y*4 + newX] == value) {
                merge = true;
                newX--;
                break;
              }

              if (field[y*4 + newX] != 0) {
                // found a 'roadblock'
                break;
              }
            }
            newX++;

            if (newX == originalX) {
              // nothing changed.
              continue;
            }

            anyChange = true;
            if (merge) {
              // this tile cannot merge again this move
              minX = newX + 1;
              score += 1<<(value+1);
              field[y*4 + newX] = value + 1;
            } else {
              field[y*4 + newX] = value;
            }

            field[y*4 + originalX] = 0;
            drawTile(newX, y);
            drawTile(originalX, y);
          }
        }
      } else if (digitalRead(BUTTON_DOWN)) {
        // low Y goes to high Y
        for (int8_t x = 0; x < 4; x++) {
          int8_t maxY = 3;
          for (int8_t originalY = 2; originalY >= 0; originalY--) {
            uint8_t value = field[originalY*4 + x];
            if (value == 0) {
              // no tile to be moved over
              continue;
            }

            // move this tile over?
            int8_t newY = originalY+1;
            bool merge = false;
            for (; newY <= maxY; newY++) {
              if (field[newY*4 + x] == value) {
                merge = true;
                newY++;
                break;
              }

              if (field[newY*4 + x] != 0) {
                // found a 'roadblock'
                break;
              }
            }
            newY--;

            if (newY == originalY) {
              // nothing changed.
              continue;
            }

            anyChange = true;
            if (merge) {
              // this tile cannot merge again this move
              maxY = newY - 1;
              score += 1<<(value+1);
              field[newY*4 + x] = value + 1;
            } else {
              field[newY*4 + x] = value;
            }

            field[originalY*4 + x] = 0;
            drawTile(x, newY);
            drawTile(x, originalY);
          }
        }
      } else if (digitalRead(BUTTON_RIGHT)) {
        // low X goes to high X
        for (int8_t y = 0; y < 4; y++) {
          int8_t maxX = 3;
          for (int8_t originalX = 2; originalX >= 0; originalX--) {
            uint8_t value = field[y*4 + originalX];
            if (value == 0) {
              // no tile to be moved over
              continue;
            }

            // move this tile over?
            int8_t newX = originalX+1;
            bool merge = false;
            for (; newX <= maxX; newX++) {
              if (field[y*4 + newX] == value) {
                merge = true;
                newX++;
                break;
              }

              if (field[y*4 + newX] != 0) {
                // found a 'roadblock'
                break;
              }
            }
            newX--;

            if (newX == originalX) {
              // nothing changed.
              continue;
            }

            anyChange = true;
            if (merge) {
              // this tile cannot merge again this move
              maxX = newX - 1;
              score += 1<<(value+1);
              field[y*4 + newX] = value + 1;
            } else {
              field[y*4 + newX] = value;
            }

            field[y*4 + originalX] = 0;
            drawTile(newX, y);
            drawTile(originalX, y);
          }

        }
      } else if (digitalRead(BUTTON_UP)) {
        // high Y goes to low Y
        for (int8_t x = 0; x < 4; x++) {
          int8_t minY = 0;
          for (int8_t originalY = 1; originalY < 4; originalY++) {
            uint8_t value = field[originalY*4 + x];
            if (value == 0) {
              // no tile to be moved over
              continue;
            }

            // move this tile over?
            int8_t newY = originalY-1;
            bool merge = false;
            for (; newY >= minY; newY--) {
              if (field[newY*4 + x] == value) {
                merge = true;
                newY--;
                break;
              }

              if (field[newY*4 + x] != 0) {
                // found a 'roadblock'
                break;
              }
            }
            newY++;

            if (newY == originalY) {
              // nothing changed.
              continue;
            }

            anyChange = true;
            if (merge) {
              // this tile cannot merge again this move
              minY = newY + 1;
              score += 1<<(value+1);
              field[newY*4 + x] = value + 1;
            } else {
              field[newY*4 + x] = value;
            }

            field[originalY*4 + x] = 0;
            drawTile(x, newY);
            drawTile(x, originalY);
          }
        }
      }
    }
  }


  void Play() {
    // draw grid around the pieces
    for (uint8_t i = 0; i < 64; i++) {
      for (uint8_t j = 0; j < 64; j++) {
        if (i%16==0||i%16==15 || j%16==0||j%16==15) {
          Utils::DrawPixel(j, i, GREY);
        } else {
          Utils::DrawPixel(j, i, BLACK);
        }
      }
    }

    memset(field, 0, 16);
    score = 0;
    gameOver = false;
    win = false;
    stoppedByButton = false;

    // for debug purposes only
    // for (int i = 0; i < 16; i++) {
    //   field[i] = i;
    //   drawTile(i % 4, i / 4);
    // }

    // delay(1000 * 60 * 5);

    while (! gameOver) {
      spawnNextBlock();
      waitDoMovement();
    }

    if (! stoppedByButton) {
      delay(2000);
    }

    // gameover or win screen
    for (int i = 0; i < 56; i++) {
      for (int j = 0; j < 26; j++) {
        Utils::DrawPixel(4 + i, 19 + j, BLACK);
      }
    }

    // horizontal lines
    for (int i = 0; i < 56; i++) {
      Utils::DrawPixel(4 + i, 18, WHITE);
      Utils::DrawPixel(4 + i, 45, WHITE);
    }
    // vertical lines
    for (int j = 0; j < 26; j++) {
      Utils::DrawPixel(3,  19 + j, WHITE);
      Utils::DrawPixel(60, 19 + j, WHITE);
    }

    if (win) {
      Utils::DrawText(6,  20, GREEN, "You");
      Utils::DrawText(31, 20, GREEN, "won!!");
      Utils::DrawText(15, 29, GREEN, "Score:");
    } else {
      Utils::DrawText(6,  20, RED, "GAME");
      Utils::DrawText(35, 20, RED, "OVER");
      Utils::DrawText(15, 29, RED, "Score:");
    }

    char buffer[7] = {};
    itoa(score, buffer, 10);
    uint8_t numberOfDigits = 7;
    for (; numberOfDigits > 0; numberOfDigits--) {
      if (buffer[numberOfDigits-1] != 0) {
        // numberOfDigits++;
        break;
      }
    }
    Utils::DrawText(12 + 3*(7 - numberOfDigits), 37, win ? GREEN : RED, buffer);

    // wait until start or menu button is unpressed and pressed
    while (digitalRead(BUTTON_START) || digitalRead(BUTTON_MENU)) {
      delay(10);
    }
    while (! digitalRead(BUTTON_START) && ! digitalRead(BUTTON_MENU)) {
      delay(10);
    }
  }
}
