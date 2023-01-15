
#define F_CPU 16000000UL

#include <avr/io.h>

#include "LCD_8_BIT_16x2.h"
#include "usart.h"
#include "USART_COMMAND.h"

int32_t counter = 0;
uint8_t receivedValidCommand = 0;

struct USART_COMMAND command_0;
struct USART_COMMAND command_1;
struct USART_COMMAND command_2;
struct USART_COMMAND command_3;
struct USART_COMMAND command_4;
struct USART_COMMAND command_5;

void USART_COMMAND_ISR_HANDLER()
{
	if( USART_COMMAND_isPresent('\n') )
	{
		if( USART_COMMAND_checkForCommand(command_0.command) )	command_0.state = 0x01;	
		if( USART_COMMAND_checkForCommand(command_1.command) )	command_1.state = 0x01;		
		
		if( USART_COMMAND_checkForCommand_Partial(command_2.command) )
		{
			command_2.value = USART_COMMAND_getNumber();
			command_2.state = 0x01;	
		}	
		
		if( USART_COMMAND_checkForCommand(command_3.command) )	command_3.state = 0x01;
		if( USART_COMMAND_checkForCommand(command_4.command) )	command_4.state = 0x01;		
		if( USART_COMMAND_checkForCommand(command_5.command) )	command_5.state = 0x01;
		
		USART_COMMAND_resetBuffer();
	}
}

int main(void)
{
	USART_init(9600);
	USART_RX_interruptEnable();
	
	LCD_init(LCD_DISPLAY_ON);
			
	DDRA |= ( 1 << 0 ) | ( 1 << 1 );
	PORTA |= ( 1 << 0 ) | ( 1 << 1 );
	
	command_0.command = "LED_0_TOGGLE\n";
	command_0.state   = 0;
	
	command_1.command = "LED_1_TOGGLE\n";
	command_1.state = 0;
	
	command_2.command = "SET_COUNTER_VALUE";
	command_2.state = 0;
	command_2.value = 0;

	command_3.command = "INCREMENT_COUNTER\n";
	command_3.state = 0;
	
	command_4.command = "DECREMENT_COUNTER\n";
	command_4.state = 0;

	command_5.command = "READ_COUNTER_VALUE\n";
	command_5.state = 0;
		
	sei();
		
    while (1) 
    {		
		if( command_0.state )
		{
			PORTA ^= ( 1 << 0 );
			command_0.state = 0;	
		}

		if( command_1.state )
		{
			PORTA ^= ( 1 << 1 );
			command_1.state = 0;
		}
		
		if( command_2.state )
		{
			LCD_write(LCD_CLEAR_DISPLAY, LCD_COMMAND);
			LCD_setCursor(0,0);
			LCD_print_int32( counter = command_2.value );
			
			command_2.state = 0;	
		}

		if( command_3.state )
		{
			counter++;
			
			LCD_write(LCD_CLEAR_DISPLAY, LCD_COMMAND);
			LCD_setCursor(0,0);
			LCD_print_int32( counter );
			
			command_3.state = 0;
		}
		
		if( command_4.state )
		{
			counter--;
			
			LCD_write(LCD_CLEAR_DISPLAY, LCD_COMMAND);
			LCD_setCursor(0,0);
			LCD_print_int32( counter );
			
			command_4.state = 0;
		}		
		
		if( command_5.state )
		{
			USART_COMMAND_printInteger(counter);
			command_5.state = 0;
		}
		
    }
}

