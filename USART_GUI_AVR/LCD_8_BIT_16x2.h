
#ifndef LCD_H_
#define LCD_H_

#include <util/delay.h>

#define LCD_DDR			DDRC
#define LCD_PORT		PORTC

#define LCD_RS_DDR		DDRA
#define LCD_RS_PORT		PORTA
#define LCD_RS_MASK		( 1 << 6 )

#define LCD_EN_DDR		DDRA
#define LCD_EN_PORT		PORTA
#define LCD_EN_MASK		( 1 << 7 )

#define LCD_COMMAND								0x00
#define LCD_DATA								0x01


#define LCD_4_BIT_MODE							0x20
#define LCD_8_BIT_MODE							0x30
#define LCD_1_LINE_MODE							0x20
#define LCD_2_LINE_MODE							0x28
#define LCD_FONT_SIZE_5x8						0x20
#define LCD_FONT_SIZE_5x11						0x24

#define LCD_CLEAR_DISPLAY						0x01

#define LCD_DISPLAY_ON							0x0C
#define LCD_CURSOR_ON							0x02
#define LCD_BLINK_ON							0x01
#define LCD_SHIFT_CURSOR_LEFT					0x10
#define LCD_SHIFT_CURSOR_RIGHT					0x14

#define LCD_RIGHT_ARROW							0x7D

uint8_t __LCD_ROW__			=	0x00;
uint8_t	__LCD_COLUMN__		=	0x00;	

uint8_t reverseByte(uint8_t dByte)
{
	dByte = (dByte & 0xF0) >> 4 | (dByte & 0x0F) << 4;
	dByte = (dByte & 0xCC) >> 2 | (dByte & 0x33) << 2;
	dByte = (dByte & 0xAA) >> 1 | (dByte & 0x55) << 1;
	return(dByte);
}

void LCD_write(uint8_t byte,uint8_t type)
{
	byte = reverseByte(byte); 
	
	if( type )
	{
		LCD_RS_PORT |= LCD_RS_MASK;
	}
	else
	{
		LCD_RS_PORT &= ~LCD_RS_MASK;
	}
	
	LCD_PORT = byte;
	
	_delay_us(10);
	LCD_EN_PORT |= LCD_EN_MASK;
	_delay_us(10);
	LCD_EN_PORT &= ~LCD_EN_MASK;
	
	_delay_ms(2);
}


void LCD_init(uint8_t configurations)
{
	LCD_DDR = 0xFF;
	LCD_EN_DDR |= LCD_EN_MASK;
	LCD_RS_DDR |= LCD_RS_MASK;
	
	_delay_ms(200);

	LCD_write( LCD_8_BIT_MODE | LCD_2_LINE_MODE ,LCD_COMMAND);
	_delay_ms(10);
	LCD_write( LCD_8_BIT_MODE | LCD_2_LINE_MODE ,LCD_COMMAND);
	_delay_ms(10);	
	LCD_write( LCD_DISPLAY_ON | configurations , LCD_COMMAND );
	_delay_ms(10);	
	LCD_write( LCD_CLEAR_DISPLAY, LCD_COMMAND );
	_delay_ms(10);
	LCD_write( 0x04, LCD_COMMAND );
	_delay_ms(10);

	LCD_write( LCD_8_BIT_MODE | LCD_2_LINE_MODE ,LCD_COMMAND);
	_delay_ms(10);
	LCD_write( LCD_8_BIT_MODE | LCD_2_LINE_MODE ,LCD_COMMAND);
	_delay_ms(10);
	LCD_write( LCD_DISPLAY_ON | configurations, LCD_COMMAND );
	_delay_ms(10);
	LCD_write( LCD_CLEAR_DISPLAY, LCD_COMMAND );
	_delay_ms(10);
}


void LCD_setCursor(uint8_t row, uint8_t column)
{
	if( row == 0 )
	{
		LCD_write( 0x80 + column, LCD_COMMAND );
	}
	else
	{
		LCD_write( 0xC0 + column, LCD_COMMAND );
	}
	
	__LCD_ROW__ = row;
	__LCD_COLUMN__ = column;
}



void LCD_print_string(char* data)
{
	uint16_t counter = 0;
	
	while( data[counter] != '\0' )
	{
		LCD_write( data[counter], LCD_DATA );
		
		__LCD_COLUMN__ += 0x01;
		
		if(__LCD_COLUMN__ == 0x10)
		{
			__LCD_COLUMN__ = 0x00;
			
			if(__LCD_ROW__ == 0x00)
			{
				__LCD_ROW__ = 0x01;
			}
			else if( __LCD_ROW__ == 0x01 )
			{
				__LCD_ROW__ = 0x00;
			}
			
			LCD_setCursor(__LCD_ROW__,__LCD_COLUMN__);
		}
		
		counter++;
	}
}


