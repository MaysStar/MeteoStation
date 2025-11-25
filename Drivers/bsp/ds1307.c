#include "ds1307.h"

/*static void ds1307_i2c_pin_config(void);
static void ds1307_i2c_config(void);*/
static void ds1307_write(uint8_t val, uint8_t reg_addr);
static uint8_t ds1307_read(uint8_t reg_addr);
static uint8_t binary_to_bcd(uint8_t bin);
static uint8_t bcd_to_binary(uint8_t bcd);

// global I2C handle
//I2C_Handle_t g_ds1307I2CHandle;
//I2C_HandleTypeDef DS1307_I2C_Handle;

/*********************************************************************
 * @fn      		  - ds1307_i2c_pin_config
 *
 * @brief             - This function initialize gpio for i2c pin
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

/*static void ds1307_i2c_pin_config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitStruct.Pin = DS1307_I2C_SCL_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(DS1307_I2C_GPIO_PORT,&GPIO_InitStruct);

	GPIO_InitStruct.Pin = DS1307_I2C_SDA_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	HAL_GPIO_Init(DS1307_I2C_GPIO_PORT, &GPIO_InitStruct);
}*/

/*********************************************************************
 * @fn      		  - ds1307_i2c_config
 *
 * @brief             - This function initialize i2c for real time clock
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

/*static void ds1307_i2c_config(void)
{



	DS1307_I2C_Handle.Instance = I2C1;
	DS1307_I2C_Handle.Init.ClockSpeed = DS1307_I2C_SPEED;
	DS1307_I2C_Handle.Init.DutyCycle = I2C_DUTYCYCLE_2;
	DS1307_I2C_Handle.Init.OwnAddress1 = 0x0;
	DS1307_I2C_Handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	DS1307_I2C_Handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	DS1307_I2C_Handle.Init.OwnAddress2 = 0;
	DS1307_I2C_Handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	DS1307_I2C_Handle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

	if (HAL_I2C_Init(&DS1307_I2C_Handle) != HAL_OK)
	{
		Error_Handler();
	}
}*/

/*********************************************************************
 * @fn      		  - ds1307_write
 *
 * @brief             - This function write value into that address
 *
 * @param[in]         -	value which we write
 * @param[in]         -	address of register in rtc
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

static void ds1307_write(uint8_t val, uint8_t reg_addr)
{
	HAL_StatusTypeDef ret;

	uint8_t tx[2];
	tx[0] = reg_addr;
	tx[1] = val;

	ret = HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)(DS1307_I2C_ADDR), tx, 2, 1000);
	if(ret == HAL_ERROR)
	{
		printf("I2C error: 0x%08lx\r\n", hi2c1.ErrorCode);

		lcd_display_clear();

		lcd_set_cursor(1, 1);

		lcd_print_string("I2C ERROR DS1307");

		Error_Handler();
		// або HAL_I2C_GetError(&hi2c1);
	}

	//HAL_I2C_Master_Seq_Transmit_IT(hi2c, DevAddress, pData, Size, XferOptions)(&hi2c1, DS1307_I2C_ADDR, &tx, 2, Timeout)
}

/*********************************************************************
 * @fn      		  - ds1307_read
 *
 * @brief             - This function read value from that address
 *
 * @param[in]         -	address of register in rtc
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

static uint8_t ds1307_read(uint8_t reg_addr)
{
	HAL_StatusTypeDef ret;
	uint8_t rx;
	// send pointer to register
	ret = HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)(DS1307_I2C_ADDR), &reg_addr, 1, 1000);
	//HAL_I2C_Master_Seq_Transmit_IT(&hi2c1, DS1307_I2C_ADDR, &reg_addr, 1, XferOptions)
	//I2C_MasterSendData(&g_ds1307I2CHandle, &reg_addr, 1, DS1307_I2C_ADDR, I2C_STOP);
	if(ret == HAL_ERROR)
	{
		printf("I2C error: 0x%08lx\r\n", hi2c1.ErrorCode);

		lcd_display_clear();

		lcd_set_cursor(1, 1);

		lcd_print_string("I2C ERROR DS1307");

		Error_Handler();
		// або HAL_I2C_GetError(&hi2c1);
	}

	HAL_Delay(10);

	// read from that register
	ret = HAL_I2C_Master_Receive(&hi2c1, (uint16_t)(DS1307_I2C_ADDR) | 0x01, &rx, 1, 1000);
	if(ret == HAL_ERROR)
	{
		printf("I2C error: 0x%08lx\r\n", hi2c1.ErrorCode);

		lcd_display_clear();

		lcd_set_cursor(1, 1);

		lcd_print_string("I2C ERROR DS1307");

		Error_Handler();
		// або HAL_I2C_GetError(&hi2c1);
	}

	//HAL_I2C_Master_Receive_IT(&DS1307_I2C_Handle, (DS1307_I2C_ADDR << 1), &rx, 1);

	return rx;
}

/*********************************************************************
 * @fn      		  - binary_to_bcd
 *
 * @brief             - This function transform binary format to bcd
 *
 * @param[in]         - number in binary format
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - number in bcd format
 *
 * @Note              - none

 */

static uint8_t binary_to_bcd(uint8_t bin)
{
	uint8_t bcd, m, n;

	bcd = bin;

	if(bin >= 10)
	{
		m = bin / 10; // to get second number
		n = bin % 10; // to first second number

		bcd = (uint8_t)(m << 4) | n; // number in bcd format
	}

	return bcd;
}


