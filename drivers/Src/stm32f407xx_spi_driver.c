#include "stm32f407xx_spi_driver.h"

static void spi_txe_interrupt_handle(SPI_Handle_t *pSPI_Handle);
static void spi_rxne_interrupt_handle(SPI_Handle_t *pSPI_Handle);
static void spi_err_interrupt_handle(SPI_Handle_t *pSPI_Handles);

// some helper function implementations

/*********************************************************************
 * @fn      		  - spi_txe_interrupt_handle
 *
 * @brief             - This function help to configure interrupt of TxBuffer
 *
 * @param[in]         - the base address of the SPIx and SPIx
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

static void spi_txe_interrupt_handle(SPI_Handle_t *pSPI_Handle)
{
	// check the DFF
	if((pSPI_Handle->pSPIx->CR1) & (1 << SPI_CR1_DFF))
	{
		// 16 bit DFF

		// load the data into the DR
		pSPI_Handle->pSPIx->DR = *((uint16_t*)pSPI_Handle->pTxBuffer);
		pSPI_Handle->TxLen--;
		pSPI_Handle->TxLen--;
		(uint16_t*)pSPI_Handle->pTxBuffer++;
	}
	else
	{
		// 8 bit DFF

		// load the data into the DR
		pSPI_Handle->pSPIx->DR = *pSPI_Handle->pTxBuffer;
		pSPI_Handle->TxLen--;
		pSPI_Handle->pTxBuffer++;
	}

	if(pSPI_Handle->TxLen == 0)
	{
		// TX buffer is zero, so close the SPI transmission and inform app. TX is over
		// This prevent interrupts from setting TXE flag
		SPI_CloseTransmission(pSPI_Handle);
		SPI_ApplicationEventCallback(pSPI_Handle, SPI_EVENT_TX_CMPLT);
	}
}

/*********************************************************************
 * @fn      		  - spi_rxne_interrupt_handle
 *
 * @brief             - This function help to configure interrupt of RxBuffer
 *
 * @param[in]         - the base address of the SPIx and SPIx
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

static void spi_rxne_interrupt_handle(SPI_Handle_t *pSPI_Handle)
{
	// check the DFF
	if((pSPI_Handle->pSPIx->CR1) & (1 << SPI_CR1_DFF))
	{
		// 16 bit DFF

		// load the data from the DR to Rxbuffer address
		*((uint16_t*)pSPI_Handle->pRxBuffer) = pSPI_Handle->pSPIx->DR;
		pSPI_Handle->RxLen--;
		pSPI_Handle->RxLen--;
		(uint16_t*)pSPI_Handle->pRxBuffer++;
	}
	else
	{
		// 8 bit DFF

		// load the data from the DR
		*pSPI_Handle->pRxBuffer = pSPI_Handle->pSPIx->DR;
		pSPI_Handle->RxLen--;
		pSPI_Handle->pRxBuffer++;
	}

	if(pSPI_Handle->RxLen == 0)
	{
		// RX buffer is zero, so close the SPI receiving and inform app. TX is over
		// This prevent interrupts from setting RXNE flag
		SPI_CloseReception(pSPI_Handle);
		SPI_ApplicationEventCallback(pSPI_Handle, SPI_EVENT_RX_CMPLT);
	}
}

/*********************************************************************
 * @fn      		  - spi_err_interrupt_handle
 *
 * @brief             - This function help to configure interrupt of errors
 *
 * @param[in]         - the base address of the SPIx and SPIx
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

static void spi_err_interrupt_handle(SPI_Handle_t *pSPI_Handle)
{
	uint8_t temp;
	// 1.Clear the OVR flag
	if(pSPI_Handle->TxState != SPI_BUSY_IN_TX)
	{
		temp = pSPI_Handle->pSPIx->DR;
		temp = pSPI_Handle->pSPIx->SR;
	}
	(void)temp;

	// 2.Inform the application
	SPI_ApplicationEventCallback(pSPI_Handle, SPI_EVENT_OVR_ERR);
}

/*********************************************************************
 * @fn      		  - SPI_Init
 *
 * @brief             - This function configure all pin's settings in start value
 *
 * @param[in]         - the base address of the SPIx and SPIx configuration settings
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

void SPI_Init(SPI_Handle_t *pSPIHandle)
{
	// 1.Configure mode of SPIx

	// enable the peripheral clock
	SPI_PeriClockControl(pSPIHandle->pSPIx, ENABLE);

	uint32_t temp = 0;

	// 2.Configure the bus
	temp |= (pSPIHandle->SPI_PinConfig.SPI_DeviseMode << SPI_CR1_MSTR);

	if(pSPIHandle->SPI_PinConfig.SPI_BusConfig == SPI_BUS_CONFIG_FD)
	{
		// BIDIMODE clear
		temp &= ~(1 << SPI_CR1_BIDIMODE);
	}
	else if(pSPIHandle->SPI_PinConfig.SPI_BusConfig == SPI_BUS_CONFIG_HD)
	{
		// BIDIMODE set
		temp |= (1 << SPI_CR1_BIDIMODE);
	}
	else if(pSPIHandle->SPI_PinConfig.SPI_BusConfig == SPI_BUS_CONFIG_SIMPLEX_RXONLY)
	{
		// BIDIMODE clear
		temp &= ~(1 << SPI_CR1_BIDIMODE);
		// RXONLY set
		temp |= (1 << SPI_CR1_RXONLY);
	}

	// 3.configure SPI serial clock speed (baud rate)
	temp |= (pSPIHandle->SPI_PinConfig.SPI_SClkSpeed << SPI_CR1_BR);

	// 4.configure the DFF
	temp |= (pSPIHandle->SPI_PinConfig.SPI_DFF << SPI_CR1_DFF);

	// 5. configure the CPOL
	temp |= (pSPIHandle->SPI_PinConfig.SPI_CPOL << SPI_CR1_CPOL);

	// 6. configure the CPHA
	temp |= (pSPIHandle->SPI_PinConfig.SPI_CPHA << SPI_CR1_CPHA);

	// 7. configure the SSM
	temp |= (pSPIHandle->SPI_PinConfig.SPI_SSM << SPI_CR1_SSM);

	pSPIHandle->pSPIx->CR1 = temp;
}

/*********************************************************************
 * @fn      		  - SPI_DeInit
 *
 * @brief             - This function reset all SPIx pin
 *
 * @param[in]         - the base address of the SPIx
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

void SPI_DeInit(SPI_RegDef_t *pSPIx)
{
	if(pSPIx == SPI1)
	{
		SPI1_REG_RESET();
	}
	else if(pSPIx == SPI2)
	{
		SPI2_REG_RESET();
	}
	else if(pSPIx == SPI3)
	{
		SPI3_REG_RESET();
	}
	else if(pSPIx == SPI4)
	{
		SPI4_REG_RESET();
	}
}

/*********************************************************************
 * @fn      		  - SPI_PeripheralControl
 *
 * @brief             - This function enables or disables peripheral for the given SPI
 *
 * @param[in]         - base address of the SPI peripheral
 * @param[in]         - ENABLE or DISABLE macros
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

void SPI_PeripheralControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		pSPIx->CR1 |= (1 << SPI_CR1_SPE);
	}
	else
	{
		pSPIx->CR1 &= ~(1 << SPI_CR1_SPE);
	}
}

/*********************************************************************
 * @fn      		  - SPI_SSIConfig
 *
 * @brief             - This function enables or disables SSI for the given SPI
 *
 * @param[in]         - base address of the SPI peripheral
 * @param[in]         - ENABLE or DISABLE macros
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

void SPI_SSIConfig(SPI_RegDef_t *pSPIx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		pSPIx->CR1 |= (1 << SPI_CR1_SSI);
	}
	else
	{
		pSPIx->CR1 &= ~(1 << SPI_CR1_SSI);
	}
}

/*********************************************************************
 * @fn      		  - SPI_SSOEConfig
 *
 * @brief             - This function enables or disables SSOE for the given SPI
 *
 * @param[in]         - base address of the SPI peripheral
 * @param[in]         - ENABLE or DISABLE macros
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

void SPI_SSOEConfig(SPI_RegDef_t *pSPIx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		pSPIx->CR2 |= (1 << SPI_CR2_SSOE);
	}
	else
	{
		pSPIx->CR2 &= ~(1 << SPI_CR2_SSOE);
	}
}

/*********************************************************************
 * @fn      		  - SPI_PeriClockControl
 *
 * @brief             - This function enables or disables peripheral clock for the given SPI
 *
 * @param[in]         - base address of the SPI peripheral
 * @param[in]         - ENABLE or DISABLE macros
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

void SPI_PeriClockControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		if(pSPIx == SPI1)
		{
			SPI1_PCLK_EN();
		}
		else if(pSPIx == SPI2)
		{
			SPI2_PCLK_EN();
		}
		else if(pSPIx == SPI3)
		{
			SPI3_PCLK_EN();
		}
		else if(pSPIx == SPI4)
		{
			SPI4_PCLK_EN();
		}
	}
	else
	{
		if(pSPIx == SPI1)
		{
			SPI1_PCLK_DI();
		}
		else if(pSPIx == SPI2)
		{
			SPI2_PCLK_DI();
		}
		else if(pSPIx == SPI3)
		{
			SPI3_PCLK_DI();
		}
		else if(pSPIx == SPI4)
		{
			SPI4_PCLK_DI();
		}
	}
}

/*********************************************************************
 * @fn      		  - SPI_SendData
 *
 * @brief             - This function allows send data with SPI
 *
 * @param[in]         - the base address of the SPIx
 * @param[in]         - tx buffer
 * @param[in]         - length of message
 *
 * @return            -
 *
 * @Note              - This is a blocking call

 */

