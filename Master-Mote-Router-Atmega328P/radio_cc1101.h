/*-------------------------------------------Register definition file for CC1101-----------------------------------------------*/
//-------this has been copied entirely
// CC2500/CC1100 STROBE, CONTROL AND STATUS REGSITER--------------------------------  
//AUTHOR: Saurabh		DATE: 26th dec 2013


//file is reg_def_cc1101.h
#ifndef RADIO_CC1101_H
#define RADIO_CC1101_H


//includes--------------------
#include "spi.h"
//#include <inttypes.h>
//#include <stdint.h>
#include <stdbool.h>


//other definations----------------------------------------------------
#define burst_bit	0x40
#define read_bit	0x80

#define ON		0x01
#define OFF		0x00

typedef int error;


//prototype declarations----------------------
void radio_man_powerON_reset(void) __attribute__((section(".stack")));
void radio_write_reg(uint8_t, uint8_t) __attribute__((section(".stack")));
void radio_write_reg_burst(char, char*, uint8_t) __attribute__((section(".stack")));
uint8_t radio_read_reg(uint8_t) __attribute__((section(".stack")));
void radio_read_reg_burst(char, char*, uint8_t) __attribute__((section(".stack")));
uint8_t radio_cmd_strobe(uint8_t) __attribute__((section(".stack")));
void radio_RFset(uint8_t) __attribute__((section(".stack")));

uint8_t radio_curr_state(void) __attribute__((section(".stack")));
void radio_reset(void) __attribute__((section(".stack")));
void radio_init(uint8_t) __attribute__((section(".stack")));
error RFsend_packet(char*, bool, char, uint8_t) __attribute__((section(".stack")));		
error RFreceive_packet(char*) __attribute__((section(".stack")));


//cc1101 Configuration registers---------------------------------------
#define CCxxx0_IOCFG2       0x00        // GDO2 output pin configuration
#define CCxxx0_IOCFG1       0x01        // GDO1 output pin configuration
#define CCxxx0_IOCFG0       0x02        // GDO0 output pin configuration
#define CCxxx0_FIFOTHR      0x03        // RX FIFO and TX FIFO thresholds
#define CCxxx0_SYNC1        0x04        // Sync word, high byte
#define CCxxx0_SYNC0        0x05        // Sync word, low byte
#define CCxxx0_PKTLEN       0x06        // Packet length
#define CCxxx0_PKTCTRL1     0x07        // Packet automation control
#define CCxxx0_PKTCTRL0     0x08        // Packet automation control
#define CCxxx0_ADDR         0x09        // Device address
#define CCxxx0_CHANNR       0x0A        // Channel number
#define CCxxx0_FSCTRL1      0x0B        // Frequency synthesizer control
#define CCxxx0_FSCTRL0      0x0C        // Frequency synthesizer control
#define CCxxx0_FREQ2        0x0D        // Frequency control word, high byte
#define CCxxx0_FREQ1        0x0E        // Frequency control word, middle byte
#define CCxxx0_FREQ0        0x0F        // Frequency control word, low byte
#define CCxxx0_MDMCFG4      0x10        // Modem configuration
#define CCxxx0_MDMCFG3      0x11        // Modem configuration
#define CCxxx0_MDMCFG2      0x12        // Modem configuration
#define CCxxx0_MDMCFG1      0x13        // Modem configuration
#define CCxxx0_MDMCFG0      0x14        // Modem configuration
#define CCxxx0_DEVIATN      0x15        // Modem deviation setting
#define CCxxx0_MCSM2        0x16        // Main Radio Control State Machine configuration
#define CCxxx0_MCSM1        0x17        // Main Radio Control State Machine configuration
#define CCxxx0_MCSM0        0x18        // Main Radio Control State Machine configuration
#define CCxxx0_FOCCFG       0x19        // Frequency Offset Compensation configuration
#define CCxxx0_BSCFG        0x1A        // Bit Synchronization configuration
#define CCxxx0_AGCCTRL2     0x1B        // AGC control
#define CCxxx0_AGCCTRL1     0x1C        // AGC control
#define CCxxx0_AGCCTRL0     0x1D        // AGC control
#define CCxxx0_WOREVT1      0x1E        // High byte Event 0 timeout
#define CCxxx0_WOREVT0      0x1F        // Low byte Event 0 timeout
#define CCxxx0_WORCTRL      0x20        // Wake On Radio control
#define CCxxx0_FREND1       0x21        // Front end RX configuration
#define CCxxx0_FREND0       0x22        // Front end TX configuration
#define CCxxx0_FSCAL3       0x23        // Frequency synthesizer calibration
#define CCxxx0_FSCAL2       0x24        // Frequency synthesizer calibration
#define CCxxx0_FSCAL1       0x25        // Frequency synthesizer calibration
#define CCxxx0_FSCAL0       0x26        // Frequency synthesizer calibration
#define CCxxx0_RCCTRL1      0x27        // RC oscillator configuration
#define CCxxx0_RCCTRL0      0x28        // RC oscillator configuration
#define CCxxx0_FSTEST       0x29        // Frequency synthesizer calibration control
#define CCxxx0_PTEST        0x2A        // Production test
#define CCxxx0_AGCTEST      0x2B        // AGC test
#define CCxxx0_TEST2        0x2C        // Various test settings
#define CCxxx0_TEST1        0x2D        // Various test settings
#define CCxxx0_TEST0        0x2E        // Various test settings



