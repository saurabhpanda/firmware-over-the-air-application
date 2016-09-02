//-------------------------prototype declarations------------------
#ifndef SPI_H
#define SPI_H

//includes--------------------
#include <inttypes.h>
//#include <stdint.h>
#include <avr/io.h>


void spi_write_single(char) __attribute__((section(".stack")));
char spi_read_single(void) __attribute__((section(".stack")));
void spi_init(void) __attribute__((section(".stack")));
void spi_radio_select(void) __attribute__((section(".stack")));
void spi_radio_deselect(void) __attribute__((section(".stack")));
//void spi_wait(uint8_t);




#define SPIport PORTB

#define CSn 	PB2
#define MOSI	PB3
#define MISO 	PB4
#define SCLK	PB5

#define GDO0	PB0
//#define GDO1	PB1
//#define GDO2	PB6

#endif



