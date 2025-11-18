#ifndef INC_STM32F407XX_I2C_DRIVER_H_
#define INC_STM32F407XX_I2C_DRIVER_H_

#include "stm32f407xx.h"

/*
 * Configuration structure for I2Cx peripheral
 */

typedef struct
{
	uint32_t	I2C_SCLSpeed;			/* possible values from @SPI_SCLSPEED */
	uint8_t		I2C_Device_Address;		/* possible values from @SPI_DEVISE_ADDRESS */
	uint8_t		I2C_ACKControl;			/* possible values from @SPI_ACK_CONTROL */
	uint8_t		I2C_FMDutyCycle;		/* possible values from @SPI_FM_DUTY_CYCLE */
}I2C_Config_t;

/*
 * Handle structure for I2Cx peripheral
 */

typedef struct
{
	I2C_RegDef_t *pI2Cx;				/* holds the base address of the I2C port which belongs */
	I2C_Config_t I2C_Config;			/* holds the configuration of the I2C */
	uint8_t		*pTxbuffer;				/* to store app. TxBuffer address */
	uint8_t		*pRxbuffer;				/* to store app. RxBuffer address */
	uint32_t	TxLen;					/* to store app. Tx length */
	uint32_t	RxLen;					/* to store app. Rx length */
	uint8_t		TxRxState;				/* to store app. Tx or Rx state */
	uint8_t 	DevAddr;				/* to store app. device address */
	uint32_t	RxSize;					/* to store app. Rx size */
	uint8_t		STOP;					/* to store app. Stop or repetitive start */
}I2C_Handle_t;


/*
 *	@SPI_SCLSPEED
 */

#define I2C_SCL_SPEED_SM		   100000
#define I2C_SCL_SPEED_FM2K		   200000
#define I2C_SCL_SPEED_FM4K		   400000

/*
 *	@SPI_ACK_CONTROL
 */

#define I2C_ACK_DISABLE					0
#define I2C_ACK_ENABLE					1

/*
 *	@SPI_FM_DUTY_CYCLE
 */

#define I2C_FM_DUTY_CYCLE2				0
#define I2C_FM_DUTY_CYCLE16_9			1

/*
 * Bit position definitions I2C_CR1
 */
#define I2C_CR1_PE						0
#define I2C_CR1_SMBUS					1
#define I2C_CR1_SMBTYPE					3
#define I2C_CR1_ENARP					4
#define I2C_CR1_ENPEC					5
#define I2C_CR1_ENGC					6
#define I2C_CR1_NOSTRETCH  				7
#define I2C_CR1_START 					8
#define I2C_CR1_STOP  				 	9
#define I2C_CR1_ACK 				 	10
#define I2C_CR1_POS 				 	11
#define I2C_CR1_PEC 				 	12
#define I2C_CR1_ALERT 				 	13
#define I2C_CR1_SWRST  				 	15

/*
 * Bit position definitions I2C_CR2
 */
#define I2C_CR2_FREQ				 	0
#define I2C_CR2_ITERREN				 	8
#define I2C_CR2_ITEVTEN				 	9
#define I2C_CR2_ITBUFEN 			    10
#define I2C_CR2_DMAEN 			    	11
#define I2C_CR2_LAST 			   		12

/*
 * Bit position definitions I2C_OAR1
 */
#define I2C_OAR1_ADD0    				 0
#define I2C_OAR1_ADD71 				 	 1
#define I2C_OAR1_ADD98  			 	 8
#define I2C_OAR1_ADDMODE   			 	15

/*
 * Bit position definitions I2C_SR1
 */

#define I2C_SR1_SB 					 	0
#define I2C_SR1_ADDR 				 	1
#define I2C_SR1_BTF 					2
#define I2C_SR1_ADD10 					3
#define I2C_SR1_STOPF 					4
#define I2C_SR1_RXNE 					6
#define I2C_SR1_TXE 					7
#define I2C_SR1_BERR 					8
#define I2C_SR1_ARLO 					9
#define I2C_SR1_AF 					 	10
#define I2C_SR1_OVR 					11
#define I2C_SR1_PECERR 				 	12
#define I2C_SR1_TIMEOUT 				14
#define I2C_SR1_SMBALERT			 	15

/*
 * Bit position definitions I2C_SR2
 */
#define I2C_SR2_MSL						0
#define I2C_SR2_BUSY 					1
#define I2C_SR2_TRA 					2
#define I2C_SR2_GENCALL 				4
#define I2C_SR2_SMBDEFAULT 				5
#define I2C_SR2_SMBHOST 				6
#define I2C_SR2_DUALF 					7

/*
 * Status flags
 */