/*********************************************************************
 * @fn      		  - bcd_to_binary
 *
 * @brief             - This function transform bcd format to binary
 *
 * @param[in]         -	number in bcd format
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - number in binary format
 *
 * @Note              - none

 */

static uint8_t bcd_to_binary(uint8_t bcd)
{
	uint8_t bin;

	bin = (uint8_t)((bcd >> 4) * 10);
	bin += (bcd & (uint8_t)0x0F);

	return bin;
}

/*********************************************************************
 * @fn      		  - ds1307_init
 *
 * @brief             - This function initialize ds1307
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - if( return 1 ) ? CH = 1 ( init failed ) : CH = 0 ( init success )
 *
 * @Note              - none

 */

uint8_t ds1307_init(void)
{
	// 1. Initialize I2C pin
	/*ds1307_i2c_pin_config();

	// 2. Initialize I2C peripheral
	ds1307_i2c_config();*/

	// 3. Make clock halt = 0
	ds1307_write(0x00, DS1307_ADDR_SEC);

	// 5. Read back clock halt bit
	uint8_t clock_state = ds1307_read(DS1307_ADDR_SEC);

	return ((clock_state >> 7) & 0x1);
}

/*********************************************************************
 * @fn      		  - ds1307_set_current_time
 *
 * @brief             - This function allows us to set current time
 *
 * @param[in]         -	time of RTC
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

void ds1307_set_current_time(RTC_time_t *rtc_time)
{
	uint8_t seconds, hours;

	// change format to bcd
	seconds = binary_to_bcd(rtc_time->seconds);
	hours = binary_to_bcd(rtc_time->hours);

	// bit 1 should not be 1, otherwise clock will be halted
	seconds &= ~(1 << 7);

	// write time to clock
	ds1307_write(seconds, DS1307_ADDR_SEC);

	ds1307_write(binary_to_bcd(rtc_time->minutes), DS1307_ADDR_MIN);

	if(rtc_time->time_format == DS1307_TIME_FORMAT_24HOUR)
	{
		// set 24 hours format
		hours &= ~(1 << 6);
	}
	else
	{
		// set 12 hours format
		hours |= (1 << 6);

		// set PM or AM
		hours = (rtc_time->time_format == DS1307_TIME_FORMAT_12HOUR_PM) ?
				hours | (1 << 5) : hours & ~(1 << 5);
	}

	ds1307_write(hours, DS1307_ADDR_HOUR);
}

/*********************************************************************
 * @fn      		  - ds1307_get_current_time
 *
 * @brief             - This function allows us to get current time
 *
 * @param[in]         - time of RTC
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

void ds1307_get_current_time(RTC_time_t *rtc_time)
{
	uint8_t seconds, hours;

	seconds = ds1307_read(DS1307_ADDR_SEC);

	// this bit is not required
	seconds &= ~(1 << 7);

	// get time from rtc
	rtc_time->seconds = bcd_to_binary(seconds);

	rtc_time->minutes = bcd_to_binary(ds1307_read(DS1307_ADDR_MIN));

	hours = ds1307_read(DS1307_ADDR_HOUR);

	// check time format
	if((hours & (1 << 6)) == RESET)
	{
		// 24 hours format
		rtc_time->time_format = DS1307_TIME_FORMAT_24HOUR;
	}
	else
	{
		// 12 hours format
		rtc_time->time_format = !((hours & (1 << 5)) == 0);
		hours &= ~(0x3 << 5); // clear 5 and 6 bit
	}

	rtc_time->hours = bcd_to_binary(hours);
}

/*********************************************************************
 * @fn      		  - ds1307_set_current_date
 *
 * @brief             - This function allows us to set current date
 *
 * @param[in]         - date of RTC
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

void ds1307_set_current_date(RTC_date_t *rtc_date)
{
	// change format to bcd and set the date
	ds1307_write(binary_to_bcd(rtc_date->day), DS1307_ADDR_DAY);

	ds1307_write(binary_to_bcd(rtc_date->date), DS1307_ADDR_DATA);

	ds1307_write(binary_to_bcd(rtc_date->month), DS1307_ADDR_MONTH);

	ds1307_write(binary_to_bcd(rtc_date->year), DS1307_ADDR_YEAR);
}

/*********************************************************************
 * @fn      		  - ds1307_get_current_date
 *
 * @brief             - This function allows us to get current date
 *
 * @param[in]         - date of RTC
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

void ds1307_get_current_date(RTC_date_t *rtc_date)
{
	// get date from rtc
	rtc_date->day = bcd_to_binary(ds1307_read(DS1307_ADDR_DAY));

	rtc_date->date = bcd_to_binary(ds1307_read(DS1307_ADDR_DATA));

	rtc_date->month = bcd_to_binary(ds1307_read(DS1307_ADDR_MONTH));

	rtc_date->year = bcd_to_binary(ds1307_read(DS1307_ADDR_YEAR));
}

/*void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c) {
    // handle transmit complete
	if (hi2c->Instance == I2C1)
	{
		TxComplite = SET;
	}
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c) {
    // handle receive complete
	if (hi2c->Instance == I2C1)
	{
		RxComplite = SET;
	}
}*/
