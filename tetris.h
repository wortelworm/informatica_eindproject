#pragma once

namespace Tetris {
  void Play();
  void createPiece();
  
  /**
   * moves the currently falling piece down, draws it
   * while checking for collisions
   * 
   * @returns true if the piece stops with falling, false otherwise
   */
  bool moveFallingPiece();

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