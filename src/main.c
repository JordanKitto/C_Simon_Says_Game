#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "buzzer.h"
#include "display.h"
#include "types.h"
#include "initialisation.h"
#include "timer.h"
#include "sequence.h"
#include "uart.h"

// Global variables for game state management.
volatile State STATE = INIT;                              // Game state initialization.
volatile uint16_t length_sequence = 1;                    // Length of the Simon's sequence.
volatile Players_Turn_State PLAYERS_STATE = PLAYER_PAUSE; // Player's turn state.
volatile Simons_Turn_State SIMONS_STATE = SIMON_SILENT;   // Simon's turn state.
volatile int32_t index_tone = 0;                          // Current tone index in the sequence.
volatile uint32_t sequence_confirmed = 1;                 // Flag for sequence confirmation.
volatile uint8_t player_input_tracker = 0;                // Tracks player inputs.
volatile uint8_t pb_released = 0;                         // Flag for button release state.
volatile Level_State LEVEL_STATE;                         // State of game level.
uint32_t players_rank = 0;                                // Player's current rank.
volatile uint8_t pb_state;                                // State of the push buttons.
volatile uint8_t new_seed;                                // Flag for new seed generation.
volatile uint8_t reset;                                   // Game reset flag.
volatile uint8_t next_lfsr_digit;                         // Next digit in LFSR sequence.
volatile uint8_t updating_playback_delay;                 // Flag for updating playback delay.
volatile uint16_t time_passed;                            // Time passed for timing events.
volatile uint16_t playback_duration;                      // Duration for playback events.

// Function declarations for game logic components.
void reset_lfsr_state();
void next();
void display_digit(uint8_t digit);
void buzzer_on(uint8_t digit);
void buzzer_off();
void clear_display();
void show_victory();
void show_defeat();
void display_score(uint32_t score);
void is_sequence_confirmed(uint8_t input, uint8_t expected);