// Strobe commands----------------------------------------------------------------------------------
#define CCxxx0_SRES         0x30        // Reset chip.
#define CCxxx0_SFSTXON      0x31        // Enable and calibrate frequency synthesizer (if MCSM0.FS_AUTOCAL=1).
                                        // If in RX/TX: Go to a wait state where only the synthesizer is
                                        // running (for quick RX / TX turnaround).
#define CCxxx0_SXOFF        0x32        // Turn off crystal oscillator.
#define CCxxx0_SCAL         0x33        // Calibrate frequency synthesizer and turn it off
                                        // (enables quick start).
#define CCxxx0_SRX          0x34        // Enable RX. Perform calibration first if coming from IDLE and
                                        // MCSM0.FS_AUTOCAL=1.
#define CCxxx0_STX          0x35        // In IDLE state: Enable TX. Perform calibration first if
                                        // MCSM0.FS_AUTOCAL=1. If in RX state and CCA is enabled:
                                        // Only go to TX if channel is clear.
#define CCxxx0_SIDLE        0x36        // Exit RX / TX, turn off frequency synthesizer and exit
                                        // Wake-On-Radio mode if applicable.
#define CCxxx0_SAFC         0x37        // Perform AFC adjustment of the frequency synthesizer
#define CCxxx0_SWOR         0x38        // Start automatic RX polling sequence (Wake-on-Radio)
#define CCxxx0_SPWD         0x39        // Enter power down mode when CSn goes high.
#define CCxxx0_SFRX         0x3A        // Flush the RX FIFO buffer.
#define CCxxx0_SFTX         0x3B        // Flush the TX FIFO buffer.
#define CCxxx0_SWORRST      0x3C        // Reset real time clock.
#define CCxxx0_SNOP         0x3D        // No operation. May be used to pad strobe commands to two
                                        // bytes for simpler software.

//Status Registers--------------------------------------
#define CCxxx0_PARTNUM          (0x30 | read_bit | burst_bit)
#define CCxxx0_VERSION          (0x31 | read_bit | burst_bit)
#define CCxxx0_FREQEST          (0x32 | read_bit | burst_bit)
#define CCxxx0_LQI              (0x33 | read_bit | burst_bit)
#define CCxxx0_RSSI             (0x34 | read_bit | burst_bit)
#define CCxxx0_MARCSTATE        (0x35 | read_bit | burst_bit)
#define CCxxx0_WORTIME1         (0x36 | read_bit | burst_bit)
#define CCxxx0_WORTIME0         (0x37 | read_bit | burst_bit)
#define CCxxx0_PKTSTATUS        (0x38 | read_bit | burst_bit)
#define CCxxx0_VCO_VC_DAC       (0x39 | read_bit | burst_bit)
#define CCxxx0_TXBYTES          (0x3A | read_bit | burst_bit)
#define CCxxx0_RXBYTES          (0x3B | read_bit | burst_bit)
#define CCxxx0_RCCTRL1_STATUS   (0x3C | read_bit | burst_bit)
#define CCxxx0_RCCTRL0_STATUS   (0x3D | read_bit | burst_bit)

#define CCxxx0_PATABLE          0x3E
#define CCxxx0_TXFIFO           0x3F	//both rx and tx belong to same adress but with R/W- bit MSB bit
#define CCxxx0_RXFIFO           0x3F


//state definations---------------------------------------------------------------------------------------
/*#define SLEEP		0x00 
#define IDLE 		0x01 
#define XOFF 		0x02 
#define VCOON_MC  	0x03  
#define REGON_MC 	0x04
#define MANCAL	 	0x05
#define VCOON 		0x06
#define FS_WAKEUP 	0x07 
#define STARTCAL 	0x08 
#define BWBOOST 	0x09
#define FS_LOCK 	0x0A 
#define IFADCON 	0x0B 
#define ENDCAL 		0x0C 
#define RX 		0x0D 
#define RX_END 		0x0E
#define RX_RST		0x0F 
#define TXRX_SWITCH 	0x10 
#define RXFIFO_OVERFLOW 0x11 
#define FSTXON 		0x12 
#define TX 		0x13 
#define TX_END 		0x14
#define RXTX_SWITCH  	0x15
#define TXFIFO_UNDERFLOW 0x16 
*/
#define IDLE 	 	0x00
#define	RX		0x01
#define TX		0x02
#define FSTXON 		0x03
#define CALIBRATE 	0x04
#define SETTLING  	0x05
#define RXFIFO_OVERFLOW 0x06
#define TXFIFO_UNDERFLOW 0x07 

