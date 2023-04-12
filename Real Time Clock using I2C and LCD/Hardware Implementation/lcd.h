/*********************************************************************************************************
											 HEADER FILES										
*********************************************************************************************************/
#define F_CPU	8000000UL	//Setting clock at 8MHz

#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

/*********************************************************************************************************
									  	   MACRO DEFINITIONS
*********************************************************************************************************/

//Control pins
#define RS		PD4
#define RW		PD5
#define EN		PD6

//LCD Commands
#define CLR_SCR						0x01
#define RET_HOME					0x02
#define CURSOR_DECREMENT			0x04
#define CURSOR_INCREMENT			0x06
#define CURSOR_DISP_OFF				0x08
#define CURSOR_SHIFT_LEFT			0x10
#define CURSOR_SHIFT_RIGHT			0x14
#define DISP_SHIFT_RIGHT			0x05
#define DISP_SHIFT_LEFT				0x07
#define DISP_ON_CURSOR_OFF			0x0C
#define DISP_ON_CURSOR_ON			0x0E
#define DISP_ON_CURSOR_BLINK		0x0F
#define DISP_OFF_CURSOR_ON			0x0A
#define MOVE_TO_BEG_LINE1			0x80
#define MOVE_TO_BEG_LINE2			0xC0
#define DISP_ONE_LINE_EIGHT_BIT		0x30
#define DISP_ONE_LINE_FOUR_BIT		0x20
#define SHIFT_ALL_LEFT				0x18
#define SHIFT_ALL_RIGHT				0x1C
#define EIGHT_BIT_MODE				0x38	
#define FOUR_BIT_MODE				0x28

//LCD specific macros
#define LCD_CAPACITY		32
#define LCD_ROW_SIZE		16
#define LCD_COLUMN_SIZE		2

#define LINE1				1
#define LINE2				2
#define LINE_END			16
#define LINE_START			0

#define LCD_CTRL_ENABLE			( 1 << RS ) | ( 1 << RW ) | ( 1 << EN )

/*******************************************************************************************************
										  FUNCTION PROTOTYPES 					
*******************************************************************************************************/

void lcd_init(void);
void lcd_command(unsigned char);
void lcd_data(unsigned char);
void lcd_printf( char*, int, int);
void lcd_set_cursor(int, int);

void timer1_delay_ms( long int );

/*********************************************************************************************************/