void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t Len)
{
	while(Len > 0)
	{
		// 1. Wait until TXE is set
 		while(SPI_GetFlagStatus(pSPIx, SPI_TXE_FLAG) == FLAG_RESET);


		// 2. Check the DFF
		if((pSPIx->CR1) & (1 << SPI_CR1_DFF))
		{
			// 16 bit DFF

			// 1) load the data into the DR
			pSPIx->DR = *((uint16_t*)pTxBuffer);
			Len--;
			Len--;
			(uint16_t*)pTxBuffer++;
		}
		else
		{
			// 8 bit DFF

			// 1) load the data into the DR
			pSPIx->DR = *pTxBuffer;
			Len--;
			pTxBuffer++;
		}
	}
}


/*********************************************************************
 * @fn      		  - SPI_ReceiveData
 *
 * @brief             - This function write data to output SPIx
 *
 * @param[in]         - the base address of the SPIx
 * @param[in]         - Rx buffer
 * @param[in]         - length of message
 *
 * @return            - none
 *
 * @Note              - none

 */

void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t Len)
{
	while(Len > 0)
	{
		// 1. Wait until RXNE is set
		while(SPI_GetFlagStatus(pSPIx, SPI_RXNE_FLAG) == FLAG_RESET);


		// 2. Check the DFF
		if((pSPIx->CR1) & (1 << SPI_CR1_DFF))
		{
			// 16 bit DFF

			// 1) load the data from the DR to Rxbuffer address
			*((uint16_t*)pRxBuffer) = pSPIx->DR;
			Len--;
			Len--;
			(uint16_t*)pRxBuffer++;
		}
		else
		{
			// 8 bit DFF

			// 1) load the data from the DR
			*pRxBuffer = pSPIx->DR;
			Len--;
			pRxBuffer++;
		}
	}
}

