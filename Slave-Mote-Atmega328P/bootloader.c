#include"bootloader.h"
#include <string.h>
#include <util/delay.h>


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

void timer_init(void)   //clear timer on compare match
{
    TCCR1A=0x00;        //CTC mode 4096 x 128 us ==600 msec
    TCCR1B=0x00;
    TIFR1 = (1<<OCF1A); //Clear OCF clear pending interrupts
    OCR1A =0x1000;  
    //TCNT1=0x0000;
    TCCR1B |= (1<<WGM12);
    
    
    TCCR1B |= (1<<CS12) | (1<<CS10); //start timer
}



void main(void)
{
    uint16_t FlashAddr=0x0000;          //rxx tx with respect to radio
    uint8_t tmpBufPg_ptr=0, pack_size=0, txPack_size=0, count=0, CRC_Byte=0x00; //src_id=0x00, lastACk=0x00
    char rxBuffer[32], txBuffer[32];
    
    radio_init(0x23);
    led_blink(0,10);
    timer_init();
    
    while(1)
    {
        
        if( (TIFR1 & (1<<OCF1A)) )    //T1 compare match //untill timeout call receive fn
        {   
            DDRB |= 0x01;
            PORTB ^= (1<<0);            //try blinking led indicating BL is running
            TIFR1 |= (1<<OCF1A);        //clear flag
        }
 
        if( RFreceive_packet(rxBuffer) == 0)      //returns NoError == 0 if packet is received
        {
            txBuffer[0] =ACKpack;
            switch(rxBuffer[ATByteNo])
            {
                /*case ATID:        //for ATCN connecting to a remote device
                    //word_addr = ( rxbuffer[lAddr_ByteNo] | (rxbuffer[hAddr_ByteNo] <<8) );   //absolute word address need access to
                    src_id =rxBuffer[srcid_ByteNo];
                    lastACK =srcid_Updt;
                    txBuffer[1] =srcid_Updt;    
                    txBuffer[2] =src_id;
                    txBuffer[3] =dst_id;
                    RFsend_packet(txBuffer, 1, src_id, 3);     //send acknowledgement packet that src id received, if connection received ackn
                    break;
                */

                case ATWR:      //just fill the temp page Buff
                    {
                        pack_size = ( (*rxBuffer) - 2);          //length in len_byte = addr + comm_byte + data= 2+pack_size  
                        unsigned int word_val=0x0000;
                        for( count =startofData; count <endofData; count +=2) 	//fill data to temporary Flash buffer
                        {
                            word_val = 0x0000;
                            word_val = *(rxBuffer + count);
                            word_val |= (*(rxBuffer + count + 1) << 8);
		                    boot_page_fill_safe( tmpBufPg_ptr, word_val );
                            tmpBufPg_ptr += 2;
                        }
    
                        txBuffer[1]=tmpBufPg_ptr;
                        txPack_size=2;
                    }
                    break;

                case ATFL:                  //write to flash need to follow up with 2 FlashAddr bytes
                    FlashAddr = ( (rxBuffer[FlashAddrh_ByteNo] <<8) + rxBuffer[FlashAddrl_ByteNo] );
                    boot_page_erase_safe(FlashAddr);                //erase one Flash page
  		            boot_page_write_safe(FlashAddr);                //write buffer to one Flash page
	    	        boot_rww_enable_safe();

                    FlashAddr += tmpBufPg_ptr;
                    txBuffer[1]=tmpBufPg_ptr;
                    tmpBufPg_ptr =0;
                    txBuffer[2] = (uint8_t)(FlashAddr & 0x00FF);
                    txBuffer[3] = (uint8_t)( (FlashAddr & 0xFF00) >>8);
                    txPack_size=4;
                    break;


                case ATRD:      //read what is written at the FlashAddr 64 bytes... calculate crc
                    FlashAddr=0x0000;
                    FlashAddr = ( (rxBuffer[FlashAddrh_ByteNo] <<8) + rxBuffer[FlashAddrl_ByteNo] );
                    for( count =0; count <16; count++)
                        txBuffer[count+1]=pgm_read_byte_near(FlashAddr++);

                    txBuffer[17] = (uint8_t)( (FlashAddr & 0xFF00) >>8);
                    txBuffer[18] = (uint8_t)(FlashAddr & 0x00FF);
                    txPack_size=19;
                    break;


                case ATVR:     // follow up with a FlashAddr byte + no of bytes to verify + crc byte
                {
                    uint8_t sum=0;

                    pack_size=rxBuffer[pack_size_ByteNo];
                    FlashAddr = ( rxBuffer[FlashAddrl_ByteNo] | (rxBuffer[FlashAddrh_ByteNo] <<8) );
                    CRC_Byte = rxBuffer[CRC_ByteNo];
                    for( count =0; count <pack_size; count++)
                         sum +=pgm_read_byte((prog_uchar*)(FlashAddr+count));
                    sum +=CRC_Byte;

                    if ( !(sum%255) )
                        txBuffer[1]=TRUE;       //send ack CRC_ok
                    else 
                        txBuffer[1]=FALSE;      //send redo ACK
                    txBuffer[2] = (uint8_t)(FlashAddr & 0x00FF);
                    txBuffer[3] = (uint8_t)( (FlashAddr & 0xFF00) >>8);
                    txPack_size=4;
                }
                    break;


                case ATST:        //tell if bootloader is running and send the last acknowledment sent
                    txBuffer[1] =tmpBufPg_ptr;          //btldr is running
                    txBuffer[2] = (uint8_t)(FlashAddr & 0x00FF);
                    txBuffer[3] = (uint8_t)( (FlashAddr & 0xFF00) >>8);
                    txPack_size=4;
                    break;

            
                case ATAP:     //jump to app
                    //(*((void(*)(void))APP_START))();
                    txPack_size=1;
                    RFsend_packet(txBuffer, 1, 0x12, txPack_size);
                    asm("jmp 0x0000");
                    //or create a WDT reset and wait for app to st 
                    break;
          
 
                default:
                    led_blink(1,5);
                    led_blink(0,5);
                    //lastACK =unknown_ATByte;
                    //txBuffer[1] =lastACK;
                    //RFsend_packet(txBuffer, 1, src_id, 2);
                    break;
            }
            RFsend_packet(txBuffer, 1, 0x12, txPack_size);
            led_blink(1,10);
            memset(rxBuffer,0x0000,sizeof(rxBuffer));           //reset every data bit---
            memset(txBuffer,0x0000,sizeof(txBuffer));           //clear rxBuffer and txBuffer
        }
        //else call function again
    }
}
