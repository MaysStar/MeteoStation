#ifndef INC_STM32F407XX_USART_DRIVER_H_
#define INC_STM32F407XX_USART_DRIVER_H_

#include "stm32f407xx.h"

/*
 * Configuration structure for USARTx peripheral
 */

typedef struct
{
	uint8_t USART_Mode;					/* possible values from @USART_MODE */
	uint32_t USART_Baud;				/* possible values from @USART_BAUD */
	uint8_t	USART_NoOfStopBits;			/* possible values from @USART_NO_STOP_BIT */
	uint8_t USART_WordLength;			/* possible values from @USART_WORD_LENGTH */
	uint8_t USART_ParityControl;		/* possible values from @USART_PARITY_CONTROL */
	uint8_t USART_HWFlowControl;		/* possible values from @USART_HW_FLOW_CONTROL */
}USART_Config_t;

/*
 * Handle structure for USARTx peripheral
 */

typedef struct
{
	USART_RegDef_t *pUSARTx;			/* holds the base address of the USART port which belongs */
	USART_Config_t USART_Config_t;		/* holds the configuration of the USART */
	uint8_t *pTxBuffer;					/* to store app. TxBuffer address */
	uint8_t *pRxBuffer;					/* to store app. RxBuffer address */
	uint32_t TxLen;						/* to store app. TxBuffer Len */
	uint32_t RxLen;						/* to store app. RxBuffer Len */
	uint8_t  TxState;					/* to store app. TxBuffer state */
	uint8_t  RxState;					/* to store app. RxBuffer state */
}USART_Handle_t;

/*
 * @USART_MODE
 */

#define USART_MODE_ONLY_TX				0
#define USART_MODE_ONLY_RX				1
#define USART_MODE_TXRX					2

/*
 * @USART_BAUD
 */

#define USART_BAUD_1200					1200
#define USART_BAUD_2400					2400
#define USART_BAUD_9600					9600
#define USART_BAUD_19200				19200
#define USART_BAUD_38400				38400
#define USART_BAUD_57600				57600
#define USART_BAUD_115200				115200
#define USART_BAUD_230400				230400
#define USART_BAUD_460800				460800
#define USART_BAUD_921600				921600
#define USART_BAUD_2M					2000000
#define USART_BAUD_3M					3000000

/*
 * @USART_NO_STOP_BIT
 */

#define USART_NO_STOP_BIT_ONE			0
#define USART_NO_STOP_BIT_HALF			1
#define USART_NO_STOP_BIT_TWO			2
#define USART_NO_STOP_BIT_ONE_HALF		3

/*
 * @USART_WORD_LENGTH
 */

#define USART_WORD_LENGTH_8				0
#define USART_WORD_LENGTH_9				1

/*
 * @USART_PARITY_CONTROL
 */

#define USART_PARITY_CONTROL_DI			0
#define USART_PARITY_CONTROL_EN_ODD		1
#define USART_PARITY_CONTROL_EN_EVEN	2
/*
 * @USART_HW_FLOW_CONTROL
 */

#define USART_HW_FLOW_CONTROL_NONE		0
#define USART_HW_FLOW_CONTROL_CTS		1
#define USART_HW_FLOW_CONTROL_RTS		2
#define USART_HW_FLOW_CONTROL_CTS_RTS	3

/*
 * Bit position definitions USART_SR
 */

#define USART_SR_PE						0
#define USART_SR_FE						1
#define USART_SR_NF						2
#define USART_SR_ORE					3
#define USART_SR_IDLE					4
#define USART_SR_RXNE					5
#define USART_SR_TC						6
#define USART_SR_TXE					7
#define USART_SR_LBD					8
#define USART_SR_CTS					9

/*
 * Bit position definitions USART_CR1
 */

#define USART_CR1_SBK					0
#define USART_CR1_RWU					1
#define USART_CR1_RE					2
#define USART_CR1_TE					3
#define USART_CR1_IDLEIE				4
#define USART_CR1_RXNEIE				5
#define USART_CR1_TCIE					6
#define USART_CR1_TXEIE					7
#define USART_CR1_PEIE					8
#define USART_CR1_PS					9
#define USART_CR1_PCE					10
#define USART_CR1_WAKE					11
#define USART_CR1_M						12
#define USART_CR1_UE					13
#define USART_CR1_OVER8					15