/*********************************************************************
 * @fn      		  - SPI_SendDataIT
 *
 * @brief             - This function allows send data with SPI in interrupts
 *
 * @param[in]         - the handle of the pSPIHandle
 * @param[in]         - tx buffer
 * @param[in]         - length of message
 *
 * @return            -
 *
 * @Note              -

 */

uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pTxBuffer, uint32_t Len)
{
	uint8_t state = pSPIHandle->TxState;

	if(state != SPI_BUSY_IN_TX)
	{
		// 1. Save TxBuffer Len in global variable in SPIHandles

		pSPIHandle->pTxBuffer = pTxBuffer;
		pSPIHandle->TxLen = Len;

		// 2. Mark the SPI state as busy in transmission so no one program can
		// not use the same SPI until transmission is over

		pSPIHandle->TxState = SPI_BUSY_IN_TX;

		// 3. Enable the TXEIE bit to get interrupt whatever TXE flag is set in SR

		pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_TXEIE);

		// 4. Data transmission will be handled by the ISR code

	}

	return state;
}


/*********************************************************************
 * @fn      		  - SPI_ReceiveDataIT
 *
 * @brief             - This function write data to output SPIx in interrupts
 *
 * @param[in]         - the handle of the pSPIHandle
 * @param[in]         - Rx buffer
 * @param[in]         - length of message
 *
 * @return            - none
 *
 * @Note              - none

 */

uint8_t SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pRxBuffer, uint32_t Len)
{
	uint8_t state = pSPIHandle->RxState;

	if(state != SPI_BUSY_IN_RX)
	{
		// 1. Save RxBuffer Len in global variable in SPIHandles

		pSPIHandle->pRxBuffer = pRxBuffer;
		pSPIHandle->RxLen = Len;

		// 2. Mark the SPI state as busy in receiving so no one program can
		// not use the same SPI until receiving is over

		pSPIHandle->RxState = SPI_BUSY_IN_RX;

		// 3. Enable the RXNEIE bit to get interrupt whatever TXE flag is set in SR

		pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_RXNEIE);

		// 4. Data receive will be handled by the ISR code

	}

	return state;
}

