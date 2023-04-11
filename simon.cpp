/**
 * this is code to simulate the game of simon
 * see: https://en.wikipedia.org/wiki/Simon_(game)
 * 
 * the display is rotated 45 degrees to match the buttons of up down left and right
 */

#include "simon.h"
#include "utils.h"

namespace Simon {
  uint8_t sequence[10];

  // makes the light bright, plays tone and dims the light again
  // E (green, left, an octave lower than blue);
  // C♯ (yellow, lower);
  // E (blue, right);
  // A (red, upper).
  void showLight(uint8_t id) {
    uint8_t x;
    uint8_t y;
    uint8_t color;
    uint16_t tone;
    switch (id & 3) {
      case 0:
        x = 10;
        y = 22;
        color = GREEN;
        // G5
        tone = 784;
        break;
      case 1:
        x = 22;
        y =  10;
        color = YELLOW;
        // C5
        tone = 523;
        break;
      case 2:
        x = 34;
        y = 22;
        color = BLUE;
        // G4
        tone = 392;
        break;
      case 3:
        x = 22;
        y = 34;
        color = RED;
        // E5
        tone = 659;
        break;
    }

    // fill a rotated square lit up
    for (uint8_t i = 0; i < 10-1; i++) {
      for (uint8_t j = 0; j < 10; j++) {
        Utils::DrawPixel(x +     i + j, y - i + j, color);
      }
      for (uint8_t j = 0; j < 10-1; j++) {
        Utils::DrawPixel(x + 1 + i + j, y - i + j,  WHITE);
      }
    }
    for (uint8_t j = 0; j < 10; j++) {
      Utils::DrawPixel(x + 9 + j, y - 9 + j, color);
    }
    Utils::WriteBuffer();

    // play tone while lit up
    Utils::PlayTone(tone, 500);

    // fill a rotated square dimmed
    for (uint8_t i = 0; i < 10-1; i++) {
      for (uint8_t j = 0; j < 10; j++) {
        Utils::DrawPixel(x +     i + j, y - i + j, BLACK);
      }
      for (uint8_t j = 0; j < 10-1; j++) {
        Utils::DrawPixel(x + 1 + i + j, y - i + j, color);
      }
    }
    for (uint8_t j = 0; j < 10; j++) {
      Utils::DrawPixel(x + 9 + j, y - 9 + j, BLACK);
    }
    Utils::WriteBuffer();

    delay(50);
  }

  void Play() {
    Utils::FillRect(0, 0, 64, 64, BLACK);

    showLight(0);
    showLight(1);
    showLight(2);
    showLight(3);

    delay(1000);

    // create random sequence
    memset(sequence, 0, 10);
    for (uint8_t i = 0; i < 40; i++) {
      uint8_t random_id = random(0, 4);
      sequence[i/4] |= random_id << (2 * (i % 4));
    }

    // main game loop
    uint8_t length = 0;
    bool gameOver = false;
    while (++length < 41) {
      // show the current sequence
      for (uint8_t i = 0; i < length; i++) {
        uint8_t id = (sequence[i/4] >> (2 * (i % 4))) & 3;
        showLight(id);
        delay(150);
      }

      for (uint8_t i = 0; i < length; i++) {
        // wait for user to press the next button
        while (digitalRead(BUTTON_LEFT) || digitalRead(BUTTON_DOWN) || digitalRead(BUTTON_RIGHT) || digitalRead(BUTTON_UP)) {
          delay(10);
        }
        while (!(digitalRead(BUTTON_LEFT) || digitalRead(BUTTON_DOWN) || digitalRead(BUTTON_RIGHT) || digitalRead(BUTTON_UP))) {
          delay(10);
        }

        // turn button press into id
        uint8_t user_id;
        if (digitalRead(BUTTON_LEFT)) {
          user_id = 0;
        } else
        if (digitalRead(BUTTON_UP)) {
          user_id = 1;
        } else
        if (digitalRead(BUTTON_RIGHT)) {
          user_id = 2;
        } else {
          user_id = 3;
        }
        
        uint8_t correct_id = (sequence[i/4] >> (2 * (i % 4))) & 3;
        if (correct_id != user_id) {
          // WRONG!
          gameOver = true;
          break;
        }

        // Correct, show it and advance to next
        showLight(user_id);
      }

      if (gameOver) {
        // :(
        break;
      }

      // player survived the round,
      // wait a bit before advancing to the next round
      delay(1000);
    }

    if (gameOver) {
      Utils::DrawText(5, 44, RED, "GAME OVER");
      Utils::DrawText(5, 52, RED, "Score: ");
      char buffer[2] = {'0', '0'};
      itoa(length-1, buffer, 10);
      Utils::DrawText((length-1) < 10 ? 53 : 47, 52, RED, buffer);

      // The player is a failure, give sound feedback to reflect that
      Utils::PlayTone(2000, 1000);
      
    } else {
      // somehow someone did so well he/she reached the end of the sequence
      Utils::DrawText(5, 44, GREEN, "YOU WON!!");
      Utils::DrawText(5, 52, GREEN, "SCORE: 40");
    }
    Utils::WriteBuffer();

    // wait till menu press
    while (! digitalRead(BUTTON_MENU)) {
      delay(10);
    }
  }
}
