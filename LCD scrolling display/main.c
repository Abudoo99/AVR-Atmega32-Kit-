/*******************************************************************************************************
*   TASK :
*
* 	1. Print HELLO WORLD in LCD
*	2. Scroll string in LCD
*	
*
********************************************************************************************************
					   HEADER FILES										
*******************************************************************************************************/

#include "main.h"
#include "lcd.h"

/*******************************************************************************************************
					   MAIN FUNCTION										
*******************************************************************************************************/

int main(void)
{
	DDRB = SET_ALL ;			//LCD data line output direction			
	DDRD = LCD_CTRL_ENABLE;		//RS, RW, and EN set as output
	lcd_init();	

	lcd_scroll("HELLO WORLD");

	return EXIT_SUCCESS;
}

/*******************************************************************************************************
					FUNCTION DEFINITIONS 					
*******************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------
	Function generates time delay with given user time ( in ms )
----------------------------------------------------------------------------------------------------------
*   
*   Function Name : timer1_delay_ms
*
*   Parameters : long int delay_ms	-	Delay to generate ( in ms )
*
*   Return     : NONE
*-------------------------------------------------------------------------------------------------------*/

void timer1_delay_ms(long int delay_ms)
{
	/*	We are setting prescalar as 1024
	 *	so clock value for timer will be Fosc / 1024 = 7812.5Hz ( Fosc = 8MHz )
	 *	time taken for one cycle = 1 / 7812.5 = 0.128 ms
	 *	
	 *	=> TCNT value = ( delay_ms / 0.128ms )	*/ 

	long int tcnt_value;

	tcnt_value = TCNT1_MAX - ( delay_ms / ONE_COUNT_TIME_1024 );	

	TCNT1H = ( (tcnt_value & ( 0xFF00 )) >> 8 );	
	TCNT1L = ( tcnt_value & ( 0x00FF ) );	 	
							
	TCCR1A = CLEAR_ALL;
	TCCR1B |= SET_PRESCALAR_1024;

	while( ( TIFR & ( 1 << TOV1 ) ) == 0 );		//Waiting until overflow occurs

	TCCR1B = CLEAR_ALL;
	TIFR = ( 1 << TOV1 );

	return;
}

/*--------------------------------------------------------------------------------------------------------
	Function returns size of string
----------------------------------------------------------------------------------------------------------
*   
*   Function Name 	: 	string_count
*
*   Parameters 		:  	char *str	- 	Input string
*
*   Return     		: 	size of string
*-------------------------------------------------------------------------------------------------------*/

int string_count(char *str)
{
	int size = 0;

 	while ( *(str + size) != NULL_CHAR )
  	{	
   		size++;
  	}
 
 	return size;
}

/******************************************************************************************************/
