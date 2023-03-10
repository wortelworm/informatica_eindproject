#include "snake.h"
#include "tetris.h"
#include "game2048.h"
#include "simon.h"
#include "utils.h"

/**
 * This file contains the code to simulates a snake
 * game on a 15x15 board displayed on a 64x64 led matrix 
 * 
 * TODOS:
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

  // Navigation arrows
  // left
  for (uint8_t i = 0; i < 4; i++) {
    Utils::DrawPixel(4 + i, 31 + i, WHITE);
    Utils::DrawPixel(4 + i, 31 - i, WHITE);
  }
  // right
  for (uint8_t i = 0; i < 4; i++) {
    Utils::DrawPixel(60 - i, 31 + i, WHITE);
    Utils::DrawPixel(60 - i, 31 - i, WHITE);
  }

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
        Tetris::Play();
        break;
      case 2:
        Game2048::Play();
        break;
      case 3:
        Simon::Play();
        break;
    }

    return true;
  }
  bool updateMenu = false;
  if (digitalRead(BUTTON_RIGHT) && ! prevStateButtonRight) {
    game++;
    if (game > 3) {
      game = 0;
    }
    updateMenu = true;
  } else
  if (digitalRead(BUTTON_LEFT) && ! prevStateButtonLeft) {
    game--;
    if (game < 0) {
      game = 3;
    }
    updateMenu = true;
  }
  prevStateButtonRight = digitalRead(BUTTON_RIGHT);
  prevStateButtonLeft  = digitalRead(BUTTON_LEFT);

  return updateMenu;
}

void loop() {

  // draw menu of the current game selected
  drawMenuBasis();
  switch (game) {
    case 0:
      // SNAKE
      Utils::DrawText(18, 28, RED, "SNAKE");
      break;
    case 1:
      // TETRIS
      Utils::DrawText(15, 28, GREEN, "TETRIS");
      break;
    case 2:
      // 2048
      Utils::DrawText(21, 28, BLUE, "2048");
      break;
    case 3:
      // simon
      Utils::DrawText(18, 28, RED, "SIMON");
      break;
    default:
      // ?????
      game = 0;
      return;
  }

  while (! readButtons()) {
    delay(10);
  }
}
