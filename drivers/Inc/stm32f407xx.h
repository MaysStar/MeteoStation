#ifndef INC_STM32F407XX_H_
#define INC_STM32F407XX_H_

#include <stddef.h>
#include <stdint.h>

#define __vo 						volatile
#define __weak 						__attribute__((weak))

/*Processor specific details*/

// ARM Cortex M4 processor NVIC ISERx registers addresses

#define NVIC_ISER0 ((__vo uint32_t*)0xE000E100)
#define NVIC_ISER1 ((__vo uint32_t*)0xE000E104)
#define NVIC_ISER2 ((__vo uint32_t*)0xE000E108)
#define NVIC_ISER3 ((__vo uint32_t*)0xE000E10C)

// ARM Cortex M4 processor NVIC ICERx registers addresses

#define NVIC_ICER0 ((__vo uint32_t*)0XE000E180)
#define NVIC_ICER1 ((__vo uint32_t*)0xE000E184)
#define NVIC_ICER2 ((__vo uint32_t*)0xE000E188)
#define NVIC_ICER3 ((__vo uint32_t*)0xE000E18C)

// ARM Cortex M4 processor NVIC IPR0 registers addresses

#define NVIC_PR_BASEADDR ((__vo uint32_t*)0xE000E400)

// number of priority bits implemented

#define NO_PR_BITS_IMPLEMENTED		4

// interrupt priority levels

#define NVIC_IRQ_PR0				0
#define NVIC_IRQ_PR1				1
#define NVIC_IRQ_PR2				2
#define NVIC_IRQ_PR3				3
#define NVIC_IRQ_PR4				4
#define NVIC_IRQ_PR5				5
#define NVIC_IRQ_PR6				6
#define NVIC_IRQ_PR7				7
#define NVIC_IRQ_PR8				8
#define NVIC_IRQ_PR9				9
#define NVIC_IRQ_PR10				10
#define NVIC_IRQ_PR11				11
#define NVIC_IRQ_PR12				12
#define NVIC_IRQ_PR13				13
#define NVIC_IRQ_PR14				14
#define NVIC_IRQ_PR15				15


#define FLASH_BASEADDR				0x08000000U // flash base address
#define SRAM1_BASEADDR				0x20000000U // sram1 base address // 112kb
#define SRAM2_BASEADDR				0x2001C000U // sram2 base address // 16kb
#define ROM							0x1FFF0000U // system memory
#define SRAM 						SRAM1_BASEADDR // all sram



/* Base address of peripherals buses */

#define PERIPH_BASE					0x40000000U
#define APB1PERIPH_BASEADDR			PERIPH_BASE // APB1
#define APB2PERIPH_BASEADDR			0x40010000U // APB2
#define AHB1PERIPH_BASEADDR			0x40020000U // AHB1
#define AHB2PERIPH_BASEADDR			0x50000000U // AHB2

/*Base address of peripherals connected to AHB1 bus */

#define GPIOA_BASEADDR				(AHB1PERIPH_BASEADDR + 0x0000)
#define GPIOB_BASEADDR				(AHB1PERIPH_BASEADDR + 0x0400)
#define GPIOC_BASEADDR				(AHB1PERIPH_BASEADDR + 0x0800)
#define GPIOD_BASEADDR				(AHB1PERIPH_BASEADDR + 0x0C00)
#define GPIOE_BASEADDR				(AHB1PERIPH_BASEADDR + 0x1000)
#define GPIOF_BASEADDR				(AHB1PERIPH_BASEADDR + 0x1400)
#define GPIOG_BASEADDR				(AHB1PERIPH_BASEADDR + 0x1800)
#define GPIOH_BASEADDR				(AHB1PERIPH_BASEADDR + 0x1C00)
#define GPIOI_BASEADDR				(AHB1PERIPH_BASEADDR + 0x2000)

#define RCC_BASEADDR 				(AHB1PERIPH_BASEADDR + 0x3800)

/*Base address of peripherals connected to APB1 bus */