#define I2C_SB_FLAG					 	(1 << I2C_SR1_SB)
#define I2C_ADDR_FLAG 				 	(1 << I2C_SR1_ADDR)
#define I2C_BTF_FLAG 					(1 << I2C_SR1_BTF)
#define I2C_ADD10_FLAG 					(1 << I2C_SR1_ADD10)
#define I2C_STOPF_FLAG 					(1 << I2C_SR1_STOPF)
#define I2C_RXNE_FLAG 					(1 << I2C_SR1_RXNE)
#define I2C_TXE_FLAG 					(1 << I2C_SR1_TXE)
#define I2C_BERR_FLAG 					(1 << I2C_SR1_BERR)
#define I2C_ARLO_FLAG 					(1 << I2C_SR1_ARLO)
#define I2C_AF_FLAG 					(1 << I2C_SR1_AF)
#define I2C_OVR_FLAG 					(1 << I2C_SR1_OVR)
#define I2C_PECERR_FLAG 				(1 << I2C_SR1_PECERR)
#define I2C_TIMEOUT_FLAG 				(1 << I2C_SR1_TIMEOUT)
#define I2C_SMBALERT_FLAG			 	(1 << I2C_SR1_SMBALERT)

#define I2C_MSL_FLAG					(1 << I2C_SR2_MSL)
#define I2C_BUSY_FLAG 					(1 << I2C_SR2_BUSY)
#define I2C_TRA_FLAG 					(1 << I2C_SR2_TRA)
#define I2C_GENCALL_FLAG 				(1 << I2C_SR2_GENCALL)
#define I2C_SMBDEFAULT_FLAG 			(1 << I2C_SR2_SMBDEFAULT)
#define I2C_SMBHOST_FLAG 				(1 << I2C_SR2_SMBHOST)
#define I2C_DUALF_FLAG 					(1 << I2C_SR2_DUALF)

/*
 * Bit position definitions I2C_CCR
 */
#define I2C_CCR_CCR 					 0
#define I2C_CCR_DUTY 					14
#define I2C_CCR_FS  				 	15

/*I2C repetitive start and stop*/

#define I2C_STOP						RESET
#define I2C_REPETITIVE_START			SET

/* I2C application states*/

#define I2C_READY						0
#define I2C_BUSY_IN_TX					1
#define I2C_BUSY_IN_RX					2

/*Possible event types*/

#define I2C_EV_TX_CMPLT					0
#define I2C_EV_RX_CMPLT					1
#define I2C_EV_STOP						2
#define I2C_EV_DATA_REQ					3
#define I2C_EV_DATA_RCV					4

#define I2C_ERROR_BERR					10
#define I2C_ERROR_ARLO					11
#define I2C_ERROR_AF					12
#define I2C_ERROR_OVR					13
#define I2C_ERROR_TIMEOUT				14


/*APIs supported by this driver*/

/* Init and De-init */

void I2C_Init(I2C_Handle_t *pI2CHandle);
void I2C_DeInit(I2C_RegDef_t *pI2Cx);

/* Peripheral I2C control*/

void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx, uint8_t EnorDi);

/* Peripheral clock setup*/

void I2C_PeriClockControl(I2C_RegDef_t *pI2Cx, uint8_t EnorDi);

/* Data send and receive*/

void I2C_MasterSendData(I2C_Handle_t *pI2CHandle, uint8_t* pTxBuffer, uint32_t Len, uint8_t SlaveAddress, uint8_t STOP);
void I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle, uint8_t* pRxBuffer, uint32_t Len, uint8_t SlaveAddress, uint8_t STOP);

uint8_t I2C_MasterSendDataIT(I2C_Handle_t *pI2CHandle, uint8_t* pTxBuffer, uint32_t Len, uint8_t SlaveAddress, uint8_t STOP);
uint8_t I2C_MasterReceiveDataIT(I2C_Handle_t *pI2CHandle, uint8_t* pRxBuffer, uint32_t Len, uint8_t SlaveAddress, uint8_t STOP);

void I2C_SlaveSendData(I2C_RegDef_t* pI2Cx, uint8_t data);
uint8_t I2C_SlaveReceiveData(I2C_RegDef_t* pI2Cx);


void I2C_ManageAcking(I2C_RegDef_t *pI2Cx, uint8_t EnorDi);

/*IRQ configuration and ISR handing*/

void I2C_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi);
void I2C_PriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void I2C_EV_IRQHandling(I2C_Handle_t *pI2CHandle);
void I2C_ER_IRQHandling(I2C_Handle_t *pI2CHandle);

void I2C_GenerateStopCondition(I2C_RegDef_t *pI2Cx);

void I2C_CloseSendData(I2C_Handle_t *pI2CHandle);
void I2C_CloseReceiveData(I2C_Handle_t *pI2CHandle);

/*Get status flag*/

uint8_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx, uint32_t FlagName);

/*Application callback*/

void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle, uint8_t EVENT);
void I2C_SlaveEnableDisableCallbackEvents(I2C_RegDef_t *pI2Cx, uint8_t EnorDi);



#endif /* INC_STM32F407XX_I2C_DRIVER_H_ */
