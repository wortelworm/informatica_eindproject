#pragma once

namespace Tetris {
  void Play();
  void createPiece();

  enum RotationType {
    normal, special, none
  };
  struct Piece {
    uint8_t color;
    uint8_t upperShape;
    uint8_t lowerShape;
    RotationType rotation;
  };
}