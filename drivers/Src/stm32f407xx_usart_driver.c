#include "stm32f407xx_usart_driver.h"

/*********************************************************************
 * @fn      		  - USART_Init
 *
 * @brief             - This function configure all pin's settings in start value
 *
 * @param[in]         - the base address of the USARTx and USARTx configuration settings
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

void USART_Init(USART_Handle_t *pUSARTHandle)
{
	// temporary register
	uint32_t tempreg = 0;

	// 1. Enable peripheral clock
	USART_PeriClockControl(pUSARTHandle->pUSARTx, ENABLE);

	/******************************** Configuration of CR1******************************************/

	// 2. Configure the USART mode
	if(pUSARTHandle->USART_Config_t.USART_Mode == USART_MODE_ONLY_RX)
	{
		// configure only RX mode
		tempreg |= (1 << USART_CR1_RE);
	}
	else if(pUSARTHandle->USART_Config_t.USART_Mode == USART_MODE_ONLY_TX)
	{
		// configure only TX mode
		tempreg |= (1 << USART_CR1_TE);
	}
	else
	{
		// configure both TX and RX modes
		tempreg |= (1 << USART_CR1_RE);
		tempreg |= (1 << USART_CR1_TE);
	}

	// 3. Configure word length
	tempreg |= (pUSARTHandle->USART_Config_t.USART_WordLength << USART_CR1_M);

	// 4. Configure parity control bit field
	if(pUSARTHandle->USART_Config_t.USART_ParityControl == USART_PARITY_CONTROL_EN_ODD)
	{
		tempreg |= (1 << USART_CR1_PCE);

		tempreg |= (1 << USART_CR1_PS);
	}
	else if(pUSARTHandle->USART_Config_t.USART_ParityControl == USART_PARITY_CONTROL_EN_EVEN)
	{
		tempreg |= (1 << USART_CR1_PCE);
	}

	// 5. Program CR1
	pUSARTHandle->pUSARTx->CR1 = tempreg;

	tempreg = 0;

	/******************************** Configuration of CR2******************************************/

	// 6. Configure stop bits
	tempreg |= (pUSARTHandle->USART_Config_t.USART_NoOfStopBits << USART_CR2_STOP);

	// 7. Program CR1
	pUSARTHandle->pUSARTx->CR2 = tempreg;

	tempreg = 0;

	/******************************** Configuration of CR3******************************************/

	// 8. Configure CTS and RTC
	if(pUSARTHandle->USART_Config_t.USART_HWFlowControl == USART_HW_FLOW_CONTROL_CTS)
	{
		// configure only CTS
		tempreg |= (1 << USART_CR3_CTSE);
	}
	else if(pUSARTHandle->USART_Config_t.USART_HWFlowControl == USART_HW_FLOW_CONTROL_RTS)
	{
		// configure only RTS
		tempreg |= (1 << USART_CR3_RTSE);
	}
	else if(pUSARTHandle->USART_Config_t.USART_HWFlowControl == USART_HW_FLOW_CONTROL_CTS_RTS)
	{
		// configure both CTS and RTS
		tempreg |= (1 << USART_CR3_CTSE);
		tempreg |= (1 << USART_CR3_RTSE);
	}

	// 9. Program CR3
	pUSARTHandle->pUSARTx->CR3 = tempreg;

	tempreg = 0;
	/******************************** Configuration of BRR(Baudrate register)******************************************/

	// 10. Configure the baud rate
	USART_SetBaudRate(pUSARTHandle->pUSARTx, pUSARTHandle->USART_Config_t.USART_Baud);
}