void LCD_print_int16(int16_t N)
{
	uint16_t counter = 0;
	uint8_t digit[5];	
	int16_t tempVar = 0;
	
	digit[4] = 0x30;
	digit[3] = 0x30;
	digit[2] = 0x30;
	digit[1] = 0x30;
	digit[0] = 0x30;
	
	if( N == 0 )
	{
		LCD_write( '0' , LCD_DATA );
		return;
	}
	
	if( N < 0 )
	{
		LCD_write( '-', LCD_DATA );
		tempVar = -N;
	}
	else
	{
		tempVar = N;
	}
	
	counter = 5;
	
	while(tempVar)
	{
		counter--;
		digit[counter] = 0x30 + (uint8_t)( tempVar % 10 );
		tempVar /= 10;
	}
	
	counter = 0;
	
	while( digit[counter] == '0' )
	{
		counter++;
	}
	
	while( counter < 5 )
	{
		LCD_write( digit[counter], LCD_DATA );
		counter++;
	}
}

void LCD_itoa(uint32_t N, char* buffer, uint16_t n_digits)
{
	int i;
		
	for( i = 0 ; i < n_digits ; i++ )
	{
		buffer[i] = '0' + (char)( N % 10 );
		N /= 10;
	}
}

void LCD_printCharactersReverse(char* buffer, uint16_t N)
{
	int i;
	
	for( i = 0 ; i < N ; i++ )
	{
		LCD_write( buffer[N-1-i], LCD_DATA );
	}	
}

void LCD_print_int32(int32_t N)
{
	uint16_t counter = 0;
	uint8_t digit[10];
	int32_t tempVar = 0;
	
	digit[9] = 0x30;
	digit[8] = 0x30;
	digit[7] = 0x30;
	digit[6] = 0x30;
	digit[5] = 0x30;
	digit[4] = 0x30;
	digit[3] = 0x30;
	digit[2] = 0x30;
	digit[1] = 0x30;
	digit[0] = 0x30;
	
	if( N == 0 )
	{
		LCD_write( '0' , LCD_DATA );
		return;
	}
	
	if( N < 0 )
	{
		LCD_write( '-', LCD_DATA );
		tempVar = -N;
	}
	else
	{
		tempVar = N;
	}
	
	counter = 10;
	
	while(tempVar)
	{
		counter--;
		digit[counter] = 0x30 + (uint8_t)( tempVar % 10 );
		tempVar /= 10;
	}
	
	counter = 0;
	
	while( digit[counter] == '0' )
	{
		counter++;
	}
	
	while( counter < 10 )
	{
		LCD_write( digit[counter], LCD_DATA );
		counter++;
	}
}


void LCD_print_float(float N, uint16_t precision)
{
	float tempVar = 0;
	int32_t tempVar_0 = 0;
	int32_t tempVar_1 = 0;
	uint8_t buffer[5];
	uint16_t counter = 0;
	
	if( N == 0.0f )
	{
		LCD_write('0', LCD_DATA);
		return;
	}
	
	if( N < 0 )
	{
		LCD_write('-', LCD_DATA);
		tempVar = -N;
	}
	else
	{
		tempVar = N;		
	}
	

	LCD_print_int32( (int32_t)tempVar );
	
	switch(precision)
	{
		case(1):{	tempVar_0 = (int32_t)( 10.0f * tempVar );		tempVar_1 = 10 * (int32_t)tempVar;		};			break;
		case(2):{	tempVar_0 = (int32_t)( 100.0f * tempVar );		tempVar_1 = 100 * (int32_t)tempVar;		};			break;			
		case(3):{	tempVar_0 = (int32_t)( 1000.0f * tempVar );		tempVar_1 = 1000 * (int32_t)tempVar;	};			break;
		case(4):{	tempVar_0 = (int32_t)( 10000.0f * tempVar );	tempVar_1 = 10000 * (int32_t)tempVar;	};			break;
		case(5):{	tempVar_0 = (int32_t)( 100000.0f * tempVar );	tempVar_1 = 100000 * (int32_t)tempVar;	};			break;
	}
	
	tempVar_0 = tempVar_0 - tempVar_1;
	
	buffer[4] = 0x30;
	buffer[3] = 0x30;
	buffer[2] = 0x30;
	buffer[1] = 0x30;
	buffer[0] = 0x30;
	 	 
	 if(precision)
	 {
		 LCD_write('.', LCD_DATA);
	 }
	 
	 counter = 0;
	 
	 while( counter < 5 )
	 {
		 buffer[4-counter] = 0x30 + (uint8_t)( tempVar_0 % 10 );
		 tempVar_0 /= 10;
		 counter++;
	 }

	counter = 5-precision;
	
	while( counter < 5 )
	{
		LCD_write( buffer[counter], LCD_DATA );
		counter++;
	}
}


#endif /* LCD_H_ */