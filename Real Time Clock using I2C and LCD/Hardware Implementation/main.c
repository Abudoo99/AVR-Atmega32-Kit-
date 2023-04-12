/*******************************************************************************************************
*   TASK :
*	
*	1.	Display RTC using I2C communication.
*	2.	Use LCD for displaying date and time
*	3.	Use hardware implementation of I2C
*
*	Date :	10/11/2021	09:15:00 AM
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
*	PC1	-	SDA pin
*	PC0	-	SCL pin
*
*	INT0 button is used for time reset
*
********************************************************************************************************
											 HEADER FILES										
*******************************************************************************************************/

#include "main.h"

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
		if ( RTC_get_time(&rtc) == FAIL )		//Gets time from RTC registers
		{
			return EXIT_FAILURE;
		}

		else
		{
			if ( time_display( rtc ) == FAIL )	//Shows time and date in LCD and 7 segment display	
			{
				return PASS;
			}		
		}
	}	

	return EXIT_SUCCESS;
}

/*******************************************************************************************************
										  FUNCTION DEFINITIONS 					
*******************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------
	Function performs I2C initializations
----------------------------------------------------------------------------------------------------------
*   
*   Function Name 	: 	I2C_init
*
*   Parameters 		:  	NONE
*
*   Return     		: 	NONE
*-------------------------------------------------------------------------------------------------------*/

void initialize_modules(void)
{ 
	//GPIO configurations
	DDRC = SET_ALL;						//Configuring LEDs for debugging purpose
	DDRB = SET_ALL ;					//Configuring LCD data lines as output
	DDRD = LCD_CTRL_ENABLE;				//RS, RW, and EN set as output
	DDRD |= (1 << PD3);					//Configuring buzzer as output 
	DDRA |= SEVEN_SEG_ENABLE;			//Configuring 7segment enable pins

	//Interrupt enabling for INT0

	GICR = INT0_ENABLE ;			
	MCUCR = INT0_RISING_EDGE_TRIG ;	
	sei();							//Enabling global interrupt


	//LCD configuration
	lcd_init();
	lcd_set_cursor(0,2);
	lcd_printf("Date - ",0,7);

	//I2C configuration
	I2C_init();

	return;
}

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
	 *	=> TCNT value = ( delay_ms / 0.128ms )
	 *	
	 *	Since timer is 8 bit, we can have TCNT value only upto 255 
	 *	We have to loop the TCNT value for quotient number of times and then load the remainder 
	 *	for the final iteration 																	*/ 

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
	Function sets initial values for RTC registers
----------------------------------------------------------------------------------------------------------
*   
*   Function Name : RTC_init
*
*   Parameters : RTC_i2c *rtc	-	structure containing time and date
*
*   Return     : NONE
*-------------------------------------------------------------------------------------------------------*/

void RTC_init( RTC_i2c *rtc )
{
	rtc->seconds = SECONDS_INIT ;
	rtc->minutes = MINUTES_INIT ;
	rtc->hours = HOURS_INIT ;
	rtc->day = DAY_INIT ;
	rtc->date = DATE_INIT ;
	rtc->month = MONTH_INIT ;
	rtc->year = YEAR_INIT ;

	return;
}

/*--------------------------------------------------------------------------------------------------------
	Function sets initial time in RTC
----------------------------------------------------------------------------------------------------------
*   
*   Function Name : RTC_set_time
*
*   Parameters : RTC_i2c rtc	-	structure containing time and date
*
*   Return     : NONE
*-------------------------------------------------------------------------------------------------------*/

