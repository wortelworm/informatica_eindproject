/**
 * This code is a replacement for the DFRobot_RGBMatrix library: https://github.com/DFRobot/DFRobot_RGBMatrix
 * 
 * 
 * Most of the code is copy pasted, but the following changes have been made:
 * 
 *    - removed a bunch of unused functions (mostly from the Adafruit_GFX)
 *    - 4 bit color depth -> 2 bit color depth
 *        - to reduce cpu and memory usage
 *        - the encoding of the colors has also changed
 *    - other thingies?
 * 
 * 
 */

#include "led_matrix.h"
#include <Arduino.h>

#define PIN_OUTPUT_ENABLED 9
#define PIN_LATCH 10
#define PIN_CLOCK 11
#define PIN_LA    A0
#define PIN_LB    A1
#define PIN_LC    A2
#define PIN_LD    A3
#define PIN_LE    A4

#define DATAPORT PORTA
#define DATADIR  DDRA
#define SCLKPORT PORTB

// format:
// 3 dimensions, from smallest to largest multiplier:
// x, bitplane, y
uint8_t *matrix_buffer;
volatile uint8_t *buffer_ptr;

// PORT register pointers, pin bitmasks, pin numbers:
volatile uint8_t
  *latport, *oeport, *addraport, *addrbport, *addrcport, *addrdport, *addreport;

uint8_t
  sclkpin, latpin, oepin, addrapin, addrbpin, addrcpin, addrdpin, addrepin;


// Counters/pointers for interrupt handler:
volatile uint8_t row, plane;


namespace LedMatrix {


  void Init() {
    // Allocate and initialize matrix buffer:

    // dimensions of 64x64, 2 pixels per byte, 2 bit color depth
    int buffsize  = 64 * 64 / 2 * 2;
    if(NULL == (matrix_buffer = (uint8_t *)malloc(buffsize))) return;
    memset(matrix_buffer, 0, buffsize);
    
    pinMode(PIN_CLOCK,OUTPUT);
    pinMode(PIN_LATCH,OUTPUT);
    pinMode(PIN_OUTPUT_ENABLED,OUTPUT);
    pinMode(24,OUTPUT); //R1
    pinMode(25,OUTPUT); //G1
    pinMode(26,OUTPUT); //B1
    pinMode(27,OUTPUT); //R2
    pinMode(28,OUTPUT); //G2
    pinMode(29,OUTPUT); //B2

    // the original code to initialize the FM6126 chip is not needed,
    // because this led matrix does not have that particulair chip
    
    // Look up port registers and pin masks ahead of time,
    // avoids many slow digitalWrite() calls later.
    sclkpin   = digitalPinToBitMask(PIN_CLOCK);
    latport   = portOutputRegister(digitalPinToPort(PIN_LATCH));
    latpin    = digitalPinToBitMask(PIN_LATCH);
    oeport    = portOutputRegister(digitalPinToPort(PIN_OUTPUT_ENABLED));
    oepin     = digitalPinToBitMask(PIN_OUTPUT_ENABLED);
    addraport = portOutputRegister(digitalPinToPort(PIN_LA));
    addrapin  = digitalPinToBitMask(PIN_LA);
    addrbport = portOutputRegister(digitalPinToPort(PIN_LB));
    addrbpin  = digitalPinToBitMask(PIN_LB);
    addrcport = portOutputRegister(digitalPinToPort(PIN_LC));
    addrcpin  = digitalPinToBitMask(PIN_LC);
    addrdport = portOutputRegister(digitalPinToPort(PIN_LD));
    addrdpin  = digitalPinToBitMask(PIN_LD);
    addreport = portOutputRegister(digitalPinToPort(PIN_LE));
    addrepin  = digitalPinToBitMask(PIN_LE);
    
    // Enable all comm & address pins as outputs, set default states:
    pinMode(PIN_CLOCK , OUTPUT);
    SCLKPORT   &= ~sclkpin;  // Low
    pinMode(PIN_LATCH, OUTPUT);
    *latport   &= ~latpin;   // Low
    pinMode(PIN_OUTPUT_ENABLED   , OUTPUT);
    *oeport    |= oepin;     // High (disable output)
    pinMode(PIN_LA    , OUTPUT);
    *addraport &= ~addrapin; // Low
    pinMode(PIN_LB    , OUTPUT);
    *addrbport &= ~addrbpin; // Low
    pinMode(PIN_LC    , OUTPUT);
    *addrcport &= ~addrcpin; // Low
    pinMode(PIN_LD  , OUTPUT);
    *addrdport &= ~addrdpin; // Low
    pinMode(PIN_LE  , OUTPUT);
    *addreport &= ~addrepin; // Low

    // The high six bits of the data port are set as outputs;
    // Might make this configurable in the future, but not yet.
    DATADIR  = B11111100;
    DATAPORT = 0;
  
    // Set up Timer1 for interrupt:
    TCCR1A  = _BV(WGM11); // Mode 14 (fast PWM), OC1A off
    TCCR1B  = _BV(WGM13) | _BV(WGM12) | _BV(CS10); // Mode 14, no prescale
    ICR1    = 100;
    TIMSK1 |= _BV(TOIE1); // Enable Timer1 interrupt
    sei();                // Enable global interrupts

  }
  void DrawPixel(int8_t x, int8_t y, uint8_t color) {
    uint8_t *ptr;
  
    if ((x < 0) || (x >= 64) || (y < 0) || (y >= 64)) {
      return;
    }

    // format of the color: 00RGBrgb
    // first the three upper bits and then the three lower bits
    
    ptr = &matrix_buffer[(y % 32) * 64 * 2 + x]; // Base addr

    if (y < 32) {
      // data for upper half of the display is stored in the lower bits
      // set the lower bitplane
      *ptr &= ~B00011100;
      *ptr |= (color & 7) << 2;

      // advance to the higher bitplane
      ptr += 64;
      
      *ptr &= ~B00011100;
      *ptr |= ((color >> 3) & 7) << 2;
    } else {
      // data for lower half of the display is stored in the upper bits
      // set the lower bitplane
      *ptr &= ~B11100000;
      *ptr |= (color & 7) << 5;

      // advance to the higher bitplane
      ptr += 64;
      
      *ptr &= ~B11100000;
      *ptr |= ((color >> 3) & 7) << 5;
      
    }
  }
  uint8_t getPixelColor(int8_t x, int8_t y) {
    uint8_t *ptr = &matrix_buffer[(y % 32) * 64 * 2 + x]; // Base addr
    uint8_t color = 0;
    if (y < 32) {
      // data for the upper half is stored in the lower bits
      // get the lower bitplane
      color |= ((*ptr) & B00011100) >> 2;
      
      // advance to the higher bitplane
      ptr += 64;
      
      // get the upper bitplane
      color |= ((*ptr) & B00011100) << 1;
    } else {
      // data for lower half of the display is stored in the upper bits
      // get the lower bitplane
      color |= ((*ptr) & B11100000) >> 5;
      
      // advance to the higher bitplane
      ptr += 64;
      
      // get the upper bitplane
      color |= ((*ptr) & B11100000) >> 2;
    }
    
    return color;
  }
  void SwapPixels(int8_t x1, int8_t y1, int8_t x2, int8_t y2) {

	  uint8_t original_color = getPixelColor(x1, y1);
    DrawPixel(x1, y1, getPixelColor(x2, y2));
    DrawPixel(x2, y2, original_color);

  }
}