#define SPI2_BASEADDR				(APB1PERIPH_BASEADDR + 0x3800)
#define SPI3_BASEADDR				(APB1PERIPH_BASEADDR + 0x3C00)

#define USART2_BASEADDR				(APB1PERIPH_BASEADDR + 0x4400)
#define USART3_BASEADDR				(APB1PERIPH_BASEADDR + 0x4800)
#define UART4_BASEADDR				(APB1PERIPH_BASEADDR + 0x4C00)
#define UART5_BASEADDR				(APB1PERIPH_BASEADDR + 0x5000)

#define I2C1_BASEADDR				(APB1PERIPH_BASEADDR + 0x5400)
#define I2C2_BASEADDR				(APB1PERIPH_BASEADDR + 0x5800)
#define I2C3_BASEADDR				(APB1PERIPH_BASEADDR + 0x5C00)

/*Base address of peripherals connected to APB2 bus */

#define USART1_BASEADDR				(APB2PERIPH_BASEADDR + 0x1000)
#define USART6_BASEADDR				(APB2PERIPH_BASEADDR + 0x1400)

#define SPI1_BASEADDR				(APB2PERIPH_BASEADDR + 0x3000)
#define SPI4_BASEADDR				(APB2PERIPH_BASEADDR + 0x3400)

#define SYSCFG_BASEADDR				(APB2PERIPH_BASEADDR + 0x3800)

#define EXTI_BASEADDR				(APB2PERIPH_BASEADDR + 0x3C00)

/* peripheral register definition structures */

typedef struct
{
	__vo uint32_t CR;		  	  // RCC clock control register address offset 0x00
	__vo uint32_t PLLCFGR;		  // RCC PLL configuration register address offset 0x04
	__vo uint32_t CFGR;		  	  // RCC clock configuration register address offset 0x08
	__vo uint32_t CIR;		  	  // RCC clock interrupt register address offset 0x0C
	__vo uint32_t AHB1RSTR;		  // RCC AHB1 peripheral reset register address offset 0x10
	__vo uint32_t AHB2RSTR;		  // RCC AHB2 peripheral reset register address offset 0x14
	__vo uint32_t AHB3RSTR;		  // RCC AHB3 peripheral reset register address offset 0x18
	uint32_t RES1;		  	  	  // Reserved address offset 0x1C
	__vo uint32_t APB1RSTR;		  // RCC APB1 peripheral reset register address offset 0x20
	__vo uint32_t APB2RSTR;		  // RCC APB2 peripheral reset register address offset 0x24
	uint32_t RES2;		  	  	  // Reserved address offset 0x28
	uint32_t RES3;		  	  	  // Reserved address offset 0x2C
	__vo uint32_t AHB1ENR;		  // RCC AHB1 peripheral clock enable register address offset 0x30
	__vo uint32_t AHB2ENR;		  // RCC AHB2 peripheral clock enable register address offset 0x34
	__vo uint32_t AHB3ENR;		  // RCC AHB3 peripheral clock enable register address offset 0x38
	uint32_t RES4;		  	   	  // Reserved address offset 0x3C
	__vo uint32_t APB1ENR;		  // RCC APB1 peripheral clock enable register address offset 0x40
	__vo uint32_t APB2ENR;		  // RCC APB2 peripheral clock enable register address offset 0x44
	uint32_t RES5;		  	  	  // Reserved address offset 0x48
	uint32_t RES6;		  	  	  // Reserved address offset 0x4C
	__vo uint32_t AHB1LPENR;	  // RCC AHB1 peripheral clock enable in low power mode register address offset 0x50
	__vo uint32_t AHB2LPENR;	  // RCC AHB2 peripheral clock enable in low power mode register address offset 0x54
	__vo uint32_t AHB3LPENR;	  // RCC AHB3 peripheral clock enable in low power mode register address offset 0x58
	uint32_t RES7;		  	  	  // Reserved address offset 0x5C
	__vo uint32_t APB1LPENR;	  // RCC APB1 peripheral clock enable in low power mode register address offset 0x60
	__vo uint32_t APB2LPENR;	  // RCC APB2 peripheral clock enabled in low power mode address offset 0x64
	uint32_t RES8;		  	  	  // Reserved address offset 0x68
	uint32_t RES9;		  	  	  // Reserved address offset 0x6C
	__vo uint32_t BDCR;		      // RCC Backup domain control register address offset 0x70
	__vo uint32_t CSR;		  	  // RCC clock control & status register address offset 0x74
	uint32_t RES10;		  		  // Reserved address offset 0x78
	uint32_t RES11;		  	 	  // Reserved address offset 0x7C
	__vo uint32_t SSCGR;		  // RCC spread spectrum clock generation register address offset 0x80
	__vo uint32_t PLLI2SCFGR;	  // RCC PLLI2S configuration register address offset 0x84
} RCC_RegDef_t;

