/*******************************************************************************************************
*   TASK :
*	
*	1.	Display RTC using I2C communication.
*	2.	Use LCD for displaying date and time
*	3.	Use software implementation of I2C
*
*	Date :	02/11/2021	02:08:15 PM
*
*
*	PIN CONFIGURATIONS :
*
*	LCD pins :
*
*	PB0	-	DATA0
*	PB1	-	DATA1
*	PB2	-	DATA2
*	PB3	-	DATA3
*	PB4	-	DATA4
*	PB5	-	DATA5
*	PB6	-	DATA6
*	PB7	-	DATA7
*
*	PD4	-	RS pin
*	PD5	-	RW pin
*	PD6	-	EN pin
*
*	LED pins :
*
*	PC0	-	LED1
*	PC1	-	LED2
*	PC2	-	LED3
*	PC3	-	LED4
*	PC4	-	LED5
*	PC5	-	LED6
*	PC6	-	LED7
*	PC7	-	LED8
*
*	Buzzer pin : PD3
*
*	I2C pins :
*
*	PD0	-	SCL pin
*	PD1	-	SDA pin
*
********************************************************************************************************
											 HEADER FILES										
*******************************************************************************************************/

#include "func.h"

/*******************************************************************************************************
									   GLOBAL VARIABLES AND ISRs									
*******************************************************************************************************/

RTC_i2c rtc;

ISR( INT0_vect )
{
	RTC_set_time(rtc);	
}

/*******************************************************************************************************
											 MAIN FUNCTION										
*******************************************************************************************************/

int main(void)
{
	initialize_modules();		//Initializes GPIO pins, button, 7segment, lcd and I2C interface

	while (1)
	{	
		RTC_get_time(&rtc);		//Gets time from RTC registers
		time_display( rtc );	//Shows time and date in LCD and 7 segment display
	}	

	return EXIT_SUCCESS;
}

/********************************************************************************************************/
