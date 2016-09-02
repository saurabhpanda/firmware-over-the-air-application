////----------------------------------- SPI port is portB----------------------
#include "spi.h"



/*//only use it while using any strobe, checking status
uint8_t spi_write_single(char byte)
{
	uint8_t chip_status_byte;
	
	SPDR = byte;           		// Start transmission, writing initiates transmission
    	while(!(SPSR & (1<<SPIF)));     // Wait for transmission complete 
	chip_status_byte = SPSR; chip_status_byte = SPDR;	// read spsr (with spif set) then spdr to clear the flag
	return chip_status_byte;  
} 
*/

//simple write to spi
void spi_write_single(char byte)
{
	uint8_t garbage=0x00;

	garbage = SPSR; 
	garbage = SPDR;	// read spsr (with spif set) then spdr to clear the flag  
	SPDR = byte;           		// Start transmission, writing initiates transmission
    	while(!(SPSR & (1<<SPIF)));     // Wait for transmission complete 
} 




char spi_read_single(void)
{
	return SPDR;           	// reading data val 
} 



void spi_init(void)		
{
	DDRB = 0x00;	DDRB |= ((1<<CSn) | (1<<MOSI) | (1<<SCLK));			//chip select is output
	SPCR = 0x00;	SPCR |= ((1<<SPE) | (1<<MSTR) | (1<<SPR0)); 	SPSR |= 0x01;// fosc/8, SCK=0 when idle, enable spi oper
	PORTB |=(1<<CSn);		//start high with chip sel=> no select
}



void spi_radio_select(void)
{
	SPIport &= ~(1<<CSn);
	while((PINB & 0x10) == 0x10);				//wait for it to get low
}



void spi_radio_deselect(void)
{
	SPIport |= (1<<CSn);
	//while(!(SPIport & MISO));			//wait until it goes high
}