typedef struct
{
	__vo uint32_t MODER;		  // GPIO port mode register address offset 0x00
	__vo uint32_t OTYPER;		  // GPIO port output type register address offset 0x04
	__vo uint32_t OSPEEDR;		  // GPIO port output speed register address offset 0x08
	__vo uint32_t PUPDR;		  // GPIO port pull-up/pull-down register address offset 0x0C
	__vo uint32_t IDR;			  // GPIO port input data register address offset 0x10
	__vo uint32_t ODR;			  // GPIO port output data register address offset 0x14
	__vo uint32_t BSRR;			  // GPIO port bit set/reset register address offset 0x18
	__vo uint32_t LCKR;			  // GPIO port configuration lock register address offset 0x1C
	__vo uint32_t AFR[2];		  /* AFR[0]: GPIO alternate function low register address
									 AFR[1]: GPIO alternate function high register
									 address offset 0x20 - 0x24 (AFRL and AFRH)*/
} GPIO_RegDef_t;

typedef struct
{
	__vo uint32_t CR1;		  	  // SPI control register 1 address offset 0x00
	__vo uint32_t CR2;		  	  // SPI control register 2 address offset 0x04
	__vo uint32_t SR;		  	  // SPI status register address offset 0x08
	__vo uint32_t DR;		  	  // SPI data register address offset 0x0C
	__vo uint32_t CRCPR;		  // SPI CRC polynomial register address offset 0x10
	__vo uint32_t RXCRCR;		  // SPI RX CRC register address offset 0x14
	__vo uint32_t TXCRCR;		  // SPI TX CRC register address offset 0x18
	__vo uint32_t I2SCFGR;		  // SPI_I2S configuration register address offset 0x1C
	__vo uint32_t I2SPR;		  // SPI_I2S prescaler register address offset 0x20
} SPI_RegDef_t;

typedef struct
{
	__vo uint32_t SR;		  	  // Status register (USART_SR) address offset 0x00
	__vo uint32_t DR;		  	  // Data register (USART_DR) address offset 0x04
	__vo uint32_t BRR;		  	  // Baud rate register (USART_BRR) address offset 0x08
	__vo uint32_t CR1;		  	  // Control register 1 (USART_CR1) address offset 0x0C
	__vo uint32_t CR2;			  // Control register 2 (USART_CR2) address offset 0x10
	__vo uint32_t CR3;			  // Control register 3 (USART_CR3) address offset 0x14
	__vo uint32_t GTPR;			  // Guard time and prescaler register (USART_GTPR) address offset 0x18
} USART_RegDef_t;

typedef struct
{
	__vo uint32_t CR1;		  	  // GI2C Control register 1 (I2C_CR1) address offset 0x00
	__vo uint32_t CR2;		  	  // I2C Control register 2 (I2C_CR2) address offset 0x04
	__vo uint32_t OAR1;		  	  // I2C Own address register 1 (I2C_OAR1) address offset 0x08
	__vo uint32_t OAR2;		  	  // I2C Own address register 2 (I2C_OAR2) address offset 0x0C
	__vo uint32_t DR;			  // I2C Data register (I2C_DR) address offset 0x10
	__vo uint32_t SR1;			  // I2C Status register 1 (I2C_SR1) address offset 0x14
	__vo uint32_t SR2;			  // I2C Status register 2 (I2C_SR2) address offset 0x18
	__vo uint32_t CCR;			  // I2C Clock control register (I2C_CCR) address offset 0x1C
	__vo uint32_t TRISE;		  // I2C TRISE register (I2C_TRISE) address offset 0x20
	__vo uint32_t FLTR;			  // I2C FLTR register (I2C_FLTR) address offset 0x24
} I2C_RegDef_t;

