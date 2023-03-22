/**
 * this is code to simulate flappy bird
 * see: https://en.wikipedia.org/wiki/Flappy_Bird
 * 
 *
 */

#include "flappybird.h"
#include "utils.h"

namespace FlappyBird {

    uint8_t pipeY[5];
    uint8_t pipesOffset;

    void movePipe(uint8_t index) {
        uint8_t x = pipesOffset + 20 * index;

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

    void move() {
        // currently only moves 1 pipe over
        pipesOffset--;
        movePipe(0);

        Utils::WriteBuffer();
    }

    void Play() {
        Utils::FillRect(0, 0, 64, 64, BLACK);
        Utils::WriteBuffer();
        // TODO
        pipesOffset = 65;
        pipeY[0] = 20;

        while(true) {
            move();
            delay(100);
        }
    }
}