#include <stdint.h>
#include "sequence.h"

#define student_id 0x10193944
#define mask 0xE2023CAB



// Initial state for the LFSR and related variables
volatile uint32_t new_state_lfsr = student_id, start_state_lfsr = student_id, state_lfsr = student_id;
volatile uint8_t next_lfsr_digit = 0, new_seed = 0; // Holds next LFSR digit and seed flag

// Resets the LFSR to the new seed if available or to the default seed
void reset_lfsr_state(void)
{
    state_lfsr = new_state_lfsr; // Use new seed if available
}

// Advances the LFSR and updates the next digit
void next(void)
{
    uint16_t shifted_bit = state_lfsr & 0b1; // Get LSB for LFSR feedback
    state_lfsr >>= 1;                        // Shift LFSR right
    if (shifted_bit)
        state_lfsr ^= mask;        // Apply polynomial tap if LSB was 1
    next_lfsr_digit = state_lfsr & 0b11; // Extract the next two bits as the next digit
}
// Checks if the user's sequence matches the generated sequence
void is_sequence_confirmed(uint8_t tone_digit, uint8_t lfsr_digit)
{
    if (sequence_confirmed)
        sequence_confirmed = (tone_digit == lfsr_digit);
}
