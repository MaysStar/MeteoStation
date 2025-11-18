#include "at24c32.h"

static void at24c32_i2c_pin_config(void);
static void at24c32_i2c_config(void);

static uint16_t curr_addr = AT24C32_START_ADDR;

// global I2C handle
I2C_Handle_t g_AT24C32I2CHandle;

/*********************************************************************
 * @fn      		  - at24c32_i2c_pin_config
 *
 * @brief             - This function initialize GPIO for i2c
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

static void at24c32_i2c_pin_config(void)
{
	GPIO_Handle_t i2c_scl, i2c_sda;

	memset(&i2c_scl, 0, sizeof(i2c_scl));
	memset(&i2c_sda, 0, sizeof(i2c_sda));

	i2c_scl.pGPIOx = AT24C32_I2C_GPIO_PORT;
	i2c_scl.GPIO_PinConfig.GPIO_PinNumber = AT24C32_I2C_SCL_PIN;
	i2c_scl.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	i2c_scl.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
	i2c_scl.GPIO_PinConfig.GPIO_PinPuPdControl = AT24C32_I2C_PUPD;
	i2c_scl.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	i2c_scl.GPIO_PinConfig.GPIO_PinAltFunMode = 4;

	i2c_sda.pGPIOx = AT24C32_I2C_GPIO_PORT;
	i2c_sda.GPIO_PinConfig.GPIO_PinNumber = AT24C32_I2C_SDA_PIN;
	i2c_sda.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	i2c_sda.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
	i2c_sda.GPIO_PinConfig.GPIO_PinPuPdControl = AT24C32_I2C_PUPD;
	i2c_sda.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	i2c_sda.GPIO_PinConfig.GPIO_PinAltFunMode = 4;

	GPIO_Init(&i2c_scl);
	GPIO_Init(&i2c_sda);
}

/*********************************************************************
 * @fn      		  - at24c32i2c_config
 *
 * @brief             - This function initialize i2c for EEPROM
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

static void at24c32_i2c_config(void)
{
	g_AT24C32I2CHandle.pI2Cx = AT24C32_I2C;
	g_AT24C32I2CHandle.I2C_Config.I2C_ACKControl = I2C_ACK_ENABLE;
	g_AT24C32I2CHandle.I2C_Config.I2C_Device_Address = AT24C32_I2C_ADDR;
	g_AT24C32I2CHandle.I2C_Config.I2C_FMDutyCycle = I2C_FM_DUTY_CYCLE2;
	g_AT24C32I2CHandle.I2C_Config.I2C_SCLSpeed = AT24C32_I2C_SPEED;

	I2C_Init(&g_AT24C32I2CHandle);
}

/*********************************************************************
 * @fn      		  - at24c32_init
 *
 * @brief             - This function initialize I2C and GPIO for EEPROM communication
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

void at24c32_init(void)
{
	// 1. Configure GPIO pins
	at24c32_i2c_pin_config();

	// 2. Configure I2C pins
	at24c32_i2c_config();

	// 3. Enable I2C peripheral
	I2C_PeripheralControl(AT24C32_I2C, ENABLE);
}

/*********************************************************************
 * @fn      		  - at24c32_set_data
 *
 * @brief             - This function allows send data to AT24C32
 *
 * @param[in]         - Tx buffer
 * @param[in]         -	len
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

void at24c32_set_data(uint8_t *Tx, uint32_t len)
{
	while(len > 0)
	{
		// 1. Write page by page
		uint8_t page_space = 32 - (curr_addr % 32);

		uint32_t curr_len = (len > page_space) ? page_space : len;

		// 2. Send address of data
		uint8_t buf[2 + curr_len];
		buf[0] = (uint8_t)((curr_addr >> 8) & 0xFF); // high bits
		buf[1] = (uint8_t)(curr_addr & 0xFF);		  // low bits

		memcpy(&buf[2], Tx, curr_len);

		// 3. Send
		I2C_MasterSendData(&g_AT24C32I2CHandle, buf, (curr_len + 2), AT24C32_I2C_ADDR, I2C_STOP);

		mdelay(10);

		len -= curr_len;
		Tx += curr_len;
		curr_addr += curr_len;
	}

	if(curr_addr >= AT24C32_END_ADDR) curr_addr = AT24C32_START_ADDR;
}

/*********************************************************************
 * @fn      		  - at24c32_set_data
 *
 * @brief             - This function allows send data to AT24C32
 *
 * @param[in]         - Tx buffer
 * @param[in]         -	address of byte
 * @param[in]         - len
 *
 * @return            - none
 *
 * @Note              - none

 */

void at24c32_get_data(uint8_t *Rx, uint16_t address, uint32_t len)
{
	//I2C_MasterReceiveData(&g_AT24C32I2CHandle, Rx, len, AT24C32_I2C_ADDR, I2C_STOP);

	while(len > 0)
	{
		// 1. Send address of data
		uint8_t addr[2];
		addr[0] = (uint8_t)((address >> 8) & 0xFF); // high bits
		addr[1] = (uint8_t)(address & 0xFF);		// low bits

		I2C_MasterSendData(&g_AT24C32I2CHandle, addr, 2, AT24C32_I2C_ADDR, I2C_REPETITIVE_START);

		// 2. Receive data from EEPROM
		I2C_MasterReceiveData(&g_AT24C32I2CHandle, Rx, 1, AT24C32_I2C_ADDR, I2C_STOP);

		len--;
		address++;
		Rx++;
	}
}