/*
 * Bit position definitions USART_CR2
 */

#define USART_CR2_ADD					0
#define USART_CR2_LBDL					5
#define USART_CR2_LBDIE					6
#define USART_CR2_LBCL					8
#define USART_CR2_CPHA					9
#define USART_CR2_CPOL					10
#define USART_CR2_CLKEN					11
#define USART_CR2_STOP					12
#define USART_CR2_LINEN					14

/*
 * Bit position definitions USART_CR3
 */

#define USART_CR3_EIE					0
#define USART_CR3_IREN					1
#define USART_CR3_IRLP					2
#define USART_CR3_HDSEL					3
#define USART_CR3_NACK					4
#define USART_CR3_SCEN					5
#define USART_CR3_DMAR					6
#define USART_CR3_DMAT					7
#define USART_CR3_RTSE					8
#define USART_CR3_CTSE					9
#define USART_CR3_CTSIE					10
#define USART_CR3_ONEBIT				11

/* USART_SR flag status*/

#define USART_PE_FLAG					(1 << USART_SR_PE)
#define USART_FE_FLAG					(1 << USART_SR_FE)
#define USART_NF_FLAG					(1 << USART_SR_NF)
#define USART_ORE_FLAG					(1 << USART_SR_ORE)
#define USART_IDLE_FLAG					(1 << USART_SR_IDLE)
#define USART_RXNE_FLAG					(1 << USART_SR_RXNE)
#define USART_TC_FLAG					(1 << USART_SR_TC)
#define USART_TXE_FLAG					(1 << USART_SR_TXE)
#define USART_LBD_FLAG					(1 << USART_SR_LBD)
#define USART_CTS_FLAG					(1 << USART_SR_CTS)

/* USART Interrupt state */

#define USART_READY						0
#define USART_BUSY_IN_TX				1
#define USART_BUSY_IN_RX				2

/* Possible EVENT */

#define USART_EVENT_RX_CMPLT			0
#define USART_EVENT_TX_CMPLT			1
#define USART_EVENT_CTS					2
#define USART_EVENT_IDLE				3
#define USART_EVENT_ORE					4
#define USART_ERREVENT_FE				5
#define USART_ERREVENT_NE				6
#define USART_ERREVENT_ORE				7

/*APIs supported by this driver*/

/* Init and De-init */

void USART_Init(USART_Handle_t *pUSARTHandle);
void USART_DeInit(USART_RegDef_t *pUSARTx);

/* Peripheral USART control*/

void USART_PeripheralControl(USART_RegDef_t *pUSARTx, uint8_t EnorDi);

/* Peripheral clock setup*/

void USART_PeriClockControl(USART_RegDef_t *pUSARTx, uint8_t EnorDi);

/* Calculate the baud rate */

void USART_SetBaudRate(USART_RegDef_t *pUSARTx, uint32_t BaudRate);

/* Data Send and Receive */

void USART_SendData(USART_Handle_t *pUSARTHandle, uint8_t *pTxBuffer, uint32_t Len);
void USART_ReceiveData(USART_Handle_t *pUSARTHandle, uint8_t *pRxBuffer, uint32_t Len);
uint8_t USART_SendDataIT(USART_Handle_t *pUSARTHandle, uint8_t *pTxBuffer, uint32_t Len);
uint8_t USART_ReceiveDataIT(USART_Handle_t *pUSARTHandle, uint8_t *pRxBuffer, uint32_t Len);

/*IRQ configuration and ISR handing*/

void USART_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi);
void USART_PriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void USART_IRQHandling(USART_Handle_t *pUSARTHandle);

/*Get status flag and clear status flag*/

uint8_t USART_GetFlagStatus(USART_RegDef_t *pUSARTx, uint32_t FlagName);
void USART_ClearFlag(USART_RegDef_t *pUSARTx, uint32_t FlagName);

/*Application callback*/

void USART_ApplicationEventCallback(USART_Handle_t *pUSARTHandle, uint8_t EVENT);

#endif /* INC_STM32F407XX_USART_DRIVER_H_ */
