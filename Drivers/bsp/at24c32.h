#ifndef BSP_AT24C32_H_
#define BSP_AT24C32_H_

#include "main.h"

extern I2C_HandleTypeDef hi2c1;

/* Application configurable items */
#define AT24C32_I2C						I2C1
#define AT24C32_I2C_GPIO_PORT			GPIOB

#define AT24C32_I2C_ADDR				(0x50 << 1)
#define AT24C32_START_ADDR				0x0000
#define AT24C32_END_ADDR				0x0FFF
#define AT24C32_TIMEOUT					1000

/* Initialization API */
void at24c32_init(void);

/* Send and receive data APIs */
void at24c32_set_data(uint8_t *Tx, uint32_t len);
void at24c32_get_data(uint8_t *Rx, uint16_t address, uint32_t len);

#endif /* BSP_AT24C32_H_ */
