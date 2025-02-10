#include "timer.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "display.h"
#include "types.h"

volatile uint8_t pb_debounced_state = 0xFF;    // Current debounced state of pushbuttons
volatile uint16_t time_passed = 0;        // Tracks the elapsed time
volatile uint16_t playback_duration = 250;     // Default playback duration
volatile uint16_t new_playback_duration = 250; // New duration calculated from ADC input
volatile uint8_t updating_playback_delay = 1;  // Flag to update playback duration
volatile uint8_t pb_state = 0xFF;              // Current state of pushbuttons

// Debounces pushbutton inputs
void pb_debounce(void)
{
    static uint8_t count0 = 0, count1 = 0;
    uint8_t pb_edge = pb_state ^ PORTA.IN; // Detect changes
    count1 = (count1 ^ count0) & pb_edge;  // Intermediate debounce counter
    count0 = ~count0 & pb_edge;            // Final debounce counter
    pb_state ^= (count1 & count0);         // Update state based on debounced inputs
}

// Interrupt Service Routine for Timer/Counter B1 - handles debouncing and display update
ISR(TCB1_INT_vect)
{
    pb_debounce();
    static uint8_t digit = 0;
    digit = !digit;                                     // Toggle digit for display update
    spi_write(digit ? segs[0] | (0x01 << 7) : segs[1]); // Update display via SPI
    TCB1.INTFLAGS = TCB_CAPT_bm;                        // Clear the interrupt flag
}

// Interrupt Service Routine for Timer/Counter B0 - handles timing
ISR(TCB0_INT_vect)
{
    time_passed++; // Increment elapsed time
    if (updating_playback_delay)
        playback_duration = 250 + ((1757UL * ADC0.RESULT) >> 8); // Adjust playback duration based on ADC
    TCB0.INTFLAGS = TCB_CAPT_bm;                                 // Clear the interrupt flag
}