#include "hd44780.h"
#include "config.h"

#include <util/delay.h>

#define LCD_ENABLE_RAISE_TIME_US 1

#define LCD_ROW1_START 0x00
#define LCD_ROW1_END 0x0F

#define LCD_ROW2_START 0x40
#define LCD_ROW2_END 0x4F

#define LCD_ROW3_START 0x10
#define LCD_ROW3_END 0x1F

#define LCD_ROW4_START 0x50
#define LCD_ROW4_END 0x5F

volatile static uint8_t *lcd_e_port;

volatile static uint8_t lcd_e_pin;

static uint8_t lcd_address_counter;

void hd44780_set_e_port_and_pin(volatile uint8_t *ddr, 
                                volatile uint8_t *port, 
                                uint8_t pin)
{

  *ddr |= _BV(pin);
  lcd_e_port = port;
  lcd_e_pin = pin;

}

void hd44780_init(void)
{

  LCD_CONTROL_PORT_DDR |= _BV(LCD_RS) | _BV(LCD_RW);
  
  /* Set 8-bit length, use all rows, small font */
  hd44780_function_set(1,1,0);

  /* entire display on, cursor on (for test), blink off */
  hd44780_display_on_off_control(1,1,0);

  hd44780_clear_display();

  /* cursor moves right, no shifting */
  hd44780_entry_mode_set(1,0);

  _delay_ms(500);

}

void hd44780_put_char(char c)
{

  switch(hd44780_address_counter)
  {
  case LCD_ROW1_END+1:
    hd44780_set_ddram_address(LCD_ROW2_START);
    break;
  case LCD_ROW2_END+1:
    hd44780_set_ddram_address(LCD_ROW3_START);
    break;
   case LCD_ROW3_END+1:
     hd44780_set_ddram_address(LCD_ROW4_START);
     break; 
  case LCD_ROW4_END+1:
    hd44780_set_ddram_address(LCD_ROW1_START);
    hd44780_clear_display();
    break;
  }

  /* swedish hack (latin1) */
  switch(c)
  {
  case 0xE5: /* å */
      c = c; /* doesn't exists in std table */
      break;
  case 0xE4: /* ä */
      c = 225;
      break;
  case 0xF6: /* ö */
      c = 239;
      break;
      /* large letters */
  case 0xC5: /* Å */
      c = c; /* doesn't exists in std table */
      break;
  case 0xC4: /* Ä */
      c = 225;
      break;
  case 0xD6: /* Ö */
      c = 239;
      break;
  }

  lcd_write_data_to_ram(c);

}

void hd44780_put_char_xy(char c, int x, int y)
{

  hd44780_goto(x,y);

  hd44780_put_char(c);
  
}

void hd44780_write_line(char* string)
{

  if (lcd_address_counter != LCD_ROW1_START &&
      lcd_address_counter != LCD_ROW2_START &&
      lcd_address_counter != LCD_ROW3_START &&
      lcd_address_counter != LCD_ROW4_START)
    hd44780_newline();
  
  hd44780_write_string(string);

}

void hd44780_write_string(char* string)
{

  uint8_t i;

  for (i = 0; i < 16 && string[i] != '\0'; i++)
    hd44780_put_char(string[i]);
 
}

void hd44780_write_string_xy(char* string, int x, int y)
{

  hd44780_goto(x,y);

  hd44780_write_string(string);

}

void hd44780_goto(int x, int y)
{

  if (x > 16 || x < 1)
    return;

  switch (y)
  {
  case 1:
    hd44780_set_ddram_address(LCD_ROW1_START+(x-1));
    break;
  case 2:
    hd44780_set_ddram_address(LCD_ROW2_START+(x-1));
    break;
  case 3:
    hd44780_set_ddram_address(LCD_ROW3_START+(x-1));
    break;
  case 4:
    hd44780_set_ddram_address(LCD_ROW4_START+(x-1));
    break;

  default:
    break;
  }

}

void hd44780_newline(void)
{

  if (lcd_address_counter <= LCD_ROW1_END+1)
    hd44780_set_ddram_address(LCD_ROW2_START);
  else
    if (lcd_address_counter >= LCD_ROW2_START &&
	lcd_address_counter <= LCD_ROW2_END+1)
      hd44780_set_ddram_address(LCD_ROW3_START);
    else 
      if (lcd_address_counter >= LCD_ROW3_START &&
	  lcd_address_counter <= LCD_ROW3_END+1)
	hd44780_set_ddram_address(LCD_ROW4_START);
      else 
	if (lcd_address_counter >= LCD_ROW4_START &&
	    lcd_address_counter <= LCD_ROW4_END+1)
	  hd44780_clear_display();

}

