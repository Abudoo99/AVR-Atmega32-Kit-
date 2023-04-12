/*********************************************************************************************************
											 HEADER FILES										
*********************************************************************************************************/
#define F_CPU	8000000UL	//Setting clock at 8MHz

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "lcd.h"

/*********************************************************************************************************
									  	   MACRO DEFINITIONS
*********************************************************************************************************/

//Generic macros
#define NULL_CHAR		'\0'
#define PASS			0
#define FAIL			-1

#define ONCE			1

#define SET_ALL			0xFF
#define CLEAR_ALL		0x00

#define LED_ENABLE		(1 << PC2) | (1 << PC3) | (1 << PC4) | (1 << PC5) | (1 << PC6) | (1 << PC7) 

//INT0 specific macros
#define INT0_ENABLE				( 1 << INT0 )
#define INT0_RISING_EDGE_TRIG	( 1 << ISC00 ) | ( 1 << ISC01 )

/************** 7segment display specific macros ***************/

#define SEVEN_SEG_ENABLE		0x0F

#define WITH_DOT				0x00
#define WITHOUT_DOT				0x01

#define SEGMENT1_ENABLE			0x00
#define SEGMENT2_ENABLE			0x01
#define SEGMENT3_ENABLE			0x02
#define SEGMENT4_ENABLE			0x03

#define SEG_ZERO				0x7E			
#define SEG_ONE					0x0C
#define SEG_TWO					0xB6
#define SEG_THREE				0x9E
#define SEG_FOUR				0xCC
#define SEG_FIVE				0xDA
#define SEG_SIX					0xFA
#define SEG_SEVEN				0x0E
#define SEG_EIGHT				0xFE
#define SEG_NINE				0xDE

#define SEG_ZERO_DOT			0x7F			
#define SEG_ONE_DOT				0x0D
#define SEG_TWO_DOT				0xB7
#define SEG_THREE_DOT			0x9F
#define SEG_FOUR_DOT			0xCD
#define SEG_FIVE_DOT			0xDB
#define SEG_SIX_DOT				0xFB
#define SEG_SEVEN_DOT			0x0F
#define SEG_EIGHT_DOT			0xFF
#define SEG_NINE_DOT			0xDF

/******************** RTC specific macros ***********************/

#define RTC_WRITE_ADDR			0xD0
#define RTC_READ_ADDR			0xD1
#define SET_TIME				0x01
#define OFF						0x00

//RTC Timekeeper registers
#define RTC_SECONDS				0x00
#define RTC_MINUTES				0x01
#define RTC_HOURS				0x02
#define RTC_DAY					0x03
#define RTC_DATE				0x04
#define RTC_MONTH				0x05
#define RTC_YEAR				0x06
#define RTC_CONTROL				0x07

//RTC initial time
#define SECONDS_INIT			0x00
#define MINUTES_INIT			0x26
#define HOURS_INIT				0x19
#define DAY_INIT				TUESDAY
#define DATE_INIT				0x16
#define MONTH_INIT				0x11
#define YEAR_INIT				0x21

/****************************************************************/

//Timer specific macros
#define TCNT1_MAX				65535
#define TCNT0_MAX				255
#define TCNT_MIN				0
#define ONE_COUNT_TIME_1024		0.128
#define SET_PRESCALAR_1024		( 1 << CS00 ) | ( 1 << CS02 )
#define STOP_TIMER				0x00

#define INCREMENT(x)	x++
#define DECREMENT(x)	x--

/******************** I2C specific macros ***********************/

#define MASK_5_BITS_FROM_MSB	0xF8

#define START				1
#define REPEATED_START		2

#define ACK			0
#define NACK		1

#define START_SUCCESS					0x08
#define REPEATED_START_SUCCESS			0x10

//Status codes for Master transmitter mode
#define MT_SLAVE_ADDR_ACK				0x18	
#define MT_SLAVE_ADDR_NACK				0x20
#define MT_DATA_ACK						0x28
#define MT_DATA_NACK					0x30

//Status codes for Master receiver mode
#define MR_SLAVE_ADDR_ACK				0x40	
#define MR_SLAVE_ADDR_NACK				0x48
#define MR_DATA_RECEIVE_ACK				0x50
#define MR_DATA_RECEIVE_NACK			0x58

/***************************************************************/

enum DAYS{
			MONDAY=1,
			TUESDAY,
			WEDNESDAY,
			THURSDAY,
			FRIDAY,
			SATURDAY,
			SUNDAY
		};


/*******************************************************************************************************
										 STRUCTURE DEFINITION								
*******************************************************************************************************/

typedef struct 
{
	uint8_t seconds, minutes, hours;
	uint8_t day, date, month, year;
}RTC_i2c;

/*******************************************************************************************************
										  FUNCTION PROTOTYPES 					
*******************************************************************************************************/

void initialize_modules(void);

void timer1_delay_ms(long int);

void RTC_init(RTC_i2c*);
int RTC_set_time(RTC_i2c);
int RTC_get_time(RTC_i2c*);

int lcd_time_display(RTC_i2c);
int time_display(RTC_i2c);
int print_time(int, int, RTC_i2c);

void write_seg(int,int);
int bcd_to_dec(int);

void I2C_init(void);
int I2C_start(unsigned char, int);
int I2C_send_data(unsigned char);
unsigned char I2C_read(int);
void I2C_stop(void);

int num_convert(int, char*);
void string_cpy( char*, char*);
int string_count(char*);

/*********************************************************************************************************/

