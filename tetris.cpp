/**
 * this is code to simulate the game of tetris
 * for the used rules, see: https://tetris.wiki/Tetris_Guideline
 * 
 */

#include "tetris.h"
#include "utils.h"

#define tetrisPixel(x, y, color) Utils::DrawPixel(x + 2, y + 2, color)
#define tetrisBlock(x, y, color) Utils::FillRect((x)*3 + 2, (y)*3 + 2, 3, 3, color)
#define ghostPixel(x, y, color) Utils::DrawPixel((x)*3 + 3, (y)*3 + 3, color)

/// A = B from byte to bit level
#define setBitThingy(a, aMask, b, bMask) a ^= ((((a)&(aMask)) == 0) != (((b)&(bMask)) == 0) ) ? (aMask) : 0;


namespace Tetris {

  uint32_t score;
  uint8_t subScore;
  uint8_t level;
  uint8_t lines;
  uint8_t field[25];
  uint8_t bag[7];
  uint8_t bagPointer;
  uint8_t fallingX;
  uint8_t fallingY;
  Piece fallingPiece;
  Piece previewPiece;
  uint8_t ghostY;
  int8_t leftCountdown = 1;
  int8_t rightCountdown = 1;
  int8_t rotationCountdown = 1;
  bool speedupFlag = false;
  bool returnToMenu = false;

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


  void calculateGhostY() {
    if (fallingPiece.color == 0) {
      // no current falling piece, nothing to do here
      return;
    }

    if (wouldCollide(fallingX, fallingY+1, fallingPiece)) {
      ghostY = fallingY;
      return;
    }

    // try moving a copy of the current piece down until it hits something
    ghostY = fallingY+1;
    while (! wouldCollide(fallingX, ghostY, fallingPiece)) {
      ghostY++;
    }
    ghostY--;
  }


  void redrawFalling(int8_t newX, int8_t newY, Piece newPiece) {
    // remove the current falling piece and ghost piece
    for (int8_t i = 0; i < 8; i++) {
      if ((fallingPiece.upperShape & (1 << (7 - i))) != 0) {
        ghostPixel( fallingX+(i%4), ghostY   +(i>>2),     BLACK);
        tetrisBlock(fallingX+(i%4), fallingY +(i>>2),     BLACK);
      }
      if ((fallingPiece.lowerShape & (1 << (7 - i))) != 0) {
        ghostPixel( fallingX+(i%4), ghostY   +(i>>2) + 2, BLACK);
        tetrisBlock(fallingX+(i%4), fallingY +(i>>2) + 2, BLACK);
      }
    }
    
    bool recalculateGhostY = fallingX != newX || fallingPiece.upperShape != newPiece.upperShape || fallingPiece.lowerShape != newPiece.lowerShape;

    fallingX = newX;
    fallingY = newY;
    fallingPiece = newPiece;

    if (recalculateGhostY) {
      calculateGhostY();
    }

    // draw at updated position
    for (int8_t i = 0; i < 8; i++) {
      if ((fallingPiece.upperShape&(1<<(7-i))) != 0) {
        ghostPixel (fallingX+(i%4), ghostY  +(i>>2),   fallingPiece.color);
        tetrisBlock(fallingX+(i%4), fallingY+(i>>2),   fallingPiece.color);
      }
      if ((fallingPiece.lowerShape&(1<<(7-i))) != 0) {
        ghostPixel (fallingX+(i%4), ghostY  +(i>>2)+2, fallingPiece.color);
        tetrisBlock(fallingX+(i%4), fallingY+(i>>2)+2, fallingPiece.color);
      }
    }
    Utils::WriteBuffer();
  }
  void redrawFalling(int8_t newX, int8_t newY) {
    redrawFalling(newX, newY, fallingPiece);
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
    calculateGhostY();

    // show the preview piece
    // any non rotated piece has nothing in its lowershape
    // so dont draw over the 'Level' text
    for (int8_t i = 0; i < 8; i++) {
      tetrisBlock(12 + (i % 4), (i >> 2),     ((previewPiece.upperShape & (1 << (7 - i))) == 0) ? BLACK : previewPiece.color);
    }

    // show piece
    for (int8_t i = 0; i < 8; i++) {
      if ((fallingPiece.upperShape & (1 << (7 - i))) != 0) {
        ghostPixel (fallingX+(i%4), ghostY  +(i>>2),   fallingPiece.color);
        tetrisBlock(fallingX+(i%4), fallingY+(i>>2),   fallingPiece.color);
      }
      if ((fallingPiece.lowerShape & (1 << (7 - i))) != 0) {
        ghostPixel (fallingX+(i%4), ghostY  +(i>>2)+2, fallingPiece.color);
        tetrisBlock(fallingX+(i%4), fallingY+(i>>2)+2, fallingPiece.color);
      }
    }
    Utils::WriteBuffer();

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
    if (digitalRead(BUTTON_LEFT)) {
      if (leftCountdown <= 0) {
        if (! wouldCollide(fallingX-1, fallingY, fallingPiece)) {
          redrawFalling(fallingX-1, fallingY);
          leftCountdown = 5;
        }
      } else {
        leftCountdown --;
      }
    } else {
      leftCountdown = 0;
    }

    if (digitalRead(BUTTON_RIGHT)) {
      if (rightCountdown <= 0) {
        if (! wouldCollide(fallingX+1, fallingY, fallingPiece)) {
          redrawFalling(fallingX+1, fallingY);
          rightCountdown = 5;
        }
      } else {
        rightCountdown --;
      }
    } else {
      rightCountdown = 0;
    }

    if (digitalRead(BUTTON_UP)) {
      if (rotationCountdown <= 0) {
        Piece rotated = rotatePiece(fallingPiece);
        if (! wouldCollide(fallingX, fallingY, rotated)) {
          redrawFalling(fallingX, fallingY, rotated);
          rotationCountdown = 10;
        }
      } else {
        rotationCountdown --;
      }
    } else {
      rotationCountdown = 0;
    }

    speedupFlag = digitalRead(BUTTON_DOWN);

    if (digitalRead(BUTTON_MENU)) {
      // pause the game
      Utils::DrawText(34, 52, RED, "Pause");
      Utils::WriteBuffer();

      // until menu button is unpressed
      while (digitalRead(BUTTON_MENU)) {
        delay(10);
      }
      
      // and menu or start button is pressed
      while (! (digitalRead(BUTTON_MENU) || digitalRead(BUTTON_START))) {
        delay(10);
      }

      if (digitalRead(BUTTON_MENU)) {
        // return to main menu
        returnToMenu = true;
        return;
      }

      // resume the game
      Utils::FillRect(34, 52, 29, 7, BLACK);
      Utils::WriteBuffer();
    }
  }


