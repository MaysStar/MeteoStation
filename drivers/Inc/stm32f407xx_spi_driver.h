#ifndef INC_STM32F407XX_SPI_DRIVER_H_
#define INC_STM32F407XX_SPI_DRIVER_H_

#include "stm32f407xx.h"

typedef struct
{
	uint8_t SPI_DeviseMode;			/* possible values from @SPI_DEVISE_MODE */
	uint8_t SPI_BusConfig; 			/* possible values from @SPI_BUS_CONFIG */
	uint8_t SPI_SClkSpeed;			/* possible values from @SPI_SCLK_SPEED */
	uint8_t SPI_DFF;				/* possible values from @SPI_DFF */
	uint8_t SPI_CPOL;				/* possible values from @SPI_CPOL */
	uint8_t SPI_CPHA;				/* possible values from @SPI_CPHA */
	uint8_t SPI_SSM;				/* possible values from @SPI_SSM */
}SPI_PinConfig_t;



/*Handle structure for a SPIx peripherals*/

typedef struct
{
	SPI_RegDef_t* pSPIx; 			// holds the base address of the SPI port which belongs
	SPI_PinConfig_t SPI_PinConfig; 	// holds pin configuration settings
	uint8_t *pTxBuffer;				// to store app. TxBuffer address
	uint8_t *pRxBuffer;				// to store app. RxBuffer address
	uint32_t TxLen;					// to store app. TxBuffer Len
	uint32_t RxLen;					// to store app. RxBuffer Len
	uint8_t  TxState;				// to store app. TxBuffer state
	uint8_t  RxState;				// to store app. RxBuffer state
}SPI_Handle_t;



/*@SPI_DEVISE_MODE*/

#define SPI_DEVISE_MODE_SLAVE				0
#define SPI_DEVISE_MODE_MASTER				1

/*@SPI_BUS_CONFIG*/

#define SPI_BUS_CONFIG_FD					1
#define SPI_BUS_CONFIG_HD					2
#define SPI_BUS_CONFIG_SIMPLEX_RXONLY		3

/*SPI_SCLK_SPEED*/

#define SPI_SCLK_SPEED_DIV2					0
#define SPI_SCLK_SPEED_DIV4					1
#define SPI_SCLK_SPEED_DIV8					2
#define SPI_SCLK_SPEED_DIV16				3
#define SPI_SCLK_SPEED_DIV32				4
#define SPI_SCLK_SPEED_DIV64				5
#define SPI_SCLK_SPEED_DIV128				6
#define SPI_SCLK_SPEED_DIV256				7

/*SPI_DFF*/

#define SPI_DFF_BITS8						0
#define SPI_DFF_BITS16						1

/*SPI_CPOL*/

#define SPI_CPOL_LOW						0
#define SPI_CPOL_HIGH						1

/*SPI_CPHA*/

#define SPI_CPHA_LOW						0
#define SPI_CPHA_HIGH						1

/*SPI_SSM*/

#define SPI_SSM_DI							0
#define SPI_SSM_EN							1

/* SPI related status flags definitions*/

#define SPI_RXNE_FLAG						(1 << SPI_SR_RXNE)
#define SPI_TXE_FLAG						(1 << SPI_SR_TXE)
#define SPI_CHSIDE_FLAG						(1 << SPI_SR_CHSIDE)
#define SPI_UDR_FLAG						(1 << SPI_SR_UDR)
#define SPI_CRCERR_FLAG						(1 << SPI_SR_CRCERR)
#define SPI_MODF_FLAG						(1 << SPI_SR_MODF)
#define SPI_OVR_FLAG						(1 << SPI_SR_OVR)
#define SPI_BSY_FLAG						(1 << SPI_SR_BSY)
#define SPI_FRE_FLAG						(1 << SPI_SR_FRE)

/*SPI interrupt state*/

#define SPI_READY							0
#define SPI_BUSY_IN_TX						1
#define SPI_BUSY_IN_RX						2

/*Possible event types*/

#define SPI_EVENT_TX_CMPLT					1
#define SPI_EVENT_RX_CMPLT					2
#define SPI_EVENT_OVR_ERR					3
#define SPI_EVENT_CRC_ERR					4



/*Bit position definitions of SPI peripheral*/

/* Bit position definitions of CR1 register of SPI */

#define SPI_CR1_CPHA				 		0
#define SPI_CR1_CPOL						1
#define SPI_CR1_MSTR						2
#define SPI_CR1_BR							3
#define SPI_CR1_SPE		  					6
#define SPI_CR1_LSBFIRST		  			7
#define SPI_CR1_SSI			 				8
#define SPI_CR1_SSM							9
#define SPI_CR1_RXONLY		  				10
#define SPI_CR1_DFF							11
#define SPI_CR1_CRCNEXT		  				12
#define SPI_CR1_CRCEN		  				13
#define SPI_CR1_BIDIOE		  				14
#define SPI_CR1_BIDIMODE		  			15

/* Bit position definitions of CR2 register of SPI */

#define SPI_CR2_RXDMAEN						0
#define SPI_CR2_TXDMAEN						1
#define SPI_CR2_SSOE						2
#define SPI_CR2_FRF							4
#define SPI_CR2_ERRIE						5
#define SPI_CR2_RXNEIE						6
#define SPI_CR2_TXEIE						7

/* Bit position definitions of status register of SPI */

#define SPI_SR_RXNE							0
#define SPI_SR_TXE							1
#define SPI_SR_CHSIDE						2
#define SPI_SR_UDR							3
#define SPI_SR_CRCERR						4
#define SPI_SR_MODF							5
#define SPI_SR_OVR							6
#define SPI_SR_BSY							7
#define SPI_SR_FRE							8

/*APIs supported by this driver*/

/* Init and De-init */

void SPI_Init(SPI_Handle_t *pSPIHandle);
void SPI_DeInit(SPI_RegDef_t *pSPIx);

/* Peripheral SPI control*/

void SPI_PeripheralControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi);

/*Configuration of NSS in software mode*/

void SPI_SSIConfig(SPI_RegDef_t *pSPIx, uint8_t EnorDi);

/*Configuration of NSS in hardware mode*/

void SPI_SSOEConfig(SPI_RegDef_t *pSPIx, uint8_t EnorDi);

/* Peripheral clock setup*/

void SPI_PeriClockControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi);

/* Data send and receive*/

void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t Len);
void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t Len);

/* Data send and receive IT*/

uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pTxBuffer, uint32_t Len);
uint8_t SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pRxBuffer, uint32_t Len);

/*IRQ configuration and ISR handing*/

void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi);
void SPI_PriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void SPI_IRQHandling(SPI_Handle_t *pSPI_Handle);
void SPI_ClearOVRFlag(SPI_RegDef_t *pSPIx);
void SPI_CloseTransmission(SPI_Handle_t *pSPI_Handle);
void SPI_CloseReception(SPI_Handle_t *pSPI_Handle);

/*Get status flag*/

uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx, uint32_t FlagName);

/*Application callback*/

void SPI_ApplicationEventCallback(SPI_Handle_t *pSPI_Handle, uint8_t EVENT);

#endif /* INC_STM32F407XX_SPI_DRIVER_H_ */