typedef struct
{
	__vo uint32_t MEMRMP;		  // SYSCFG memory remap register address offset 0x00
	__vo uint32_t PMC;		  	  // SYSCFG peripheral mode configuration register address offset 0x04
	__vo uint32_t EXTICR[4];	  // SYSCFG external interrupt configuration registers 1 - 4 address offset
								  // 0x08 - 0x14
	uint32_t RES1;			  	  // Reserved address offset 0x18
	uint32_t RES2;			  	  // Reserved address offset 0x1C
	__vo uint32_t CMPCR;		  // Compensation cell control register address offset 0x20
} SYSCFG_RegDef_t;


typedef struct
{
	__vo uint32_t IMR;		  	  // EXTI Interrupt mask register address offset 0x00
	__vo uint32_t EMR;		  	  // EXTI Event mask register address offset 0x04
	__vo uint32_t RTSR;		  	  // EXTI Rising trigger selection register address offset 0x08
	__vo uint32_t FTSR;		  	  // EXTI Falling trigger selection register address offset 0x0C
	__vo uint32_t SWIER;		  // EXTI Software interrupt event register address offset 0x10
	__vo uint32_t PR;		  	  // EXTI Pending register address offset 0x14
} EXTI_RegDef_t;



/* peripheral base address type casted to xxx_RegDef_t*/

#define GPIOA					  ((GPIO_RegDef_t*)GPIOA_BASEADDR)
#define GPIOB					  ((GPIO_RegDef_t*)GPIOB_BASEADDR)
#define GPIOC					  ((GPIO_RegDef_t*)GPIOC_BASEADDR)
#define GPIOD					  ((GPIO_RegDef_t*)GPIOD_BASEADDR)
#define GPIOE					  ((GPIO_RegDef_t*)GPIOE_BASEADDR)
#define GPIOF					  ((GPIO_RegDef_t*)GPIOF_BASEADDR)
#define GPIOG					  ((GPIO_RegDef_t*)GPIOG_BASEADDR)
#define GPIOH					  ((GPIO_RegDef_t*)GPIOH_BASEADDR)
#define GPIOI					  ((GPIO_RegDef_t*)GPIOI_BASEADDR)

#define RCC						  ((RCC_RegDef_t*)RCC_BASEADDR)

#define SPI1					  ((SPI_RegDef_t*)SPI1_BASEADDR)
#define SPI2					  ((SPI_RegDef_t*)SPI2_BASEADDR)
#define SPI3					  ((SPI_RegDef_t*)SPI3_BASEADDR)
#define SPI4					  ((SPI_RegDef_t*)SPI4_BASEADDR)

#define USART1					  ((USART_RegDef_t*)USART1_BASEADDR)
#define USART2					  ((USART_RegDef_t*)USART2_BASEADDR)
#define USART3					  ((USART_RegDef_t*)USART3_BASEADDR)
#define UART4					  ((USART_RegDef_t*)UART4_BASEADDR)
#define UART5					  ((USART_RegDef_t*)UART5_BASEADDR)
#define USART6					  ((USART_RegDef_t*)USART6_BASEADDR)

#define I2C1					  ((I2C_RegDef_t*)I2C1_BASEADDR)
#define I2C2					  ((I2C_RegDef_t*)I2C2_BASEADDR)
#define I2C3					  ((I2C_RegDef_t*)I2C3_BASEADDR)

#define SYSCFG			  		  ((SYSCFG_RegDef_t*)SYSCFG_BASEADDR)

#define EXTI					  ((EXTI_RegDef_t*)EXTI_BASEADDR)



/* Enable clock for GPIOx peripherals */

