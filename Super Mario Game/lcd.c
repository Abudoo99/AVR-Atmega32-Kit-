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
	lcd_command( CLR_SCR );
	lcd_command( RET_HOME ); 
	lcd_command( MOVE_TO_BEG_LINE1 ); 
	lcd_command( DISP_ON_CURSOR_OFF ); 	

	return;
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

void lcd_printf( char *str )
{
	int char_num;

	for (char_num = pos; *( str + char_num ) != NULL_CHAR ; char_num += 1)
	{
		if ( char_num == (LINE_END) )
		{
			lcd_command( MOVE_TO_BEG_LINE2 );
		}
		lcd_data( *(str + char_num) );
	}
}

/*--------------------------------------------------------------------------------------------------------
	Function stores data in CGRAM addresses
----------------------------------------------------------------------------------------------------------
*   
*   Function Name 	: 	lcd_create_char
*
*   Parameters 		:  	int addr				-	Address of CGRAM
*						unsigned char *data		-	Data to be stored
*
*   Return     		: 	NONE
*-------------------------------------------------------------------------------------------------------*/

void lcd_create_char( int addr, unsigned char *data )
{
	int char_num;

	//Setting CGRAM address
	lcd_command( CGRAM_ADDR + addr * 8 );

	//Storing data in current CGRAM address
	for (char_num = 0; char_num < 8; char_num += 1)
	{
		lcd_data( *( data + char_num ) );
	}

	return;
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

/*********************************************************************************************************/
