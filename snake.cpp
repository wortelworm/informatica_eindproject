/**
 * this is code to simulate the game of snake
 * 
 * TODOS:
 *    sound effects & music
 *    high score?
 */

#include "snake.h"
#include "utils.h"


#define snakePixel(x, y) Utils::DrawPixel(x + 1, y + 1, RED);
#define foodPixel(x, y) Utils::DrawPixel(x + 0, y + 0, GREEN);

namespace Snake {
  
  bool gameOver;
  char directionX;
  char directionY;
  char nextDirectionX;
  char nextDirectionY;
  char snakeLength;
  char snakeX[225];
  char snakeY[225];

  char foodX;
  char foodY;

  void Play() {
    Snake::reset();

    while (! gameOver) {
      Snake::move(true);

      // maybe TODO this as a interrupt
      for (char i = 0; i < 40; i++) {
        Snake::readDirection();

        delay(20);
      }      
    }

    // TODO gameover screen
    Utils::FillRect(22, 22, 20, 20, WHITE);
  }


  // draws the food on the board
  void drawFood() {
    foodPixel(foodX*4+2, foodY*4+2);
    foodPixel(foodX*4+2, foodY*4+3);
    foodPixel(foodX*4+3, foodY*4+2);
    foodPixel(foodX*4+3, foodY*4+3);
  }


  // read the buttons for input of direction
  void readDirection() {
    if (directionX == 0) {
      // current direction is up or down
      if (digitalRead(BUTTON_LEFT)) {
        nextDirectionX = -1;
        nextDirectionY =  0;
      } else if (digitalRead(BUTTON_RIGHT)) {
        nextDirectionX =  1;
        nextDirectionY =  0;
      }
    } else {
      // current direction is up or down
      if (digitalRead(BUTTON_UP)) {
        nextDirectionX =  0;
        nextDirectionY = -1;
      } else if (digitalRead(BUTTON_DOWN)) {
        nextDirectionX =  0;
        nextDirectionY =  1;
      }
    }
  }


  // main game logic, moves the snake one spot
  void move(bool removeTail) {

    directionX = nextDirectionX;
    directionY = nextDirectionY;
    
    // new head location
    char newHeadX = directionX + snakeX[0];
    char newHeadY = directionY + snakeY[0];

    // draw the head
    snakePixel(newHeadX * 4+1, newHeadY * 4+1);
    snakePixel(newHeadX * 4+2, newHeadY * 4+1);
    snakePixel(newHeadX * 4+1, newHeadY * 4+2);
    snakePixel(newHeadX * 4+2, newHeadY * 4+2);

    // draw the pixels before the head
    if (directionY == 1) {
      // down
      snakePixel(newHeadX * 4 + 1, newHeadY * 4 + 0);
      snakePixel(newHeadX * 4 + 2, newHeadY * 4 + 0);
      snakePixel(newHeadX * 4 + 1, newHeadY * 4 - 1);
      snakePixel(newHeadX * 4 + 2, newHeadY * 4 - 1);
    } else if (directionY == -1) {
      // up
      snakePixel(newHeadX * 4 + 1, newHeadY * 4 + 3);
      snakePixel(newHeadX * 4 + 2, newHeadY * 4 + 3);
      snakePixel(newHeadX * 4 + 1, newHeadY * 4 + 4);
      snakePixel(newHeadX * 4 + 2, newHeadY * 4 + 4);
    } else if (directionX == 1) {
      // right
      snakePixel(newHeadX * 4 + 0, newHeadY * 4 + 1);
      snakePixel(newHeadX * 4 + 0, newHeadY * 4 + 2);
      snakePixel(newHeadX * 4 - 1, newHeadY * 4 + 1);
      snakePixel(newHeadX * 4 - 1, newHeadY * 4 + 2);
    } else {
      // left
      snakePixel(newHeadX * 4 + 3, newHeadY * 4 + 1);
      snakePixel(newHeadX * 4 + 3, newHeadY * 4 + 2);
      snakePixel(newHeadX * 4 + 4, newHeadY * 4 + 1);
      snakePixel(newHeadX * 4 + 4, newHeadY * 4 + 2);
    }
    
    // check if this move the head eats food
    if (newHeadX == foodX && newHeadY == foodY) {
      // no collision, no removal of tail
      
      // snake gets longer
      // the 'move tail over' code will handle the snakeX and snakeY variables correctly
      snakeLength++;

      if (snakeLength >= 225) {
        // TODO: win screen
        gameOver = true;
        return;
      }

      // respawn food, need to check that the food doesn't spawn on the snake
      // TODO: more efficient algoritm. this one is shit.
      bool good = false;
      while (! good) {
        foodX = random(0, 16);
        foodY = random(0, 16);
        good = true;

        for (char i = 0; i < snakeLength; i++) {
          if (snakeX[i] == foodX && snakeY[i] == foodY) {
            good = false;
          }
        }
      }


    } else {
      // remove the last tail
      char tailX = snakeX[snakeLength-1]*4 + 2;
      char tailY = snakeY[snakeLength-1]*4 + 2;

      if (removeTail) {
        for (int8_t i = 0; i < 4; i++) {
          for (int8_t j = 0; j < 4; j++) {
            Utils::DrawPixel(tailX + i, tailY + j, 0);
          }
        }
      }
      
      // collision check with walls and self
      if (newHeadX < 0 || newHeadX >= 15 || newHeadY < 0 || newHeadY >= 15) {
        gameOver = true;
        return;
      } else {
        // TODO: more efficient algoritm. this one is shit.
        for (int i = 1; i < snakeLength; i++) {
          if (newHeadX == snakeX[i] && newHeadY == snakeY[i]) {
            gameOver = true;
            return;
          }
        }    
      }
    }

    // update the variables
    // move tail over
    for (int i = snakeLength-1; i > 0; i--) {
      snakeX[i] = snakeX[i-1];
      snakeY[i] = snakeY[i-1];
    }
    // create the head
    snakeX[0] = newHeadX;
    snakeY[0] = newHeadY;
  }


  // game reset or game start
  void reset() {
    // the game starts with the snake coming from the middle left going to the right
    gameOver = false;
    directionX = 1;
    directionY = 0;
    nextDirectionX = 1;
    nextDirectionY = 0;
    snakeLength = 3;

    // the first food will spawn directly in front of the snake
    foodX = 10;
    foodY = 7;
    
    memset(snakeX, 0, 225);
    memset(snakeY, 0, 225);
    memset(snakeY, 7, 3);

    // draw the side of the screen
    for (int8_t i = 0; i < 64; i++) {
      for (int8_t j = 0; j < 64; j++) {
        if (i > 1 && i < 62 && j > 1 && j < 62) {
          Utils::DrawPixel(j, i, BLACK);
        } else
        if ((i + j) % 2 == 0) {
          Utils::DrawPixel(j, i, Utils::Color222(0, 3, 3));
        } else {
          Utils::DrawPixel(j, i, BLACK);
        }
      }
    }

    drawFood();

    // draw the initial snake
    // memset(snakeX, -1, 1);
    Snake::move(false);
    Snake::move(false);
  }
}