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

    for (int8_t i = 0; i < 10; i++) {
      for (int8_t j = 0; j < 20; j++) {
        tetrisBlock(i, j, ((i+j) % 3 == 0) ? RED : GREEN);
      }
    }

    while (true) {
      for (int8_t i = 0; i < 7; i++) {
        Tetris::piecePreview(i);
        delay(2000);
      }
    }
  }

  void piecePreview(uint8_t index) {
    if (index > 7) {
      // ????
      return;
    }

    uint8_t upper = Pieces[index].upperShape;
    uint8_t lower = Pieces[index].lowerShape;
    uint8_t color = Pieces[index].color;

    for (uint8_t i = 0; i < 8; i++) {
      tetrisBlock(12 + (i % 4), i >> 2,       ((upper & 128) == 0) ? BLACK : color);
      tetrisBlock(12 + (i % 4), (i >> 2) + 2, ((lower & 128) == 0) ? BLACK : color);
      upper <<= 1;
      lower <<= 1;
    }
  }
}