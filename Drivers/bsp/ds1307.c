#include "ds1307.h"

static HAL_StatusTypeDef ds1307_write(uint8_t val, uint8_t reg_addr);
static DS1307_rx_t ds1307_read(uint8_t reg_addr);
static uint8_t binary_to_bcd(uint8_t bin);
static uint8_t bcd_to_binary(uint8_t bcd);

/*********************************************************************
 * @fn      		  - ds1307_write
 *
 * @brief             - This function write value into that address
 *
 * @param[in]         -	value which we write
 * @param[in]         -	address of register in rtc
 * @param[in]         -
 *
 * @return            - HAL state
 *
 * @Note              - none

 */

static HAL_StatusTypeDef ds1307_write(uint8_t val, uint8_t reg_addr)
{
	HAL_StatusTypeDef ret;

	uint8_t tx[2];
	tx[0] = reg_addr;
	tx[1] = val;

	ret = HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)(DS1307_I2C_ADDR), tx, 2, 1000);
	if(ret != HAL_OK)
	{
		lcd_display_clear();

		lcd_set_cursor(1, 1);

		lcd_print_string("I2C ERROR DS1307");

		lcd_set_cursor(2, 1);

		lcd_print_string("DS WRITE");
	}

	return ret;
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
 * @return            - HAL state and byte
 *
 * @Note              - none

 */

static DS1307_rx_t ds1307_read(uint8_t reg_addr)
{
	DS1307_rx_t ret;
	ret.data = 0;

	// send pointer to register
	ret.state = HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)(DS1307_I2C_ADDR), &reg_addr, 1, DS1307_TIMEOUT);
	if(ret.state != HAL_OK)
	{
		lcd_display_clear();

		lcd_set_cursor(1, 1);

		lcd_print_string("I2C ERROR DS1307");

		lcd_set_cursor(2, 1);

		lcd_print_string("DS WRITE");

		return ret;
	}

	ret.state = HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(DS1307_I2C_ADDR) | 0x01, 10, DS1307_TIMEOUT);

	if(ret.state != HAL_OK)
	{
		lcd_display_clear();

		lcd_set_cursor(1, 1);

		lcd_print_string("I2C ERROR DS READY");
	}

	// read from that register
	ret.state = HAL_I2C_Master_Receive(&hi2c1, (uint16_t)(DS1307_I2C_ADDR) | 0x01, &ret.data, 1, DS1307_TIMEOUT);
	if(ret.state != HAL_OK)
	{
		printf("I2C error: 0x%08lx\r\n", hi2c1.ErrorCode);

		lcd_display_clear();

		lcd_set_cursor(1, 1);

		lcd_print_string("I2C ERROR DS1307");

		lcd_set_cursor(2, 1);

		lcd_print_string("DS READ");
	}

	return ret;
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
 * @return            - bcd code
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
 * @return            - HAL state
 *
 * @Note              - none

 */

HAL_StatusTypeDef ds1307_init(void)
{
	HAL_StatusTypeDef ret;
	// 1. Initialize I2C pin
	/*ds1307_i2c_pin_config();

	// 2. Initialize I2C peripheral
	ds1307_i2c_config();*/

	// 3. Make clock halt = 0
	ret = ds1307_write(0x00, DS1307_ADDR_SEC);

	if(ret != HAL_OK) return ret;

	// 5. Read back clock halt bit
	ret = ds1307_read(DS1307_ADDR_SEC).state;

	return ret;
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
 * @return            - HAL state
 *
 * @Note              - none

 */

HAL_StatusTypeDef ds1307_set_current_time(RTC_time_t *rtc_time)
{
	HAL_StatusTypeDef ret;
	uint8_t seconds, hours;

	// change format to bcd
	seconds = binary_to_bcd(rtc_time->seconds);
	hours = binary_to_bcd(rtc_time->hours);

	// bit 1 should not be 1, otherwise clock will be halted
	seconds &= ~(1 << 7);

	// write time to clock
	ret = ds1307_write(seconds, DS1307_ADDR_SEC);

	if(ret != HAL_OK) return ret;

	ret = ds1307_write(binary_to_bcd(rtc_time->minutes), DS1307_ADDR_MIN);

	if(ret != HAL_OK) return ret;

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

	ret = ds1307_write(hours, DS1307_ADDR_HOUR);

	return ret;
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
 * @return            - HAL state
 *
 * @Note              - none

 */

HAL_StatusTypeDef ds1307_get_current_time(RTC_time_t *rtc_time)
{
	DS1307_rx_t ret;
	uint8_t seconds, hours;

	ret = ds1307_read(DS1307_ADDR_SEC);

	if(ret.state != HAL_OK) return ret.state;
	seconds = ret.data;

	// this bit is not required
	seconds &= ~(1 << 7);

	// get time from rtc
	rtc_time->seconds = bcd_to_binary(seconds);

	ret = ds1307_read(DS1307_ADDR_MIN);
	if(ret.state != HAL_OK) return ret.state;

	rtc_time->minutes = bcd_to_binary(ret.data);

	ret = ds1307_read(DS1307_ADDR_HOUR);
	hours = ret.data;

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

	return ret.state;
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
 * @return            - HAL state
 *
 * @Note              - none

 */

HAL_StatusTypeDef ds1307_set_current_date(RTC_date_t *rtc_date)
{
	HAL_StatusTypeDef ret;

	// change format to bcd and set the date
	ret = ds1307_write(binary_to_bcd(rtc_date->day), DS1307_ADDR_DAY);
	if(ret != HAL_OK) return ret;

	ret = ds1307_write(binary_to_bcd(rtc_date->date), DS1307_ADDR_DATA);
	if(ret != HAL_OK) return ret;

	ret = ds1307_write(binary_to_bcd(rtc_date->month), DS1307_ADDR_MONTH);
	if(ret != HAL_OK) return ret;

	ret = ds1307_write(binary_to_bcd(rtc_date->year), DS1307_ADDR_YEAR);
	return ret;
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
 * @return            - HAL state
 *
 * @Note              - none

 */

HAL_StatusTypeDef ds1307_get_current_date(RTC_date_t *rtc_date)
{
	DS1307_rx_t ret;
	// get date from rtc
	ret = ds1307_read(DS1307_ADDR_DAY);
	if(ret.state != HAL_OK) return ret.state;

	rtc_date->day = bcd_to_binary(ret.data);

	ret = ds1307_read(DS1307_ADDR_DATA);
	if(ret.state != HAL_OK) return ret.state;

	rtc_date->date = bcd_to_binary(ret.data);

	ret = ds1307_read(DS1307_ADDR_MONTH);
	if(ret.state != HAL_OK) return ret.state;

	rtc_date->month = bcd_to_binary(ret.data);

	ret = ds1307_read(DS1307_ADDR_YEAR);

	rtc_date->year = bcd_to_binary(ret.data);

	return ret.state;
}