// -------------------- Interrupt handler stuff --------------------
// Two constants are used in timing each successive BCM interval.
// These were found empirically, by checking the value of TCNT1 at
// certain positions in the interrupt code.
// CALLOVERHEAD is the number of CPU 'ticks' from the timer overflow
// condition (triggering the interrupt) to the first line in the
// updateDisplay() method.  It's then assumed (maybe not entirely 100%
// accurately, but close enough) that a similar amount of time will be
// needed at the opposite end, restoring regular program flow.
// LOOPTIME is the number of 'ticks' spent inside the shortest data-
// issuing loop (not actually a 'loop' because it's unrolled, but eh).
// Both numbers are rounded up slightly to allow a little wiggle room
// should different compilers produce slightly different results.
#define CALLOVERHEAD 60   // Actual value measured = 56
#define LOOPTIME     200  // Actual value measured = 188
// The "on" time for bitplane 0 (with the shortest BCM interval) can
// then be estimated as LOOPTIME + CALLOVERHEAD * 2.  Each successive
// bitplane then doubles the prior amount of time.  We can then
// estimate refresh rates from this:
// 4 bitplanes = 320 + 640 + 1280 + 2560 = 4800 ticks per row.
// 4800 ticks * 16 rows (for 32x32 matrix) = 76800 ticks/frame.
// 16M CPU ticks/sec / 76800 ticks/frame = 208.33 Hz.
// Actual frame rate will be slightly less due to work being done
// during the brief "LEDs off" interval...it's reasonable to say
// "about 200 Hz."  The 16x32 matrix only has to scan half as many
// rows...so we could either double the refresh rate (keeping the CPU
// load the same), or keep the same refresh rate but halve the CPU
// load.  We opted for the latter.
// Can also estimate CPU use: bitplanes 1-3 all use 320 ticks to
// issue data (the increasing gaps in the timing invervals are then
// available to other code), and bitplane 0 takes 920 ticks out of
// the 2560 tick interval.
// 320 * 3 + 920 = 1880 ticks spent in interrupt code, per row.
// From prior calculations, about 4800 ticks happen per row.
// CPU use = 1880 / 4800 = ~39% (actual use will be very slightly
// higher, again due to code used in the LEDs off interval).
// 16x32 matrix uses about half that CPU load.  CPU time could be
// further adjusted by padding the LOOPTIME value, but refresh rates
// will decrease proportionally, and 200 Hz is a decent target.

// The flow of the interrupt can be awkward to grasp, because data is
// being issued to the LED matrix for the *next* bitplane and/or row
// while the *current* plane/row is being shown.  As a result, the
// counter variables change between past/present/future tense in mid-
// function...hopefully tenses are sufficiently commented.

