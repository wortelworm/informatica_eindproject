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

/// A = B from byte to bit level
#define setBitThingy(a, aMask, b, bMask) a ^= ((((a)&(aMask)) == 0) != (((b)&(bMask)) == 0) ) ? (aMask) : 0;


namespace Tetris {

  uint8_t field[25];
  uint8_t bag[7];
  uint8_t bagPointer;
  uint8_t fallingX;
  uint8_t fallingY;
  Piece fallingPiece;
  Piece previewPiece;
  bool leftWasPressed = false;
  bool rightWasPressed = false;
  bool rotationWasPressed = false;
  bool speedupFlag = false;

  // rotates the piece 90 degrees clockwise
  Piece rotatePiece(Piece piece) {
    uint8_t u = piece.upperShape;
    uint8_t l = piece.lowerShape;
    bool temp;
    switch (piece.rotation) {
      case RotationType::none:
        break;
      case RotationType::special:
        // this will only work for the I piece
        // switch lower & 4 and upper & 1
        if ((u&1) == 0) {
          u |=  1;
          l &= ~4;
        } else {
          u &= ~1;
          l |=  4;
        }
      case RotationType::normal:
        // rotate around 1, 1
        // first the side pieces
        /*
         * Edges:
         * Top:   upperShape & 64
         * Down:  lowerShape & 64
         * Left:  upperShape &  8
         * Right: upperShape &  2
         */

        // top
        temp = (u&64) == 0;
        
        // top = left
        setBitThingy(u,64, u,8 );
        // left = bottom
        setBitThingy(u,8,  l,64);
        // bottom = right
        setBitThingy(l,64, u,2 );
        // right = bottom
        setBitThingy(u,2,  temp ? 0 : 1,1);

        /* 
         * Corners:
         * Top  Left :  upperShape & 128
         * Top  Right:  upperShape &  32
         * Down Left :  lowerShape & 128
         * Down Right:  lowerShape &  32
         */

        // top left
        temp = (u&128) == 0;

        // top left   = down left
        setBitThingy(u,128, l,128);
        // down left  = down right
        setBitThingy(l,128, l, 32);
        // down right = top right
        setBitThingy(l, 32, u, 32);
        // top right  = top left
        setBitThingy(u, 32, temp ? 0 : 1,1);

        break;
    }
    piece.upperShape = u;
    piece.lowerShape = l;
    return piece;
  }


  void redrawFalling(int8_t newX, int8_t newY, Piece newPiece) {
    // remove the current drawing
    for (int8_t i = 0; i < 8; i++) {
      if ((fallingPiece.upperShape & (1 << (7 - i))) != 0) {
        tetrisBlock(fallingX+(i%4), fallingY +(i>>2),     BLACK);
      }
      if ((fallingPiece.lowerShape & (1 << (7 - i))) != 0) {
        tetrisBlock(fallingX+(i%4), fallingY +(i>>2) + 2, BLACK);
      }
    }

    fallingX = newX;
    fallingY = newY;
    fallingPiece = newPiece;

    // draw at updated position
    for (int8_t i = 0; i < 8; i++) {
      if ((fallingPiece.upperShape&(1<<(7-i))) != 0) {
        tetrisBlock(fallingX+(i%4), fallingY+(i>>2),   fallingPiece.color);
      }
      if ((fallingPiece.lowerShape&(1<<(7-i))) != 0) {
        tetrisBlock(fallingX+(i%4), fallingY+(i>>2)+2, fallingPiece.color);
      }
    }
  }
  void redrawFalling(int8_t newX, int8_t newY) {
    redrawFalling(newX, newY, fallingPiece);
  }


  bool wouldCollide(int8_t x, int8_t y, Piece piece) {
    // check for collision
    for (int8_t i = 0; i < 8; i++) {
      uint8_t upperpos = (x + (i%4)) + 10 * (y + (i/4));
      uint8_t lowerpos = upperpos + 20;

      if ((piece.upperShape&(1<<(7-i))) != 0) {
        // block of the piece here
        if (y+(i/4) >= 20 || x+(i%4) < 0 || x+(i%4) >= 10) {
          // collision with the border
          return true;
        }
        if ((field[upperpos/8]&(1<<(upperpos%8))) != 0) {
          // collision with something on the field
          return true;
        }
      }

      if ((piece.lowerShape&(1<<(7-i))) != 0) {
        // block of the piece here
        if (y+(i/4)+2 >= 20 || x+(i%4) < 0 || x+(i%4) >= 10) {
          // collision with the border
          return true;
        }
        if ((field[lowerpos/8]&(1<<(lowerpos%8))) != 0) {
          // collision with something on the field
          return true;
        }
      }
    }

    return false;
  }


