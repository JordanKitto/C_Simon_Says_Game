#include "initialisation.h"
#include <avr/io.h>

// Initialize button inputs with pull-up resistors
void button_init(void)
{
    // Enable pull-up resistors for pushbuttons connected to PORTA pins 4 to 7
    PORTA.PIN4CTRL = PORT_PULLUPEN_bm; // S1
    PORTA.PIN5CTRL = PORT_PULLUPEN_bm; // S2
    PORTA.PIN6CTRL = PORT_PULLUPEN_bm; // S3
    PORTA.PIN7CTRL = PORT_PULLUPEN_bm; // S4
}

// Configure PORTB for output functions, such as the buzzer and USART0 TXD
void port_init(void)
{
    // Set the direction of PORTB pins to output for the buzzer and USART0 TX
    PORTB.DIRSET = PIN0_bm | PIN2_bm; // Buzzer on PIN0, USART0 TXD on PIN2
}

void pwm_init(void)
{
    // Enable output override on PB0.
    TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_SINGLESLOPE_gc | TCA_SINGLE_CMP0EN_bm;

    // PWM initially OFF
    TCA0.SINGLE.PER = 1;
    TCA0.SINGLE.CMP0 = 0;

    // Enable TCA0
    TCA0.SINGLE.CTRLA = TCA_SINGLE_ENABLE_bm;
}

// Initialize SPI interface for 7-segment display communication
void spi_init(void)
{
    // Configure Display Latch and SPI interface pins
    PORTA.OUTCLR = PIN1_bm;           // Clear the latch pin to prepare for SPI transaction
    PORTA.DIRSET = PIN1_bm;           // Set display latch (PA1) as output
    PORTC.DIRSET = PIN0_bm | PIN2_bm; // Set SPI Clock (PC0) and MOSI (PC2) as outputs

    // Configure SPI settings for Master mode and enable SPI
    PORTMUX.SPIROUTEA = PORTMUX_SPI0_ALT1_gc;   // Use alternate pin configuration for SPI
    SPI0.CTRLB = SPI_SSD_bm;                    // Disable Slave Select
    SPI0.INTCTRL = SPI_IE_bm;                   // Enable SPI interrupts
    SPI0.CTRLA = SPI_MASTER_bm | SPI_ENABLE_bm; // Set as master and enable SPI
}

void timer_init(void)
{
    // 1ms interrupt for elapsed time
    TCB0.CCMP = 3333;
    TCB0.INTCTRL = TCB_CAPT_bm;
    TCB0.CTRLA = TCB_ENABLE_bm;

    // 5ms interrupt for pushbutton sampling
    TCB1.CCMP = 16667;
    TCB1.INTCTRL = TCB_CAPT_bm;
    TCB1.CTRLA = TCB_ENABLE_bm;
}

// Initialize ADC for reading analog inputs such as potentiometers
void adc_init(void)
{
    ADC0.CTRLA = ADC_ENABLE_bm;                              // Enable ADC
    ADC0.CTRLB = ADC_PRESC_DIV2_gc;                          // Set prescaler to divide by 2
    ADC0.CTRLC = (4 << ADC_TIMEBASE_gp) | ADC_REFSEL_VDD_gc; // Configure time base and VDD as reference
    ADC0.CTRLE = 64;                                         // Set sample length
    ADC0.CTRLF = ADC_FREERUN_bm | ADC_LEFTADJ_bm;            // Set free-running mode and left adjust result

    ADC0.MUXPOS = ADC_MUXPOS_AIN2_gc;                                // Select AIN2 as the positive input
    ADC0.COMMAND = ADC_MODE_SINGLE_8BIT_gc | ADC_START_IMMEDIATE_gc; // Configure for 8-bit single-ended conversion and start immediately
}

void uart_init(void)
{
    // 9600 baud
    USART0.BAUD = 1389;

    // Enable receive complete interrupt
    USART0.CTRLA = USART_RXCIE_bm;
    USART0.CTRLB = USART_RXEN_bm | USART_TXEN_bm;
}