#define GPIOA_PCLK_EN() 		  (RCC->AHB1ENR |= (1 << 0))
#define GPIOB_PCLK_EN() 		  (RCC->AHB1ENR |= (1 << 1))
#define GPIOC_PCLK_EN() 		  (RCC->AHB1ENR |= (1 << 2))
#define GPIOD_PCLK_EN() 		  (RCC->AHB1ENR |= (1 << 3))
#define GPIOE_PCLK_EN() 		  (RCC->AHB1ENR |= (1 << 4))
#define GPIOF_PCLK_EN() 		  (RCC->AHB1ENR |= (1 << 5))
#define GPIOG_PCLK_EN() 		  (RCC->AHB1ENR |= (1 << 6))
#define GPIOH_PCLK_EN() 		  (RCC->AHB1ENR |= (1 << 7))
#define GPIOI_PCLK_EN() 		  (RCC->AHB1ENR |= (1 << 8))

/* Enable clock for SPIx peripherals */

#define SPI1_PCLK_EN() 			  (RCC->APB2ENR |= (1 << 12))
#define SPI2_PCLK_EN() 			  (RCC->APB1ENR |= (1 << 14))
#define SPI3_PCLK_EN() 			  (RCC->APB1ENR |= (1 << 15))
#define SPI4_PCLK_EN() 			  (RCC->APB2ENR |= (1 << 13))

/* Enable clock for USARTx peripherals */

#define USART1_PCLK_EN() 		  (RCC->APB2ENR |= (1 << 4))
#define USART2_PCLK_EN() 		  (RCC->APB1ENR |= (1 << 17))
#define USART3_PCLK_EN() 		  (RCC->APB1ENR |= (1 << 18))
#define UART4_PCLK_EN()  		  (RCC->APB1ENR |= (1 << 19))
#define UART5_PCLK_EN()  		  (RCC->APB1ENR |= (1 << 20))
#define USART6_PCLK_EN() 		  (RCC->APB2ENR |= (1 << 5))

/* Enable clock for I2Cx peripherals */

#define I2C1_PCLK_EN() 		  	  (RCC->APB1ENR |= (1 << 21))
#define I2C2_PCLK_EN() 		  	  (RCC->APB1ENR |= (1 << 22))
#define I2C3_PCLK_EN() 		  	  (RCC->APB1ENR |= (1 << 23))

/* Enable clock for SYSCFG peripherals */

#define SYSCFG_PCLK_EN() 		  (RCC->APB2ENR |= (1 << 14))



/* Disable clock for GPIOx peripherals */

#define GPIOA_PCLK_DI() 		  (RCC->AHB1ENR &= ~(1 << 0))
#define GPIOB_PCLK_DI() 		  (RCC->AHB1ENR &= ~(1 << 1))
#define GPIOC_PCLK_DI() 		  (RCC->AHB1ENR &= ~(1 << 2))
#define GPIOD_PCLK_DI() 		  (RCC->AHB1ENR &= ~(1 << 3))
#define GPIOE_PCLK_DI() 		  (RCC->AHB1ENR &= ~(1 << 4))
#define GPIOF_PCLK_DI() 		  (RCC->AHB1ENR &= ~(1 << 5))
#define GPIOG_PCLK_DI() 		  (RCC->AHB1ENR &= ~(1 << 6))
#define GPIOH_PCLK_DI() 		  (RCC->AHB1ENR &= ~(1 << 7))
#define GPIOI_PCLK_DI() 		  (RCC->AHB1ENR &= ~(1 << 8))

/* Disable clock for SPIx peripherals */

#define SPI1_PCLK_DI() 			  (RCC->APB2ENR &= ~(1 << 12))
#define SPI2_PCLK_DI() 			  (RCC->APB1ENR &= ~(1 << 14))
#define SPI3_PCLK_DI() 			  (RCC->APB1ENR &= ~(1 << 15))
#define SPI4_PCLK_DI() 			  (RCC->APB2ENR &= ~(1 << 13))

/* Disable clock for USARTx peripherals */

