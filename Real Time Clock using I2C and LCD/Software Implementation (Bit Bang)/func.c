/*******************************************************************************************************
											 HEADER FILES										
*******************************************************************************************************/

#include "func.h"

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
	Function performs I2C initializations
----------------------------------------------------------------------------------------------------------
*   
*   Function Name 	: 	I2C_init
*
*   Parameters 		:  	NONE
*
*   Return     		: 	NONE
*-------------------------------------------------------------------------------------------------------*/

void I2C_init(void)
{ 
	DDRD |= (1 << PD0) | (1 << PD1);	//Configuring SCL and SDA as output
	SET_SCL;		//Making SCL high as initial state
	SET_SDA;		//Making SDA high as initial state	

	return;
}

/*--------------------------------------------------------------------------------------------------------
	Function establishes communication between master and slave
----------------------------------------------------------------------------------------------------------
*   
*   Function Name 	: 	I2C_start
*
*   Parameters 		:  	NONE
*
*   Return     		: 	NONE
*-------------------------------------------------------------------------------------------------------*/

void I2C_start(void)
{
	SET_SCL;		//Making SCL high as initial state
	SET_SDA;		//Making SDA high as initial state	

	_delay_us( CLOCK_PERIOD / 2 );
	CLR_SDA;
	_delay_us( CLOCK_PERIOD );
	CLR_SCL;
	_delay_us( CLOCK_PERIOD );

	return;
}

/*--------------------------------------------------------------------------------------------------------
	Function reads bit from SDA 
----------------------------------------------------------------------------------------------------------
*   
*   Function Name 	: 	I2C_read_bit
*
*   Parameters 		:  	NONE
*
*   Return     		: 	integer
*-------------------------------------------------------------------------------------------------------*/

int I2C_read_bit()
{
	int bit;	

	_delay_us( CLOCK_PERIOD / 2 );	
	SET_SCL;
	_delay_us( CLOCK_PERIOD );
		
	if ( PIND & (1 << PD1) )
	{
		bit = 1;
	}
	else
	{
		bit = 0;
	}
	
	CLR_SCL;
	_delay_us( CLOCK_PERIOD / 2 );

	return bit;
}

/*--------------------------------------------------------------------------------------------------------
	Function reads a byte from SDA 
----------------------------------------------------------------------------------------------------------
*   
*   Function Name 	: 	I2C_read_byte
*
*   Parameters 		:  	NONE
*
*   Return     		: 	integer
*-------------------------------------------------------------------------------------------------------*/

int I2C_read_byte(void)
{
	int byte = 0x00, bit;	

	//Configuring SDA as input for reading ack bit
	DDRD &= ~(1 << PD1);	
	SET_SDA;

	for (bit = 0; bit < 8; bit += 1)
	{
		byte = byte << 1;
		byte |= I2C_read_bit();
	}

	//Configuring SDA as output after reading ack bit
	DDRD |= (1 << PD1);	
	
	return byte;
}

/*--------------------------------------------------------------------------------------------------------
	Function sends bit to SDA 
----------------------------------------------------------------------------------------------------------
*   
*   Function Name 	: 	I2C_send_bit
*
*   Parameters 		:  	int bit
*
*   Return     		: 	NONE
*-------------------------------------------------------------------------------------------------------*/

void I2C_send_bit( int bit )
{

	if ( bit )
	{
		SET_SDA;
	}
	else
	{
		CLR_SDA;
	}	
	CLR_SCL;
	_delay_us( CLOCK_PERIOD / 2 );
	SET_SCL;
	_delay_us( CLOCK_PERIOD );
	CLR_SCL;
	_delay_us( CLOCK_PERIOD / 2 );

	return;
}

/*--------------------------------------------------------------------------------------------------------
	Function sends a byte of data to SDA 
----------------------------------------------------------------------------------------------------------
*   
*   Function Name 	: 	I2C_send_byte
*
*   Parameters 		:  	int byte
*
*   Return     		: 	ACK or NACK
*-------------------------------------------------------------------------------------------------------*/

int I2C_send_byte( int byte )
{
	int itr, ack_bit;

	while(1)
	{
		for (itr = 7; itr >= 0; itr-- )
		{
			I2C_send_bit( byte & ( 1 << itr ) );
		}

		//Configuring SDA as input for reading ack bit
		DDRD &= ~(1 << PD1);	
		SET_SDA;

		ack_bit = I2C_read_bit();

		//Configuring SDA as output after reading ack bit
		DDRD |= (1 << PD1);		

		if ( ack_bit == BIT_ACK )
		{
			break;
		}
	}

	return ack_bit;
}


/*--------------------------------------------------------------------------------------------------------
	Function stops I2C communication
----------------------------------------------------------------------------------------------------------
*   
*   Function Name 	: 	I2C_stop
*
*   Parameters 		:  	NONE
*
*   Return     		: 	NONE
*-------------------------------------------------------------------------------------------------------*/

void I2C_stop(void)
{
	_delay_us( CLOCK_PERIOD );	
	SET_SCL;
	_delay_us( CLOCK_PERIOD );
	SET_SDA;
	
	return;
}

