/**
 * this is code to simulate the game of tetris
 * for the used rules, see: https://tetris.wiki/Tetris_Guideline
 * 
 * TODOS:
 *    sound effects & music
 *    high score?
 */

#include "tetris.h"
#include "utils.h"

#define tetrisPixel(x, y, color) Utils::DrawPixel(x + 2, y + 2, color);
#define tetrisBlock(x, y, color) Utils::FillRect((x)*3 + 2, (y)*3 + 2, 3, 3, color);

const Tetris::Piece Pieces[] = {
  // I piece (sideways)
  { CYAN,    15,  0, Tetris::RotationType::special},
  // J piece
  { BLUE,    142, 0, Tetris::RotationType::normal },
  // L piece
  { ORANGE,  46,  0, Tetris::RotationType::normal },
  // O piece
  { YELLOW,  102, 0, Tetris::RotationType::none   },
  // S piece
  { GREEN,   108, 0, Tetris::RotationType::normal },
  // Z piece
  { RED,     198, 0, Tetris::RotationType::normal },
  // T piece
  { MAGENTA, 78,  0, Tetris::RotationType::normal },
};


namespace Tetris {
  uint8_t field[25];
  uint8_t bag[7];
  uint8_t bagPointer;
  uint8_t fallingX;
  uint8_t fallingY;
  Piece fallingPiece;
  Piece previewPiece;

  void Play() {
    // TODO
    Utils::FillRect(0, 0, 64, 64, BLACK);

    // draw outlines of boxes
    for (int8_t i = 0; i < 30; i++) {
      tetrisPixel(i, -1,  WHITE);
      tetrisPixel(i, 60, WHITE);
    }
    for (int8_t j = 0; j < 62; j++) {
      tetrisPixel(-1, j-1, WHITE);
      tetrisPixel(30, j-1, WHITE);
    }

    bagPointer = 7;
    fallingPiece = { 0, 0, 0, Tetris::RotationType::normal };
    memset(field, 0, 25);
    createPiece();

    while (! digitalRead(BUTTON_MENU)) {
      createPiece();
      while (! moveFallingPiece()) {
        delay(500);
      }
      delay(1000);
    }
  }

  void displayPiece(Piece piece) {
    // TODO?
  }

  bool moveFallingPiece() {
    // check for collision
    fallingY++;
    for (int8_t i = 0; i < 8; i++) {
      uint8_t upperpos = (fallingX + (i%4)) + 10 * (fallingY + (i/4));
      uint8_t lowerpos = upperpos + 20;

      if (fallingPiece.upperShape&(1<<(7-i)) != 0) {
        // block of the piece here
        if (fallingY+1 + (i/4) >= 20) {
          // collision with bottom of the screen
          return true;
        }
        if (field[upperpos%8]&(upperpos/8) != 0) {
          // collision with something on the field
          return true;
        }
      }

      if (fallingPiece.lowerShape&(1<<(7-i)) != 0) {
        // block of the piece here
        if (fallingY+1 + (i/4) + 2 >= 20) {
          // collision with bottom of the screen
          return true;
        }
        if (field[lowerpos%8]&(lowerpos/8) != 0) {
          // collision with something on the field
          return true;
        }
      }
    }
    
    // remove the current drawing
    for (int8_t i = 0; i < 8; i++) {
      if ((fallingPiece.upperShape & (1 << (7 - i))) != 0) {
        tetrisBlock(fallingX+(i%4), fallingY-1 +(i>>2),       BLACK);
      }
      if ((fallingPiece.lowerShape & (1 << (7 - i))) != 0) {
        tetrisBlock(fallingX+(i%4), fallingY-1 +(i>>2) + 2, BLACK);
      }
    }

    // add updated drawing
    for (int8_t i = 0; i < 8; i++) {
      if ((fallingPiece.upperShape&(1<<(7-i))) != 0) {
        tetrisBlock(fallingX+(i%4), fallingY+(i>>2),     fallingPiece.color);
      }
      if ((fallingPiece.lowerShape&(1<<(7-i))) != 0) {
        tetrisBlock(fallingX+(i%4), fallingY+(i>>2)+2, fallingPiece.color);
      }
    }

    return false;
  }

  void createPiece() {
    fallingPiece = previewPiece;

    // get the next preview piece
    bagPointer++;
    if (bagPointer >= 7) {
      // create a new bag
      // initialize sorted
      for (int8_t i = 0; i < 8; i++) {
        bag[i] = i;
      }

      // randomize using Fisher–Yates shuffle
      for (int8_t i = 0; i < 7-1; i++) {
        int8_t r = random(i, 7);
        uint8_t temp = bag[i];
        bag[i] = bag[r];  
        bag[r] = temp;  
      }
      bagPointer = 0;
    }
    previewPiece = Pieces[bag[bagPointer]];
    fallingX = 3;
    fallingY = 0;

    // show the preview piece
    for (int8_t i = 0; i < 8; i++) {
      tetrisBlock(12 + (i % 4), (i >> 2),      ((previewPiece.upperShape & (1 << (7 - i))) == 0) ? BLACK : previewPiece.color);
      tetrisBlock(12 + (i % 4), (i >> 2) + 2, ((previewPiece.lowerShape & (1 << (7 - i))) == 0) ? BLACK : previewPiece.color);
    }

    // show piece
    for (int8_t i = 0; i < 8; i++) {
      tetrisBlock(fallingX + (i % 4), fallingY + (i >> 2),      ((fallingPiece.upperShape & (1 << (7 - i))) == 0) ? BLACK : fallingPiece.color);
      tetrisBlock(fallingX + (i % 4), fallingY + (i >> 2) + 2, ((fallingPiece.lowerShape & (1 << (7 - i))) == 0) ? BLACK : fallingPiece.color);
    }
  }
}