#include "router.h"
#include <util/delay.h>

//tx and rxBuffer are with respect to radio............
void led_blink(uint8_t pinNo, uint8_t count)
{
    uint8_t i=0;
    
    DDRB |= (1<<pinNo);
    for(i=0; i<=count; i++)
    {
        
        PORTB ^= (1<<pinNo);
        _delay_ms(100);
    }
    DDRB &= ~(1<<pinNo);
}

void txRadioPack(char *txBuffer, char first_byte, uint8_t size ) 
{
    *txBuffer = first_byte;
    //txUart_single(first_byte);
    rxUart_burst( (txBuffer+1), size);
    RFsend_packet(txBuffer, 1, 0x23, (size+1));
}


void txUartPack(char *rxBuffer, char *txBuffer)
{
    TCCR1B |= (1<<CS12) | (1<<CS10); //start timer
    while( !(TIFR1 & (1<<OCF1A)) )    //T1 compare match untill timeout call receive fn--- if timeout=>nothing received
    {
        if ( (RFreceive_packet(rxBuffer) == 0) )  //if received correct ack type (first byte)
        {
            TCCR1B &= ~( (1<<CS12) | (1<<CS10) );   TCNT1=0x0000;//stop timer
            txUart_burst( (rxBuffer+2), ((*rxBuffer)-1) );   //send total packet to uart, pack + addr = length
            return;
        }
    }
    TCCR1B &= ~( (1<<CS12) | (1<<CS10) );      TCNT1=0x0000;   //stop timer
    TIFR1 |= (1<<OCF1A);    //clear flag
    //txUart_single( timeout_error );     //send timeout error ack to uart to signal timeout, OR the PC waits for sometime then quits
}


void timer_init(void)   //clear timer on compare match
{
    TCCR1A=0x00;
    TCCR1B=0x00;
    TIFR1 = (1<<OCF1A); //Clear OCF clear pending interrupts
    OCR1A = 0x3FF;          //130 ms of delay
    //TCCR1A = (1<<COM1A1);     not needed as we only need timer
    TCCR1B = (1<<WGM12);//CTC, Timer clock =system clock/1024 
}


void main(void)
{
    char txBuffer[32], rxBuffer[32];//with respect to radio
    uint8_t command_byte, pack_size;
    
    //initialize
    led_blink(1,5);        //----
    uart_init();
    timer_init();
    //timer1_init();
    radio_init(0x12);
    
    

    while(1)
    {
        //check UART receive, command
        command_byte = rxUart_single();     //echo command byte and pack size to the PC to receive next....
        txUart_single(command_byte); 

        switch(command_byte)
        {
            case ATWR:
                //check no of bytes to be received.....
                pack_size = rxUart_single();
                txUart_single(pack_size);

                txRadioPack(txBuffer,command_byte,pack_size);       //recieve 16 uart data bytes,transmit it to
                txUartPack(rxBuffer,txBuffer);            //remove 16 after testing....
                led_blink(0,10);
                break;

            case ATFL:
                txRadioPack( txBuffer,command_byte,2);     //flash command + 2 flashadress
                txUartPack(rxBuffer,txBuffer);
                led_blink(0,10);
                break;

            case ATRD:
                txRadioPack( txBuffer,ATRD,2);     //read 16bytes from flash address provided 
                txUartPack(rxBuffer,txBuffer);
                led_blink(0,10);
                break; 

            case ATVR:
                txRadioPack( txBuffer,ATVR,4);      //1crc + 1no of bytes to be verified + 2start Flash Addr
                txUartPack(rxBuffer,txBuffer);      //16 data+ 1CRC + 2Flash Addr
                led_blink(0,10);
                break;
      
            case ATST:
                txRadioPack( txBuffer,ATST,0);     //read 16bytes from flash address provided 
                txUartPack(rxBuffer,txBuffer);               //return prev ACK temp buff ptr position current flash addr
                led_blink(0,10);
                break;
        
            case ATAP:
                txRadioPack( txBuffer,ATAP,0);     //jump to APP 
                txUartPack(rxBuffer,txBuffer);
                led_blink(0,10);               
                break;
            
            default:
                led_blink(0,1);
                led_blink(1,1);
                break;   
        }
    }
}

