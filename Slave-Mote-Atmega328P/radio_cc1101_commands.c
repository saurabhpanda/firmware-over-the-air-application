#include "radio_cc1101.h"

///// added later for prasann custom............................................................................


//knowing current state of the radio-----------------------------------------------------------------------------
uint8_t radio_curr_state(void)
{
	radio_cmd_strobe(CCxxx0_SNOP);
	return ((spi_read_single() & 0x70)>>4);		// simply read those bytes necessary from CSB
}

//-------------------------------
void radio_reset(void)
{
	int i=0;
	
	radio_cmd_strobe(CCxxx0_SRES);
	for(i=0;i<1000;i++);		//not required if manual power on reset done, make sure u enable automatic reset
	//	for (j=0;j<1000;j++);
}


//--------------------------------------
void radio_init(uint8_t ADDR)
{
	int i;
	
	for(i=0;i<1000;i++);		//starting delay
	//radio_man_powerON_reset();
	spi_init();
	//radio_reset();
	radio_RFset(ADDR);
	for(i=0;i<500;i++);		// delay before sending or recieving a packet
	//radio_cmd_strobe(CCxxx0_SIDLE);
	radio_cmd_strobe(CCxxx0_SRX);	//for prasann
	for(i=0;i<500;i++);
}
		

//-----------------------------
//-----------------------------------------------------------------------------
error RFsend_packet(char *txBuffer, bool force_trans, char addr, uint8_t size)		//txbuffer and rx buffer are global		
{
	uint8_t prev_state=IDLE;	  
	while(!((prev_state == TX) && (radio_curr_state() == IDLE))) 
	{
		switch(radio_curr_state())
		{
			case RX:
				if (force_trans == ON)
				{
					radio_cmd_strobe(CCxxx0_SFRX);
					radio_cmd_strobe(CCxxx0_SIDLE);
					while(radio_curr_state() != IDLE);
				}
				else 
					return RxError;
				break;

			case RXFIFO_OVERFLOW:
				radio_cmd_strobe(CCxxx0_SFRX);
				radio_cmd_strobe(CCxxx0_SIDLE);
				while(radio_curr_state() != IDLE);
				
				break;
			
			case TXFIFO_UNDERFLOW:			//only need if u r not flushing TXFIFO always or using SWOR strobe
				radio_cmd_strobe(CCxxx0_SFTX);
				prev_state = TXFIFO_UNDERFLOW;
				break;

			case TX:
				prev_state=TX;
				break;
			
			case CALIBRATE:
				; 		//wait for calibration to be over
				break;
										
			case IDLE:
			 	if(prev_state != TX)
				{
					radio_write_reg(CCxxx0_TXFIFO, (size+1));
					radio_write_reg(CCxxx0_TXFIFO, addr);
					radio_write_reg_burst(CCxxx0_TXFIFO, txBuffer, size);// ADD.DATA + lenbyte-- load tx fifo data
					radio_cmd_strobe(CCxxx0_STX);
				}
				// sync transmitted, wait till it goes high
				while((radio_cmd_strobe(CCxxx0_PKTSTATUS) & 0x04) == 0x00);
				// Wait for GDO0 to be cleared -> end of packet
				while((radio_cmd_strobe(CCxxx0_PKTSTATUS) & 0x04) == 0x04);
				prev_state = TX;					//after TX it goes to IDLE state
				break;
			
			default:
				SPIport &= ~(1<<CSn);		//force CSn low to go to IDLE state (here RX state)
				break;
		}
	}
	//radio_cmd_strobe(CCxxx0_SWOR);				// or flush TX data if you want
	radio_cmd_strobe(CCxxx0_SRX);	///// only for prasann stay in RX always
	while(radio_curr_state() != RX);
	return NoError;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
error RFreceive_packet(char *rxBuffer)	//------ returns packet lenth if packet extracted                                                                                                //variable packet length mode 1st byte after sync is len, retrn pkt len
{							//address check on no broadcast, max-pktlen= 64-3
   	uint8_t packet_len=0, ret_val=0;
	 
	if (radio_cmd_strobe(CCxxx0_RXBYTES) > 0x00)
	{
		packet_len = radio_read_reg(CCxxx0_RXFIFO);
		while (radio_cmd_strobe(CCxxx0_RXBYTES) < packet_len);		//packet not completely recieved, wait;
		*rxBuffer = packet_len;
		radio_read_reg_burst(CCxxx0_RXFIFO, (++rxBuffer), packet_len);
		ret_val = NoError;
		
		if (radio_curr_state() == RXFIFO_OVERFLOW)
		{
			radio_cmd_strobe(CCxxx0_SFRX);
			while (radio_curr_state() != IDLE);
			radio_cmd_strobe(CCxxx0_SRX);
			while (radio_curr_state() != RX);
			ret_val = OvfUndError;
		}
		
	}
	else ret_val = RxError;
	
	return ret_val;

	/*uint8_t packet_len=0; //addr, i=0, count=0;	//using this will make microcontroller continously busy checking for a packet
	
	radio_cmd_strobe(CCxxx0_SRX);		
	while((radio_cmd_strobe(CCxxx0_PKTSTATUS) & 0x04) == 0x00);
	while((radio_cmd_strobe(CCxxx0_PKTSTATUS) & 0x04) == 0x04);
	//count = radio_cmd_strobe(CCxxx0_RXBYTES);
	packet_len = radio_read_reg(CCxxx0_RXFIFO);
	//count = radio_cmd_strobe(CCxxx0_RXBYTES);
	//i=radio_cmd_strobe(CCxxx0_SNOP | read_bit);
	radio_read_reg_burst(CCxxx0_RXFIFO, rxBuffer, (packet_len));
	radio_cmd_strobe(CCxxx0_SWOR);
	
	return 0;
	*/
}

