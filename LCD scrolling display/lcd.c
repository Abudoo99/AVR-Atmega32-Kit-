/*******************************************************************************************************
											 HEADER FILES										
*******************************************************************************************************/

#include "lcd.h"


/*******************************************************************************************************
										    GLOBAL VARIABLES										
*******************************************************************************************************/

int pos = 0, line = 1;

/*******************************************************************************************************
										  FUNCTION DEFINITIONS 					
*******************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------
	Function performs LCD initilizations
----------------------------------------------------------------------------------------------------------
*   
*   Function Name 	: 	lcd_init
*
*   Parameters 		:  	NONE
*
*   Return     		: 	NONE
*-------------------------------------------------------------------------------------------------------*/

void lcd_init()
{
	lcd_command( EIGHT_BIT_MODE ); 
	timer1_delay_ms(1);
	lcd_command( CLR_SCR );
	timer1_delay_ms(1);
	lcd_command( RET_HOME ); 
	timer1_delay_ms(1);
	lcd_command( MOVE_TO_BEG_LINE1 ); 
	timer1_delay_ms(1);
	lcd_command( DISP_ON_CURSOR_OFF ); 
	timer1_delay_ms(1); 	
}

/*--------------------------------------------------------------------------------------------------------
	Function send command from user to LCD
----------------------------------------------------------------------------------------------------------
*   
*   Function Name 	: 	lcd_command
*
*   Parameters 		:  	unsigned char cmd
*
*   Return     		: 	NONE
*-------------------------------------------------------------------------------------------------------*/

void lcd_command( unsigned char cmd )
{
	PORTB = cmd;	

	PORTD &= ~(1 << RS);		//Command mode
	PORTD &= ~(1 << RW);		//Write mode
	PORTD |= (1 << EN);			//Enable high

	timer1_delay_ms(1);

	PORTD &= ~(1 << EN);		//Enable low
	return;
}

/*--------------------------------------------------------------------------------------------------------
	Function send data from user to LCD
----------------------------------------------------------------------------------------------------------
*   
*   Function Name 	: 	lcd_data
*
*   Parameters 		:  	unsigned char data
*
*   Return     		: 	NONE
*-------------------------------------------------------------------------------------------------------*/

void lcd_data( unsigned char data )
{
	PORTB = data;	

	PORTD |= (1 << RS);			//Command mode
	PORTD &= ~(1 << RW);		//Write mode
	PORTD |= (1 << EN);			//Enable high

	timer1_delay_ms(1);

	PORTD &= ~(1 << EN);		//Enable low
	return;
}

/*--------------------------------------------------------------------------------------------------------
	Function send string of data from user to LCD
----------------------------------------------------------------------------------------------------------
*   
*   Function Name 	: 	lcd_printf
*
*   Parameters 		:  	char *str	-	String of characters
*						int pos		-	printing start position 
*						int size	- 	Size of string
*
*   Return     		: 	NONE
*-------------------------------------------------------------------------------------------------------*/

void lcd_printf( char *str, int pos, int size )
{
	int char_num;

	for (char_num = pos; char_num < size ; char_num += 1)
	{
		if ( char_num == (LINE_END) )
		{
			lcd_command( MOVE_TO_BEG_LINE2 );
		}
		lcd_data( *(str + char_num) );
	}
}

/*--------------------------------------------------------------------------------------------------------
	Function send string of data from user to LCD
----------------------------------------------------------------------------------------------------------
*   
*   Function Name 	: 	lcd_scroll
*
*   Parameters 		:  	char *str
*
*   Return     		: 	NONE
*-------------------------------------------------------------------------------------------------------*/

void lcd_scroll( char *str )
{
	int size;
	size = string_count( str );

	while(1)
	{
		step_right( str );		//Moving cursor by one position 
		lcd_printf( str, 0, size );
		timer1_delay_ms( 300 );
	}
}

/*--------------------------------------------------------------------------------------------------------
	Function sets cursor of lcd at given position in 16x2 display
----------------------------------------------------------------------------------------------------------
*   
*   Function Name 	: 	lcd_set_cursor
*
*   Parameters 		:  	int pos		-	Position of cursor
*						int line	-	Line in which cursor should be present
*
*   Return     		: 	NONE
*-------------------------------------------------------------------------------------------------------*/

void lcd_set_cursor( int pos, int line )
{	
	int itr;		//Temporary iteration variable

	if ( line == LINE1 )
	{
		lcd_command( MOVE_TO_BEG_LINE1 );
	}	
	else if ( line == LINE2 )
	{
		lcd_command( MOVE_TO_BEG_LINE2 );
	}

	if ( pos > LINE_END )
	{
		pos = pos % LINE_END;
	}

	for (itr = 0; itr < pos; itr += 1)
	{
		lcd_command( CURSOR_SHIFT_RIGHT );
	}

}

/*--------------------------------------------------------------------------------------------------------
	Function gets a string and moves it one step to right
----------------------------------------------------------------------------------------------------------
*   
*   Function Name 	: 	step_right
*
*   Parameters 		:  	char *str	- 	Input string
*
*   Return     		: 	NONE
*-------------------------------------------------------------------------------------------------------*/

void step_right( char *str )
{	
	int size, i = 1;
	size = string_count( str );

	lcd_command( CLR_SCR );				
	
	if ( line == LINE1 )
	{
		lcd_command( MOVE_TO_BEG_LINE1 );
	}

	if ( line == LINE2 )
	{
		lcd_command( MOVE_TO_BEG_LINE2 );
	}

	lcd_set_cursor( pos++, line );			//Setting cursor to next position in LCD

	if ( pos == ( LINE_END - (size-2) ) )
	{	
		while( i <= size )
		{
			lcd_command( CLR_SCR );				
			
			if ( line == LINE1 )
			{
				lcd_command( MOVE_TO_BEG_LINE1 );
			}

			if ( line == LINE2 )
			{
				lcd_command( MOVE_TO_BEG_LINE2 );
			}

			lcd_set_cursor( ( (pos-1) + (i-1) ), line );
			lcd_printf( str, 0, size - i );

			if ( line == LINE1 )
			{
				lcd_command( MOVE_TO_BEG_LINE2 );
			}
			else if ( line == LINE2 )
			{
				lcd_command( MOVE_TO_BEG_LINE1 );
			}	

			lcd_printf( str, size - i, size );

			if ( line == LINE1 )
			{
				lcd_command( MOVE_TO_BEG_LINE2 );
			}
			else if ( line == LINE2 )
			{
				lcd_command( MOVE_TO_BEG_LINE1 );
			}

			i++;
			
			if (i <= size )
			{
				timer1_delay_ms(300);				
			}
		}

		pos = 1;

		if ( line == LINE1 )
		{
			line = LINE2;
		}
		else if ( line == LINE2 )
		{
			line = LINE1;
		}
	}

	if ( pos == LINE_END )
	{
		pos = LINE_START;

		if ( line == LINE1 )
		{
			line = LINE2;
		}
		else if ( line == LINE2 )
		{
			line = LINE1;
		}
	}
}

/*********************************************************************************************************/