/*********************************************************************
 * @fn      		  - USART_DeInit
 *
 * @brief             - This function reset all USARTx pin
 *
 * @param[in]         - the base address of the USARTx
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

void USART_DeInit(USART_RegDef_t *pUSARTx)
{
	if(pUSARTx == USART1)
	{
		USART1_REG_RESET();
	}
	else if(pUSARTx == USART2)
	{
		USART2_REG_RESET();
	}
	else if(pUSARTx == USART3)
	{
		USART3_REG_RESET();
	}
	else if(pUSARTx == UART4)
	{
		UART4_REG_RESET();
	}
	else if(pUSARTx == UART5)
	{
		UART5_REG_RESET();
	}
	else if(pUSARTx == USART6)
	{
		USART6_REG_RESET();
	}
}

/*********************************************************************
 * @fn      		  - USART_PeripheralControl
 *
 * @brief             - This function enables or disables peripheral for the given USART
 *
 * @param[in]         - base address of the USART peripheral
 * @param[in]         - ENABLE or DISABLE macros
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

void USART_PeripheralControl(USART_RegDef_t *pUSARTx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		pUSARTx->CR1 |= (1 << USART_CR1_UE);
	}
	else
	{
		pUSARTx->CR1 &= ~(1 << USART_CR1_UE);
	}
}

/*********************************************************************
 * @fn      		  - USART_PeriClockControl
 *
 * @brief             - This function enables or disables peripheral clock for the given USART
 *
 * @param[in]         - base address of the USART peripheral
 * @param[in]         - ENABLE or DISABLE macros
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

void USART_PeriClockControl(USART_RegDef_t *pUSARTx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		if(pUSARTx == USART1)
		{
			USART1_PCLK_EN();
		}
		else if(pUSARTx == USART2)
		{
			USART2_PCLK_EN();
		}
		else if(pUSARTx == USART3)
		{
			USART3_PCLK_EN();
		}
		else if(pUSARTx == UART4)
		{
			UART4_PCLK_EN();
		}
		else if(pUSARTx == UART5)
		{
			UART5_PCLK_EN();
		}
		else if(pUSARTx == USART6)
		{
			USART6_PCLK_EN();
		}
	}
	else
	{
		if(pUSARTx == USART1)
		{
			USART1_PCLK_DI();
		}
		else if(pUSARTx == USART2)
		{
			USART2_PCLK_DI();
		}
		else if(pUSARTx == USART3)
		{
			USART3_PCLK_DI();
		}
		else if(pUSARTx == UART4)
		{
			UART4_PCLK_DI();
		}
		else if(pUSARTx == UART5)
		{
			UART5_PCLK_DI();
		}
		else if(pUSARTx == USART6)
		{
			USART6_PCLK_DI();
		}
	}
}

/*********************************************************************
 * @fn      		  - USART_SetBaudRate
 *
 * @brief             - This function set the baud rate
 *
 * @param[in]         - the USART handle
 * @param[in]         - baud rate given in @USART_BAUD
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -

 */

void USART_SetBaudRate(USART_RegDef_t *pUSARTx, uint32_t BaudRate)
{
	// variable to hold the APB clock
	uint32_t PCLKx;

	// variable to hold USARTDIV
	uint32_t usartdiv;

	// variable to hold Mantissa and Fraction values
	uint32_t M_part, F_part;

	// temporary register
	uint32_t tempreg = 0;

	// 1. Get value of APB bus clock to the PCLKx
	if(pUSARTx == USART1 || pUSARTx == USART6)
	{
		PCLKx = RCC_GetPCLK2Value();
	}
	else
	{
		PCLKx = RCC_GetPCLK1Value();
	}

	// 2. Check for OVER8 configuration bit
	if(pUSARTx->CR1 & (1 << USART_CR1_OVER8))
	{
		usartdiv = (25 * PCLKx / (2 * BaudRate));
	}
	else
	{
		usartdiv = (25 * PCLKx / (4 * BaudRate));
	}

	// 3. Calculate Mantissa part
	M_part = usartdiv / 100;

	// 4. Write M part to tempreg
	tempreg |= (M_part << 4);

	// 5. Calculate Fraction part
	F_part = (usartdiv - (M_part * 100 ));

	// 6. Check for OVER8 configuration bit to calculate Fraction part
	if(pUSARTx->CR1 & (1 << USART_CR1_OVER8))
	{
		F_part = (((F_part * 8 + 50) / 100) & (uint8_t)0x07);
	}
	else
	{
		F_part = (((F_part * 16 + 50) / 100) & (uint8_t)0x0F);
	}

	// 7. Write F part to tempreg

	tempreg |= F_part;

	// 8. Set baud rate to BRR
	pUSARTx->BRR = tempreg;
}

