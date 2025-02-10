#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <avr/io.h>

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define PB1 PIN4_bm //s1
#define PB2 PIN5_bm //s2
#define PB3 PIN6_bm //s3
#define PB4 PIN7_bm //s4

#define DISP_1 0
#define DISP_2 1
#define DISP_3 2
#define DISP_4 3

#define TONE_1 0
#define TONE_2 1
#define TONE_3 2
#define TONE_4 3

void clear_display(void);
void spi_init(void);
void spi_write(uint8_t b);
void display_score(uint32_t score);
void display_digit(uint8_t sequence_digit);
void show_defeat(void);
void show_victory(void);

extern volatile uint8_t number_segs[10];
extern volatile uint8_t segs[2];

void is_sequence_confirmed(uint8_t tone_digit, uint8_t lfsr_digit);

#endif // DISPLAY_H
