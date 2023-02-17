#include "snake.h"
#include "utils.h"

/**
 * This file contains the code to simulates a snake
 * game on a 15x15 board displayed on a 64x64 led matrix 
 * 
 * TODOS:
 *    make a menu screen
 *    library optimization/specification
 * 
 */

void setup() {
#ifdef ARDUINO
  // attempt to randomize
  randomSeed(analogRead(0));
#endif
	
  // initialization
  Utils::Init();


}


void drawMenuBasis() {
  Utils::FillRect(0, 0, 64, 64, BLACK);

  // Utils::matrix.setTextColor(RED);
  // Utils::matrix.setCursor(15, 40);
  // Utils::matrix.print("Press start");

  // Navigation arrows
  // TODO

}

int8_t game = 0;
bool prevStateButtonLeft = false;
bool prevStateButtonRight = false;

/** 
 * returns true if the menu needs updating
 * false otherwise
 *
 */
bool readButtons() {
  if (digitalRead(BUTTON_START)) {
    // start the game
    switch (game) {
      case 0:
        Snake::Play();
        break;
      case 1:
      case 2:
        Utils::FillRect(0, 0, 64, 64, WHITE);
        break;
    }

    return true;
  }
  if (digitalRead(BUTTON_RIGHT) && ! prevStateButtonRight) {
    game++;
    if (game > 2) {
      game = 0;
    }
    return true;
  } else
  if (digitalRead(BUTTON_LEFT) && ! prevStateButtonLeft) {
    game--;
    if (game < 0) {
      game = 2;
    }
    return true;
  }
  prevStateButtonRight = digitalRead(BUTTON_RIGHT);
  prevStateButtonLeft  = digitalRead(BUTTON_LEFT);

  return false;
}

void loop() {

  // draw menu of the current game selected
  drawMenuBasis();
  // switch (game) {
  //   case 0:
  //     // SNAKE
  //     Utils::matrix.setTextColor(RED);
  //     Utils::matrix.setCursor(15, 15);
  //     Utils::matrix.print("SNAKE");
  //     break;
  //   case 1:
  //     // TETRIS
  //     Utils::matrix.setTextColor(GREEN);
  //     Utils::matrix.setCursor(15, 15);
  //     Utils::matrix.print("TETRIS");
  //     break;
  //   case 2:
  //     // 2048
  //     Utils::matrix.setTextColor(BLUE);
  //     Utils::matrix.setCursor(15, 15);
  //     Utils::matrix.print("2048");
  //     break;
  //   default:
  //     // ?????
  //     game = 0;
  //     break;
  // }

  while (! readButtons()) {
    delay(10);
  }
}
