#include "radio_cc1101.h"

//RF settings for the radio------------------------
void radio_RFset(uint8_t ADDR)
{
	radio_write_reg(CCxxx0_IOCFG0,		0x25);
	radio_write_reg(CCxxx0_IOCFG2,		0x06);
	radio_write_reg(CCxxx0_PKTCTRL0,	0x01); radio_write_reg(CCxxx0_PKTCTRL1,0x03);//1=0x0B//address check 0,255 broadcast
						       radio_write_reg(CCxxx0_ADDR, ADDR);
	radio_write_reg(CCxxx0_FSCTRL1,		0x0C);//0=0x05
	radio_write_reg(CCxxx0_FREQ2,		0x10);
	radio_write_reg(CCxxx0_FREQ1,		0xB1);
	radio_write_reg(CCxxx0_FREQ0,		0x3B);
	radio_write_reg(CCxxx0_MDMCFG4,		0x2D);
	radio_write_reg(CCxxx0_MDMCFG3,		0x3B);
	radio_write_reg(CCxxx0_MDMCFG2,		0x13);
	radio_write_reg(CCxxx0_DEVIATN,		0x62);
	radio_write_reg(CCxxx0_MCSM0,		0x18);
	radio_write_reg(CCxxx0_MCSM1,		0x3C);		//3C for RX-RX, 30 for alws IDLE, 33 for TX-RX
	radio_write_reg(CCxxx0_MCSM2,		0x01);
	radio_write_reg(CCxxx0_FOCCFG,		0x1D);
	radio_write_reg(CCxxx0_BSCFG,		0x1C);
	radio_write_reg(CCxxx0_AGCCTRL2,	0xC7);
	radio_write_reg(CCxxx0_AGCCTRL1,	0x00);
	radio_write_reg(CCxxx0_AGCCTRL0,	0xB0);
	//radio_write_reg(CCxxx0_WORCTRL,		0x78);
	//radio_write_reg(CCxxx0_WOREVT1,		0x01);		//count =511, min required is 497 0x01F1
	//radio_write_reg(CCxxx0_WOREVT0,		0xFF);		
	radio_write_reg(CCxxx0_FREND1,		0xB6);
	radio_write_reg(CCxxx0_FREND0,		0x10);
	radio_write_reg(CCxxx0_FSCAL3,		0xEA);
	radio_write_reg(CCxxx0_FSCAL2,		0x2A);
	radio_write_reg(CCxxx0_FSCAL1,		0x00);
	radio_write_reg(CCxxx0_FSCAL0,		0x1F);
	//radio_write_reg(CCxxx0_TEST0,		0x09);
	radio_write_reg(CCxxx0_PKTLEN, 		32);  //check!!!!!!!
//patable FF
}


// manual reset-- pg 51/98  -------uses the radio-reset fn initial setting.----
/*void radio_man_powerON_reset(void)
{
	int i=0;

	SPIport |= (1<<SCLK);
 	SPIport &= ~(1<<MOSI);		//define CSN HIGH LOW MOSI SCLK in some h file or use set_pin clr_pin defines
	spi_radio_select();
	spi_radio_deselect();	for(i=0;i<1000;i++);	//wait for another 40 usec relative to low
	radio_reset();
}*/



//write to register function in single write then burst mode--------------------------
void radio_write_reg(uint8_t addr, uint8_t value)	
{
	spi_radio_select();			//CSn option at the MCU side--- get it to low--- always wait till it gets low
	spi_write_single(addr);		//wait for TXBUF to be empty--- load buff-- wait for it be empty again
	spi_write_single(value);	//wait for TXBUF to be empty--- load buff-- wait for it be empty again
	spi_radio_deselect();		//CSn option at the MCU side--- get it to high
}


void radio_write_reg_burst(char start_addr, char *value_array, uint8_t size)		
{
	uint8_t i=0;

	spi_radio_select();
	spi_write_single(start_addr | burst_bit);		//enable burst 1st time		
	for (i=0; i<size; i++)
		spi_write_single(value_array[i]);//put it in loop send data in burst for consecutive adressfor next 8 clock pulses
	spi_radio_deselect();		
}



//---------------------------------------------------------------------------
//read from register function in single write then burst mode------------------
uint8_t radio_read_reg(uint8_t addr)	
{
	char ret_val;
	
	spi_radio_select();
  	spi_write_single(addr | read_bit);	//reading is same as writing with readbit on, transmit this addrs
  	spi_write_single(0x00);
	ret_val = spi_read_single();		//ret_val should be global or val should be passed to calling fn
  	spi_radio_deselect();			//read single--should empty data from rx buffer, wait for it to be empty 
	return ret_val;				//return 8bit data value
}


void radio_read_reg_burst(char start_addr, char* ret_val, uint8_t size)		
{
	//char ret_val[size-1];						//array where read values are stored and returned
	uint8_t i=0;

	spi_radio_select();
	spi_write_single(start_addr | read_bit | burst_bit);		//burst mode read enable
	for (i=0;i<size;i++)
		{	
			spi_write_single(0x00);
			ret_val[i] = spi_read_single();		//keep storing values in char type array
		}
	spi_radio_deselect();
	//return ret_val;		
}



//sending strobe commands--------------------------------------------------------------------------
uint8_t radio_cmd_strobe(uint8_t strobe)
{
	uint8_t ret_val=0;

	spi_radio_select();
	spi_write_single(strobe);
	if (strobe >= (0x30 | read_bit | burst_bit))		//this is for commands and reading status bytes
	{
		spi_write_single(0x00);
		ret_val = spi_read_single();
	}
	spi_radio_deselect();
	
	return ret_val;
}
