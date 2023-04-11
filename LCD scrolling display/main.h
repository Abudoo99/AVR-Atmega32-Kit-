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

//Generic macros
#define NULL_CHAR		'\0'
#define SET_ALL			0xFF
#define CLEAR_ALL		0x00



//Timer specific macros
#define TCNT1_MAX				65535
#define TCNT_MAX				255
#define TCNT_MIN				0
#define ONE_COUNT_TIME_1024		0.128
#define SET_PRESCALAR_1024		( 1 << CS00 ) | ( 1 << CS02 )
#define STOP_TIMER				0x00

#define INCREMENT(x)	x++
#define DECREMENT(x)	x--

/*******************************************************************************************************
										  FUNCTION PROTOTYPES 					
*******************************************************************************************************/

void timer1_delay_ms(long int);
int string_count(char*);

void lcd_init(void);
void lcd_command(unsigned char);
void lcd_data(unsigned char);
void lcd_printf(char*, int, int);
void lcd_scroll(char*);
void lcd_set_cursor(int,int);
void step_right( char* );
void clear_data(void);

/*********************************************************************************************************/

