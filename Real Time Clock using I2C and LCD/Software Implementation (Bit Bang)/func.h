/*********************************************************************************************************
											 HEADER FILES										
*********************************************************************************************************/
#define F_CPU	8000000UL	//Setting clock at 8MHz

#include <stdint.h>
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

#define NUM_CONVERT		48
#define ONCE			1

#define SET_ALL			0xFF
#define CLEAR_ALL		0x00

//INT0 specific macros
#define INT0_ENABLE				( 1 << INT0 )
#define INT0_RISING_EDGE_TRIG	( 1 << ISC00 ) | ( 1 << ISC01 )

//7segment specific macros
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

//RTC specific macros
#define RTC_WRITE_ADDR			0xD0
#define RTC_READ_ADDR			0xD1
#define SET_TIME				0x01
#define OFF						0x00

//RTC Timekeeper register addresses
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
#define MINUTES_INIT			0x32
#define HOURS_INIT				0x14
#define DAY_INIT				WEDNESDAY
#define DATE_INIT				0x17
#define MONTH_INIT				0x11
#define YEAR_INIT				0x21

//Timer specific macros
#define TCNT1_MAX				65535
#define TCNT0_MAX				255
#define TCNT_MIN				0

#define ONE_COUNT_TIME_1024		0.128
#define ONE_COUNT_TIME_64		0.008

#define INCREMENT(x)	x++
#define DECREMENT(x)	x--

//Clock divider macros
#define STOP_TIMER				0x00
#define NO_PRESCALAR			( 1 << CS00 )
#define SET_PRESCALAR_8			( 1 << CS01 )
#define SET_PRESCALAR_64		( 1 << CS00 ) | ( 1 << CS01 )
#define SET_PRESCALAR_256		( 1 << CS02 )
#define SET_PRESCALAR_1024		( 1 << CS10 ) | ( 1 << CS12 )

//I2C specific macros
#define MY_SCL			PD0
#define MY_SDA			PD1

#define SET_SCL			PORTD |= (1 << MY_SCL)
#define CLR_SCL			PORTD &= ~(1 << MY_SCL)
#define SET_SDA			PORTD |= (1 << MY_SDA)
#define CLR_SDA			PORTD &= ~(1 << MY_SDA)

#define REPEAT_START		0

#define CLOCK_PERIOD		10

#define BIT_ACK				0
#define BIT_NACK			1

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
void I2C_init(void);

void I2C_start();
void I2C_send_bit(int);
int I2C_send_byte(int);
int I2C_read_byte(void);
void I2C_stop();

void RTC_init(RTC_i2c*);
void RTC_set_time(RTC_i2c);
void RTC_get_time(RTC_i2c*);

int lcd_time_display(RTC_i2c);
void time_display(RTC_i2c);
void print_time(int, int, RTC_i2c);

void write_seg(int,int);
int bcd_to_dec(int);

void timer1_delay_ms(long int);
void start_timer(long int);
void stop_timer(void);

int num_convert(int, char*);
void string_cpy( char*, char*);
int string_count(char*);

/*********************************************************************************************************/
