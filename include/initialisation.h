#ifndef INITIALISATION_H
#define INITIALISATION_H

#include <stdint.h>

void button_init(void);
void port_init(void);
void pwm_init(void);
void spi_init(void);
void timer_init(void);
void adc_init(void);

extern volatile uint16_t length_sequence;
extern volatile uint8_t player_input_tracker;

#endif // INITIALISATION_H
