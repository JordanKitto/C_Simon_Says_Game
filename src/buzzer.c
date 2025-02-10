#include "buzzer.h"
#include <avr/io.h>
#include <stdint.h>

// Define frequencies for the buzzer based on musical notes
#define TONE_E_HIGH 40040  // Frequency for E high note
#define TONE_E_LOW 80320   // Frequency for E low note
#define TONE_A 30028       // Frequency for A note
#define TONE_C_SHARP 47620 // Frequency for C# note


volatile int8_t octave = 0; // Current octave shift, modified elsewhere in the program

// Resets the frequency to the default octave (no shift)
void reset_frequency(void)
{
    octave = 0;
}

// Activates the buzzer with the specified tone index
void buzzer_on(uint8_t tone)
{
    static const uint32_t periods[4] = {TONE_E_HIGH, TONE_C_SHARP, TONE_A, TONE_E_LOW};
    // Calculate period taking into account the octave shift
    TCA0.SINGLE.PERBUF = periods[tone] >> (octave + 2);
    TCA0.SINGLE.CMP0BUF = TCA0.SINGLE.PERBUF >> 1; // Duty cycle of 50%
    TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm; // Enable the timer to start buzzing
}

// Turns off the buzzer
void buzzer_off(void)
{
    TCA0.SINGLE.CTRLA &= ~TCA_SINGLE_ENABLE_bm; // Disable the timer to stop buzzing
}
