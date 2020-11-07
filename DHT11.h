/*
 * DHT11.h
 *
 * Created: 14/10/2020 21:10:01
 *  Author: Alexey Ivanov
 * Driver for DHT11 v1.0
 *
 * NOTA!
 * 1. Timer 0 is used for calculation of communication timeout
 * 2. no checksum verification
 */ 


#ifndef DHT11_H_
#define DHT11_H_

//#define DHT11_TEST

#ifndef F_CPU
#define F_CPU 16000000UL
#endif


#include <avr/io.h>
#include <util/delay.h>

//Timer 0 is used for calculations
//define prescale for timer 0
#define DHT11_SETTPRESCALE	TCCR0B|=(1<<CS01)|(1<<CS00)
#define DHT11_TCNT	TCNT0

//Timer tick, us = 1/F_CPU*1.000.000*TIMER_PRESCALE
//For F_CPU 16MHz and PreScale 64, Timer increment every 4 us
#define DHT11_T0_INC	4


#define DHT11_DDR		DDRC		//port direction
#define DHT11_PORT		PORTC		//port for writing
#define DHT11_PIN		PINC		//port for reading
#define DHT11_PINX		PINC0		//pin for communication



/************************************************************************/
/*  Initialize PIN for communication                                    */
/************************************************************************/
void DHT11_Init();

/************************************************************************/
/* Read Sensor Value
cHumInt - integer part of humidity
cHumDec - decimal part of humidity
cTempInt - integer part of Temperature
cTempDec - decimal part of Temperature

for example, final value fo humidity will be cHumInt.cHumDec            */
/************************************************************************/
void DHT11_GetData(char* cHumInt, char* cHumDec, char* cTempInt, char* cTempDec);

static char DHT11_GetBit();



#endif /* DHT11_H_ */