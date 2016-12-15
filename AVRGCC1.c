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

#include "uart.h"
#include "lcd.h"
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

    /*
     *  Initialize UART library, pass baudrate and AVR cpu clock
     *  with the macro 
     *  UART_BAUD_SELECT() (normal speed mode )
     *  or 
     *  UART_BAUD_SELECT_DOUBLE_SPEED() ( double speed mode)
     */
    uart1_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) ); 
    
    /*
     * now enable interrupt, since UART library is interrupt controlled
     */
    sei();
    
    /*
     *  Transmit string to UART
     *  The string is buffered by the uart library in a circular buffer
     *  and one character at a time is transmitted to the UART using interrupts.
     *  uart_puts() blocks if it can not write the whole string to the circular 
     *  buffer
     */
    uart1_puts("String stored in SRAM\n");
    
    /*
     * Transmit string from program memory to UART
     */
    uart1_puts_P("String stored in FLASH\n");
    
        
    /* 
     * Use standard avr-libc functions to convert numbers into string
     * before transmitting via UART
     */     
    itoa( num, buffer, 10);   // convert interger into string (decimal format)         
    uart1_puts(buffer);        // and transmit string to UART

    /*
     * Transmit single character to UART
     */
    uart1_putc('\r');
    
    for(;;)
    {
        /*
         * Get received character from ringbuffer
         * uart_getc() returns in the lower byte the received character and 
         * in the higher byte (bitmask) the last receive error
         * UART_NO_DATA is returned when no data is available.
         *
         */
        c = uart1_getc();
        if ( c & UART_NO_DATA )
        {
            /* 
             * no data available from UART 
             */
        }
        else
        {
            /*
             * new data available from UART
             * check for Frame or Overrun error
             */
            if ( c & UART_FRAME_ERROR )
            {
                /* Framing Error detected, i.e no stop bit detected */
                uart1_puts_P("UART Frame Error: ");
            }
            if ( c & UART_OVERRUN_ERROR )
            {
                /* 
                 * Overrun, a character already present in the UART UDR register was 
                 * not read by the interrupt handler before the next character arrived,
                 * one or more received characters have been dropped
                 */
                uart1_puts_P("UART Overrun Error: ");
            }
            if ( c & UART_BUFFER_OVERFLOW )
            {
                /* 
                 * We are not reading the receive buffer fast enough,
                 * one or more received character have been dropped 
                 */
                uart1_puts_P("Buffer overflow error: ");
            }
            /* 
             * send received character back
             */
            uart1_putc( (unsigned char)c );
        }
    }
    
}

