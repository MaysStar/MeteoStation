#include "ds1307.h"

static void ds1307_i2c_pin_config(void);
static void ds1307_i2c_config(void);
static void ds1307_write(uint8_t val, uint8_t reg_addr);
static uint8_t ds1307_read(uint8_t reg_addr);
static uint8_t binary_to_bcd(uint8_t bin);
static uint8_t bcd_to_binary(uint8_t bcd);

// global I2C handle
I2C_Handle_t g_ds1307I2CHandle;

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

static void ds1307_i2c_pin_config(void)
{
	GPIO_Handle_t i2c_scl, i2c_sda;

	memset(&i2c_scl, 0, sizeof(i2c_scl));
	memset(&i2c_sda, 0, sizeof(i2c_sda));

	i2c_scl.pGPIOx = DS1307_I2C_GPIO_PORT;
	i2c_scl.GPIO_PinConfig.GPIO_PinNumber = DS1307_I2C_SCL_PIN;
	i2c_scl.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	i2c_scl.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
	i2c_scl.GPIO_PinConfig.GPIO_PinPuPdControl = DS1307_I2C_PUPD;
	i2c_scl.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	i2c_scl.GPIO_PinConfig.GPIO_PinAltFunMode = 4;

	i2c_sda.pGPIOx = DS1307_I2C_GPIO_PORT;
	i2c_sda.GPIO_PinConfig.GPIO_PinNumber = DS1307_I2C_SDA_PIN;
	i2c_sda.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	i2c_sda.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
	i2c_sda.GPIO_PinConfig.GPIO_PinPuPdControl = DS1307_I2C_PUPD;
	i2c_sda.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	i2c_sda.GPIO_PinConfig.GPIO_PinAltFunMode = 4;

	GPIO_Init(&i2c_scl);
	GPIO_Init(&i2c_sda);
}

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

static void ds1307_i2c_config(void)
{
	g_ds1307I2CHandle.pI2Cx = DS1307_I2C;
	g_ds1307I2CHandle.I2C_Config.I2C_ACKControl = I2C_ACK_ENABLE;
	g_ds1307I2CHandle.I2C_Config.I2C_Device_Address = DS1307_I2C_ADDR;
	g_ds1307I2CHandle.I2C_Config.I2C_FMDutyCycle = I2C_FM_DUTY_CYCLE2;
	g_ds1307I2CHandle.I2C_Config.I2C_SCLSpeed = DS1307_I2C_SPEED;

	I2C_Init(&g_ds1307I2CHandle);
}

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
	uint8_t tx[2];
	tx[0] = reg_addr;
	tx[1] = val;

	I2C_MasterSendData(&g_ds1307I2CHandle, tx, 2, DS1307_I2C_ADDR, I2C_STOP);
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
	uint8_t rx;
	// send pointer to register
	I2C_MasterSendData(&g_ds1307I2CHandle, &reg_addr, 1, DS1307_I2C_ADDR, I2C_STOP);

	// read from that register
	I2C_MasterReceiveData(&g_ds1307I2CHandle, &rx, 1, DS1307_I2C_ADDR, I2C_STOP);

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
	ds1307_i2c_pin_config();

	// 2. Initialize I2C peripheral
	ds1307_i2c_config();

	// 3. Enable I2C peripheral
	I2C_PeripheralControl(DS1307_I2C, ENABLE);

	// 4. Make clock halt = 0
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