/*********************************************************************
 * @fn      		  - USART_GetFlagStatus
 *
 * @brief             - This function allows get status flag for some USART flag
 *
 * @param[in]         - the base address of the USARTx
 * @param[in]         - flag name
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -

 */

uint8_t USART_GetFlagStatus(USART_RegDef_t *pUSARTx, uint32_t FlagName)
{
	if(pUSARTx->SR & FlagName)
	{
		return FLAG_SET;
	}
	return FLAG_RESET;
}

/*********************************************************************
 * @fn      		  - USART_ClearFlag
 *
 * @brief             - This function clear status flag for some USART
 *
 * @param[in]         - the base address of the USARTx
 * @param[in]         - flag name
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -

 */

void USART_ClearFlag(USART_RegDef_t *pUSARTx, uint32_t FlagName)
{
	pUSARTx->SR &= ~(FlagName);
}

/*********************************************************************
 * @fn      		  - USART_SendData
 *
 * @brief             - This function allows send data
 *
 * @param[in]         - the USART handle
 * @param[in]         - Tx buffer
 * @param[in]         -	length
 *
 * @return            -
 *
 * @Note              - blocking API
 */

void USART_SendData(USART_Handle_t *pUSARTHandle, uint8_t *pTxBuffer, uint32_t Len)
{
	uint16_t *pdata;

	// 1.Loop until len number of bytes are transfered
	for(uint32_t i = 0; i < Len; ++i)
	{
		// 2. Wait for TXE
		while(!USART_GetFlagStatus(pUSARTHandle->pUSARTx, USART_TXE_FLAG));

		// 3. Check bit is 9 or 8
		if(pUSARTHandle->USART_Config_t.USART_WordLength == USART_WORD_LENGTH_9)
		{
			// 9 bits
			pdata = (uint16_t*)pTxBuffer;
			pUSARTHandle->pUSARTx->DR = (*pdata & (uint16_t)0x1FF);

			// Check for USART_ParityControl
			if(pUSARTHandle->USART_Config_t.USART_ParityControl == USART_PARITY_CONTROL_DI)
			{
				// NO parity, so 9 bits of user data will be sent
				pTxBuffer++;
				pTxBuffer++;
			}
			else
			{
				// Parity is present, so 8 bits of user data will be sent and 1 bit of parity by hardware
				pTxBuffer++;
			}
		}
		else
		{
			// 8 bits
			pUSARTHandle->pUSARTx->DR = (*pTxBuffer & (uint8_t)0x0FF);

			// Increment the Txbuffer
			pTxBuffer++;
		}
	}

	// 4. Wait until transmission is finished
	while(USART_GetFlagStatus(pUSARTHandle->pUSARTx, USART_TC_FLAG));
}

/*********************************************************************
 * @fn      		  - USART_ReceiveData
 *
 * @brief             - This function allows receive data
 *
 * @param[in]         - the USART handle
 * @param[in]         - Rx buffer
 * @param[in]         -	length
 *
 * @return            -
 *
 * @Note              - blocking API
 */

void USART_ReceiveData(USART_Handle_t *pUSARTHandle, uint8_t *pRxBuffer, uint32_t Len)
{
	// 1. Loop until len number of bytes are transfered
	for(uint32_t i = 0; i < Len; ++i)
	{
		// 2. Wait until RXNE is set
		while(!USART_GetFlagStatus(pUSARTHandle->pUSARTx, USART_RXNE_FLAG));

		// 3. Check bit is 9 or 8
		if(pUSARTHandle->USART_Config_t.USART_WordLength == USART_WORD_LENGTH_9)
		{
			// 9 bits

			// Check for USART_ParityControl
			if(pUSARTHandle->USART_Config_t.USART_ParityControl == USART_PARITY_CONTROL_DI)
			{
				// read 9 bits
				*((uint16_t*)pRxBuffer) = (pUSARTHandle->pUSARTx->DR & (uint16_t)0x1FF);

				// Increment the Rxbuffer 2 times
				pRxBuffer++;
				pRxBuffer++;
			}
			else
			{
				// read 8 bits
				*(pRxBuffer) = (pUSARTHandle->pUSARTx->DR & (uint8_t)0x0FF);

				// Increment the Rxbuffer
				pRxBuffer++;
			}
		}
		else
		{
			// 8 bits

			// Check for USART_ParityControl
			if(pUSARTHandle->USART_Config_t.USART_ParityControl == USART_PARITY_CONTROL_DI)
			{
				// read 8 bits
				*(pRxBuffer) = (pUSARTHandle->pUSARTx->DR & (uint8_t)0x0FF);
			}
			else
			{
				// read 7 bits
				*(pRxBuffer) = (pUSARTHandle->pUSARTx->DR & (uint8_t)0x7F);
			}

			// Increment the Rxbuffer
			pRxBuffer++;
		}
	}
}

