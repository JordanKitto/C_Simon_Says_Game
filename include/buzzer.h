#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>

void buzzer_off(void);
void buzzer_on(uint8_t tone);
void reset_frequency(void);

#endif // BUZZER_H
