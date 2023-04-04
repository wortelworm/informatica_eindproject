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

    void movePipe(uint8_t index) {
        int8_t x = pipesOffset - 7 + PIPE_DISTANCE * index;

        if (x > 64) {
            // fully offscreen, dont draw
            return;
        }

        for (uint8_t i = 0; i < pipeY[index]-4; i++) {
            Utils::DrawPixel(x, i, GREEN);
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
        return random(10, 64-15-10);
    }

    void redrawBird(int8_t newY) {
        Utils::FillRect(5, birdY, 4, 3, BLACK);
        birdY = newY;
        Utils::FillRect(5, birdY, 4, 3, YELLOW);
        // Utils::DrawPixel(7, birdY + 1, WHITE);
    }

    void move() {
        // currently only moves 1 pipe over
        pipesOffset--;
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

        // read move
        redrawBird(30);

        Utils::WriteBuffer();
    }

    void Play() {
        Utils::FillRect(0, 0, 64, 64, BLACK);
        Utils::WriteBuffer();

        birdY = 0;
        pipesOffset = 72;
        pipeY[0] = randomPipeY();
        pipeY[1] = randomPipeY();
        pipeY[2] = randomPipeY();
        pipeY[3] = randomPipeY();

        while(true) {
            move();
            delay(120);
        }
    }
}