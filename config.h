#ifndef HD44780_CONFIG_H_
#define HD44780_CONFIG_H_

#include <avr/io.h>

#define LCD_CONTROL_PORT PORTD
#define LCD_CONTROL_PORT_DDR DDRD
#define LCD_RS PD5
#define LCD_RW PD4

#define LCD_DATA_PORT PORTA
#define LCD_DATA_PORT_PINS PINA
#define LCD_DATA_PORT_DDR DDRA
#define LCD_DB0 PA0
#define LCD_DB1 PA1
#define LCD_DB2 PA2
#define LCD_DB3 PA3
#define LCD_DB4 PA4
#define LCD_DB5 PA5
#define LCD_DB6 PA6
#define LCD_DB7 PA7

#endif