void updateDisplay(void)
{
  uint8_t  i, tick, tock, *ptr;
  uint16_t t, duration;

  *oeport  |= oepin;  // Disable LED output during row/plane switchover
  *latport |= latpin; // Latch data loaded during *prior* interrupt

  // Calculate time to next interrupt BEFORE incrementing plane #.
  // This is because duration is the display time for the data loaded
  // on the PRIOR interrupt.  CALLOVERHEAD is subtracted from the
  // result because that time is implicit between the timer overflow
  // (interrupt triggered) and the initial LEDs-off line at the start
  // of this method.
//  t = (nRows > 8) ? LOOPTIME : (LOOPTIME * 2);
  // if(nRows > 16)
  //   t = LOOPTIME>>1;
  // else if(nRows > 8)
  //   t = LOOPTIME;
  // else
  //   t = LOOPTIME<<1;
  t = LOOPTIME;

  duration = ((t + CALLOVERHEAD * 2) << plane) - CALLOVERHEAD;

  // Borrowing a technique here from Ray's Logic:
  // www.rayslogic.com/propeller/Programming/AdafruitRGB/AdafruitRGB.htm
  // This code cycles through all four planes for each scanline before
  // advancing to the next line.  While it might seem beneficial to
  // advance lines every time and interleave the planes to reduce
  // vertical scanning artifacts, in practice with this panel it causes
  // a green 'ghosting' effect on black pixels, a much worse artifact.

  if(++plane >= 2) {            // Advance plane counter.  Maxed out?
    plane = 0;                  // Yes, reset to plane 0, and
    if(++row >= 32) {           // advance row counter.  Maxed out?
      row     = 0;              // Yes, reset row counter, then...
      buffer_ptr = matrix_buffer; // Reset into front of the buffer
    }
  } else if(plane == 1) {
    // Plane 0 was loaded on prior interrupt invocation and is about to
    // latch now, so update the row address lines before we do that:
    if(row & 0x1)   *addraport |=  addrapin; // high
    else            *addraport &= ~addrapin; // low
    if(row & 0x2)   *addrbport |=  addrbpin;
    else            *addrbport &= ~addrbpin;
    if(row & 0x4)   *addrcport |=  addrcpin;
    else            *addrcport &= ~addrcpin;
    if(row & 0x8)   *addrdport |=  addrdpin;
    else            *addrdport &= ~addrdpin;
    if(row == 0x10) *addreport |=  addrepin;
    else if(row == 0x00) *addreport &= ~addrepin;
  }

  // buffer, being 'volatile' type, doesn't take well to optimization.
  // A local register copy can speed some things up:
  ptr = (uint8_t *)matrix_buffer;

  ICR1      = duration; // Set interval for next interrupt
  TCNT1     = 0;        // Restart interrupt timer
  *oeport  &= ~oepin;   // Re-enable output
  *latport &= ~latpin;  // Latch down

  // Record current state of SCLKPORT register, as well as a second
  // copy with the clock bit set.  This makes the innnermost data-
  // pushing loops faster, as they can just set the PORT state and
  // not have to load/modify/store bits every single time.  It's a
  // somewhat rude trick that ONLY works because the interrupt
  // handler is set ISR_BLOCK, halting any other interrupts that
  // might otherwise also be twiddling the port at the same time
  // (else this would clobber them).
  tock = SCLKPORT;
  tick = tock | sclkpin;

  // 188 ticks from TCNT1=0 (above) to end of function

  // Planes 1-3 copy bytes directly from RAM to PORT without unpacking.
  // The least 2 bits (used for plane 0 data) are presumed masked out
  // by the port direction bits.

  // A tiny bit of inline assembly is used; compiler doesn't pick
  // up on opportunity for post-increment addressing mode.
  // 5 instruction ticks per 'pew' = 160 ticks total
#define pew asm volatile(                 \
    "ld  __tmp_reg__, %a[ptr]+"    "\n\t"   \
    "out %[data]    , __tmp_reg__" "\n\t"   \
    "out %[clk]     , %[tick]"     "\n\t"   \
    "out %[clk]     , %[tock]"     "\n"     \
    :: [ptr]  "e" (ptr),                    \
       [data] "I" (_SFR_IO_ADDR(DATAPORT)), \
       [clk]  "I" (_SFR_IO_ADDR(SCLKPORT)), \
       [tick] "r" (tick),                   \
       [tock] "r" (tock));

  // Loop is unrolled for speed:
  pew pew pew pew  pew pew pew pew
  pew pew pew pew  pew pew pew pew
  pew pew pew pew  pew pew pew pew
  pew pew pew pew  pew pew pew pew
  
  pew pew pew pew  pew pew pew pew
  pew pew pew pew  pew pew pew pew
  pew pew pew pew  pew pew pew pew
  pew pew pew pew  pew pew pew pew

  buffer_ptr = ptr; //+= 32;
}



ISR(TIMER1_OVF_vect, ISR_BLOCK)   // ISR_BLOCK important -- see notes before
{
  updateDisplay();   // Call refresh func for active display
  TIFR1 |= TOV1;                  // Clear Timer1 interrupt flag
}
