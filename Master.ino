#include "snake.h"
#include "tetris.h"
#include "game2048.h"
#include "simon.h"
#include "breakout.h"
#include "flappybird.h"
#include "utils.h"

bool musicWait = false;

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
        Utils::PlayMelody(MELODY_BEETHOVEN);
        Snake::Play();
        break;
      case 1:
        Utils::PlayMelody(MELODY_TETRIS);
        Tetris::Play();
        break;
      case 2:
        Utils::PlayMelody(MELODY_BEETHOVEN);
        Game2048::Play();
        break;
      case 3:
        Simon::Play();
        break;
      case 4:
        Utils::PlayMelody(MELODY_FROZEN);
        Breakout::Play();
        break;
      case 5:
        Utils::PlayMelody(MELODY_FROZEN);
        FlappyBird::Play();
        break;
    }
  
    Utils::PlayMelody(MELODY_NONE);
    musicWait = true;
    return true;
  }
  bool updateMenu = false;
  if (digitalRead(BUTTON_RIGHT) && ! prevStateButtonRight) {
    game++;
    if (game > 5) {
      game = 0;
    }
    updateMenu = true;
  } else
  if (digitalRead(BUTTON_LEFT) && ! prevStateButtonLeft) {
    game--;
    if (game < 0) {
      game = 5;
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
    case 4:
      // breakout
      Utils::DrawText(18, 24, GREEN, "BREAK");
      Utils::DrawText(24, 32, GREEN, "OUT");
      break;
    case 5:
      // flappy bird
      Utils::DrawText(15, 24, BLUE, "FLAPPY");
      Utils::DrawText(21, 32, BLUE, "BIRD");
      break;
    default:
      // ?????
      game = 0;
      return;
  }
  Utils::WriteBuffer();
  
  if (musicWait) {
    delay(1000);
    musicWait = false;
  }

  while (! readButtons()) {
    delay(10);
  }
}
