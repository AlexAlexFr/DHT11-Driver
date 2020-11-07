/*
 * DHT11.c
 *
 * Created: 14/10/2020 21:10:21
*  Author: Alexey Ivanov
* Driver for DHT11 v1.0
*
* NOTA!
* 1. Timer 0 is used for calculation of communication timeout
* 2. no checksum verification
 */ 

#include "DHT11.h"

void DHT11_Init()
{
	DHT11_DDR |= (1<<DHT11_PINX);
	DHT11_PORT |= (1<<DHT11_PINX);
	
	DHT11_SETTPRESCALE;
	
	for (char c=0; c<8;c++)
	{
		_delay_ms(250);	
	}
	
}

void DHT11_GetData(char* cHumInt, char* cHumDec, char* cTempInt, char* cTempDec)
{
	unsigned char acResult[5];
	
	for (char i=0;i<5;i++)
	{
		acResult[i] = 0x00;
	}
	
	
	DHT11_PORT &= ~(1<<DHT11_PINX);
	_delay_ms(20);
	
	DHT11_DDR &= ~(1<<DHT11_PINX);
	DHT11_PORT &= ~(1<<DHT11_PINX);

	///////////////////////////////////
	//After setting PINX to HIGH and wait.
	//DHT11 must set line to low within 20-40 us, check timer overrun for 60 us
	//////////////////////////////////
	
	#ifdef DHT11_TEST
	PORTC = (1<<PINC2);
	#endif
	
	DHT11_TCNT = 0;	
	//Set Timer value to  after 60 us

	while (((DHT11_PIN & (1<<DHT11_PINX))>0) && (DHT11_TCNT < 60 / DHT11_T0_INC));
	
	if (DHT11_TCNT >= 60 / DHT11_T0_INC - 1)
	{
		
		/*************************************************
		!!!!!!   Do Something, DHT11 didn't answer !!!!!!!
		**************************************************/
		#ifdef DHT11_TEST
		PORTC |= (1<<PINC1);
		#endif
	}
	

	
	//////////////////////////////////
	//DHT must set Line to low for 80 us, check if timer overrun 100 us
	/////////////////////////////////
	
	DHT11_TCNT = 0;
	while (((DHT11_PIN & (1<<DHT11_PINX))==0) && (DHT11_TCNT < 100/DHT11_T0_INC));
	
	if (DHT11_TCNT >= 100 / DHT11_T0_INC - 1 )
	{
		/**************************************************
		!!!!!!   Do Something, DHT11 didn't answer  !!!!!!!
		***************************************************/
		#ifdef DHT11_TEST
		PORTC |= (1<<PINC1);
		#endif
	}
	

	//////////////////////////////////
	//DHT must set Line to high for 80 us, check if timer overrun 100 us
	/////////////////////////////////
	
	DHT11_TCNT = 0;
	while (((DHT11_PIN & (1<<DHT11_PINX))>0) && (DHT11_TCNT < 100/DHT11_T0_INC));
	
	if (DHT11_TCNT >= 100 / DHT11_T0_INC - 1 )
	{
		
		/**************************************************
		!!!!!!   Do Something, DHT11 didn't answer  !!!!!!!
		***************************************************/		
		#ifdef DHT11_TEST
		PORTC |= (1<<PINC1);
		#endif
	}
	
	///////////////////////////////////////////
	// start of data transmission 40 bits:
	// 8 bits for Humidity Int (Hint)
	// 8 bits for Humidity Dec (Hdec)
	// 8 bits for Temperature Int (Tint)
	// 8 bits for Temperature Dec (Tdec)
	// 8 bits for checksum = last 8 bits of Hint+Hdec+Tint+Tdec
	//////////////////////////////////////////
	
	
	
	//get 5 bytes of 8 bits
	for (char count=0; count<5; count++)
	{
		for (char bitcount=0;bitcount<8; bitcount++)
		{
			acResult[count] = acResult[count]<<1;
			acResult[count] += DHT11_GetBit();
		}
	}
	
	//////////NO CHECKSUM VERIFICATION	
	
	*cHumInt = acResult[0];
	*cHumDec = acResult[1];
	*cTempInt = acResult[2];
	*cTempDec = acResult[3];
	
	_delay_ms(5);	
	
	//Put line to output
	DHT11_DDR |= (1<<DHT11_PINX);
	DHT11_PORT |= (1<<DHT11_PINX);
	
}

static char DHT11_GetBit()
{
	char cResult = 0xFF;
	
		//////////////////////////////////
		//DHT set Line to low for 50 us, check if timer overrun 64 us
		/////////////////////////////////
		
		DHT11_TCNT = 0;
		while (((DHT11_PIN & (1<<DHT11_PINX))==0) && (DHT11_TCNT < 64/DHT11_T0_INC));
		
		if (DHT11_TCNT >= 64 / DHT11_T0_INC - 1 )
		{
			
			/****************************************
			!!! DHT puts line low for more then 64 us!!!
			*****************************************/
			
			//Do Something
			#ifdef DHT11_TEST
			PORTC |= (1<<PINC1);
			#endif
		}
		
		//////////////////////////////
		//DHT set Line to high for 28 us for 0 or 70 us for 1
		/////////////////////////////
		DHT11_TCNT = 0;
		while (((DHT11_PIN & (1<<DHT11_PINX))>0) && (DHT11_TCNT < 100/DHT11_T0_INC));
		
		
		//IF timer less then
		if (DHT11_TCNT < 52 / DHT11_T0_INC)
		{
			return 0;
		}
		else if (DHT11_TCNT <= 100 / DHT11_T0_INC - 1)
		{
			return 1;
		}
		
		#ifdef DHT11_TEST
		PORTC |= (1<<PINC1);
		#endif
				
	return cResult;
}