#define USART1_PCLK_DI() 		  (RCC->APB2ENR &= ~(1 << 4))
#define USART2_PCLK_DI() 		  (RCC->APB1ENR &= ~(1 << 17))
#define USART3_PCLK_DI() 		  (RCC->APB1ENR &= ~(1 << 18))
#define UART4_PCLK_DI()  		  (RCC->APB1ENR &= ~(1 << 19))
#define UART5_PCLK_DI()  		  (RCC->APB1ENR &= ~(1 << 20))
#define USART6_PCLK_DI() 		  (RCC->APB2ENR &= ~(1 << 5))

/* Disable clock for I2Cx peripherals */

#define I2C1_PCLK_DI() 		  	  (RCC->APB1ENR &= ~(1 << 21))
#define I2C2_PCLK_DI() 		  	  (RCC->APB1ENR &= ~(1 << 22))
#define I2C3_PCLK_DI() 		  	  (RCC->APB1ENR &= ~(1 << 23))

/* Disable clock for SYSCFG peripherals */

#define SYSCFG_PCLK_DI() 		  (RCC->APB2ENR &= ~(1 << 14))



/* Reset GPIOx peripherals*/

#define GPIOA_REG_RESET()		  do{(RCC->AHB1RSTR |= (1 << 0));	(RCC->AHB1RSTR &= ~(1 << 0)); }while(0)
#define GPIOB_REG_RESET()		  do{(RCC->AHB1RSTR |= (1 << 1));	(RCC->AHB1RSTR &= ~(1 << 1)); }while(0)
#define GPIOC_REG_RESET()		  do{(RCC->AHB1RSTR |= (1 << 2));	(RCC->AHB1RSTR &= ~(1 << 2)); }while(0)
#define GPIOD_REG_RESET()		  do{(RCC->AHB1RSTR |= (1 << 3));	(RCC->AHB1RSTR &= ~(1 << 3)); }while(0)
#define GPIOE_REG_RESET()		  do{(RCC->AHB1RSTR |= (1 << 4));	(RCC->AHB1RSTR &= ~(1 << 4)); }while(0)
#define GPIOF_REG_RESET()		  do{(RCC->AHB1RSTR |= (1 << 5));	(RCC->AHB1RSTR &= ~(1 << 5)); }while(0)
#define GPIOG_REG_RESET()		  do{(RCC->AHB1RSTR |= (1 << 6));	(RCC->AHB1RSTR &= ~(1 << 6)); }while(0)
#define GPIOH_REG_RESET()		  do{(RCC->AHB1RSTR |= (1 << 7));	(RCC->AHB1RSTR &= ~(1 << 7)); }while(0)
#define GPIOI_REG_RESET()		  do{(RCC->AHB1RSTR |= (1 << 8));	(RCC->AHB1RSTR &= ~(1 << 8)); }while(0)

/* Reset SPIx peripherals*/

#define SPI1_REG_RESET()		  do{(RCC->APB2RSTR |= (1 << 12));	(RCC->APB2RSTR &= ~(1 << 12)); }while(0)
#define SPI2_REG_RESET()		  do{(RCC->APB1RSTR |= (1 << 14));	(RCC->APB1RSTR &= ~(1 << 14)); }while(0)
#define SPI3_REG_RESET()		  do{(RCC->APB1RSTR |= (1 << 15));	(RCC->APB1RSTR &= ~(1 << 15)); }while(0)
#define SPI4_REG_RESET()		  do{(RCC->APB2RSTR |= (1 << 13));	(RCC->APB2RSTR &= ~(1 << 13)); }while(0)

/* Reset I2Cx peripherals*/

#define I2C1_REG_RESET()		  do{(RCC->APB1RSTR |= (1 << 21));	(RCC->APB1RSTR &= ~(1 << 21)); }while(0)
#define I2C2_REG_RESET()		  do{(RCC->APB1RSTR |= (1 << 22));	(RCC->APB1RSTR &= ~(1 << 22)); }while(0)
#define I2C3_REG_RESET()		  do{(RCC->APB1RSTR |= (1 << 23));	(RCC->APB1RSTR &= ~(1 << 23)); }while(0)

/* Reset USARTx peripherals*/

