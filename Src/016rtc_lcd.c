#include "stm32f407xx.h"

#define SYSTICK_TIM_CLK					16000000UL
#define RTC_LCD_TEST					SET
#define EEPROM_TEST						RESET

RTC_time_t curr_time;
RTC_date_t curr_date;

uint8_t tx_buf[4096];
uint8_t rx_buf[9];

static uint16_t cuur_data = 0;

void InfiniteLoop(void)
{
	while(1);
}

void init_systick_timer(uint32_t tick_hz)
{
	uint32_t *pSRVR = (uint32_t*)0xE000E014;
	uint32_t *pSCSR = (uint32_t*)0xE000E010;

    //calculation of reload value
    uint32_t count_value = (SYSTICK_TIM_CLK/tick_hz)-1;

    //Clear the value of SVR
    *pSRVR &= ~(0x00FFFFFFFF);

    //load the value in to SVR
    *pSRVR |= count_value;

    //do some settings
    *pSCSR |= ( 1 << 1); //Enables SysTick exception request:
    *pSCSR |= ( 1 << 2);  //Indicates the clock source, processor clock source

    //enable the systick
    *pSCSR |= ( 1 << 0); //enables the counter
}


uint8_t *get_day_of_weak(uint8_t dayCode)
{
	uint8_t* days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

	return days[dayCode - 1];
}

void number_to_string(uint8_t num, uint8_t *buf)
{
	if(num < 10)
	{
		buf[0] = '0';
		buf[1] = num + 48;
	}else if(num >= 10 && num <= 99)
	{
		buf[0] = (num / 10) + 48;
		buf[1] = (num % 10) + 48;
	}
}

void large_delay(void)
{
	for(uint32_t i = 0; i < 5000000; ++i);
}

// hh:mm:ss
uint8_t *time_to_string(RTC_time_t *curr_time)
{
	static char buf[9];

	buf[2] = ':';
	buf[5] = ':';

	number_to_string(curr_time->hours, buf);
	number_to_string(curr_time->minutes, &buf[3]);
	number_to_string(curr_time->seconds, &buf[6]);

	buf[8] = '\0';

	return buf;
}

// dd.mm.yy
uint8_t *date_to_string(RTC_date_t *curr_date)
{
	static char buf[9];

	buf[2] = '.';
	buf[5] = '.';

	number_to_string(curr_date->date, buf);
	number_to_string(curr_date->month, &buf[3]);
	number_to_string(curr_date->year, &buf[6]);

	buf[8] = '\0';

	return buf;
}

int main(void)
{
#if EEPROM_TEST
	at24c32_init();

	//uint32_t len = strlen((char*)tx_buf);

	//at24c32_set_data(tx_buf, 4095);

	at24c32_get_data(tx_buf, AT24C32_START_ADDR, 4095);

	tx_buf[4095] = '\0';

	printf("DATA from EEPROM: %s\n", tx_buf);
}
#endif
#if RTC_LCD_TEST
 	//printf("RTC text\n");

	lcd_init();

	lcd_print_string("RTC Test...");

	mdelay(200);

	lcd_display_clear();

	lcd_display_return_home();

	if(ds1307_init())
	{
		printf("Error ds1304 init");
		InfiniteLoop();
	}

	init_systick_timer(1);

	curr_date.day = THURSDAY;
	curr_date.date = 18;
	curr_date.month = 11;
	curr_date.year = 25;

	curr_time.time_format = DS1307_TIME_FORMAT_24HOUR;
	curr_time.hours = 14;
	curr_time.minutes = 15;
	curr_time.seconds = 30;

	ds1307_set_current_date(&curr_date);

	ds1307_set_current_time(&curr_time);

	at24c32_init();

	while(1);

	return 0;
}

void saveDataIntoEEPROM(void)
{
	// get data
	ds1307_get_current_time(&curr_time);
	ds1307_get_current_date(&curr_date);

	// save into ROM
	at24c32_set_data(time_to_string(&curr_time), strlen(time_to_string(&curr_time)));
	at24c32_set_data(date_to_string(&curr_date), strlen(date_to_string(&curr_date)));
}

void get_data(void)
{
	at24c32_get_data(rx_buf, cuur_data, 8);

	cuur_data += 8;

	rx_buf[8] = '\0';

	printf("Data: %s\n", rx_buf);
}

void SysTick_Handler(void)
{
	saveDataIntoEEPROM();

	lcd_set_cursor(1, 1);

	get_data();
	uint8_t *am_pm;
	if(curr_time.time_format != DS1307_TIME_FORMAT_24HOUR)
	{
		am_pm = (curr_time.time_format) ? "PM" : "AM";

		lcd_print_string(rx_buf);
		lcd_print_string(am_pm);
	}
	else
	{
		lcd_print_string(rx_buf);
	}

	lcd_set_cursor(2, 1);

	get_data();
	lcd_print_string(rx_buf);
	lcd_print_string(" ");
	lcd_print_string(get_day_of_weak(curr_date.day));
}

#endif

/*void I2C1_EV_IRQHandler(void)
{
	I2C_EV_IRQHandling(ds1307_get_handle());
}

void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle, uint8_t EVENT)
{
	if(EVENT == I2C_EV_TX_CMPLT)
	{
		printf("Tx is completed\n");
	}
	else if(EVENT == I2C_EV_RX_CMPLT)
	{
		printf("Rx is completed\n");
	}
	else if(EVENT == I2C_EV_STOP)
	{
		printf("STOP event\n");
	}
	else if(EVENT == I2C_EV_DATA_REQ)
	{
		printf("Data request event\n");
		// event happens only in slave mode

	}
	else if(EVENT == I2C_EV_DATA_RCV)
	{
		printf("Data receive event\n");
		// event happens only in slave mode

	}
	else if(EVENT == I2C_ERROR_AF)
	{
		printf("ACK failure\n");
		// in master ACK failure happens when slave fails to send ACK byte
		// send from master
		I2C_CloseSendData(pI2CHandle);

		// Generate the stop condition
		I2C_GenerateStopCondition(I2C1);

		// Hang infinite loop
		while(1);
	}
	else if(EVENT == I2C_ERROR_OVR)
	{
		printf("OVR failure\n");
		// OVR failure happens, when master can't receive the data corectly
		I2C_CloseReceiveData(pI2CHandle);

		// Generate the stop condition
		I2C_GenerateStopCondition(I2C1);

		// Hang infinite loop
		while(1);
	}
	else if(EVENT == I2C_ERROR_BERR)
	{
		printf("BERR failure\n");
		// BERR failure happens, when Start is generated while stop isn't finished

		// Generate the stop condition
		I2C_GenerateStopCondition(I2C1);

		// Hang infinite loop
		while(1);
	}
}*/