/*********************************************************************
 * @fn      		  - USART_SendDataIT
 *
 * @brief             - This function allows send data due to interrupts
 *
 * @param[in]         - the USART handle
 * @param[in]         - Tx buffer
 * @param[in]         -	length
 *
 * @return            - state of interrupt
 *
 * @Note              - non blocking API
 */

uint8_t USART_SendDataIT(USART_Handle_t *pUSARTHandle, uint8_t *pTxBuffer, uint32_t Len)
{
	uint8_t state = pUSARTHandle->TxState;

	if(state != USART_BUSY_IN_TX)
	{
		pUSARTHandle->TxLen = Len;
		pUSARTHandle->pTxBuffer = pTxBuffer;
		pUSARTHandle->TxState = USART_BUSY_IN_TX;

		// Enable interrupt for TXEIE
		pUSARTHandle->pUSARTx->CR1 |= (1 << USART_CR1_TXEIE);

		// Enable interrupt for TCIE
		pUSARTHandle->pUSARTx->CR1 |= (1 << USART_CR1_TCIE);
	}

	return state;
}

/*********************************************************************
 * @fn      		  - USART_ReceiveDataIT
 *
 * @brief             - This function allows receive data due to interrupts
 *
 * @param[in]         - the USART handle
 * @param[in]         - Rx buffer
 * @param[in]         -	length
 *
 * @return            - state of interrupt
 *
 * @Note              - non blocking API
 */

uint8_t USART_ReceiveDataIT(USART_Handle_t *pUSARTHandle, uint8_t *pRxBuffer, uint32_t Len)
{
	uint8_t state = pUSARTHandle->RxState;

	if(state != USART_BUSY_IN_RX)
	{
		pUSARTHandle->RxLen = Len;
		pUSARTHandle->pRxBuffer = pRxBuffer;
		pUSARTHandle->RxState = USART_BUSY_IN_RX;

		(void)pUSARTHandle->pUSARTx->DR;

		// Enable interrupt for RXNEIE
		pUSARTHandle->pUSARTx->CR1 |= (1 << USART_CR1_RXNEIE);
	}

	return state;
}

/*********************************************************************
 * @fn      		  - USART_IRQInterruptConfig
 *
 * @brief             - This function IRQ is (interrupt request) configuration
 *
 * @param[in]         - the number of the USARTx
 * @param[in]         - Enable or Disable
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

void USART_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		if(IRQNumber <= 31)
		{
			// program for ISER0 register
			*NVIC_ISER0 |= (1 << IRQNumber);
		}
		else if(IRQNumber > 31 && IRQNumber <= 63)
		{
			// program for ISER1 register
			*NVIC_ISER1 |= (1 << (IRQNumber % 32));
		}
		else if(IRQNumber > 63 && IRQNumber <= 95)
		{
			// program for ISER2 register
			*NVIC_ISER2 |= (1 << (IRQNumber % 64));
		}
	}
	else
	{
		if(IRQNumber <= 31)
		{
			// program for ISER0 register
			*NVIC_ICER0 |= (1 << IRQNumber);
		}
		else if(IRQNumber > 31 && IRQNumber <= 63)
		{
			// program for ISER1 register
			*NVIC_ICER1 |= (1 << (IRQNumber % 32));
		}
		else if(IRQNumber > 63 && IRQNumber <= 95)
		{
			// program for ISER2 register
			*NVIC_ICER2 |= (1 << (IRQNumber % 64));
		}
	}
}

/* @fn      		 - USART_PriorityConfig
*
* @brief             - This function USART_PriorityConfig is (priority) configuration
*
* @param[in]         - the number of the USARTx
* @param[in]         - priority of IRQ (interrupt request)
* @param[in]         -
*
* @return            - none
*
* @Note              - none

*/

