

#ifndef USART_H_
#define USART_H_

#include <util/delay.h>
#include <avr/interrupt.h>

void USART_init(uint32_t baud_rate);
void USART_transmit(uint8_t data);

uint8_t USART_receive();
void USART_RX_interruptEnable();

void USART_newline(uint16_t N);

void USART_printString(char* data);
void USART_printStringUpto(char* data, char stop_character);

void USART_init(uint32_t baud_rate)
{
	uint32_t baud_raw;
	
	baud_raw = ( F_CPU / baud_rate / 16 ) - 1;
	
	UBRRH = (uint8_t)( baud_raw >> 8 );
	UBRRL = (uint8_t)( baud_raw );
	
	UCSRB = ( 1 << RXEN ) | ( 1 << TXEN );
	UCSRC = ( 1 << URSEL ) | ( 3 << UCSZ0 );
}

void USART_RX_interruptEnable()
{
	UCSRB |= ( 1 << RXCIE );
	sei();
}

void USART_TX_interruptEnable()
{
	UCSRB |= ( 1 << TXCIE );
	sei();
}

void USART_transmit(uint8_t data)
{
	while( ( UCSRA & ( 1 << UDRE ) ) == 0x00 );
	UDR = data;
}

uint8_t USART_receive()
{
	while( ( UCSRA & ( 1 << RXC ) ) == 0x00 );	
	return UDR;
}

int32_t USART_atoi(char* data)
{
	int32_t N = 0;
	uint16_t N_d = 0;			//	Number of digits.
	int16_t i = 0;	
	uint8_t buffer[10];
	int32_t mult = 1;

	if( ( data[0] != '-' ) && ( data[0] != '+' ) ) 
	{
		while( data[i] != '\0' )
		{
			buffer[i] = data[i];
			N_d++;
			i++;
		}
		
		for( i = 0 ; i < N_d ; i++ )
		{
			N += ( mult * (int32_t)(buffer[N_d-1-i] -'0') );
			mult *= 10;
		} 
	}
	else
	{
		i = 1;
		
		while( data[i] != '\0' )
		{
			buffer[i-1] = data[i];
			N_d++;
			i++;
		}
		
		for( i = 0 ; i < N_d ; i++ )
		{
			N += ( mult * (int32_t)(buffer[N_d-1-i] - '0') );
			mult *= 10;
		}
		
		if( data[0] == '+' )
		{
			N = N;
		}
		else if( data[0] == '-' )
		{
			N = -N;
		}		
	}
	
	return(N);
}

/********	General functions.	*****/

void USART_printString(char* data)
{
	uint16_t i = 0;
	
	while( data[i] != '\0' )
	{
		USART_transmit( data[i] );
		i++;
	}
}

void USART_newline(uint16_t n)
{
	uint16_t i = 0;
	
	for( i = 0 ; i < n ; i++ )
	{
		USART_transmit('\r');
		USART_transmit('\n');
	}
}

void USART_printInteger(int32_t N)
{
	uint8_t buffer[10];
	int32_t t = 0;
	uint16_t i = 0;
	
	if( N == 0 )
	{
		USART_transmit('0');
		return;
	}
	else if( N < 0 )
	{
		USART_transmit('-');
		t = -N;
	}
	else
	{
		t = N;
	}
	
	for( i = 0 ; i < 10 ; i++ )
	{
		buffer[i] = '0' + (uint8_t)( t % 10 );
		t = t / 10; 
	}
	
	i = 0;
	
	while( buffer[9-i] =='0' )
	{
		i++;
	}
	
	while( i < 10 )
	{
		USART_transmit( buffer[9-i] );
		i++;
	}
}

void USART_printFloat(float N, uint16_t precision)
{
	int32_t N_f = 0;						
	uint16_t i = 0;
	
	uint8_t b[10];

	USART_printInteger( (int32_t)N );

	if( N < 0.0f ){ N = -N; };
	
	if( precision == 0 ){ return; };
	
	switch(precision)
	{
		case(1):{ N_f = (int32_t)( 10.0f * N ) - 10 * (int32_t)N; }; break;		
		case(2):{ N_f = (int32_t)( 100.0f * N ) - 100 * (int32_t)N; }; break;
		case(3):{ N_f = (int32_t)( 1000.0f * N ) - 1000 * (int32_t)N; }; break;			
		case(4):{ N_f = (int32_t)( 10000.0f * N ) - 10000 * (int32_t)N; }; break;
	}
	
	i = 0;
	
	for( i = 0 ; i < precision ; i++ )
	{
		b[i] = '0' + (uint8_t)( N_f % 10 );
		N_f = N_f / 10;
	}
	
	USART_transmit('.');
	
	for( i = 0 ; i < precision ; i++ )
	{
		USART_transmit(b[i]);
	}
}

void USART_printBufferUpto(uint8_t* buffer, uint8_t stop_byte)
{
	uint16_t counter = 0;
	
	while( 1 )
	{
		USART_transmit( buffer[counter] );
		
		if( buffer[counter] == stop_byte )
		{
			break;
		}
		
		counter++;
	}
}

void USART_printBuffer(uint8_t* buffer, int limit)
{
	int counter = 0;
	
	while( counter < limit )
	{
		USART_transmit( buffer[counter] );
		counter++;
	}
}

void USART_readNewline(char* buffer)
{
	char t;
	uint16_t counter = 0;
	
	while( ( t = USART_receive() ) != '\0' )
	{
		//USART_transmit(t);
		buffer[counter] = t;
		counter++;
	}	
}

#endif /* USART_H_ */