int RTC_set_time( RTC_i2c rtc )
{
	RTC_init(&rtc);		//Initializing values for storing in RTC registers 

	//lcd_command( CLR_SCR );
	/*lcd_set_cursor(0,1);
	lcd_printf("Real Time Clock     TIME SET    ", 0, 32 );
	timer1_delay_ms(1000);
	lcd_command( CLR_SCR );
	*/

	if ( I2C_start( RTC_WRITE_ADDR, START ) == FAIL )				//Starting I2C communication  
	{
		return FAIL;
	}
											
	if ( I2C_send_data( RTC_SECONDS ) == FAIL )				//Sending word address and waiting for ACK
	{
		return FAIL;
	}

	//Sending data to corresponding registers
	
	if ( I2C_send_data( rtc.seconds ) == FAIL )			
	{
		return FAIL;
	}

	if ( I2C_send_data( rtc.minutes ) == FAIL )			
	{
		return FAIL;
	}

	if ( I2C_send_data( rtc.hours ) == FAIL )				
	{
		return FAIL;
	}

	if ( I2C_send_data( rtc.day ) == FAIL )				
	{
		return FAIL;
	}

	if ( I2C_send_data( rtc.date ) == FAIL )				
	{
		return FAIL;
	}

	if ( I2C_send_data( rtc.month ) == FAIL )				
	{
		return FAIL;
	}

	if ( I2C_send_data( rtc.year ) == FAIL )				
	{
		return FAIL;
	}

	I2C_stop();

	return PASS;
}

/*--------------------------------------------------------------------------------------------------------
	Function gets date and time from RTC registers
----------------------------------------------------------------------------------------------------------
*   
*   Function Name : RTC_get_time
*
*   Parameters : RTC_i2c rtc	-	structure to receive time and date
*
*   Return     : PASS or FAIL
*-------------------------------------------------------------------------------------------------------*/

int RTC_get_time(RTC_i2c *rtc)
{
	if ( I2C_start( RTC_WRITE_ADDR, START ) == FAIL )				//Starting I2C communication  
	{
		return FAIL;
	}

	if ( I2C_send_data( RTC_SECONDS ) == FAIL )				//Sending word address and waiting for ACK
	{
		return FAIL;
	}

	if ( I2C_start( RTC_READ_ADDR, REPEATED_START ) == FAIL )				//Enabling repeated start
	{
		return FAIL;
	}		
	
	rtc->seconds = I2C_read( ACK );
	rtc->minutes = I2C_read( ACK );
	rtc->hours = I2C_read( ACK );
	rtc->day = I2C_read( ACK );
	rtc->date = I2C_read( ACK );	
	rtc->month = I2C_read( ACK );
	rtc->year = I2C_read( NACK );
	
	I2C_stop();

	return PASS;
}

/*--------------------------------------------------------------------------------------------------------
	Function writes value to 7segment display based on digits
----------------------------------------------------------------------------------------------------------
*   
*   Function Name : write_seg
*
*   Parameters : int bit	-	Bit that is displayed on 7segment
*				 int type	-	with or without dot
*
*   Return     : NONE
*-------------------------------------------------------------------------------------------------------*/

void write_seg( int bit, int type )
{
	if (type == WITH_DOT )
	{
		switch( bit )
		{
			case 0	: 	PORTB = SEG_ZERO_DOT;
						break;
			case 1	:	PORTB = SEG_ONE_DOT;
						break;
			case 2 	: 	PORTB = SEG_TWO_DOT;
						break;
			case 3	:	PORTB = SEG_THREE_DOT;
						break;
			case 4	:	PORTB = SEG_FOUR_DOT;
						break;
			case 5	:	PORTB = SEG_FIVE_DOT;
						break;
			case 6	:	PORTB = SEG_SIX_DOT;
						break;
			case 7	:	PORTB = SEG_SEVEN_DOT;
						break;
			case 8 	:	PORTB = SEG_EIGHT_DOT;
						break;
			case 9	:	PORTB = SEG_NINE_DOT;
						break;		
		}	
	}

	else
	{
		switch( bit )
		{
			case 0	: 	PORTB = SEG_ZERO;
						break;
			case 1	:	PORTB = SEG_ONE;
						break;
			case 2 	: 	PORTB = SEG_TWO;
						break;
			case 3	:	PORTB = SEG_THREE;
						break;
			case 4	:	PORTB = SEG_FOUR;
						break;
			case 5	:	PORTB = SEG_FIVE;
						break;
			case 6	:	PORTB = SEG_SIX;
						break;
			case 7	:	PORTB = SEG_SEVEN;
						break;
			case 8 	:	PORTB = SEG_EIGHT;
						break;
			case 9	:	PORTB = SEG_NINE;
						break;		
		}		
	}

	return;
}

