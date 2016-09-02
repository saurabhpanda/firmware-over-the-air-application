//prototype declarations
#include<avr/io.h>
#include<inttypes.h>
#include<avr/pgmspace.h>
#include<avr/boot.h>

#include "radio_cc1101.h"


//general definations
#define ACKpack     0x55
#define dst_id      0x23 
#define TRUE        1
#define FALSE       0


//AT types
#define ATID        0x70
#define ATWR        0x74        //for 't'
#define ATFL        0x66        //'f'
#define ATRD        0x72        //'r'
#define ATVR        0x76        //'v'
#define ATST        0x73        //s
#define ATAP        0x6A        //j


//rxBuffer byte positions
#define packLen_ByteNo      0               // includes addr + pack_len byte
#define destID_byteNo       1               //
#define ATByteNo            2               //1-pack_len, 2 is 
//for ATID
//#define srcID_ByteNo        3             //not using now mayb later              
//for ATWR
#define startofData         3               //byte no from where data bytes start... only incase of buffer write... 16 bytes of dat
#define endofData           ( (startofData + pack_size) -1)              //18 if 16 bytes of data
//for ATFL
#define FlashAddrl_ByteNo   4
#define FlashAddrh_ByteNo   3
//for ATRD
//for ATVR
#define pack_size_ByteNo    5
#define CRC_ByteNo          6          


/*ackn types follows ACKpack type byte immediately
#define srcID_Updt          0x70
#define tmpBufPg_Updt       0x74
#define Flash_Updt          0x66
#define Read_Val            0x72
#define CRC_ok              0x76
#define CRC_error           0x15
#define app_jmp             0x16
#define unknown_ATByte      0x17    */