/*********************************************************************
 * @fn      		  - SPI_IRQInterruptConfig
 *
 * @brief             - This function IRQ is (interrupt request) configuration
 *
 * @param[in]         - the number of the SPIx
 * @param[in]         - Enable or Disable
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi)
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

/* @fn      		 - SPI_PriorityConfig
*
* @brief             - This function SPI_PriorityConfig is (priority) configuration
*
* @param[in]         - the number of the SPIx
* @param[in]         - priority of IRQ (interrupt request)
* @param[in]         -
*
* @return            - none
*
* @Note              - none

*/

void SPI_PriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	// 1.First lets find out which IPR register use
	uint8_t iprx = IRQNumber / 4;
	uint8_t iprx_section = IRQNumber % 4;
	uint8_t shift_amount = ((8 * iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED));

	*(NVIC_PR_BASEADDR + iprx) |= (IRQPriority << shift_amount);
}

/*********************************************************************
 * @fn      		  - SPI_IRQConfig
 *
 * @brief             - This function is ISR (interrupt service routine) handling
 *
 * @param[in]         - SPIx handle
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

void SPI_IRQHandling(SPI_Handle_t *pSPI_Handle)
{
	uint8_t temp1, temp2;

	// first check TXE
	temp1 = pSPI_Handle->pSPIx->SR & (1 << SPI_SR_TXE);
	temp2 = pSPI_Handle->pSPIx->CR2 & (1 << SPI_CR2_TXEIE);

	if(temp1 && temp2)
	{
		// handle TXE
		spi_txe_interrupt_handle(pSPI_Handle);
	}

	// second check RXNE
	temp1 = pSPI_Handle->pSPIx->SR & (1 << SPI_SR_RXNE);
	temp2 = pSPI_Handle->pSPIx->CR2 & (1 << SPI_CR2_RXNEIE);

	if(temp1 && temp2)
	{
		// handle RXE
		spi_rxne_interrupt_handle(pSPI_Handle);
	}

	// third check OVR flag
	temp1 = pSPI_Handle->pSPIx->SR & (1 << SPI_SR_OVR);
	temp2 = pSPI_Handle->pSPIx->CR2 & (1 << SPI_CR2_ERRIE);

	if(temp1 && temp2)
	{
		// handle OVR error
		spi_err_interrupt_handle(pSPI_Handle);
	}
}

/*********************************************************************
 * @fn      		  - SPI_ClearOVRFlag
 *
 * @brief             - This function is ISR (interrupt service routine) handling for close receiving
 *						transmission
 *
 * @param[in]         - SPIx handle
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

void SPI_ClearOVRFlag(SPI_RegDef_t *pSPIx)
{
	uint8_t temp;
	temp = pSPIx->DR;
	temp = pSPIx->SR;
	(void)temp;
}

/*********************************************************************
 * @fn      		  - SPI_CloseTransmission
 *
 * @brief             - This function is ISR (interrupt service routine) handling for close transmission
 *
 * @param[in]         - SPIx handle
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

void SPI_CloseTransmission(SPI_Handle_t *pSPI_Handle)
{
	pSPI_Handle->pSPIx->CR2 &= ~(1 << SPI_CR2_TXEIE);
	pSPI_Handle->pTxBuffer = NULL;
	pSPI_Handle->TxLen = 0;
	pSPI_Handle->TxState = SPI_READY;
}

/*********************************************************************
 * @fn      		  - SPI_CloseReception
 *
 * @brief             - This function is ISR (interrupt service routine) handling for close receiving
 *
 * @param[in]         - SPIx handle
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

void SPI_CloseReception(SPI_Handle_t *pSPI_Handle)
{
	pSPI_Handle->pSPIx->CR2 &= ~(1 << SPI_CR2_RXNEIE);
	pSPI_Handle->pRxBuffer = NULL;
	pSPI_Handle->RxLen = 0;
	pSPI_Handle->RxState = SPI_READY;
}

/*********************************************************************
 * @fn      		  - SPI_GetFlagStatus
 *
 * @brief             - This function allows get status flag for some SPI flag
 *
 * @param[in]         - the base address of the SPIx
 * @param[in]         - flag name
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -

 */

uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx, uint32_t FlagName)
{
	if(pSPIx->SR & FlagName)
	{
		return FLAG_SET;
	}
	return FLAG_RESET;
}

/*********************************************************************
 * @fn      		  - SPI_ApplicationEventCallback
 *
 * @brief             - This function is Application callback for TX / RX / OVR
 *
 * @param[in]         - SPIx handle
 * @param[in]         - Event type
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

__weak void SPI_ApplicationEventCallback(SPI_Handle_t *pSPI_Handle, uint8_t EVENT)
{
	// This is the weak implementation, so the application can override this function

}
