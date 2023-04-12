/*********************************************************************************************************
											 HEADER FILES										
*********************************************************************************************************/
#define F_CPU	8000000UL	//Setting clock at 8MHz

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/*********************************************************************************************************
									  	   MACRO DEFINITIONS
*********************************************************************************************************/

//Generic macros
#define NULL_CHAR		'\0'
#define SET_ALL			0xFF
#define CLEAR_ALL		0x00

//INT0 specific macros
#define INT0_ENABLE				( 1 << INT0 )
#define INT0_RISING_EDGE_TRIG	( 1 << ISC00 ) | ( 1 << ISC01 )

//Timer specific macros
#define TCNT1_MAX				65535
#define TCNT_MAX				255
#define TCNT_MIN				0
#define ONE_COUNT_TIME_1024		0.128
#define SET_PRESCALAR_1024		( 1 << CS00 ) | ( 1 << CS02 )
#define STOP_TIMER				0x00

#define INCREMENT(x)	x++
#define DECREMENT(x)	x--

//GAME specific macros

#define MARIO_DATA				0
#define MARIO_RUN_DATA			1
#define OBSTACLE1_DATA			2
#define OBSTACLE2_DATA			3
#define OBSTACLE3_DATA			4

#define RANDOM_OBSTACLE			( rand() % 3 ) + 2
#define RANDOM_OBS_LINE			( rand() % 2 ) + 1 
#define RANDOM_OBS_COUNT		( rand() % 4 ) + 1

#define LINE_END_OBSTACLE		15
#define STARTING_POSITION		2
#define SCORE_SIZE				5
#define SCORE_POS				12

#define GAME_PAUSE				0
#define GAME_START				1

#define FINAL_GAME_SPEED		20

#define INCREASE_DIFFICULTY		300
#define INC_DIFF				4

#define MAX_DIFFICULTY			500
#define MAX_DIFF				6

/*******************************************************************************************************
										  FUNCTION PROTOTYPES 					
*******************************************************************************************************/

void timer1_delay_ms(long int);
void initialize_modules(void);

void lcd_init(void);
void lcd_command(unsigned char);
void lcd_data(unsigned char);
void lcd_printf(char*);
void lcd_create_char(int, unsigned char*);
void lcd_set_cursor(int,int);
void clear_data(void);

/*********************************************************************************************************/

