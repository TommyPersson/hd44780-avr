#ifndef HD44780_H_
#define HD44780_H_

#include <inttypes.h>

#include <stdbool.h>

/*************** User functions ****************/

void hd44780_set_e_port_and_pin(volatile uint8_t *ddr, 
                                volatile uint8_t *port, 
                                uint8_t pin); 

void hd44780_init(void);

void hd44780_put_char(char c);

void hd44780_put_char_xy(char c, int x, int y);

void hd44780_write_line(char* string);

void hd44780_write_string(char* string);

void hd44780_write_string_xy(char* string, int x, int y);

void hd44780_goto(int x, int y);

void hd44780_newline(void);

bool hd44780_busy(void);

/*************** Primitive functions  ***************/

void hd44780_clear_display(void);

void hd44780_return_home(void);

void hd44780_entry_mode_set(uint8_t incdec, uint8_t shift);

void hd44780_display_on_off_control(uint8_t display, uint8_t cursor, uint8_t blink);

void hd44780_cursor_or_display_shift(uint8_t sc, uint8_t rl);

void hd44780_function_set(uint8_t data_length, uint8_t line_number, uint8_t font_type);

void hd44780_set_cgram_address(uint8_t ac);

void hd44780_set_ddram_address(uint8_t ac);

void hd44780_read_busy_flag_and_address(uint8_t* busy_flag, uint8_t* ac);

void hd44780_write_data_to_ram(uint8_t data);

void hd44780_read_data_from_ram(uint8_t* data);

void hd44780_output_data(uint8_t data, uint8_t rs, uint8_t rw);

#endif