//RFsettings--------------------------------------------------------------------------------CHANGE THIS TO CHANGE THE SETTINGS
#define SMARTRF_SETTING_IOCFG2       0x00        // GDO2 output pin configuration
#define SMARTRF_SETTING_IOCFG1       0x01        // GDO1 output pin configuration
#define SMARTRF_SETTING_IOCFG0       0x02        // GDO0 output pin configuration
#define SMARTRF_SETTING_FIFOTHR      0x03        // RX FIFO and TX FIFO thresholds
#define SMARTRF_SETTING_SYNC1        0x04        // Sync word, high byte
#define SMARTRF_SETTING_SYNC0        0x05        // Sync word, low byte
#define SMARTRF_SETTING_PKTLEN       0x06        // Packet length
#define SMARTRF_SETTING_PKTCTRL1     0x07        // Packet automation control
#define SMARTRF_SETTING_PKTCTRL0     0x08        // Packet automation control
#define SMARTRF_SETTING_ADDR         0x09        // Device address
#define SMARTRF_SETTING_CHANNR       0x0A        // Channel number
#define SMARTRF_SETTING_FSCTRL1      0x0B        // Frequency synthesizer control
#define SMARTRF_SETTING_FSCTRL0      0x0C        // Frequency synthesizer control
#define SMARTRF_SETTING_FREQ2        0x0D        // Frequency control word, high byte
#define SMARTRF_SETTING_FREQ1        0x0E        // Frequency control word, middle byte
#define SMARTRF_SETTING_FREQ0        0x0F        // Frequency control word, low byte
#define SMARTRF_SETTING_MDMCFG4      0x10        // Modem configuration
#define SMARTRF_SETTING_MDMCFG3      0x11        // Modem configuration
#define SMARTRF_SETTING_MDMCFG2      0x12        // Modem configuration
#define SMARTRF_SETTING_MDMCFG1      0x13        // Modem configuration
#define SMARTRF_SETTING_MDMCFG0      0x14        // Modem configuration
#define SMARTRF_SETTING_DEVIATN      0x15        // Modem deviation setting
#define SMARTRF_SETTING_MCSM2        0x16        // Main Radio Control State Machine configuration
#define SMARTRF_SETTING_MCSM1        0x17        // Main Radio Control State Machine configuration
#define SMARTRF_SETTING_MCSM0        0x18        // Main Radio Control State Machine configuration
#define SMARTRF_SETTING_FOCCFG       0x19        // Frequency Offset Compensation configuration
#define SMARTRF_SETTING_BSCFG        0x1A        // Bit Synchronization configuration
#define SMARTRF_SETTING_AGCCTRL2     0x1B        // AGC control
#define SMARTRF_SETTING_AGCCTRL1     0x1C        // AGC control
#define SMARTRF_SETTING_AGCCTRL0     0x1D        // AGC control
#define SMARTRF_SETTING_WOREVT1      0x1E        // High byte Event 0 timeout
#define SMARTRF_SETTING_WOREVT0      0x1F        // Low byte Event 0 timeout
#define SMARTRF_SETTING_WORCTRL      0x20        // Wake On Radio control
#define SMARTRF_SETTING_FREND1       0x21        // Front end RX configuration
#define SMARTRF_SETTING_FREND0       0x22        // Front end TX configuration
#define SMARTRF_SETTING_FSCAL3       0x23        // Frequency synthesizer calibration
#define SMARTRF_SETTING_FSCAL2       0x24        // Frequency synthesizer calibration
#define SMARTRF_SETTING_FSCAL1       0x25        // Frequency synthesizer calibration
#define SMARTRF_SETTING_FSCAL0       0x26        // Frequency synthesizer calibration
#define SMARTRF_SETTING_RCCTRL1      0x27        // RC oscillator configuration
#define SMARTRF_SETTING_RCCTRL0      0x28        // RC oscillator configuration
#define SMARTRF_SETTING_FSTEST       0x29        // Frequency synthesizer calibration control
#define SMARTRF_SETTING_PTEST        0x2A        // Production test
#define SMARTRF_SETTING_AGCTEST      0x2B        // AGC test
#define SMARTRF_SETTING_TEST2        0x2C        // Various test settings
#define SMARTRF_SETTING_TEST1        0x2D        // Various test settings
#define SMARTRF_SETTING_TEST0        0x2E        // Various test settings


//error definations---------------------------------------------------------------------------------
enum ERR{
NoError,		//no error, this is the return indicating no error
RxError,		//radio in RX state or RX is empty, try again later for transmit or recieve, or issue force transmit
			//RxCompleteError,reception of packet is complete, expecting a recieve call from MCU immediately
OvfUndError,		//error due to rx overflow or underflow
};

/*Extra Notes----	Write		Read
single byte		+0x00		+0x80
Burst			+0x40		+0xC0
*/

#endif//RADIO_CC1101_H
//end of file ------------------------------------------------------------
