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
#define PARSE_CONST         9

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
    uint8_t answer;
    uint8_t temp_min[3];
    uint8_t temp_max[3];
    uint8_t http_respon_data[64]="12,25";
    volatile uint8_t room1_temp;
    char s[2] = ",";
    char *token;
    
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
    
       // memset(temp_min,'\0',2);
        //memset(temp_max,'\0',2);
        
//lcd_write_string_4d(" ");
        //lcd_write_string_4d(temp_max);
    /*
     * now enable interrupt, since UART library is interrupt controlled
     */
    sei();
    lcd_write_instruction_4d(lcd_Clear);
    _delay_ms(10);
    if(sim900_gprs_is_opened())
    {
        answer = sim900_gprs_close_connection();
        _delay_ms(2000);
    }
    
    lcd_write_string_4d("Opening GPRS...");
        sim900_gprs_open_connection(
            (const uint8_t*)"internet", (const uint8_t*)"MobiCom ", (const uint8_t*)" ");
  lcd_write_instruction_4d(lcd_Clear);
    _delay_ms(10);
  lcd_write_string_4d("Initializing HTTP...");


    while(1)
    {
    sim900_http_send_data(
            HTTP_POST,
            (const uint8_t*)"http://intense-fjord-78468.herokuapp.com/temp",
      (const uint8_t*)"{\"temp\": 16}",
            64,
            http_respon_data);
        token = strtok(http_respon_data,s);
        strcpy(temp_min,token);
        token = strtok(NULL,s);
        strcpy(temp_max,token);
       /* temp_min[0] = http_respon_data[PARSE_CONST-1];
        if(http_respon_data[PARSE_CONST]=='\"')
        {
          temp_min[1] = '\0';
          temp_min[2] = '\0';
          temp_max[0] = http_respon_data[PARSE_CONST+9];
          if(http_respon_data[PARSE_CONST+10]!='\"')
          {
            temp_max[1] = http_respon_data[PARSE_CONST+10];
            temp_max[2] = '\0'; 
          }
          else
            temp_max[1] = '\0'; 
            temp_min[2] = '\0';
        }
        else
        {          
          temp_min[1] = http_respon_data[PARSE_CONST];
          temp_min[2] = '\0';
          temp_max[0] = http_respon_data[PARSE_CONST+11];
          if(http_respon_data[PARSE_CONST+11]!='\"')
          {
            temp_max[1] = http_respon_data[PARSE_CONST+12];
            temp_max[2] = '\0'; 
          } 
          else
            temp_max[1] = '\0'; 
        }*/
        lcd_write_instruction_4d(lcd_Clear);
        _delay_ms(10);
        lcd_write_string_4d("main     min: ");
        lcd_write_string_4d(temp_min);
        lcd_write_instruction_4d(lcd_SetCursor|lcd_LineTwo);
        _delay_ms(10);
        lcd_write_string_4d("temp:   ");
        lcd_write_string_4d(room1_temp);
        lcd_write_string_4d(" ");
        lcd_write_string_4d("max: ");
        lcd_write_string_4d(temp_max);
  }
    
}

