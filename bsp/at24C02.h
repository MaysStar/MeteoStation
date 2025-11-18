#ifndef AT24C02_H_
#define AT24C02_H_

#include "stm32f407xx.h"

/* Application configurable items */
#define AT24C02_I2C						I2C1
#define AT24C02_I2C_GPIO_PORT			GPIOB
#define AT24C02_I2C_SCL_PIN				GPIO_PIN_NO_6
#define AT24C02_I2C_SDA_PIN				GPIO_PIN_NO_7
#define AT24C02_I2C_SPEED				I2C_SCL_SPEED_SM
#define AT24C02_I2C_PUPD				GPIO_PIN_PU // we use internal pull up

#define AT24C02_I2C_ADDR				0x50

#define AT24C02_START_ADDR				0x00
#define AT24C02_END_ADDR				0xFF

#define AT24C02_PAGE_SIZE               8

/* Initialization API */
void at24c02_init(void);

/* Send and receive data APIs */
void at24c02_set_data(uint8_t *Tx, uint16_t len);
void at24c02_get_data(uint8_t *Rx, uint16_t address, uint16_t len);


#endif /* AT24C02_H_ */
