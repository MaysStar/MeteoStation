#include "lcd.h"

static void write_4_bits(uint8_t num);
static void lcd_enable(void);

/*********************************************************************
 * @fn      		  - write_4_bits
 *
 * @brief             - This function allows us to write to the D7 - D4
 *
 * @param[in]         - number in hex, which we want to write
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

static void write_4_bits(uint8_t num)
{
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D4, ((num >> 0) & 0x1));
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D5, ((num >> 1) & 0x1));
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D6, ((num >> 2) & 0x1));
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D7, ((num >> 3) & 0x1));

	lcd_enable();
}

/*********************************************************************
 * @fn      		  - lcd_enable
 *
 * @brief             - This function enable write to LCD
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

static void lcd_enable(void)
{
	// from low to high
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_EN, GPIO_PIN_SET);
	udelay(40);

	// from high to low
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_EN, GPIO_PIN_RESET);
	udelay(100); // execution time > 37 micro seconds
}

/*********************************************************************
 * @fn      		  - mdelay
 *
 * @brief             - This function do delay for some milliseconds
 *
 * @param[in]         - number of milliseconds
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

void mdelay(uint32_t mseconds)
{
	for(__vo uint32_t i = 0; i < (mseconds * 1000); ++i);
}

/*********************************************************************
 * @fn      		  - udelay
 *
 * @brief             - This function do delay for some microseconds
 *
 * @param[in]         - number of microseconds
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

void udelay(uint32_t mcseconds)
{
	for(__vo uint32_t i = 0; i < mcseconds; ++i);
}

/*********************************************************************
 * @fn      		  - lcd_init
 *
 * @brief             - This function allows us to initialize LCD for 4 lines
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

void lcd_init(void)
{
	// 1. Configure GPIO pins which used for lcd connections
	GPIO_Handle_t LCD;

	LCD.pGPIOx = LCD_GPIO_PORT;

	LCD.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	LCD.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	LCD.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	LCD.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;

	LCD.GPIO_PinConfig.GPIO_PinNumber = LCD_GPIO_RS;
	GPIO_Init(&LCD);

	LCD.GPIO_PinConfig.GPIO_PinNumber = LCD_GPIO_RW;
	GPIO_Init(&LCD);

	LCD.GPIO_PinConfig.GPIO_PinNumber = LCD_GPIO_EN;
	GPIO_Init(&LCD);

	LCD.GPIO_PinConfig.GPIO_PinNumber = LCD_GPIO_D4;
	GPIO_Init(&LCD);

	LCD.GPIO_PinConfig.GPIO_PinNumber = LCD_GPIO_D5;
	GPIO_Init(&LCD);

	LCD.GPIO_PinConfig.GPIO_PinNumber = LCD_GPIO_D6;
	GPIO_Init(&LCD);

	LCD.GPIO_PinConfig.GPIO_PinNumber = LCD_GPIO_D7;
	GPIO_Init(&LCD);

	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RS, GPIO_PIN_RESET);

	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RW, GPIO_PIN_RESET);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_EN, GPIO_PIN_RESET);

	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D4, GPIO_PIN_RESET);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D5, GPIO_PIN_RESET);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D6, GPIO_PIN_RESET);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D7, GPIO_PIN_RESET);

	// 2. Do the LCD initialization
	mdelay(40);

	/* RS = 0, for lcd command */
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RS, GPIO_PIN_RESET);

	/* RW = 0, to write to LCD */
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RW, GPIO_PIN_RESET);

	/* Command for initialization */
	write_4_bits(0x3);

	mdelay(5);

	/* Command for initialization */
	write_4_bits(0x3);

	udelay(150);

	/* Command for initialization */
	write_4_bits(0x3);
	write_4_bits(0x2);

	/*// Return home
	write_4_bits(LCD_CMD_DIS_RETURN_HOME);*/

	/* Function set command */
	lcd_send_command(LCD_CMD_4DL_2N_5X8F);

	/* Display on cursor on */
	lcd_send_command(LCD_CMD_DON_CURON);

	/* Display clear */
	lcd_display_clear();

	/* Entry command */
	lcd_send_command(LCD_CMD_INCADD);
}
/*********************************************************************
 * @fn      		  - lcd_send_command
 *
 * @brief             - This function allows to send command to LCD
 *
 * @param[in]         - command
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

void lcd_send_command(uint8_t cmd)
{
	/* RS = 0 for LCD command */
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RS, GPIO_PIN_RESET);

	/* RW = 0, to write to LCD */
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RW, GPIO_PIN_RESET);

	/* send higher 4 bits */
	write_4_bits((cmd >> 4) & 0x0F);

	/* send lower 4 bits */
	write_4_bits(cmd & 0x0F);
}

/*********************************************************************
 * @fn      		  - lcd_send_char
 *
 * @brief             - This function allows to send data to LCD
 *
 * @param[in]         - data
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

void lcd_print_char(uint8_t data)
{
	/* RS = 1 for LCD user data */
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RS, GPIO_PIN_SET);

	/* RW = 0, to write to LCD */
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RW, GPIO_PIN_RESET);

	/* send higher 4 bits */
	write_4_bits(data >> 4);

	/* send lower 4 bits */
	write_4_bits(data & 0x0F);
}

/*********************************************************************
 * @fn      		  - lcd_print_string
 *
 * @brief             - This function allows to send message to LCD
 *
 * @param[in]         - message
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

void lcd_print_string(char* message)
{
	do
	{
		lcd_print_char((uint8_t)*message++);
	}
	while(*message != '\0');
}

/*********************************************************************
 * @fn      		  - lcd_display_clear
 *
 * @brief             - This function clear the display of LCD
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

void lcd_display_clear(void)
{
	lcd_send_command(LCD_CMD_DIS_CLEAR);

	mdelay(2);
}

/*********************************************************************
 * @fn      		  - lcd_display_return_home
 *
 * @brief             - This function allows to return to home
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

void lcd_display_return_home(void)
{

	lcd_send_command(LCD_CMD_DIS_RETURN_HOME);
	/*
	 * check page number 24 of datasheet.
	 * return home command execution wait time is around 2ms
	 */
	mdelay(2);
}

/*********************************************************************
 * @fn      		  - lcd_set_cursor
 *
 * @brief             - This function allows to set cursor in some place
 *
 * @param[in]         - row from 1 to 2
 * @param[in]         -	column from 1 to 16
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */
/**
  *   Set Lcd to a specified location given by row and column information
  *   Row Number (1 to 2)
  *   Column Number (1 to 16) Assuming a 2 X 16 characters display
  */
void lcd_set_cursor(uint8_t row, uint8_t column)
{
  column--;
  switch (row)
  {
    case 1:
      /* Set cursor to 1st row address and add index */
      lcd_send_command((column |= 0x80));
      break;
    case 2:
      /* Set cursor to 2nd row address and add index */
        lcd_send_command((column |= 0xC0));
      break;
    default:
      break;
  }
}
