
#ifndef USART_COMMAND_H_
#define USART_COMMAND_H_

#include "usart.h"

#include <stdlib.h>

struct USART_COMMAND
{
	
	char* command;
	uint16_t COMMAND_ID;
	int32_t value;
	uint8_t state;
	
}USART_COMMAND;

volatile uint8_t USART_COMMAND_RX_FLAG = 0;
volatile uint8_t USART_RX_FLAG = 0;

#define USART_COMMAND_BUFFER_SIZE 64

uint8_t USART_COMMAND_BUFFER[ USART_COMMAND_BUFFER_SIZE ];
volatile uint16_t USART_COMMAND_BUFFER_POSITION = 0; 

void USART_COMMAND_writeBuffer();
void USART_COMMAND_ISR_HANDLER();
uint8_t USART_COMMAND_resetBufferOn(uint8_t value);

ISR(USART_RXC_vect)
{
	USART_COMMAND_writeBuffer();
	USART_RX_FLAG = 0x01;
	USART_COMMAND_RX_FLAG = 0x01;
	USART_COMMAND_ISR_HANDLER();
}

void USART_COMMAND_writeBuffer()
{
	USART_COMMAND_BUFFER[ USART_COMMAND_BUFFER_POSITION ] = UDR;	
	USART_COMMAND_BUFFER_POSITION++;
	USART_COMMAND_BUFFER_POSITION = USART_COMMAND_BUFFER_POSITION % USART_COMMAND_BUFFER_SIZE;	
}

void USART_COMMAND_resetBuffer()
{
	uint16_t i = 0;
	
	while( i < USART_COMMAND_BUFFER_SIZE )
	{
		USART_COMMAND_BUFFER[i] = 0x01;
		i++;
	}
	
	USART_COMMAND_BUFFER_POSITION = 0;
}

void USART_COMMAND_resetBufferOnCharacter(char value)
{
	uint16_t i = 0;
	
	if( USART_COMMAND_BUFFER_POSITION == 0 )
	{
		return;
	}
	
	while( i < USART_COMMAND_BUFFER_SIZE )
	{
		if( USART_COMMAND_BUFFER[i] == value )
		{
			USART_COMMAND_resetBuffer();
			return;
		}
		
		i++;
	}	
}

uint8_t USART_COMMAND_checkForCommand(char* command)
{
	uint16_t N = 0;
	uint16_t counter = 0;

	if( command[0] != USART_COMMAND_BUFFER[0] )
	{
		return(0x00);
	}
	
	while( ( command[counter] != '\0' ) && ( counter < USART_COMMAND_BUFFER_SIZE ) )
	{
		counter++;
	}
	
	N = counter;
	counter = 0;
	
	while( counter < N )
	{
		if( command[counter] != USART_COMMAND_BUFFER[counter] )
		{
			return(0x00);
		}
		
		counter++;
	}
	
	return(0x01);
}

uint8_t USART_COMMAND_checkForCommand_Partial(char* command)
{
	uint16_t N = 0;
	uint16_t i = 0;
	uint16_t j = 0;

	if( USART_COMMAND_BUFFER[0] != command[0] )
	{
		return(0x00);
	}
	
	while( ( command[N] != '\0' ) && ( N < USART_COMMAND_BUFFER_SIZE ) )
	{
		N++;
	}
	
	while( ( USART_COMMAND_BUFFER[i] != command[0] ) && ( i < USART_COMMAND_BUFFER_SIZE ) )
	{
		i++;
	}
	
	j = 0;
	
	while( j < N )
	{
		if( USART_COMMAND_BUFFER[i] != command[j] )
		{
			return(0x00);
		}
		
		i++;
		j++;
		
		i = i % USART_COMMAND_BUFFER_SIZE;
	}
	
	return(0x01);
}

int16_t USART_COMMAND_getIndex(char c)
{
	int i = 0;
	
	while( i < USART_COMMAND_BUFFER_SIZE )
	{
		if( USART_COMMAND_BUFFER[i] == c )
		{
			return(i);
		}
		
		i++;
	}
	
	return(-1);
}

int32_t USART_COMMAND_toInt(char* buffer, char stop_byte)
{
	uint16_t N = 0;
	uint16_t i = 0;
	int32_t pow10 = 1;
	int32_t result = 0;
	
	while( buffer[N] != stop_byte )
	{
		N++;
	}
	
	if( buffer[0] != '-' )
	{	
		while( i < N )
		{
			result += (int32_t)( buffer[N-1-i] - '0' ) * pow10;
			pow10 = pow10 * 10;
			i++;
		}
	}
	else
	{
		while( i < (N-1) )
		{
			result += (int32_t)( buffer[N-1-i] - '0' ) * pow10;
			pow10 = pow10 * 10;
			i++;
		}		
		
		result = -result;
	}
	
	return( result );
}

uint8_t USART_COMMAND_isPresent(char key)
{
	uint16_t i = 0;
	
	if( USART_COMMAND_BUFFER_POSITION == 0 )
	{
		return(0x00);
	}
	
	while( i < USART_COMMAND_BUFFER_POSITION )
	{
		if( USART_COMMAND_BUFFER[i] == key )
		{
			return(0x01);
		}
		
		i++;
	}
	
	return(0x00);
}

int32_t USART_COMMAND_getNumber()
{
	char buffer[20];
	
	int16_t start_index;
	int16_t end_index;
	
	int16_t i = 0;
	int16_t N = 0;
	
	//	Get all the chcracters between the command seperator and the end of string character.
	
	i = 0;
	
	while( i < USART_COMMAND_BUFFER_SIZE )
	{
		if( USART_COMMAND_BUFFER[i] == ':' )
		{
			start_index = i;
		}
		
		if( USART_COMMAND_BUFFER[i] == '\n' )
		{
			end_index = i;
		}
		
		i++;
	}
		
	if( start_index == -1 ) return(1);
	if( end_index == -1 ) return(1);
	
	N = end_index - start_index;
	
	i = 0;
	
	while( i < N )
	{
		buffer[i] = USART_COMMAND_BUFFER[start_index+1+i];
		i++;
	}
			
	return( USART_COMMAND_toInt(buffer,'\n') );	
}

void USART_COMMAND_printInteger(int32_t N)
{
	USART_printInteger(N);
	USART_transmit('\n');
}

#endif /* USART_COMMAND_H_ */