/*--------------------------------------------------------------------------------------------------------
	Function prints time in seven segment display
----------------------------------------------------------------------------------------------------------
*   
*   Function Name : print_time
*
*   Parameters : int minute		-	current time ( in minutes )
*				 int hour		-	current time ( in hours )
*				 RTC_i2c rtc	-	structure containing time and date
*
*   Return     : PASS or FAIL
*-------------------------------------------------------------------------------------------------------*/


int print_time( int minute, int hour, RTC_i2c rtc )
{
	int bit;
	int temp1 = minute;
	int temp2 = hour;

	long int tcnt_value;

	tcnt_value = TCNT1_MAX - ( 500 / ONE_COUNT_TIME_1024 );	

	TCNT1H = ( (tcnt_value & ( 0xFF00 )) >> 8 );	
	TCNT1L = ( tcnt_value & ( 0x00FF ) );	 	
							
	TCCR1A = CLEAR_ALL;
	TCCR1B |= SET_PRESCALAR_1024;

	while( ( TIFR & ( 1 << TOV1 ) ) == 0 )		//Waiting until overflow occurs
	{
		bit = minute % 10;

		PORTA |= (1 << SEGMENT1_ENABLE);
		write_seg( bit, WITHOUT_DOT );
		_delay_us(400);
		PORTA &= ~(1 << SEGMENT1_ENABLE);

		minute = minute / 10;

		bit = minute % 10;

		PORTA |= (1 << SEGMENT2_ENABLE);
		write_seg( bit, WITHOUT_DOT );
		_delay_us(400);
		PORTA &= ~(1 << SEGMENT2_ENABLE);

		minute = temp1;	

		bit = hour % 10;

		PORTA |= (1 << SEGMENT3_ENABLE);
		write_seg( bit, WITH_DOT );
		_delay_us(400);
		PORTA &= ~(1 << SEGMENT3_ENABLE);

		hour = hour / 10;

		bit = hour % 10;

		PORTA |= (1 << SEGMENT4_ENABLE);
		write_seg( bit, WITHOUT_DOT );
		_delay_us(400);
		PORTA &= ~(1 << SEGMENT4_ENABLE);		
		
		hour = temp2;

	}

	//Stopping timer
	TCCR1B = CLEAR_ALL;
	TIFR |= ( 1 << TOV1 );

	if ( lcd_time_display(rtc) == FAIL )
	{
		return FAIL;	
	}	

	return PASS;
}

/*--------------------------------------------------------------------------------------------------------
	Function displays current time in seven segment display
----------------------------------------------------------------------------------------------------------
*   
*   Function Name : time_display
*
*   Parameters 	  : RTC_i2c rtc	-	structure to receive time and date
*
*   Return     	  :	PASS or FAIL
*-------------------------------------------------------------------------------------------------------*/

int time_display( RTC_i2c rtc )
{
	int mins, hrs;

	mins = bcd_to_dec( rtc.minutes );
	hrs = bcd_to_dec( rtc.hours );
	
	if ( print_time( mins, hrs, rtc ) == FAIL )
	{
		return FAIL;
	}

	return PASS;
}

/*--------------------------------------------------------------------------------------------------------
	Function converts given BCD number to decimal number
----------------------------------------------------------------------------------------------------------
*   
*   Function Name : bcd_to_dec
*
*   Parameters 	  : int num			-	Number to convert to decimal
*
*   Return     	  : converted decimal number
*-------------------------------------------------------------------------------------------------------*/

int bcd_to_dec( int bcd_num )
{
	int dec_num = 0 , bit;

	bit = ( (bcd_num >> 4) & 0x0F );
	dec_num = (dec_num * 10) + bit;		
	bit = bcd_num % 16;
	dec_num = (dec_num * 10) + bit;		

	return dec_num;
}

/*--------------------------------------------------------------------------------------------------------
	Function displays current time in LCD
----------------------------------------------------------------------------------------------------------
*   
*   Function Name : lcd_time_display
*
*   Parameters : RTC_i2c rtc	-	structure to receive time and date
*
*   Return     : PASS or FAIL
*-------------------------------------------------------------------------------------------------------*/