// State machine function to manage game states and transitions.
static void state_machine(void)
{
    uint8_t pb_previous_state = 0xFF; // Previous state of push buttons.
    uint8_t pb_new_state = 0xFF;      // Current state of push buttons.
    uint8_t pb_falling_edge;          // Detects falling edge for button press.
    uint8_t pb_rising_edge;           // Detects rising edge for button release.

    // Main game loop: continuously checks and updates game state
    while (1)
    {
        // Update button state detection variables
        pb_previous_state = pb_new_state;
        pb_new_state = pb_state;
        pb_falling_edge = (pb_previous_state ^ pb_new_state) & pb_previous_state; // Detect button press
        pb_rising_edge = (pb_previous_state ^ pb_new_state) & pb_new_state;       // Detect button release

        // Handle game state transitions
        switch (STATE)
        {
        case INIT:
            // Initialize game settings for a new game
            length_sequence = 1;         // Start sequence length
            updating_playback_delay = 1; // Ensure playback delay is updated
            STATE = SIMONS_TURN;         // Move to Simon's turn
            break;

        case SIMONS_TURN:
            // Logic for Simon's turn in the game
            if (new_seed)
            {
                reset_lfsr_state(); // Reset the LFSR state for new sequence generation
                new_seed = 0;
            }

            state_lfsr = start_state_lfsr; // Set the initial LFSR state
            index_tone = 0;                // Reset tone index
            SIMONS_STATE = SIMON_START;    // Begin Simon's sequence playback

            while (index_tone < length_sequence)
            {
                uint16_t half_playback_duration = playback_duration >> 1;

                if (reset)
                    break; // Exit loop if reset is triggered

                switch (SIMONS_STATE)
                {
                case SIMON_START:
                    // Start of Simon's turn
                    next();                         // Advance to next tone in sequence
                    display_digit(next_lfsr_digit); // Display the corresponding digit
                    buzzer_on(next_lfsr_digit);     // Play the corresponding tone
                    time_passed = 0;                // Reset time tracking
                    updating_playback_delay = 0;    // Stop updating playback delay during tone
                    SIMONS_STATE = SIMON_PLAY;      // Move to playing state
                    break;
                case SIMON_PLAY:
                    // During tone playback
                    if (time_passed >= 125) // Check if tone has played long enough
                    {
                        buzzer_off();                // Turn off buzzer
                        clear_display();             // Clear display
                        updating_playback_delay = 1; // Resume updating playback delay
                        SIMONS_STATE = SIMON_SILENT; // Silence before next tone
                    }
                    break;
                case SIMON_SILENT:
                    // Silent period between tones
                    if (time_passed >= (2 * half_playback_duration))
                    {
                        index_tone++;               // Move to next tone in sequence
                        SIMONS_STATE = SIMON_START; // Restart tone playback
                    }
                    break;
                default:
                    break;
                }
            }

            // Handle state transition or reset
            if (reset)
            {
                STATE = INIT;
                reset = 0;
            }
            else
            {
                STATE = PLAYERS_TURN; // Change to player's turn
            }
            break;

        case PLAYERS_TURN:
            // Logic for the player's turn to replicate Simon's sequence
            PLAYERS_STATE = PLAYER_PAUSE;  // Start with player in pause state
            index_tone = 0;                // Reset tone index
            sequence_confirmed = 1;        // Assume sequence is correct unless proven otherwise
            state_lfsr = start_state_lfsr; // Reset LFSR state

            while (1)
            {
                pb_previous_state = pb_new_state;
                pb_new_state = pb_state;
                pb_falling_edge = (pb_previous_state ^ pb_new_state) & pb_previous_state;
                pb_rising_edge = (pb_previous_state ^ pb_new_state) & pb_new_state;

                if (index_tone >= length_sequence || sequence_confirmed == 0)
                    break; // Exit loop if sequence ends or error occurs

                switch (PLAYERS_STATE)
                {
                case PLAYER_PAUSE:
                    // Player is waiting to press a button
                    updating_playback_delay = 1;

                    // Handle button presses and map them to tones
                    if (pb_falling_edge & PB1)
                    {
                        updating_playback_delay = 0;
                        buzzer_on(TONE_1);
                        display_digit(DISP_1);
                        player_input_tracker = 0;
                    }
                    else if (pb_falling_edge & PB2)
                    {
                        updating_playback_delay = 0;
                        buzzer_on(TONE_2);
                        display_digit(DISP_2);
                        player_input_tracker = 1;
                    }
                    else if (pb_falling_edge & PB3)
                    {
                        updating_playback_delay = 0;
                        buzzer_on(TONE_3);
                        display_digit(DISP_3);
                        player_input_tracker = 2;
                    }
                    else if (pb_falling_edge & PB4)
                    {
                        updating_playback_delay = 0;
                        buzzer_on(TONE_4);
                        display_digit(DISP_4);
                        player_input_tracker = 3;
                    }

                    // Move to playing state if any button is pressed
                    if (pb_falling_edge & (PB1 | PB2 | PB3 | PB4))
                    {
                        pb_released = 0;
                        time_passed = 0;
                        PLAYERS_STATE = PLAYER_PLAY;
                    }
                    break;

                case PLAYER_PLAY:
                    // Player is replicating the sequence
                    if (!pb_released)
                    {
                        // Check for button release
                        if (pb_rising_edge & PB1 && player_input_tracker == 0)
                            pb_released = 1;
                        else if (pb_rising_edge & PB2 && player_input_tracker == 1)
                            pb_released = 1;
                        else if (pb_rising_edge & PB3 && player_input_tracker == 2)
                            pb_released = 1;
                        else if (pb_rising_edge & PB4 && player_input_tracker == 3)
                            pb_released = 1;
                    }
                    else if (time_passed >= 125) // Ensure display and tone are off after appropriate time
                    {
                        buzzer_off();
                        clear_display();
                        updating_playback_delay = 1;
                        next();                                                       // Move to next sequence item
                        is_sequence_confirmed(player_input_tracker, next_lfsr_digit); // Check if player's input matches sequence
                        index_tone++;
                        PLAYERS_STATE = PLAYER_PAUSE; // Return to pause state
                    }
                    break;
                default:
                    break;
                }
            }

            // Handle game state transition or reset
            if (reset)
            {
                STATE = INIT;
                reset = 0;
            }
            else
            {
                STATE = RESULT; // Move to result handling
            }
            break;

        case RESULT:
            // Handle the result of the player's sequence
            LEVEL_STATE = RANK;

            while (1)
            {
                if (reset)
                    break; // Exit if reset is triggered

                if (STATE == SIMONS_TURN || STATE == RECORD_RESULT)
                    break; // Exit if state changes unexpectedly

                switch (LEVEL_STATE)
                {
                case RANK:
                    // Determine if player's performance was successful
                    if (sequence_confirmed)
                    {
                        time_passed = 0;
                        show_victory();                 // Display victory message
                        players_rank = length_sequence; // Update rank
                        length_sequence++;              // Prepare for next level
                        LEVEL_STATE = SHOW_LEVEL;
                    }
                    else
                    {
                        time_passed = 0;
                        show_defeat();                      // Display defeat message
                        players_rank = length_sequence - 1; // Set final score
                        length_sequence = 1;                // Reset sequence length
                        start_state_lfsr = state_lfsr;      // Reset LFSR
                        LEVEL_STATE = SHOW_RANK;            // Move to rank display
                    }

                case SHOW_RANK:
                    // Display player's rank after game end
                    if (time_passed >= playback_duration)
                    {
                        time_passed = 0;
                        display_score(players_rank); // Show score
                        LEVEL_STATE = SHOW_LEVEL;
                    }
                    break;

                case SHOW_LEVEL:
                    // Prepare for next level or restart
                    if (time_passed >= 250)
                    {
                        clear_display();     // Clear display
                        STATE = SIMONS_TURN; // Restart Simon's turn
                    }
                    break;

                default:
                    break;
                }
            }

            // Reset game if necessary
            if (reset)
            {
                STATE = INIT;
                updating_playback_delay = 1;
                playback_duration = 250 + ((1757UL * ADC0.RESULT) >> 8); // Calculate new playback duration
                reset = 0;
            }
            break;

        default:
            break;
        }
    }
}

// Main function to initialize system and run game.
int main(void)
{
    cli();         // Disable global interrupts for setup.
    button_init(); // Initialize button hardware.
    spi_init();    // Initialize SPI interface.
    adc_init();    // Initialize analog-to-digital converter.
    pwm_init();    // Initialize pulse width modulation for buzzers.
    timer_init();  // Initialize system timers.
    port_init();   // Initialize I/O ports.
    // uart_init(); // Initialize UART for serial communication.
    sei();                                                   // Enable global interrupts.
    playback_duration = 250 + ((1757UL * ADC0.RESULT) >> 8); // Calculate initial playback duration.
    state_machine();                                         // Run the main state machine.
}
