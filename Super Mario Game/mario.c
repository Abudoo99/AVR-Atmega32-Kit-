/*******************************************************************************************************
*   TASK :
*
* 	1. MARIO game using buttons
*	
*
*	Date :  19/11/2021	10:20:00 AM
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
*	Buzzer pin : PD3
*
* 	Use INT0 button to play game
*
********************************************************************************************************
											 HEADER FILES										
*******************************************************************************************************/

#include "mario.h"
#include "lcd.h"

/*******************************************************************************************************
									   GLOBAL VARIABLES AND ISRs									
*******************************************************************************************************/

int  line_mario = LINE2;

ISR( INT0_vect )
{
	if ( line_mario == LINE1 )
	{
		line_mario = LINE2;
	}
	else
	{
		line_mario = LINE1;		
	}
}

/*******************************************************************************************************
											 MAIN FUNCTION										
*******************************************************************************************************/

int main(void)
{
	int move_mario = 0, move_obs = LINE_END_OBSTACLE, line_obs = LINE2;
	int obstacle, obs_num, obs_count;
	
	int tick = 100 ; //Initial time in which the game starts running

	int score = 0;	//Variable to count score in game

	char *score_buf = NULL;	//Buffer to store score

	//Pixel data for custom characters used in game
	unsigned char mario[8] = {0x0E, 0x0E, 0x0E, 0x04, 0x1F, 0x04, 0x0A, 0x11};
	unsigned char mario_run[8] = {0x0E, 0x0E, 0x0E, 0x04, 0x1F, 0x04, 0x0A, 0x0A};
	unsigned char obstacle1[8] = {0x04, 0x15, 0x0E, 0x15, 0x0E, 0x15, 0x0E, 0x04};
	unsigned char obstacle2[8] = {0x04, 0x04, 0x07, 0x14, 0x1C, 0x05, 0x07, 0x04};
	unsigned char obstacle3[8] = {0x1F, 0x04, 0x1F, 0x04, 0x04, 0x1F, 0x04, 0x1F};

	initialize_modules();

	//Allocating memory for score buffer
	score_buf = (char *)malloc( SCORE_SIZE * sizeof(char) );
	if (score_buf == NULL)
	{
		PORTD |= (1 << PD3);
	}

	//Storing game characters at corresponding CGRAM addresses 
	lcd_create_char( 0, mario );
	lcd_create_char( 1, mario_run );
	lcd_create_char( 2, obstacle1 );
	lcd_create_char( 3, obstacle2 );
	lcd_create_char( 4, obstacle3 );


	//Initial display of character
	for (move_mario = 0; move_mario < STARTING_POSITION ; move_mario += 1)
	{
		lcd_set_cursor(move_mario, LINE2);
		lcd_data( MARIO_DATA );
		timer1_delay_ms(100);
		lcd_set_cursor(move_mario, LINE2);
		lcd_data( MARIO_RUN_DATA );
		timer1_delay_ms(100);
		lcd_command( CLR_SCR );
		lcd_command( MOVE_TO_BEG_LINE2 );			
	}

	//Initializing obstacles
	obstacle = RANDOM_OBSTACLE ;
	line_obs = RANDOM_OBS_LINE ;
	obs_count = RANDOM_OBS_COUNT ;  

	while(1)
	{
		//Checking if mario hit any obstacle
		if ( ( move_mario == move_obs ) && ( line_mario == line_obs ) )
		{
			lcd_command( CLR_SCR );
			lcd_printf("    GAME OVER");
			lcd_set_cursor(0,2);
			lcd_printf("  SCORE : ");
			lcd_printf(score_buf);
			break;
		}

		//Displaying current positions of mario and obstacles 
		lcd_set_cursor(move_mario, line_mario);
		lcd_data( MARIO_DATA );

		for (obs_num = 0; obs_num < obs_count; obs_num += 1)
		{
			lcd_set_cursor(move_obs + obs_num, line_obs);
			lcd_data( obstacle );			
		}
		timer1_delay_ms(tick);
		lcd_set_cursor(move_mario, line_mario);
		lcd_data( MARIO_RUN_DATA );
		timer1_delay_ms(tick);
		lcd_command( CLR_SCR );
		lcd_command( MOVE_TO_BEG_LINE2 );
		move_obs--;

		if (move_obs == 0)
		{
			if ( tick != FINAL_GAME_SPEED )
			{
				tick = tick - 10;				
			}
			
			//Generating random obstacles			
			obstacle = RANDOM_OBSTACLE ;
			line_obs = RANDOM_OBS_LINE ;
			obs_count = RANDOM_OBS_COUNT ;  

			move_obs = LINE_END_OBSTACLE;
		}

		//Increasing difficulty level of game based on score
		score++;

		if ( score == INCREASE_DIFFICULTY )
		{
			move_mario = INC_DIFF;
		}
		if ( score == MAX_DIFFICULTY )
		{
			move_mario = MAX_DIFF;
		}

		//Displaying score
		lcd_set_cursor(SCORE_POS, LINE1);
		sprintf(score_buf, "%d", score);
		lcd_printf(score_buf);
	}

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
	 *	=> TCNT value = ( delay_ms / 0.128ms )
	 *	
	 *	Since timer is 8 bit, we can have TCNT value only upto 255 
	 *	We have to loop the TCNT value for quotient number of times and then load the remainder 
	 *	for the final iteration 																	*/ 

	TCNT1 = TCNT1_MAX - ( delay_ms / ONE_COUNT_TIME_1024 );	
							
	TCCR1A = CLEAR_ALL;
	TCCR1B |= SET_PRESCALAR_1024;

	while( ( TIFR & ( 1 << TOV1 ) ) == 0 );		//Waiting until overflow occurs

	TCCR1B = CLEAR_ALL;
	TIFR = ( 1 << TOV1 );

	return;
}

/*--------------------------------------------------------------------------------------------------------
	Function performs initializations for various modules
----------------------------------------------------------------------------------------------------------
*   
*   Function Name 	: 	initialize_modules
*
*   Parameters 		:  	NONE
*
*   Return     		: 	NONE
*-------------------------------------------------------------------------------------------------------*/

void initialize_modules(void)
{ 
	//GPIO configurations

	DDRB = SET_ALL ;				//LCD data line output direction
	DDRD &= ~(1 << PD7);			//Configuring buzzer as input	
	DDRD |= LCD_CTRL_ENABLE;		//RS, RW, and EN set as output

	//Interrupt enabling for INT0

	GICR = INT0_ENABLE ;			
	MCUCR = INT0_RISING_EDGE_TRIG ;	
	sei();							//Enabling global interrupt

	//LCD configurations		
	lcd_init();	

	return;
}

/******************************************************************************************************/
