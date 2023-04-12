/*******************************************************************************************************
											 HEADER FILES										
*******************************************************************************************************/

#include "main.h"

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

void I2C_init(void)
{ 
	TWSR = 0x00;			//Initializing status register

   /*  	SCL FREQUENCY CALCULATION
	*
	*	SCL Frequency = ( CPU Clock ) / ( 16 + ( 2 * (TWBR) * (4^(TWPS)) ) ) ____________(1)
	*	where TWBR = Value of TWBR register
	*		  TWPS = Value of prescalar bits in TWSR 
	*
	*	For RTC, optimum frequency range is from 0 to 100 kHz
	*
	*	We will use SCL = 50 kHz and TWPS = 00
	*	Substituting these values
	*
	*   => TWBR = 72 (or) 0x48 							*/
	
	TWBR = 0x12;	

	return;
}

/*--------------------------------------------------------------------------------------------------------
	Function establishes communication between master and slave
----------------------------------------------------------------------------------------------------------
*   
*   Function Name 	: 	I2C_start
*
*   Parameters 		:  	unsigned char addr	-	Slave address
*						int start_type		-	START or REPEATED START
*
*   Return     		: 	PASS or FAIL
*-------------------------------------------------------------------------------------------------------*/

int I2C_start( unsigned char addr, int start_type )
{
	//Enabling I2C interface and initiating START condition
	TWCR |= (1 << TWINT) | (1 << TWSTA) | (1 << TWEN) ;
	
	//Waiting for transmission of START condition
	while ( (TWCR & (1 << TWINT)) == 0 );

	if ( start_type == START )
	{
		//Checking if START condition transmission is successful
		if ( ( TWSR & MASK_5_BITS_FROM_MSB ) != START_SUCCESS )
		{
			PORTC |= (1 << PC2);
			return FAIL;
		}		
	}
	
	else if ( start_type == REPEATED_START )
	{
		//Checking if repeated start condition transmission is successful
		if ( ( TWSR & MASK_5_BITS_FROM_MSB ) != REPEATED_START_SUCCESS )
		{
			PORTC |= (1 << PC3);
			return FAIL;
		}
	}

	//Storing address in data register
	TWDR = addr;

	//Clearing start condition and clearing INT flag

	TWCR &= ~(1 << TWSTA);
	TWCR |= (1 << TWINT);

	//Waiting until slave address is transmitted
	while ( (TWCR & (1 << TWINT)) == 0 );
	
	if ( start_type == START )
	{
		if ( ( TWSR & MASK_5_BITS_FROM_MSB ) != MT_SLAVE_ADDR_ACK )
		{
			PORTC |= (1 << PC4);		
			return FAIL;
		}
	}
	else 
	{
		if( ( TWSR & MASK_5_BITS_FROM_MSB ) != MR_SLAVE_ADDR_ACK )
		{
			PORTC |= (1 << PC4);		
			return FAIL;
		}
	}	

	return PASS;	
}

/*--------------------------------------------------------------------------------------------------------
	Function sends data and waits for ack
----------------------------------------------------------------------------------------------------------
*   
*   Function Name 	: 	I2C_send_data
*
*   Parameters 		:  	unsigned char data
*
*   Return     		: 	PASS or FAIL
*-------------------------------------------------------------------------------------------------------*/

int I2C_send_data( unsigned char data )
{
	//Storing data in TWDR
	TWDR = data;

	//Clearing INT flag
	TWCR |= (1 << TWINT);

	//Waiting until data is transmitted
	while ( (TWCR & (1 << TWINT)) == 0 );

	if ( ( TWSR & MASK_5_BITS_FROM_MSB ) != MT_DATA_ACK )
	{
		PORTC |= (1 << PC5);	
		return FAIL;
	}	

	return PASS;
}

/*--------------------------------------------------------------------------------------------------------
	Function reads ack from slave
----------------------------------------------------------------------------------------------------------
*   
*   Function Name 	: 	I2C_read
*
*   Parameters 		:  	int bit
*
*   Return     		: 	value in TWDR
*-------------------------------------------------------------------------------------------------------*/

unsigned char I2C_read( int bit )
{
	if ( bit == ACK )
	{
		//Enabling I2C interface and clearing INT flag
		TWCR |= (1 << TWINT) | (1 << TWEA);

		//Waiting until ack is transmitted
		while ( (TWCR & (1 << TWINT)) == 0 );

		if ( ( TWSR & MASK_5_BITS_FROM_MSB ) != MR_DATA_RECEIVE_ACK )
		{
			PORTC |= (1 << PC6);
		}		
	}
	else if ( bit == NACK )
	{
		//Enabling I2C interface and clearing INT flag		
		TWCR &= ~(1 << TWEA);
		TWCR |= (1 << TWINT);

		//Waiting until ack is transmitted
		while ( (TWCR & (1 << TWINT)) == 0 );

		if ( ( TWSR & MASK_5_BITS_FROM_MSB ) != MR_DATA_RECEIVE_NACK )
		{
			PORTC |= (1 << PC7);
		}		
	}

	return TWDR;	
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
	//Generating STOP condition

	TWCR |= (1 << TWINT) | (1 << TWSTO);
	//_delay_us(3);							//Aprroximate time taken to generate stop condition
}

/*******************************************************************************************************/