  /**
   * returns true if piece created successfully
   * returns false if game over
   */
  bool createPiece() {
    fallingPiece = previewPiece;

    // get the next preview piece
    bagPointer++;
    if (bagPointer >= 7) {
      // create a new bag
      // initialize sorted
      for (int8_t i = 0; i < 7; i++) {
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
      tetrisBlock(12 + (i % 4), (i >> 2),     ((previewPiece.upperShape & (1 << (7 - i))) == 0) ? BLACK : previewPiece.color);
      tetrisBlock(12 + (i % 4), (i >> 2) + 2, ((previewPiece.lowerShape & (1 << (7 - i))) == 0) ? BLACK : previewPiece.color);
    }

    // show piece
    for (int8_t i = 0; i < 8; i++) {
      if ((fallingPiece.upperShape & (1 << (7 - i))) != 0) {
        tetrisBlock(fallingX+(i%4), fallingY+(i>>2),   fallingPiece.color);
      }
      if ((fallingPiece.lowerShape & (1 << (7 - i))) != 0) {
        tetrisBlock(fallingX+(i%4), fallingY+(i>>2)+2, fallingPiece.color);
      }
    }

    return !wouldCollide(fallingX, fallingY, fallingPiece);
  }


  /**
   * moves the currently falling piece down, draws it
   * while checking for collisions
   * 
   * @returns true if the piece stops with falling, false otherwise
   */
  bool moveFallingPiece() {
    // check for collision
    if (wouldCollide(fallingX, fallingY + 1, fallingPiece)) {
      return true;
    }
    
    redrawFalling(fallingX, fallingY + 1);

    return false;
  }


  void readInputs() {
    if (digitalRead(BUTTON_LEFT) && ! leftWasPressed) {
      if (! wouldCollide(fallingX-1, fallingY, fallingPiece)) {
        redrawFalling(fallingX-1, fallingY);
      }
    }

    if (digitalRead(BUTTON_RIGHT) && ! rightWasPressed) {
      if (! wouldCollide(fallingX+1, fallingY, fallingPiece)) {
        redrawFalling(fallingX+1, fallingY);
      }
    }

    if (digitalRead(BUTTON_UP) && ! rotationWasPressed) {
      Piece rotated = rotatePiece(fallingPiece);
      if (! wouldCollide(fallingX, fallingY, rotated)) {
        redrawFalling(fallingX, fallingY, rotated);
      }
    }

    leftWasPressed  = digitalRead(BUTTON_LEFT);
    rightWasPressed = digitalRead(BUTTON_RIGHT);
    rotationWasPressed = digitalRead(BUTTON_UP);

    speedupFlag = digitalRead(BUTTON_DOWN);
  }


  void endOfFalling() {
    // solidifies the currently falling piece
    for (int8_t i = 0; i < 8; i++) {
      uint8_t upperpos = (fallingX + (i%4)) + 10 * (fallingY + (i/4));
      uint8_t lowerpos = upperpos + 20;

      if ((fallingPiece.upperShape&(1<<(7-i))) != 0) {
        field[upperpos/8] |= 1 << (upperpos%8);
      }
      if ((fallingPiece.lowerShape&(1<<(7-i))) != 0) {
        field[lowerpos/8] |= 1 << (lowerpos%8);
      }
    }

    fallingPiece = { 0, 0, 0, RotationType::none };

    for (uint8_t i = 0; i < 4; i++) {
      if (fallingY + i >= 20) {
        break;
      }
      
      bool completed = true;
      for (uint8_t j = 0; j < 10; j++) {
        uint8_t position = (j) + 10 * (fallingY + i);
        if ((field[position/8]&(1<<(position%8))) == 0) {
          completed = false;
          break;
        }
      }

      if (completed) {
        // Woooo
        // flash it white
        for (uint8_t j = 0; j < 10; j++) {
          tetrisBlock(j, fallingY + i, WHITE);
        }
        delay(200);

        for (uint8_t j = 0; j < 10; j++) {
          tetrisBlock(j, fallingY + i, BLACK);
        }
        delay(50);


        // loop over all the rows
        for (int8_t j = fallingY+i; j > 0; j--) {

          // move pixels over
          for (int8_t k = 2; k >= 0; k--) {
            for (int8_t l = 0; l < 30; l++) {
              Utils::swapPixels(2+l, 2+j*3 + k, 2+l, 2 + (j-1)*3 + k);
            }
          }
          Utils::writeBuffer();

          // move over in memory
          for (uint8_t k = 0; k < 10; k++) {
            // move position2 to position1
            uint8_t position1 = k + 10*j;
            uint8_t position2 = k + 10*(j-1);

            if ((field[position2/8]&(1<<(position2%8))) == 0) {
              field[position1/8] &= ~(1 << (position1%8));
            } else {
              field[position1/8] |=  (1 << (position1%8));
            }

            field[position2/8] &= ~ (1 << (position2%8));
          }
        }
      }
    }
  }


  void Play() {
    Utils::FillRect(0, 0, 64, 64, BLACK);

    // draw outlines of boxes
    for (int8_t i = 0; i < 30; i++) {
      tetrisPixel(i, -1, WHITE);
      tetrisPixel(i, 60, WHITE);
    }
    for (int8_t j = 0; j < 62; j++) {
      tetrisPixel(-1, j-1, WHITE);
      tetrisPixel(30, j-1, WHITE);
    }

    bagPointer = 7;
    fallingPiece = { 0, 0, 0, RotationType::normal };
    memset(field, 0, 25);
    createPiece();

    while (createPiece()) {
      delay(500);
      while (! moveFallingPiece()) {
        for (int8_t i = 0; i < 15; i++) {
          readInputs();
          delay(20);
          if (speedupFlag) {
            i += 10;
          }
        }
      }
      delay(300);
      endOfFalling();
    }

    // TODO: game over screen
    delay(5000);
  }
}