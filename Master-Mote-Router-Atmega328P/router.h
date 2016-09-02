#ifndef ROUTER_H
#define ROUTER_H

#include <avr/io.h>
#include "radio_cc1101.h"
#include "uart.h"
#include <util/delay.h>

void txRadioPack(char*, char, uint8_t);
void txUartPack(char*, char*);        
void timer_init(void);



#define ATWR    0x74    //t
#define ATFL    0x66    //f
#define ATRD    0x72    //r
#define ATVR    0x76    //v
#define ATST    0x73    //s
#define ATAP    0x6A    //j


#define timeout_error   0x01
#define NoError     0x00

#endif
