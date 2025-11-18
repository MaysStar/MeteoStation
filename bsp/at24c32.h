#ifndef AT24C32_H_
#define AT24C32_H_

#include "stm32f407xx.h"

/* Application configurable items */
#define AT24C32_I2C						I2C1
#define AT24C32_I2C_GPIO_PORT			GPIOB
#define AT24C32_I2C_SCL_PIN				GPIO_PIN_NO_6
#define AT24C32_I2C_SDA_PIN				GPIO_PIN_NO_7
#define AT24C32_I2C_SPEED				I2C_SCL_SPEED_SM
#define AT24C32_I2C_PUPD				GPIO_PIN_PU // we use internal pull up

#define AT24C32_I2C_ADDR				0x50
#define AT24C32_START_ADDR				0x0000
#define AT24C32_END_ADDR				0x0FFF

/* Initialization API */
void at24c32_init(void);

/* Send and receive data APIs */
void at24c32_set_data(uint8_t *Tx, uint32_t len);
void at24c32_get_data(uint8_t *Rx, uint16_t address, uint32_t len);

#endif /* AT24C32_H_ */
