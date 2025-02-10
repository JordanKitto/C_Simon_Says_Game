#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

void pb_debounce(void);

extern volatile uint8_t pb_debounced_state;
extern volatile uint8_t updating_playback_delay;
extern volatile uint16_t time_passed;
extern volatile uint16_t new_playback_duration;
extern volatile uint16_t playback_duration;
extern volatile uint8_t pb_state;

#endif // TIMER_H