int lcd_time_display( RTC_i2c rtc )
{
	int str_size, count = ONCE;
	char *str;

	str = (char*)malloc( 3 * sizeof(char) );
	if (str == NULL)
	{
		return FAIL;
	}

	lcd_set_cursor( 0,1 );

	str_size = 2;

	if ( (rtc.minutes == 0x00) || (count == ONCE) )
	{
		num_convert( rtc.hours, str );
		lcd_printf( str, 0, str_size );
		lcd_data(':');		
		*str = NULL_CHAR;
	}
	
	if ( (rtc.seconds == 0x00) || (count == ONCE) )
	{
		num_convert( rtc.minutes, str );
		lcd_printf( str, 0, str_size );
		lcd_data(':');
		*str = NULL_CHAR;
	}

	lcd_set_cursor(6,1);
	num_convert( rtc.seconds, str );
	lcd_printf( str, 0, str_size );
	lcd_printf("  ", 0, str_size);

	*str = NULL_CHAR;

	if ( (rtc.hours == 0x00) || (count == ONCE) )
	{
		switch( rtc.day )
		{
			case MONDAY 	: 	string_cpy( str, "MON");
								break;
			case TUESDAY	: 	string_cpy( str, "TUE");
								break;
			case WEDNESDAY 	: 	string_cpy( str, "WED");
								break;
			case THURSDAY 	: 	string_cpy( str, "THU");
								break;
			case FRIDAY 	: 	string_cpy( str, "FRI");
								break;
			case SATURDAY 	: 	string_cpy( str, "SAT");
								break;
			case SUNDAY 	: 	string_cpy( str, "SUN");
								break;
			default			:	string_cpy( str, "NIL");
								break;
		}

		str_size = string_count( str );

		str = realloc( str, ( str_size + 1 ) * sizeof(char) );
		if (str == NULL)
		{
			return FAIL;
		}

		lcd_printf( str, 0, str_size );		


		//lcd_command( MOVE_TO_BEG_LINE2 );
		lcd_set_cursor(7, 2);

		str_size = 2;	

		if ( num_convert( rtc.date, str ) == FAIL )
		{
			return FAIL;
		}
		lcd_printf( str, 0, str_size );
		lcd_data('/');

		*str = NULL_CHAR;

		if ( num_convert( rtc.month, str ) == FAIL )
		{
			return FAIL;
		}
		lcd_printf( str, 0, str_size );
		lcd_data('/');

		*str = NULL_CHAR;

		if ( num_convert( rtc.year, str ) == FAIL )
		{
			return FAIL;
		}
		lcd_printf( str, 0, str_size );

		*str = NULL_CHAR;

		count = 0;
	}

	//Freeing heap memory after using it
	if (str != NULL)
	{
		free(str);
		str = NULL;
	}

	return PASS;
}


/*--------------------------------------------------------------------------------------------------------
	Function gets register value and converts it to digits
----------------------------------------------------------------------------------------------------------
*   
*   Function Name : num_convert
*
*   Parameters 	  : int num			-	Number to convert
*					char *str		-	String containing digits
*
*   Return     	  : PASS or FAIL
*-------------------------------------------------------------------------------------------------------*/

int num_convert( int num, char *str )
{
	int rem = 0, temp, itr=1;

	str = realloc( str, 3 * sizeof(char) );
	if (str == NULL)
	{
		return FAIL;
	}
	
	temp = num;

	if (temp == 0)
	{
		*str = '0';
		*( str + 1 ) = '0';
	}
	
	else
	{
		while ( temp > 0 )
		{
			rem = temp % 16;
			*( str + itr-- ) = rem + '0';
			temp = temp / 16;		
		}

		if (itr == 0)
		{
			*( str + itr ) = '0';
		}		
	}

	*( str + 2 ) = NULL_CHAR;

	return PASS;
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

/*--------------------------------------------------------------------------------------------------------
	Function copies contents of string 2 to string 1
----------------------------------------------------------------------------------------------------------
*   
*   Function Name : string_cpy
*
*   Parameters 	  : char *str1		-	String to store contents
*					char *str2		-	String from which contents are sent
*
*   Return     	  : NONE
*-------------------------------------------------------------------------------------------------------*/

void string_cpy( char *str1, char *str2 )
{
	int char_num;

	for (char_num = 0; *( str2 + char_num ) != NULL_CHAR ; char_num++ )
	{
		*( str1 + char_num ) = *( str2 + char_num );
	}

	*( str1 + char_num ) = NULL_CHAR;

	return;
}

/**********************************************************************************************************/
