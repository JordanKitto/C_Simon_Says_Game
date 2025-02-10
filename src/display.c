#include "display.h"
#include <avr/io.h>
#include <avr/interrupt.h>

// Segment codes for 7-segment displays, using common cathode configuration
#define SEGS_BC 0b01101011      // BC segments for tone display
#define SEGS_OFF 0b01111111     // All segments off
#define SEGS_EF 0b00111110      // EF segments for tone display
#define SEGS_SUCCESS 0b00000000 // All segments on for success
#define SEGS_FAIL 0b01110111    // 'G' segment on for failure

// Initial display state: all segments off
volatile uint8_t segs[2] = {SEGS_OFF, SEGS_OFF};

// Segment mapping for numbers 0-9 for scoring
volatile uint8_t number_segs[10] = {
    0x08, 0x6B, 0x44, 0x41, 0x23, 0x11, 0x10, 0x4B, 0x00, 0x01};

// Write data to SPI register for display update
void spi_write(uint8_t b)
{
    SPI0.DATA = b; // Load data into the SPI data register, automatically begins transmission
}

void display_digit(uint8_t sequence_digit)
{
    // Define segment configurations for each digit
    static uint8_t seg_configs[4][2] = {
        {SEGS_EF, SEGS_OFF}, // DISP_1
        {SEGS_BC, SEGS_OFF}, // DISP_2
        {SEGS_OFF, SEGS_EF}, // DISP_3
        {SEGS_OFF, SEGS_BC}  // DISP_4
    };

    if (sequence_digit >= DISP_1 && sequence_digit <= DISP_4)
    {
        // Apply the segment configuration for the given digit
        segs[0] = seg_configs[sequence_digit - DISP_1][0];
        segs[1] = seg_configs[sequence_digit - DISP_1][1];
    }
    else
    {
        // Clear display for unexpected digit index
        clear_display();
    }
}

// Clears both segments of the 7-segment display
void clear_display(void)
{
    segs[0] = segs[1] = SEGS_OFF;
}

// Displays a success pattern on the 7-segment display
void show_victory(void)
{
    segs[0] = segs[1] = SEGS_SUCCESS; // All segments on for both displays
}

// Displays a failure pattern on the 7-segment display
void show_defeat(void)
{
    segs[0] = segs[1] = SEGS_FAIL; // Only 'G' segment on for both displays
}

// Displays the player's score on the 7-segment display
void display_score(uint32_t count)
{
    if (count < 10)
    {
        segs[0] = SEGS_OFF;            // Turn off the left-hand segment
        segs[1] = number_segs[count];  // Set the right-hand segment to the units digit
    }
    else
    {
        uint32_t units = count % 10;       // Units digit is the remainder of count divided by 10
        uint32_t tens = (count / 10) % 10; // Tens digit is the remainder of count divided by 100 divided by 10

        segs[0] = number_segs[tens];  // Set the left-hand segment to the tens digit
        segs[1] = number_segs[units]; // Set the right-hand segment to the units digit
    }
}

// SPI interrupt service routine to handle display updates
ISR(SPI0_INT_vect)
{
    // Toggle the display latch to update the physical display
    PORTA.OUTCLR = PIN1_bm; // Clear the latch pin
    PORTA.OUTSET = PIN1_bm; // Set the latch pin high to commit data

    SPI0.INTFLAGS = SPI_IF_bm; // Clear the interrupt flag to prepare for next SPI transmission
}
