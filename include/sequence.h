#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <stdint.h>

void next(void);
void reset_lfsr_state(void);
void update_seed(uint32_t seed);

extern volatile uint32_t new_state_lfsr;
extern volatile uint32_t start_state_lfsr;
extern volatile uint32_t state_lfsr;
extern volatile uint8_t next_lfsr_digit;
extern volatile uint8_t new_seed;
extern volatile uint32_t sequence_confirmed;

#endif // SEQUENCE_H
