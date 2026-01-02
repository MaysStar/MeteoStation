#include "at24c32.h"

static uint16_t curr_addr = AT24C32_START_ADDR;

/*********************************************************************
 * @fn      		  - at24c32_set_data
 *
 * @brief             - This function allows send data to AT24C32
 *
 * @param[in]         - Tx buffer
 * @param[in]         -	len
 * @param[in]         -
 *
 * @return            - HAL state
 *
 * @Note              - none

 */

HAL_StatusTypeDef at24c32_set_data(uint8_t *Tx, uint32_t len)
{
	HAL_StatusTypeDef ret;
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

		while(HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(AT24C32_I2C_ADDR), 10, AT24C32_TIMEOUT) != HAL_OK);

		// 3. Send
		ret = HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)(AT24C32_I2C_ADDR), buf, (curr_len + 2), AT24C32_TIMEOUT);
		if(ret != HAL_OK) return ret;

		len -= curr_len;
		Tx += curr_len;
		curr_addr += curr_len;
	}

	if(curr_addr >= AT24C32_END_ADDR) curr_addr = AT24C32_START_ADDR;

	return ret;
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
 * @return            - HAL state
 *
 * @Note              - none

 */

HAL_StatusTypeDef at24c32_get_data(uint8_t *Rx, uint16_t address, uint32_t len)
{
	HAL_StatusTypeDef ret;

	// 1. Send address of data
	uint8_t addr[2];
	addr[0] = (uint8_t)((address >> 8) & 0xFF); // high bits
	addr[1] = (uint8_t)(address & 0xFF);		// low bits

	ret = HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)(AT24C32_I2C_ADDR), addr, 2, AT24C32_TIMEOUT);
	if(ret != HAL_OK) return ret;

	HAL_Delay(1);

	// 2. Receive data from EEPROM
	ret = HAL_I2C_Master_Receive(&hi2c1, (uint16_t)(AT24C32_I2C_ADDR) | 0x01, Rx, len, AT24C32_TIMEOUT);
	if(ret != HAL_OK) return ret;

	return ret;
}