bool hd44780_busy(void)
{

  uint8_t busy_flag, throw_away;

  hd44780_read_busy_flag_and_address(&busy_flag, &throw_away);

  return (bool) busy_flag;

}

void hd44780_clear_display(void)
{

  uint8_t data = _BV(LCD_DB0);

  hd44780_output_data(data, 0, 0);

  _delay_ms(2);

  lcd_address_counter = 0x00;

}

void hd44780_return_home(void)
{

  uint8_t data = _BV(LCD_DB1);

  hd44780_output_data(data, 0, 0);

  _delay_ms(2);

  lcd_address_counter = 0x00;

}

void hd44780_entry_mode_set(uint8_t incdec, uint8_t shift)
{

  uint8_t data = _BV(LCD_DB2) | (incdec << LCD_DB1) | (shift << LCD_DB0);

  hd44780_output_data(data, 0, 0);

  _delay_ms(2);
  
}

void hd44780_display_on_off_control(uint8_t display, uint8_t cursor, uint8_t blink)
{

  uint8_t data =  _BV(LCD_DB3) | (display << LCD_DB2) | (cursor << LCD_DB1) | (blink << LCD_DB0);

  hd44780_output_data(data, 0, 0);

}

void hd44780_cursor_or_display_shift(uint8_t sc, uint8_t rl)
{

  uint8_t data = _BV(LCD_DB4) | (sc << LCD_DB3) | (rl << LCD_DB2);

  hd44780_output_data(data, 0, 0);
  
}

void hd44780_function_set(uint8_t data_length, uint8_t line_number, uint8_t font_type)
{

  uint8_t data = _BV(LCD_DB5) | (data_length << LCD_DB4) | (line_number << LCD_DB3) | (font_type << LCD_DB2);
  
  hd44780_output_data(data, 0, 0);
    
}

void hd44780_set_cgram_address(uint8_t ac)
{

  ac = 0x3F & ac; // discard two high bits

  ac |= _BV(LCD_DB6);

  hd44780_output_data(ac, 0, 0);

}

void hd44780_set_ddram_address(uint8_t ac)
{

  ac = 0x7F & ac; // discard highest bit

  lcd_address_counter = ac;

  ac |= _BV(LCD_DB7);

  hd44780_output_data(ac, 0, 0);

}

void hd44780_read_busy_flag_and_address(uint8_t* busy_flag, uint8_t* ac)
{

  LCD_DATA_PORT_DDR = 0x00;

  LCD_CONTROL_PORT &= ~_BV(LCD_RS) | _BV(LCD_RW);

  *lcd_e_port |= _BV(lcd_e_pin);

  _delay_us(LCD_ENABLE_RAISE_TIME_US);

  *ac = ~_BV(LCD_DB7) & LCD_DATA_PORT_PINS; 

  *busy_flag = (LCD_DB7 & LCD_DATA_PORT_PINS) >> LCD_DB7;

  *lcd_e_port &= ~_BV(lcd_e_pin);

}

void hd44780_write_data_to_ram(uint8_t data)
{

  hd44780_output_data(data, 1, 0);
  
  lcd_address_counter++;

}

void hd44780_read_data_from_ram(uint8_t* data)
{

  LCD_DATA_PORT_DDR = 0x00;

  LCD_CONTROL_PORT &= _BV(LCD_RS) | _BV(LCD_RW);

  *lcd_e_port |= _BV(lcd_e_pin);

  _delay_us(LCD_ENABLE_RAISE_TIME_US);

  *data = LCD_DATA_PORT_PINS; 

  *lcd_e_port &= ~_BV(lcd_e_pin);

  _delay_us(43);

}

void hd44780_output_data(uint8_t data, uint8_t rs, uint8_t rw)
{

  // mirror the data
  uint8_t mirrored_data = 0x00;
  uint8_t i;

  for (i = 0; i < 8; i++)
    mirrored_data |= (((0x01 << i) & data) >> i) << (7-i);

  LCD_DATA_PORT_DDR = 0xFF;

  if (rs == 0)
  {
    LCD_CONTROL_PORT &= ~_BV(LCD_RS);
  }  
  else
  {
    LCD_CONTROL_PORT |= _BV(LCD_RS);
  }

  if (rw == 0)
  {
    LCD_CONTROL_PORT &= ~_BV(LCD_RW);
  }
  else
  {
    LCD_CONTROL_PORT |= _BV(LCD_RW);
  }

  _delay_us(1);

  *lcd_e_port |= _BV(lcd_e_pin);

  LCD_DATA_PORT = mirrored_data;

  _delay_us(LCD_ENABLE_RAISE_TIME_US);

  *lcd_e_port &= ~_BV(lcd_e_pin);

  _delay_us(100);

}