void USART_PriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	// 1.First lets find out which IPR register use
	uint8_t iprx = IRQNumber / 4;
	uint8_t iprx_section = IRQNumber % 4;
	uint8_t shift_amount = ((8 * iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED));

	*(NVIC_PR_BASEADDR + iprx) |= (IRQPriority << shift_amount);
}

/* @fn      		 - USART_IRQHandling
*
* @brief             - This function handle the USART interrupts
*
* @param[in]         - the USART handle
* @param[in]         -
* @param[in]         -
*
* @return            - none
*
* @Note              - none

*/

void USART_IRQHandling(USART_Handle_t *pUSARTHandle)
{

	uint32_t temp1 , temp2, temp3;

	uint16_t *pdata;

/*************************Check for TC flag ********************************************/

    //Implement the code to check the state of TC bit in the SR
	temp1 = pUSARTHandle->pUSARTx->SR & ( 1 << USART_SR_TC);

	 //Implement the code to check the state of TCEIE bit
	temp2 = pUSARTHandle->pUSARTx->CR1 & ( 1 << USART_CR1_TCIE);

	if(temp1 && temp2 )
	{
		//this interrupt is because of TC

		//close transmission and call application callback if TxLen is zero
		if ( pUSARTHandle->TxState == USART_BUSY_IN_TX)
		{
			//Check the TxLen . If it is zero then close the data transmission
			if(! pUSARTHandle->TxLen )
			{
				//Implement the code to clear the TC flag
				pUSARTHandle->pUSARTx->SR &= ~( 1 << USART_SR_TC);

				//Implement the code to clear the TCIE control bit

				//Reset the application state
				pUSARTHandle->TxState = USART_READY;

				//Reset Buffer address to NULL
				pUSARTHandle->pTxBuffer = NULL;

				//Reset the length to zero
				pUSARTHandle->TxLen = 0;

				//Call the applicaton call back with event USART_EVENT_TX_CMPLT
				USART_ApplicationEventCallback(pUSARTHandle,USART_EVENT_TX_CMPLT);
			}
		}
	}

/*************************Check for TXE flag ********************************************/

	//Implement the code to check the state of TXE bit in the SR
	temp1 = pUSARTHandle->pUSARTx->SR & ( 1 << USART_SR_TXE);

	//Implement the code to check the state of TXEIE bit in CR1
	temp2 = pUSARTHandle->pUSARTx->CR1 & (1 << USART_CR1_TXEIE);


	if(temp1 && temp2 )
	{
		//this interrupt is because of TXE

		if(pUSARTHandle->TxState == USART_BUSY_IN_TX)
		{
			//Keep sending data until Txlen reaches to zero
			if(pUSARTHandle->TxLen > 0)
			{
				//Check the USART_WordLength item for 9BIT or 8BIT in a frame
				if(pUSARTHandle->USART_Config_t.USART_WordLength ==USART_WORD_LENGTH_9)
				{
					//if 9BIT , load the DR with 2bytes masking the bits other than first 9 bits
					pdata = (uint16_t*) pUSARTHandle->pTxBuffer;

					//loading only first 9 bits , so we have to mask with the value 0x01FF
					pUSARTHandle->pUSARTx->DR = (*pdata & (uint16_t)0x01FF);

					//check for USART_ParityControl
					if(pUSARTHandle->USART_Config_t.USART_ParityControl == USART_PARITY_CONTROL_DI)
					{
						//No parity is used in this transfer , so, 9bits of user data will be sent
						//Implement the code to increment pTxBuffer twice
						pUSARTHandle->pTxBuffer++;
						pUSARTHandle->pTxBuffer++;

						//Implement the code to decrement the length
						pUSARTHandle->TxLen-=2;
					}
					else
					{
						//Parity bit is used in this transfer . so , 8bits of user data will be sent
						//The 9th bit will be replaced by parity bit by the hardware
						pUSARTHandle->pTxBuffer++;

						//Implement the code to decrement the length
						pUSARTHandle->TxLen-=1;
					}
				}
				else
				{
					//This is 8bit data transfer
					pUSARTHandle->pUSARTx->DR = (*pUSARTHandle->pTxBuffer  & (uint8_t)0xFF);

					//Implement the code to increment the buffer address
					pUSARTHandle->pTxBuffer++;

					//Implement the code to decrement the length
					pUSARTHandle->TxLen-=1;
				}

			}
			if (pUSARTHandle->TxLen == 0 )
			{
				//TxLen is zero
				//Implement the code to clear the TXEIE bit (disable interrupt for TXE flag )
				pUSARTHandle->pUSARTx->CR1 &= ~(1 << USART_CR1_TXEIE);
			}
		}
	}

/*************************Check for RXNE flag ********************************************/

	temp1 = pUSARTHandle->pUSARTx->SR & ( 1 << USART_SR_RXNE);
	temp2 = pUSARTHandle->pUSARTx->CR1 & ( 1 << USART_CR1_RXNEIE);


	if(temp1 && temp2 )
	{
		//this interrupt is because of rxne
		if(pUSARTHandle->RxState == USART_BUSY_IN_RX)
		{
			//TXE is set so send data
			if(pUSARTHandle->RxLen > 0)
			{
				//Check the USART_WordLength to decide whether we are going to receive 9bit of data in a frame or 8 bit
				if(pUSARTHandle->USART_Config_t.USART_WordLength == USART_WORD_LENGTH_9)
				{
					//We are going to receive 9bit data in a frame

					//Now, check are we using USART_ParityControl control or not
					if(pUSARTHandle->USART_Config_t.USART_ParityControl == USART_PARITY_CONTROL_DI)
					{
						//No parity is used. so, all 9bits will be of user data

						//read only first 9 bits so mask the DR with 0x01FF
						*((uint16_t*) pUSARTHandle->pRxBuffer) = (pUSARTHandle->pUSARTx->DR  & (uint16_t)0x01FF);

						//Now increment the pRxBuffer two times
						pUSARTHandle->pRxBuffer++;
						pUSARTHandle->pRxBuffer++;

						//Implement the code to decrement the length
						pUSARTHandle->RxLen-=2;
					}
					else
					{
						//Parity is used. so, 8bits will be of user data and 1 bit is parity
						 *pUSARTHandle->pRxBuffer = (pUSARTHandle->pUSARTx->DR  & (uint8_t)0xFF);

						 //Now increment the pRxBuffer
						 pUSARTHandle->pRxBuffer++;

						 //Implement the code to decrement the length
						 pUSARTHandle->RxLen-=1;
					}
				}
				else
				{
					//We are going to receive 8bit data in a frame

					//Now, check are we using USART_ParityControl control or not
					if(pUSARTHandle->USART_Config_t.USART_ParityControl == USART_PARITY_CONTROL_DI)
					{
						//No parity is used , so all 8bits will be of user data

						//read 8 bits from DR
						 *pUSARTHandle->pRxBuffer = (uint8_t) (pUSARTHandle->pUSARTx->DR  & (uint8_t)0xFF);
					}

					else
					{
						//Parity is used, so , 7 bits will be of user data and 1 bit is parity

						//read only 7 bits , hence mask the DR with 0X7F
						 *pUSARTHandle->pRxBuffer = (uint8_t) (pUSARTHandle->pUSARTx->DR  & (uint8_t)0x7F);

					}

					//Now , increment the pRxBuffer
					pUSARTHandle->pRxBuffer++;

					//Implement the code to decrement the length
					pUSARTHandle->RxLen-=1;
				}


			}//if of >0

			if(! pUSARTHandle->RxLen)
			{
				//disable the rxne
				pUSARTHandle->pUSARTx->CR1 &= ~( 1 << USART_CR1_RXNEIE );
				pUSARTHandle->RxState = USART_READY;
				USART_ApplicationEventCallback(pUSARTHandle,USART_EVENT_RX_CMPLT);
			}
		}
	}


/*************************Check for CTS flag ********************************************/
//Note : CTS feature is not applicable for UART4 and UART5

	//Implement the code to check the status of CTS bit in the SR
	temp1 = pUSARTHandle->pUSARTx->SR & (1 << USART_SR_CTS);

	//Implement the code to check the state of CTSE bit in CR1
	temp2 = pUSARTHandle->pUSARTx->CR3 & ( 1 << USART_CR3_CTSE);

	//Implement the code to check the state of CTSIE bit in CR3 (This bit is not available for UART4 & UART5.)
	temp3 = pUSARTHandle->pUSARTx->CR3 & ( 1 << USART_CR3_CTSIE);


	if(temp1  && temp2 )
	{
		//Implement the code to clear the CTS flag in SR
		pUSARTHandle->pUSARTx->SR &= ~(1 << USART_SR_CTS);

		//this interrupt is because of cts
		USART_ApplicationEventCallback(pUSARTHandle,USART_EVENT_CTS);
	}

/*************************Check for IDLE detection flag ********************************************/

	//Implement the code to check the status of IDLE flag bit in the SR
	temp1 = pUSARTHandle->pUSARTx->SR & (1 << USART_SR_IDLE);

	//Implement the code to check the state of IDLEIE bit in CR1
	temp2 = pUSARTHandle->pUSARTx->CR3 & ( 1 << USART_CR3_CTSE);


	if(temp1 && temp2)
	{
		//Implement the code to clear the IDLE flag. Refer to the RM to understand the clear sequence
		/*__vo uint32_t dummy;
		dummy = pUSARTHandle->pUSARTx->SR;
		dummy = pUSARTHandle->pUSARTx->DR;
		(void)dummy;*/
		temp1 = pUSARTHandle->pUSARTx->SR &= ~( 1 << USART_SR_IDLE);

		//this interrupt is because of idle
		USART_ApplicationEventCallback(pUSARTHandle,USART_EVENT_IDLE);
	}

/*************************Check for Overrun detection flag ********************************************/

	//Implement the code to check the status of ORE flag  in the SR
	temp1 = pUSARTHandle->pUSARTx->SR & USART_SR_ORE;

	//Implement the code to check the status of RXNEIE  bit in the CR1
	temp2 = pUSARTHandle->pUSARTx->CR1 & USART_CR1_RXNEIE;


	if(temp1  && temp2 )
	{
		//Need not to clear the ORE flag here, instead give an api for the application to clear the ORE flag .

		//this interrupt is because of Overrun error
		USART_ApplicationEventCallback(pUSARTHandle,USART_EVENT_ORE);
	}



/*************************Check for Error Flag ********************************************/

//Noise Flag, Overrun error and Framing Error in multibuffer communication
//We dont discuss multibuffer communication in this course. please refer to the RM
//The blow code will get executed in only if multibuffer mode is used.

	temp2 =  pUSARTHandle->pUSARTx->CR3 & ( 1 << USART_CR3_EIE) ;

	if(temp2 )
	{
		temp1 = pUSARTHandle->pUSARTx->SR;
		if(temp1 & ( 1 << USART_SR_FE))
		{
			/*
				This bit is set by hardware when a de-synchronization, excessive noise or a break character
				is detected. It is cleared by a software sequence (an read to the USART_SR register
				followed by a read to the USART_DR register).
			*/
			USART_ApplicationEventCallback(pUSARTHandle,USART_ERREVENT_FE);
		}

		if(temp1 & ( 1 << USART_SR_FE))
		{
			/*
				This bit is set by hardware when noise is detected on a received frame. It is cleared by a
				software sequence (an read to the USART_SR register followed by a read to the
				USART_DR register).
			*/
			USART_ApplicationEventCallback(pUSARTHandle,USART_ERREVENT_NE);
		}

		if(temp1 & ( 1 << USART_SR_ORE) )
		{
			USART_ApplicationEventCallback(pUSARTHandle,USART_ERREVENT_ORE);
		}
	}
}


/*********************************************************************
 * @fn      		  - USART_ApplicationEventCallback
 *
 * @brief             - This function is Application callback for TX / RX / OVR
 *
 * @param[in]         - USARTx handle
 * @param[in]         - Event type
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

__weak void USART_ApplicationEventCallback(USART_Handle_t *pUSARTHandle, uint8_t EVENT)
{
	// This is the weak implementation, so the application can override this function
}
