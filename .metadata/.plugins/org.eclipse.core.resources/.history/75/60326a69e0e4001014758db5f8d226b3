#ifndef BSP_LCD_H_
#define BSP_LCD_H_

#include "main.h"

/* Application configurable items */
#define LCD_GPIO_PORT					GPIOD

#define LCD_GPIO_RS						GPIO_PIN_0

#define LCD_GPIO_RW						GPIO_PIN_1
#define LCD_GPIO_EN						GPIO_PIN_7

#define LCD_GPIO_D4						GPIO_PIN_3
#define LCD_GPIO_D5						GPIO_PIN_4
#define LCD_GPIO_D6						GPIO_PIN_5
#define LCD_GPIO_D7						GPIO_PIN_6

/* LCD commands */
#define LCD_CMD_4DL_2N_5X8F				0x28 /* 4 bit, 2 lines, 5x8 font-size */
#define LCD_CMD_DON_CURON				0x0E
#define LCD_CMD_INCADD					0x06
#define LCD_CMD_DIS_CLEAR				0x01
#define LCD_CMD_DIS_RETURN_HOME			0x02



/* BSP APIs  */

/* Initialization LCD display */
void lcd_init(void);

/* Send instruction and data APIs*/
void lcd_send_command(uint8_t cmd);
void lcd_print_char(uint8_t data);
void lcd_print_string(char* message);

/* Command APIs */
void lcd_display_clear(void);
void lcd_display_return_home(void);
void lcd_set_cursor(uint8_t row, uint8_t column);



#endif /* BSP_LCD_H_ */
