#pragma once

namespace Tetris {
  void Play();
  void piecePreview(uint8_t index);

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