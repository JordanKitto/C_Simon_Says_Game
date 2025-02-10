#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "timer.h"
#include "buzzer.h"
#include "sequence.h"
#include "types.h"

volatile uint8_t reset = 0;