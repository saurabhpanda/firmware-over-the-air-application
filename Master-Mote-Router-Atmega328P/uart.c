#include "uart.h"

void uart_init(void)
{
    //UART settings**************
    UBRR0L = (uint8_t)(F_CPU/(BAUD_RATE*16L)-1);
    //UBRR0H = (F_CPU/(BAUD_RATE*16L)-1) >> 8;     // always 0
    UCSR0B = (1<<RXEN0) | (1<<TXEN0);
    UCSR0C = (1<<UCSZ00) | (1<<UCSZ01);
    /* Enable internal pull-up resistor on pin D0 (RX), in order
    to supress line noise that prevents the bootloader from
    timing out*/
    DDRD &= ~_BV(PIND0);
    PORTD |= _BV(PIND0);
}


uint8_t rxUart_single(void)
{
    char x;
    uint8_t y;

    //UCSR0B = (1<<RXEN0);
    while(!(UCSR0A & _BV(RXC0)));
    x = UDR0;
    y = (uint8_t)x;
    //UCSR0B = 0x00;
    return y;
}

void rxUart_burst(uint8_t* uartBuffer, uint8_t size)
{
    while(size--)
        *(uartBuffer++) =rxUart_single();
}

void txUart_single(uint8_t data)
{
    //UCSR0B = (1<<TXEN0);
    while(!(UCSR0A & (1<<UDRE0)));
    UDR0 = data;
    //UCSR0B = 0x00;
} 

void txUart_burst(uint8_t *uartBuffer, uint8_t size)
{
    while(size--)
        txUart_single(*(uartBuffer++));
}


void echo(uint8_t size)
{
    uint8_t buffer[32];
    rxUart_burst(buffer, size);
    
    txUart_burst(buffer, size);   
}

/*void main(void)
{
    uint8_t size;

    uart_init();
    
    //char test[]=" asdf123 ", receive;
    while(1)
    {
        size = rxUart_single();
        txUart_single(size);
        echo(size);
    }
        //txUart_burst(test, sizeof(test));
    //receive= rxUart_single();
}*/