  void drawScore() {
    if (score > 99999) {
      score = 99999;
    }
    // clear previous
    Utils::FillRect(34, 39, 29, 7, BLACK);

    // draw current
    char buffer[5] = {};
    itoa(score, buffer, 10);
    uint8_t numberOfDigits = 5;
    for (; numberOfDigits > 0; numberOfDigits--) {
      if (buffer[numberOfDigits-1] != 0) {
        // numberOfDigits++;
        break;
      }
    }
    Utils::DrawText(34 + 6*(5 - numberOfDigits), 39, RED, buffer);
    Utils::WriteBuffer();

    if (score >= 99999) {
      // HOW?? Sure, you win
      Utils::DrawText(34, 47, GREEN, "You");
      Utils::DrawText(34, 55, GREEN, "WON!");
      returnToMenu = true;
      Utils::WriteBuffer();
    
      // wait till button press before exit
      while (digitalRead(BUTTON_MENU) || digitalRead(BUTTON_START)) {
        delay(10);
      }

      while (! (digitalRead(BUTTON_MENU) || digitalRead(BUTTON_START))) {
        delay(10);
      }
    }
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

    uint8_t rowsCompleted = 0;

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
        rowsCompleted++;
        // flash it white
        for (uint8_t j = 0; j < 10; j++) {
          tetrisBlock(j, fallingY + i, WHITE);
        }
        Utils::WriteBuffer();
        delay(200);

        for (uint8_t j = 0; j < 10; j++) {
          tetrisBlock(j, fallingY + i, BLACK);
        }
        Utils::WriteBuffer();
        delay(50);


        // loop over all the rows
        for (int8_t j = fallingY+i; j > 0; j--) {

          // move pixels over
          for (int8_t k = 2; k >= 0; k--) {
            for (int8_t l = 0; l < 30; l++) {
              Utils::SwapPixels(2+l, 2+j*3 + k, 2+l, 2 + (j-1)*3 + k);
            }
          }
          Utils::WriteBuffer();

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

    switch (rowsCompleted) {
      case 1:
        score += 1 * level;
        break;
      case 2:
        score += 3 * level;
        break;
      case 3:
        score += 5 * level;
        break;
      case 4:
        score += 8 * level;
        break;
    }

    if (rowsCompleted > 0) {
      drawScore();
    }

    // every 10 lines cleared, level goes up
    // max level is 15
    lines += rowsCompleted;
    if ((lines / 10) + 1 > level && level < 15) {
      level++;

      // clear
      Utils::FillRect(57, 19, 5, 7, BLACK);

      // draw current
      if (level == 10) {
        Utils::DrawText(52, 19, RED, "1");
      }

      char c[1];
      itoa(level % 10, c, 10);
      Utils::DrawText(57, 19, RED, c);
      Utils::WriteBuffer();
    }
  }


  void Play() {
    Utils::FillRect(0, 0, 64, 64, BLACK);

    // draw outlines of the playing field
    for (int8_t i = 0; i < 30; i++) {
      tetrisPixel(i, -1, WHITE);
      tetrisPixel(i, 60, WHITE);
    }
    for (int8_t j = 0; j < 62; j++) {
      tetrisPixel(-1, j-1, WHITE);
      tetrisPixel(30, j-1, WHITE);
    }

    Utils::DrawText(34, 11, RED, "Level");
    Utils::DrawText(58, 19, RED, "1");
    Utils::DrawText(34, 28, RED, "Score");
    Utils::DrawText(58, 39, RED, "0");

    returnToMenu = false;
    bagPointer = 7;
    fallingPiece = { 0, 0, 0, RotationType::normal };
    previewPiece = { 0, 0, 0, RotationType::normal };
    score = 0;
    subScore = 0;
    level = 1;
    lines = 0;
    memset(field, 0, 25);
    createPiece();

    while (createPiece()) {
      delay(500);
      while (! moveFallingPiece()) {
        for (int8_t i = 0; i < 30 - (level*2); i++) {
          readInputs();
          if (returnToMenu) {
            return;
          }
          delay(25);
          if (speedupFlag) {
            i += 10;
            subScore += 1;
            if (subScore >= 100) {
              subScore %= 100;
              score += 1;
              drawScore();
            }
          }
        }
      }
      delay(300);
      endOfFalling();
    }

    // game over!
    Utils::DrawText(34, 47, RED, "Game");
    Utils::DrawText(34, 55, RED, "Over");
    Utils::WriteBuffer();

    // wait till button press before exit
    while (digitalRead(BUTTON_MENU) || digitalRead(BUTTON_START)) {
      delay(10);
    }

    while (! (digitalRead(BUTTON_MENU) || digitalRead(BUTTON_START))) {
      delay(10);
    }
  }
}
