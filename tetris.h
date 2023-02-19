#pragma once
#include "utils.h"

namespace Tetris {
  void Play();

  enum RotationType {
    normal, special, none
  };
  struct Piece {
    uint8_t color;
    uint8_t upperShape;
    uint8_t lowerShape;
    RotationType rotation;
  };
  const Piece Pieces[] = {
    // I piece (sideways)
    { CYAN,    15,  0, RotationType::special},
    // J piece
    { BLUE,    142, 0, RotationType::normal },
    // L piece
    { ORANGE,  46,  0, RotationType::normal },
    // O piece
    { YELLOW,  102, 0, RotationType::none   },
    // S piece
    { GREEN,   108, 0, RotationType::normal },
    // Z piece
    { RED,     198, 0, RotationType::normal },
    // T piece
    { MAGENTA, 78,  0, RotationType::normal },
  };
}