#define USART1_REG_RESET()		  do{(RCC->APB2RSTR |= (1 << 4));	(RCC->APB2RSTR &= ~(1 << 4)); }while(0)
#define USART2_REG_RESET()		  do{(RCC->APB1RSTR |= (1 << 17));	(RCC->APB1RSTR &= ~(1 << 17)); }while(0)
#define USART3_REG_RESET()		  do{(RCC->APB1RSTR |= (1 << 18));	(RCC->APB1RSTR &= ~(1 << 18)); }while(0)
#define UART4_REG_RESET()		  do{(RCC->APB1RSTR |= (1 << 19));	(RCC->APB1RSTR &= ~(1 << 19)); }while(0)
#define UART5_REG_RESET()		  do{(RCC->APB1RSTR |= (1 << 20));	(RCC->APB1RSTR &= ~(1 << 20)); }while(0)
#define USART6_REG_RESET()		  do{(RCC->APB2RSTR |= (1 << 5));	(RCC->APB2RSTR &= ~(1 << 5)); }while(0)



/* return port code for GPIOx address*/

#define GPIO_BASEADDR_TO_CODE(x)  ((x == GPIOA) ? 0 :\
								  (x == GPIOB) ? 1 :\
								  (x == GPIOC) ? 2 :\
								  (x == GPIOD) ? 3 :\
								  (x == GPIOE) ? 4 :\
								  (x == GPIOF) ? 5 :\
								  (x == GPIOG) ? 6 :\
								  (x == GPIOH) ? 7 :\
								  (x == GPIOI) ? 8 : 0)




/* General macros*/

#define ENABLE 					  1
#define DISABLE 				  0
#define SET						  ENABLE
#define RESET					  DISABLE
#define GPIO_PIN_SET			  SET
#define GPIO_PIN_RESET			  RESET
#define FLAG_RESET				  RESET
#define FLAG_SET				  SET



/*numbers for IRQ numbers*/

#define IRQ_NO_EXTI0			  6
#define IRQ_NO_EXTI1			  7
#define IRQ_NO_EXTI2			  8
#define IRQ_NO_EXTI3			  9
#define IRQ_NO_EXTI4			  10
#define IRQ_NO_EXTI5_9			  23
#define IRQ_NO_EXTI10_15		  40

#define IRQ_NO_I2C1_EV			  31
#define IRQ_NO_I2C1_ER			  32
#define IRQ_NO_I2C2_EV			  33
#define IRQ_NO_I2C2_ER			  34
#define IRQ_NO_I2C3_EV			  72
#define IRQ_NO_I2C3_ER			  73

#define IRQ_NO_SPI1				  35
#define IRQ_NO_SPI2				  36
#define IRQ_NO_SPI3				  51
#define IRQ_NO_SPI4				  84
#define IRQ_NO_SPI5				  85
#define IRQ_NO_SPI6				  86

#define IRQ_NO_USART1			  37
#define IRQ_NO_USART2			  38
#define IRQ_NO_USART3			  39
#define IRQ_NO_UART4			  52
#define IRQ_NO_UART5			  53
#define IRQ_NO_USART6			  71

/*Command codes*/

#define COMMAND_LED_CTRL					0x50
#define COMMAND_SENSOR_READ					0x51
#define COMMAND_LED_READ					0x52
#define COMMAND_PRINT						0x53
#define COMMAND_ID_READ						0x54

#define LED_ON								1
#define LED_OFF								0



/*Arduiono analog pins*/

#define ANALOG_PIN0							0
#define ANALOG_PIN1							1
#define ANALOG_PIN2							2
#define ANALOG_PIN3							3
#define ANALOG_PIN4							4
#define ANALOG_PIN5							5



#include "stm32f407xx_rcc_driver.h"
#include "stm32f407xx_gpio_driver.h"
#include "stm32f407xx_i2c_driver.h"
#include "stm32f407xx_spi_driver.h"
#include "stm32f407xx_usart_driver.h"
#include "ds1307.h"
#include "lcd.h"
#include "at24c32.h"
#include "at24C02.h"


#endif /* INC_STM32F407XX_H_ */
