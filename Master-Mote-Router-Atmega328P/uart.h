#ifndef UART_H
#define UART_H

#include <avr/io.h>
#include <inttypes.h>

#define F_CPU 8000000
#define BAUD_RATE 9600

void uart_init(void);
uint8_t rxUart_single(void);
void rxUart_burst(uint8_t*, uint8_t);
void txUart_single(uint8_t);
void txUart_burst(uint8_t*, uint8_t);
void echo(uint8_t);

#endif
