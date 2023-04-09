/**
 * this is code to simulate flappy bird
 * see: https://en.wikipedia.org/wiki/Flappy_Bird
 * 
 *
 */

#include "flappybird.h"
#include "utils.h"

#define PIPE_DISTANCE 27

namespace FlappyBird {

  int8_t pipeY[4];
  int8_t pipesOffset;
  int8_t birdY;
  int8_t birdSpeed;
  uint8_t score;
  bool buttonWasPressed;
  bool gameOver;

  void movePipe(uint8_t index) {
    int8_t x = pipesOffset - 7 + PIPE_DISTANCE * index;

    if (x > 64) {
      // fully offscreen, dont draw
      return;
    }

    // do not draw over the score
    for (uint8_t i = (x < 14 ? 8 : 0); i < pipeY[index]-4; i++) {
      Utils::DrawPixel(x, i, GREEN);
    }
    for (uint8_t i = (x < 9 ? 8 : 0); i < pipeY[index]-4; i++) {
      Utils::DrawPixel(x + 5, i, BLACK);
    }
    for (uint8_t i = pipeY[index]-4; i < pipeY[index]; i++) {
      Utils::DrawPixel(x - 1, i, GREEN);
      Utils::DrawPixel(x + 6, i, BLACK);
    }
    
    for (uint8_t i = 63; i >= pipeY[index] + 15 + 4; i--) {
      Utils::DrawPixel(x, i, GREEN);
      Utils::DrawPixel(x + 5, i, BLACK);
    }
    for (uint8_t i = pipeY[index] + 15 + 3; i >= pipeY[index] + 15; i--) {
      Utils::DrawPixel(x - 1, i, GREEN);
      Utils::DrawPixel(x + 6, i, BLACK);
    }
  }

  uint8_t randomPipeY() {
    return random(14, 40);
  }

  void redrawBird(int8_t newY) {
    Utils::FillRect(5, birdY, 4, 3, BLACK);
    birdY = newY;
    Utils::FillRect(5, birdY, 4, 3, YELLOW);
    Utils::DrawPixel(7, birdY + 1, WHITE);
  }

  void move() {
    // currently only moves 1 pipe over
    pipesOffset--;

    if (pipesOffset == 6) {
      score++;
      Utils::FillRect(0, 0, 13, 7, BLACK);
      char buffer[2] = {};
      itoa(score, buffer, 10);
      Utils::DrawText((score < 10 ? 7 : 1), 0, RED, buffer);

      if (score >= 99) {
        gameOver = true;
      }
    }

    if (pipesOffset == 0) {
      // pipe[0] is now off screen
      // move over and generate new
      for (uint8_t i = 0; i < 3; i++) {
        pipeY[i] = pipeY[i+1];
      }
      pipeY[3] = randomPipeY();
      pipesOffset += PIPE_DISTANCE;
    }
    for (uint8_t i = 0; i < 4; i++) {
      movePipe(i);
    }

    if (digitalRead(BUTTON_MENU)) {
      gameOver = true;
    }


    bool buttonPress = digitalRead(BUTTON_LEFT) || digitalRead(BUTTON_DOWN) || digitalRead(BUTTON_RIGHT) || digitalRead(BUTTON_UP) || digitalRead(BUTTON_START);
    if (buttonPress && ! buttonWasPressed) {
      birdSpeed = -20;
      buttonWasPressed = true;
    }
    buttonWasPressed = buttonPress;

    birdSpeed += 5;
    if (birdSpeed > 15) {
      birdSpeed = 5;
    }
    int8_t newBirdY = birdY + (birdSpeed / 4);


    // collision detection
    // this code also makes sure that the bird
    // will only go a single pixel into the pipe/floor/ceiling
    bool collision = false;
    if (newBirdY < 8) {
      newBirdY = 7;
      collision = true;
    } else if (newBirdY > 62) {
      newBirdY = 62;
      collision = true;
    } else if (pipesOffset < 17 && pipesOffset > 6 && birdY < pipeY[0]) {
      // hit the upper pipe
      if (pipesOffset != 16) {
        newBirdY = pipeY[0]-1;
      }
      collision = true;
    } else if (pipesOffset < 17 && pipesOffset > 6 && birdY > pipeY[0] + 10) {
      // hit the lower pipe
      if (pipesOffset != 16) {
        newBirdY = pipeY[0] + 13;
      }
      collision = true;
    }
    redrawBird(newBirdY);

    Utils::WriteBuffer();

    if (collision) {
      gameOver = true;
    }
  }

  void Play() {
    Utils::FillRect(0, 0, 64, 64, BLACK);
    // borders for the score
    Utils::DrawLineHorizontal(0, 7, 13, WHITE);
    Utils::DrawLineVertical(13, 0, 8, WHITE);
    Utils::WriteBuffer();

    birdY = 30;
    birdSpeed = 0;
    pipesOffset = 72;
    buttonWasPressed = 0;
    gameOver = false;
    score = 0;
    pipeY[0] = randomPipeY();
    pipeY[1] = randomPipeY();
    pipeY[2] = randomPipeY();
    pipeY[3] = randomPipeY();

    while(! gameOver) {
      move();
      delay(100);
    }

    // show that its game over
    Utils::FillRect(19, 23, 25, 18, BLACK);
    Utils::DrawLineHorizontal(19, 22, 25, WHITE);
    Utils::DrawLineHorizontal(19, 41, 25, WHITE);
    Utils::DrawLineVertical(18, 23, 18, WHITE);
    Utils::DrawLineVertical(44, 23, 18, WHITE);
    Utils::DrawText(20, 24, RED, "GAME");
    Utils::DrawText(20, 33, RED, "OVER");
    Utils::WriteBuffer();

    // wait till menu button is unpressed and pressed
    while (digitalRead(BUTTON_MENU)) {
      delay(10);
    }
    while (!digitalRead(BUTTON_MENU)) {
      delay(10);
    }
  }
}
