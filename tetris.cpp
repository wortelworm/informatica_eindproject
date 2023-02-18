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
  uint8_t bag[7];
  uint8_t bagPointer;
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

    while (! digitalRead(BUTTON_MENU)) {
      createPiece();
      delay(3000);
    }
  }

  void displayPiece(Piece piece) {
    // TODO?
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

    // show the preview piece
    for (int8_t i = 0; i < 8; i++) {
      tetrisBlock(12 + (i % 4),  i >> 2,      ((previewPiece.upperShape & (1 << (7 - i))) == 0) ? BLACK : previewPiece.color);
      tetrisBlock(12 + (i % 4), (i >> 2) + 2, ((previewPiece.lowerShape & (1 << (7 - i))) == 0) ? BLACK : previewPiece.color);
    }

    // TODO: show piece






  }
}