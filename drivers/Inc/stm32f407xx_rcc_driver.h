#ifndef INC_STM32F407XX_RCC_DRIVER_H_
#define INC_STM32F407XX_RCC_DRIVER_H_

#include "stm32f407xx.h"

/*
 * Bit position definitions I2C_OAR1
 */

#define RCC_CFGR_HPRE						4
#define RCC_CFGR_PPRE1						10
#define RCC_CFGR_PPRE2						13

//This returns the APB1 clock value
uint32_t RCC_GetPCLK1Value(void);

//This returns the APB2 clock value
uint32_t RCC_GetPCLK2Value(void);

uint32_t  RCC_GetPLLOutputClock(void);



#endif /* INC_STM32F407XX_RCC_DRIVER_H_ */