/*--------------------------------------------------------------------------------------------------------
	Function starts timer1 with given user time
----------------------------------------------------------------------------------------------------------
*   
*   Function Name : start_timer
*
*   Parameters : long int delay	-	Delay to generate 
*
*   Return     : NONE
*-------------------------------------------------------------------------------------------------------*/

void start_timer(long int delay)
{
	/*	We are setting prescalar as 1024
	 *	so clock value for timer will be Fosc / 1024 = 7812.5Hz ( Fosc = 8MHz )
	 *	time taken for one cycle = 1 / 7812.5 = 0.128 ms
	 *	
	 *	=> TCNT value = ( delay / 0.128ms )	*/ 

	long int tcnt_value;

	tcnt_value = TCNT1_MAX - ( delay / ONE_COUNT_TIME_1024 );	

	TCNT1H = ( (tcnt_value & ( 0xFF00 )) >> 8 );	
	TCNT1L = ( tcnt_value & ( 0x00FF ) );	 	
							
	TCCR1A = CLEAR_ALL;
	TCCR1B |= SET_PRESCALAR_1024;

	TIFR |= ( 1 << TOV1 );

	return;
}

/*--------------------------------------------------------------------------------------------------------
	Function stops timer1
----------------------------------------------------------------------------------------------------------
*   
*   Function Name : stop_timer
*
*   Parameters : NONE
*
*   Return     : NONE
*-------------------------------------------------------------------------------------------------------*/

void stop_timer(void)
{
	while( ( TIFR & ( 1 << TOV1 ) ) == 0 );		//Waiting until overflow occurs

	//Stopping timer
	TCCR1B = CLEAR_ALL;
	TIFR |= ( 1 << TOV1 );

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
	start_timer( delay_ms );	
	stop_timer();

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

void RTC_set_time( RTC_i2c rtc )
{
	RTC_init(&rtc);		//Initializing values for storing in RTC registers 

	/*lcd_set_cursor(0,1);
	lcd_printf("Real Time Clock     TIME SET    ", 0, 32 );
	timer1_delay_ms(1000);
	lcd_command( CLR_SCR );
	*/

	I2C_start();											//Starting I2C communication 
	I2C_send_byte( RTC_WRITE_ADDR );						//Sending slave address and waiting for ACK
	I2C_send_byte( RTC_SECONDS );							//Sending slave address and waiting for ACK

	//Sending data to corresponding registers
	I2C_send_byte( rtc.seconds );
	I2C_send_byte( rtc.minutes );
	I2C_send_byte( rtc.hours );
	I2C_send_byte( rtc.day );
	I2C_send_byte( rtc.date );
	I2C_send_byte( rtc.month );
	I2C_send_byte( rtc.year );	
	I2C_stop();

	return;
}

/*--------------------------------------------------------------------------------------------------------
	Function gets date and time from RTC registers
----------------------------------------------------------------------------------------------------------
*   
*   Function Name : RTC_get_time
*
*   Parameters : RTC_i2c *rtc	-	structure to receive time and date
*
*   Return     : NONE
*-------------------------------------------------------------------------------------------------------*/

void RTC_get_time(RTC_i2c *rtc)
{
	I2C_start();
	I2C_send_byte( RTC_WRITE_ADDR );					//Sending slave address in write mode 
	I2C_send_byte( RTC_SECONDS );						//Sending starting address to read
	I2C_start();										//Enabling repeated start
	I2C_send_byte( RTC_READ_ADDR );						//Sending slave address in read mode
	
	//Getting data from RTC registers
	rtc->seconds = I2C_read_byte();
	I2C_send_bit( BIT_ACK );

	rtc->minutes = I2C_read_byte();
	I2C_send_bit( BIT_ACK );

	rtc->hours = I2C_read_byte();
	I2C_send_bit( BIT_ACK );

	rtc->day = I2C_read_byte();
	I2C_send_bit( BIT_ACK );

	rtc->date = I2C_read_byte();
	I2C_send_bit( BIT_ACK );

	rtc->month = I2C_read_byte();
	I2C_send_bit( BIT_ACK );

	rtc->year = I2C_read_byte();
	I2C_send_bit( BIT_NACK );

	//Stopping reception of data
	I2C_stop();

	return;
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
*   Return     : NONE
*-------------------------------------------------------------------------------------------------------*/


void print_time( int minute, int hour, RTC_i2c rtc )
{
	int bit;
	int temp1 = minute;
	int temp2 = hour;

	TCNT1 = TCNT1_MAX - ( 500 / ONE_COUNT_TIME_1024 );	
							
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

	lcd_time_display(rtc);
	
	return;
}

/*--------------------------------------------------------------------------------------------------------
	Function displays current time in seven segment display
----------------------------------------------------------------------------------------------------------
*   
*   Function Name : time_display
*
*   Parameters 	  : RTC_i2c rtc	-	structure to receive time and date
*
*   Return     	  :	NONE
*-------------------------------------------------------------------------------------------------------*/

void time_display( RTC_i2c rtc )
{
	int mins, hrs;

	mins = bcd_to_dec( rtc.minutes );
	hrs = bcd_to_dec( rtc.hours );
	
	print_time( mins, hrs, rtc );

	return;
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

/*******************************************************************************************************/
