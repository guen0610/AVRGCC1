/*************************************************************************
Title:    Example program for the Interrupt controlled UART library
Author:   Peter Fleury <pfleury@gmx.ch>   http://tinyurl.com/peterfleury
File:     $Id: test_uart.c,v 1.7 2015/01/31 17:46:31 peter Exp $
Software: AVR-GCC 4.x
Hardware: AVR with built-in UART/USART

DESCRIPTION:
          This example shows how to use the UART library uart.c

*************************************************************************/
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "lcd.h"
#include "sim900.h"
#define __AVR_ATmega128A__
/* define CPU frequency in Hz in Makefile */
#ifndef F_CPU
#define F_CPU 8000000UL
#endif

/* Define UART buad rate here */
#define UART_BAUD_RATE      9600      


int main(void)
{
	DDRC = 0xFF;    // lcd pins
	// configure the microprocessor pins for the control lines
    lcd_E_ddr |= (1<<lcd_E_bit);                    // E line - output
    lcd_RS_ddr |= (1<<lcd_RS_bit);                  // RS line - output
 
    lcd_RS_port &= ~(1<<lcd_RS_bit);
	
    unsigned int c;
    char buffer[7];
    int  num=134;

    lcd_init_4d();
	lcd_write_instruction_4d(lcd_Clear);
    _delay_ms(10);
    lcd_write_string_4d("main");
    uint8_t response = 0;
    /*
     *  Initialize UART library, pass baudrate and AVR cpu clock
     *  with the macro 
     *  UART_BAUD_SELECT() (normal speed mode )
     *  or 
     *  UART_BAUD_SELECT_DOUBLE_SPEED() ( double speed mode)
     */
    sim900_init_uart(9600);


    
    /*
     * now enable interrupt, since UART library is interrupt controlled
     */
    sei();
    lcd_write_instruction_4d(lcd_Clear);
    _delay_ms(10);
	sim900_setup(SETUP_WAIT_INFINITE);
    _delay_ms(1000);
	lcd_write_string_4d("Sending sms");
    sim900_send_sms("+97699667842","hello borld\r\n\x1a");


    while(1)
	{
		lcd_write_character_4d('k');
		_delay_ms(1000);
	}
    
}

