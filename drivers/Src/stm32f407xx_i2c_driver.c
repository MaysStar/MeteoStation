#include "stm32f407xx_i2c_driver.h"

static void I2C_GenerateStartCondition(I2C_RegDef_t *pI2Cx);
static void I2C_ExecuteAddressPhaseWrite(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddress);
static void I2C_ExecuteAddressPhaseRead(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddress);
static void I2C_ClearADDRFlag(I2C_Handle_t *pI2CHandle);
static void I2C_MasterHandleTXEInterrupt(I2C_Handle_t *pI2CHandle);
static void I2C_MasterHandleRXNEInterrupt(I2C_Handle_t *pI2CHandle);

// some helper function implementations

/*********************************************************************
 * @fn      		  - I2C_GenerateStartCondition
 *
 * @brief             - This function generate start condition for slave
 *
 * @param[in]         - the base address of the I2Cx
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

static void I2C_GenerateStartCondition(I2C_RegDef_t *pI2Cx)
{
	pI2Cx->CR1 |= (1 << I2C_CR1_START);
}

/*********************************************************************
 * @fn      		  - I2C_ExecuteAddressPhaseRead
 *
 * @brief             - This function execute right address 7 bit and with 1 write bit = 0
 *
 * @param[in]         - the base address of the I2Cx
 * @param[in]         - slave address
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

static void I2C_ExecuteAddressPhaseWrite(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddress)
{
	SlaveAddress = SlaveAddress << 1;
	SlaveAddress &= ~(1);
	pI2Cx->DR = SlaveAddress; // SlaveAddress + read / write bit = 0
}

/*********************************************************************
 * @fn      		  - I2C_ExecuteAddressPhaseRead
 *
 * @brief             - This function execute right address 7 bit and with 1 read bit = 1
 *
 * @param[in]         - the base address of the I2Cx
 * @param[in]         - slave address
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

static void I2C_ExecuteAddressPhaseRead(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddress)
{
	SlaveAddress = SlaveAddress << 1;
	SlaveAddress |= (1);
	pI2Cx->DR = SlaveAddress; // SlaveAddress + read / write bit = 1
}

/*********************************************************************
 * @fn      		  - I2C_ClearADDRFlag
 *
 * @brief             - This function clear ADDR flag by reading ! first SR1 and next SR2
 *
 * @param[in]         - the Handle
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

static void I2C_ClearADDRFlag(I2C_Handle_t *pI2CHandle)
{
	__vo uint32_t dummyRead;
	// check device mode
	if(pI2CHandle->pI2Cx->SR2 & I2C_MSL_FLAG)
	{
		// device in master mode
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
		{
			if(pI2CHandle->RxSize == 1)
			{
				// first disable ACKing
				I2C_ManageAcking(pI2CHandle->pI2Cx, I2C_ACK_DISABLE);

				// clear ADDR flag ( read SR1 and next SR2 )
				dummyRead = pI2CHandle->pI2Cx->SR1;
				dummyRead = pI2CHandle->pI2Cx->SR2;
				(void)dummyRead;
			}
		}
		else
		{
			// clear ADDR flag ( read SR1 and next SR2 )
			dummyRead = pI2CHandle->pI2Cx->SR1;
			dummyRead = pI2CHandle->pI2Cx->SR2;
			(void)dummyRead;
		}
	}
	else
	{
		// device in slave mode
		// clear ADDR flag ( read SR1 and next SR2 )
		dummyRead = pI2CHandle->pI2Cx->SR1;
		dummyRead = pI2CHandle->pI2Cx->SR2;
		(void)dummyRead;
	}
}

/*********************************************************************
 * @fn      		  - I2C_GenerateStopCondition
 *
 * @brief             - This function generate stop condition for slave
 *
 * @param[in]         - the base address of the I2Cx
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

void I2C_GenerateStopCondition(I2C_RegDef_t *pI2Cx)
{
	pI2Cx->CR1 |= (1 << I2C_CR1_STOP);
}

/*********************************************************************
 * @fn      		  - I2C_MasterHandleTXEInterrupt
 *
 * @brief             - This function perform I2C data transmit interrupt
 *
 * @param[in]         - the I2CHandle
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

static void I2C_MasterHandleTXEInterrupt(I2C_Handle_t *pI2CHandle)
{
	// Device is master
	if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX)
	{
		 if(pI2CHandle->TxLen > 0)
		 {
			 // 1. Load data in DR
			 pI2CHandle->pI2Cx->DR = *(pI2CHandle->pTxbuffer);

			 // 2. Decrement length
			 pI2CHandle->TxLen--;

			 // 3. Increment buffer
			 pI2CHandle->pTxbuffer++;
		 }
	}
}

/*********************************************************************
 * @fn      		  - I2C_MasterHandleRXNEInterrupt
 *
 * @brief             - This function perform I2C data receive interrupt
 *
 * @param[in]         - the I2CHandle
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

static void I2C_MasterHandleRXNEInterrupt(I2C_Handle_t *pI2CHandle)
{
	// Device is master
	if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
	{
		if(pI2CHandle->RxSize == 1)
		{
			//clear the ack bit
			I2C_ManageAcking(pI2CHandle->pI2Cx,DISABLE);

			*pI2CHandle->pRxbuffer = pI2CHandle->pI2Cx->DR;

			pI2CHandle->RxLen--;
		}

		if (pI2CHandle->RxSize > 1)
		{
			if(pI2CHandle->RxLen == 2)
			{
				// clear the ACK bit
				I2C_ManageAcking(pI2CHandle->pI2Cx, I2C_ACK_DISABLE);
			}

			// 1. Load data from  DR
			*pI2CHandle->pRxbuffer = pI2CHandle->pI2Cx->DR;

			// 2. Decrement length
			pI2CHandle->RxLen--;

			// 3. Increment buffer
			pI2CHandle->pRxbuffer++;
		}

		if(pI2CHandle->RxLen == 0)
		{
			// close the I2C data reception and notify application

			// 1. Generate stop condition
			if(pI2CHandle->STOP == I2C_STOP)
			{
				//while (I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_BUSY_FLAG));
				I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
			}

			// 2. Reset all member elements of the handle structure
			I2C_CloseReceiveData(pI2CHandle);

			// 3. Notify application about completing transmission
			I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_RX_CMPLT);
		}
	}
}

/*********************************************************************
 * @fn      		  - I2C_CloseSendData
 *
 * @brief             - This function close transmission of data
 *
 * @param[in]         - the I2CHandle
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

void I2C_CloseSendData(I2C_Handle_t *pI2CHandle)
{
	// Disable ITBUFEN control bit
	pI2CHandle->pI2Cx->CR2 &= ~(1 << I2C_CR2_ITBUFEN);

	// Disable ITEVTEN control bit
	pI2CHandle->pI2Cx->CR2 &= ~(1 << I2C_CR2_ITEVTEN);

	pI2CHandle->TxRxState = I2C_READY;
	pI2CHandle->pTxbuffer = NULL;
	pI2CHandle->TxLen = 0;

}

/*********************************************************************
 * @fn      		  - I2C_CloseReceiveData
 *
 * @brief             - This function close reception of data
 *
 * @param[in]         - the I2CHandle
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

void I2C_CloseReceiveData(I2C_Handle_t *pI2CHandle)
{
	// Disable ITBUFEN control bit
	pI2CHandle->pI2Cx->CR2 &= ~(1 << I2C_CR2_ITBUFEN);

	// Disable ITEVTEN control bit
	pI2CHandle->pI2Cx->CR2 &= ~(1 << I2C_CR2_ITEVTEN);

	pI2CHandle->TxRxState = I2C_READY;
	pI2CHandle->pRxbuffer = NULL;
	pI2CHandle->RxLen = 0;
	pI2CHandle->RxSize = 0;

	if(pI2CHandle->I2C_Config.I2C_ACKControl == I2C_ACK_ENABLE)
	{
		I2C_ManageAcking(pI2CHandle->pI2Cx, I2C_ACK_ENABLE);
	}
}

/*********************************************************************
 * @fn      		  - I2C_Init
 *
 * @brief             - This function configure all pin's settings in start value
 *
 * @param[in]         - the base address of the I2Cx and I2Cx configuration settings
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

void I2C_Init(I2C_Handle_t *pI2CHandle)
{
	uint32_t tempreg = 0;

	// 1. Enable the peripheral clock
	I2C_PeriClockControl(pI2CHandle->pI2Cx, ENABLE);

	// 2. Configure the ACK bit control of CR1
	tempreg |= (pI2CHandle->I2C_Config.I2C_ACKControl << I2C_CR1_ACK);

	pI2CHandle->pI2Cx->CR1 = tempreg;

	// 3. Configure the FREQ field of CR2
	tempreg = 0;
	tempreg |= RCC_GetPCLK1Value() / 1000000U;
	pI2CHandle->pI2Cx->CR2 = (tempreg & 0x3F);

	// 4. Configure the Device address of OAR1
	tempreg = 0;
	tempreg |= (pI2CHandle->I2C_Config.I2C_Device_Address << I2C_OAR1_ADD71);
	tempreg |= (1 << 14);
	pI2CHandle->pI2Cx->OAR1 = tempreg;

	// 5. CCR calculations
	uint16_t ccr_val = 0;
	tempreg = 0;

	if(pI2CHandle->I2C_Config.I2C_SCLSpeed <= I2C_SCL_SPEED_SM)
	{
		// standard mode
		ccr_val = (RCC_GetPCLK1Value() / (2 * pI2CHandle->I2C_Config.I2C_SCLSpeed));
		tempreg |= (ccr_val & 0xFFF);
	}
	else
	{
		// fast mode
		tempreg |= (1 << I2C_CCR_FS);
		tempreg |= (pI2CHandle->I2C_Config.I2C_FMDutyCycle << I2C_CCR_DUTY);

		if(pI2CHandle->I2C_Config.I2C_FMDutyCycle == I2C_FM_DUTY_CYCLE2)
		{
			ccr_val = (RCC_GetPCLK1Value() / (3 * pI2CHandle->I2C_Config.I2C_SCLSpeed));
		}
		else
		{
			ccr_val = (RCC_GetPCLK1Value() / (25 * pI2CHandle->I2C_Config.I2C_SCLSpeed));
		}
		tempreg |= (ccr_val & 0xFFF);
	}

	pI2CHandle->pI2Cx->CCR = tempreg;


	// 6. Configure TRISE

	if(pI2CHandle->I2C_Config.I2C_SCLSpeed <= I2C_SCL_SPEED_SM)
	{
		// standard mode
		tempreg = ((RCC_GetPCLK1Value() / 1000000U) + 1);
	}
	else
	{
		// fast mode
		tempreg = ((RCC_GetPCLK1Value() * 300) / 1000000000U + 1);
	}

	pI2CHandle->pI2Cx->TRISE = tempreg & (0x3F);
}

/*********************************************************************
 * @fn      		  - I2C_DeInit
 *
 * @brief             - This function reset all I2Cx pin
 *
 * @param[in]         - the base address of the I2Cx
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

void I2C_DeInit(I2C_RegDef_t *pI2Cx)
{
	if(pI2Cx == I2C1)
	{
		I2C1_REG_RESET();
	}
	else if(pI2Cx == I2C2)
	{
		I2C2_REG_RESET();
	}
	else if(pI2Cx == I2C3)
	{
		I2C3_REG_RESET();
	}
}

/*********************************************************************
 * @fn      		  - I2C_PeripheralControl
 *
 * @brief             - This function enables or disables peripheral for the given I2C
 *
 * @param[in]         - base address of the I2C peripheral
 * @param[in]         - ENABLE or DISABLE macros
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		pI2Cx->CR1 |= (1 << I2C_CR1_PE);
	}
	else
	{
		pI2Cx->CR1 &= ~(1 << I2C_CR1_PE);
	}
}

/*********************************************************************
 * @fn      		  - I2C_PeriClockControl
 *
 * @brief             - This function enables or disables peripheral clock for the given I2C
 *
 * @param[in]         - base address of the I2C peripheral
 * @param[in]         - ENABLE or DISABLE macros
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

void I2C_PeriClockControl(I2C_RegDef_t *pI2Cx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		if(pI2Cx == I2C1)
		{
			I2C1_PCLK_EN();
		}
		else if(pI2Cx == I2C2)
		{
			I2C2_PCLK_EN();
		}
		else if(pI2Cx == I2C3)
		{
			I2C3_PCLK_EN();
		}
	}
	else
	{
		if(pI2Cx == I2C1)
		{
			I2C1_PCLK_DI();
		}
		else if(pI2Cx == I2C2)
		{
			I2C2_PCLK_DI();
		}
		else if(pI2Cx == I2C3)
		{
			I2C3_PCLK_DI();
		}
	}
}

/*********************************************************************
 * @fn      		  - I2C_MasterSendData
 *
 * @brief             - This function allows send data with I2C
 *
 * @param[in]         - the pI2CHandle
 * @param[in]         - tx buffer
 * @param[in]         - length of message
 * @param[in]         - slave address
 * @param[in]         - repeated start or stop
 *
 * @return            -
 *
 * @Note              - This is a blocking call

 */

void I2C_MasterSendData(I2C_Handle_t *pI2CHandle, uint8_t* pTxBuffer, uint32_t Len, uint8_t SlaveAddress, uint8_t STOP)
{
	// 1. Generate the Start condition
	I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

	// 2. Confirm the Start generation is completed by checking the SB flag in SR1
	while(!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_SB_FLAG));

	// 3. Send the address of the slave with r/w bit to slave
	I2C_ExecuteAddressPhaseWrite(pI2CHandle->pI2Cx, SlaveAddress);

	// 4. Confirm the Address is completed by checking the ADDR flag in SR1
	while(!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_ADDR_FLAG));

	// 5. Clear ADDR flag
	I2C_ClearADDRFlag(pI2CHandle);

	// 6. Send the data until Len become 0
	while(Len > 0)
	{
		while(!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_TXE_FLAG)); // wait until TXE is set
		pI2CHandle->pI2Cx->DR = *pTxBuffer;
		pTxBuffer++;
		Len--;
	}

	// 7. When Len become zero wait for TXE = 1 BTF = 1 before generate stop condition
	while(!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_TXE_FLAG));
	while(!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_BTF_FLAG));

	// 8. Generate the stop condition and wait for the completing of stop condition
	if(STOP == I2C_STOP) I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
}

/*********************************************************************
 * @fn      		  - I2C_MasterReceiveData
 *
 * @brief             - This function allows receive data from slave to master with I2C
 *
 * @param[in]         - the pI2CHandle
 * @param[in]         - rx buffer
 * @param[in]         - length of message
 * @param[in]         - slave address
 * @param[in]		  - repeated start or stop
 *
 * @return            -
 *
 * @Note              - This is a blocking call

 */

void I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle, uint8_t* pRxBuffer, uint32_t Len, uint8_t SlaveAddress, uint8_t STOP)
{
	// 1. Generate the Start condition
	I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

	// 2. Confirm the Start generation is completed by checking the SB flag in SR1
	while(!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_SB_FLAG));

	// 3. Send the address of the slave with r/w bit to slave
	I2C_ExecuteAddressPhaseRead(pI2CHandle->pI2Cx, SlaveAddress);

	// 4. Confirm the Address is completed by checking the ADDR flag in SR1
	while(!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_ADDR_FLAG));

	// 5. Receive 1 byte of Len
	if(Len == 1)
	{
		// Disable Acking
		I2C_ManageAcking(pI2CHandle->pI2Cx, I2C_ACK_DISABLE);

		// Clear ADDR flag
		I2C_ClearADDRFlag(pI2CHandle);

		// Wait until RXEN become 1
		while(!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_RXNE_FLAG));

		if(STOP == I2C_STOP)
		{
			// Generate stop condition
			I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
		}

		// Read data in to buffer
		*pRxBuffer = pI2CHandle->pI2Cx->DR;
	}

	if(Len > 1)
	{
		// Clear ADDR flag
		I2C_ClearADDRFlag(pI2CHandle);


		for(uint32_t i = Len; i > 0; i--)
		{
			// Wait until RXEN become 1
			while(!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_RXNE_FLAG));

			// if last 2 bytes are remaining
			if(i == 2)
			{
				// Disable Acking
				I2C_ManageAcking(pI2CHandle->pI2Cx, I2C_ACK_DISABLE);
			}

			if(i == 1 && STOP == I2C_STOP)
			{
				// Generate stop condition
				I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
			}

			// Read data in to buffer
			*pRxBuffer = pI2CHandle->pI2Cx->DR;

			// Increment the buffer address
			pRxBuffer++;
		}
	}

	// re-enable acking
	if(pI2CHandle->I2C_Config.I2C_ACKControl == I2C_ACK_ENABLE)
	{
		I2C_ManageAcking(pI2CHandle->pI2Cx, I2C_ACK_ENABLE);
	}
}

/*********************************************************************
 * @fn      		  - I2C_ManageAcking
 *
 * @brief             - This function configure ack bit in CR1
 *
 * @param[in]         - the base address of the I2Cx
 * @param[in]         - enable or disable
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

void I2C_ManageAcking(I2C_RegDef_t *pI2Cx, uint8_t EnorDi)
{
	if(EnorDi == I2C_ACK_ENABLE)
	{
		//enable the ack
		pI2Cx->CR1 |= ( 1 << I2C_CR1_ACK);
	}else
	{
		//disable the ack
		pI2Cx->CR1 &= ~( 1 << I2C_CR1_ACK);
	}
}

/*********************************************************************
 * @fn      		  - I2C_SlaveSendData
 *
 * @brief             - This function allows send to master with I2C
 *
 * @param[in]         - the registers of I2Cx
 * @param[in]         - byte of data
 *
 * @return            -
 *
 * @Note              -

 */

void I2C_SlaveSendData(I2C_RegDef_t* pI2Cx, uint8_t data)
{
	pI2Cx->DR = data;
}

/*********************************************************************
 * @fn      		  - I2C_SlaveReceiveData
 *
 * @brief             - This function allows receive the data from master with I2C
 *
 * @param[in]         - the registers of I2Cx
 *
 * @return            -
 *
 * @Note              -

 */

uint8_t I2C_SlaveReceiveData(I2C_RegDef_t* pI2Cx)
{
	return (uint8_t)pI2Cx->DR;
}

/*********************************************************************
 * @fn      		  - I2C_MasterSendDataIT
 *
 * @brief             - This function allows iterrupts send data with I2C
 *
 * @param[in]         - the pI2CHandle
 * @param[in]         - tx buffer
 * @param[in]         - length of message
 * @param[in]         - slave address
 * @param[in]         - repeated start or stop
 *
 * @return            -
 *
 * @Note              -

 */

uint8_t I2C_MasterSendDataIT(I2C_Handle_t *pI2CHandle, uint8_t* pTxBuffer, uint32_t Len, uint8_t SlaveAddress, uint8_t STOP)
{
	uint8_t state = pI2CHandle->TxRxState;

	if((state != I2C_BUSY_IN_TX) && (state != I2C_BUSY_IN_RX))
	{
		pI2CHandle->pTxbuffer = pTxBuffer;
		pI2CHandle->TxLen = Len;
		pI2CHandle->DevAddr = SlaveAddress;
		pI2CHandle->TxRxState = I2C_BUSY_IN_TX;
		pI2CHandle->STOP = STOP;


		// Clear stop in CR1 to avoid BERR
		pI2CHandle->pI2Cx->CR1 &= ~(1 << I2C_CR1_STOP);

		// Generate start condition
		I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

		// Enable ITBUFEN control bit
		pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_ITBUFEN);

		// Enable ITEVFEN control bit
		pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_ITEVTEN);

		// Enable ITEEREN control bit
		pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_ITERREN);
	}

	return state;
}

/*********************************************************************
 * @fn      		  - I2C_MasterReceiveDataIT
 *
 * @brief             - This function allows iterrupts receive from slave data with I2C
 *
 * @param[in]         - the pI2CHandle
 * @param[in]         - tx buffer
 * @param[in]         - length of message
 * @param[in]         - slave address
 * @param[in]         - repeated start or stop
 *
 * @return            -
 *
 * @Note              -

 */

uint8_t I2C_MasterReceiveDataIT(I2C_Handle_t *pI2CHandle, uint8_t* pRxBuffer, uint32_t Len, uint8_t SlaveAddress, uint8_t STOP)
{
	uint8_t state = pI2CHandle->TxRxState;

	if((state != I2C_BUSY_IN_TX) && (state != I2C_BUSY_IN_RX))
	{
		pI2CHandle->pRxbuffer = pRxBuffer;
		pI2CHandle->RxLen = Len;
		pI2CHandle->DevAddr = SlaveAddress;
		pI2CHandle->RxSize = Len;
		pI2CHandle->TxRxState = I2C_BUSY_IN_RX;
		pI2CHandle->STOP = STOP;

		// Clear stop in CR1 to avoid BERR
		pI2CHandle->pI2Cx->CR1 &= ~(1 << I2C_CR1_STOP);

		// Generate start condition
		I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

		// Enable ITBUFEN control bit
		pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_ITBUFEN);

		// Enable ITEVFEN control bit
		pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_ITEVTEN);

		// Enable ITEEREN control bit
		pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_ITERREN);
	}

	return state;
}

/*********************************************************************
 * @fn      		  - I2C_IRQInterruptConfig
 *
 * @brief             - This function IRQ is (interrupt request) configuration
 *
 * @param[in]         - the number of the I2Cx
 * @param[in]         - Enable or Disable
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

void I2C_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi)
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

/* @fn      		 - I2C_PriorityConfig
*
* @brief             - This function I2C_PriorityConfig is (priority) configuration
*
* @param[in]         - the number of the I2Cx
* @param[in]         - priority of IRQ (interrupt request)
* @param[in]         -
*
* @return            - none
*
* @Note              - none

*/

void I2C_PriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	// 1.First lets find out which IPR register use
	uint8_t iprx = IRQNumber / 4;
	uint8_t iprx_section = IRQNumber % 4;
	uint8_t shift_amount = ((8 * iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED));

	*(NVIC_PR_BASEADDR + iprx) |= (IRQPriority << shift_amount);
}

/* @fn      		 - I2C_EV_IRQHandling
*
* @brief             - This function is ISR for I2C events
*
* @param[in]         - the pI2CHandle
* @param[in]         -
* @param[in]         -
*
* @return            - none
*
* @Note              - none

*/

void I2C_EV_IRQHandling(I2C_Handle_t *pI2CHandle)
{
	// Interrupt handling for both master and slave mode of device

	uint32_t temp1, temp2, temp3;

	temp1 = (pI2CHandle->pI2Cx->CR2 & (1 << I2C_CR2_ITEVTEN));
	temp2 = (pI2CHandle->pI2Cx->CR2 & (1 << I2C_CR2_ITBUFEN));

	temp3 = (pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_SB));

	// 1. Handle for interrupt generated by SB event
	if(temp1 && temp3)
	{
		// SB flag is set
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX)
		{
			I2C_ExecuteAddressPhaseWrite(pI2CHandle->pI2Cx, pI2CHandle->DevAddr);
		}
		else if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
		{
			I2C_ExecuteAddressPhaseRead(pI2CHandle->pI2Cx, pI2CHandle->DevAddr);
		}
	}

	temp3 = (pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_ADDR));

	// 2. Handle for interrupt generated by ADDR event
	// Master mode: Address is sent
	// Slave mode: address matches with own address
	if(temp1 && temp3)
	{
		// ADDR flag is set
		I2C_ClearADDRFlag(pI2CHandle);
	}

	temp3 = (pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_ADD10));

	// 3. Handle for interrupt generated by ADDR10 event
	// Master mode: Address is sent
	// Slave mode: address matches with own address
	if(temp1 && temp3)
	{
		// ADD10 flag is set
	}

	temp3 = (pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_BTF));

	// 4. Handle for interrupt generated by BTF event
	if(temp1 && temp3)
	{
		// BFF flag is set
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX )
		{
			if(pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_TXE))
			{
				// BTF, TXE = 1

				if(pI2CHandle->TxLen == 0)
				{
					// 1. Generate stop condition
					if(pI2CHandle->STOP == I2C_STOP)
					{
						I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
					}

					// 2. Reset all member elements of the handle structure
					I2C_CloseSendData(pI2CHandle);

					// 3. Notify application about completing transmission
					I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_TX_CMPLT);
				}
			}
		}
		else if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
		{
			// do nothing
		}
	}

	temp3 = (pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_STOPF));

	// 5. Handle for interrupt generated by STOPF event
	// stop flag available only in slave mode
	if(temp1 && temp3)
	{
		// STOPF flag is set
		// Clear STOPF  ( read SR1 then write to CR1 )
		pI2CHandle->pI2Cx->CR1 |= 0x0000;

		// Notify application about stop is generated by the master
		I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_STOP);
	}

	temp3 = (pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_TXE));

	// 6. Handle for interrupt generated by TxE event
	if(temp1 && temp2 && temp3)
	{
		// TxE flag is set
		// We have to do data transmission

		// Check device mode
		if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_MSL))
		{
			I2C_MasterHandleTXEInterrupt(pI2CHandle);
		}
		else
		{
			// transfer for slave
			// make sure slave actually it transmitter mode
			if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_TRA))
			{
				I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_DATA_REQ);
			}

		}
	}

	temp3 = (pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_RXNE));

	// 7. Handle for interrupt generated by RxNE event
	if(temp1 && temp2 && temp3)
	{
		// RxNE flag is set
		// We have to do data receive

		// Check device mode
		if(pI2CHandle->pI2Cx->SR2 & I2C_MSL_FLAG)
		{
			I2C_MasterHandleRXNEInterrupt(pI2CHandle);
		}
		else
		{
			// transfer for slave
			// make sure slave actually it receiver mode
			if(!(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_TRA)))
			{
				I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_DATA_RCV);
			}

		}
	}
}

/* @fn      		 - I2C_ER_IRQHandling
*
* @brief             - This function is ISR for I2C errors
*
* @param[in]         - the pI2CHandle
* @param[in]         -
* @param[in]         -
*
* @return            - none
*
* @Note              - none

*/

void I2C_ER_IRQHandling(I2C_Handle_t *pI2CHandle)
{

	uint32_t temp1,temp2;

    //Know the status of  ITERREN control bit in the CR2
	temp2 = (pI2CHandle->pI2Cx->CR2) & ( 1 << I2C_CR2_ITERREN);


/***********************Check for Bus error************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1<< I2C_SR1_BERR);
	if(temp1  && temp2 )
	{
		//This is Bus error

		//Implement the code to clear the buss error flag
		pI2CHandle->pI2Cx->SR1 &= ~(I2C_BERR_FLAG);

		//Implement the code to notify the application about the error
	   I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_BERR);
	}

/***********************Check for arbitration lost error************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_ARLO );
	if(temp1  && temp2)
	{
		//This is arbitration lost error

		//Implement the code to clear the arbitration lost error flag
		pI2CHandle->pI2Cx->SR1 &= ~(I2C_ARLO_FLAG);

		//Implement the code to notify the application about the error
		I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_ARLO);
	}

/***********************Check for ACK failure  error************************************/

	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_AF);
	if(temp1  && temp2)
	{
		//This is ACK failure error

	    //Implement the code to clear the ACK failure error flag
		pI2CHandle->pI2Cx->SR1 &= ~(I2C_AF_FLAG);

		//Implement the code to notify the application about the error
		I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_AF);
	}

/***********************Check for Overrun/underrun error************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_OVR);
	if(temp1  && temp2)
	{
		//This is Overrun/underrun

	    //Implement the code to clear the Overrun/underrun error flag
		pI2CHandle->pI2Cx->SR1 &= ~(I2C_OVR_FLAG);

		//Implement the code to notify the application about the error
		I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_OVR);
	}

/***********************Check for Time out error************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_TIMEOUT);
	if(temp1  && temp2)
	{
		//This is Time out error

	    //Implement the code to clear the Time out error flag
		pI2CHandle->pI2Cx->SR1 &= ~(I2C_TIMEOUT_FLAG);

		//Implement the code to notify the application about the error
		I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_TIMEOUT);
	}

}

/*********************************************************************
 * @fn      		  - I2C_GetFlagStatus
 *
 * @brief             - This function allows get status flag for some I2C flag
 *
 * @param[in]         - the base address of the I2Cx
 * @param[in]         - flag name
 * @param[in]         -
 *
 * @return            -
 *
 * @Note              -

 */

uint8_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx, uint32_t FlagName)
{
	if(pI2Cx->SR1 & FlagName)
	{
		return FLAG_SET;
	}
	return FLAG_RESET;
}

/*********************************************************************
 * @fn      		  - I2C_ApplicationEventCallback
 *
 * @brief             - This function is Application callback for TX / RX / OVR
 *
 * @param[in]         - I2Cx handle
 * @param[in]         - Event type
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

__weak void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle, uint8_t EVENT)
{
	// This is the weak implementation, so the application can override this function
}

/*********************************************************************
 * @fn      		  - I2C_SlaveEnableDisableCallbackEvents
 *
 * @brief             - This function enable or disable callback for events in the slave mode
 *
 * @param[in]         - I2Cx registers
 * @param[in]         - Enable or disable
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              - none

 */

void I2C_SlaveEnableDisableCallbackEvents(I2C_RegDef_t *pI2Cx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		pI2Cx->CR2 |= (1 << I2C_CR2_ITBUFEN);
		pI2Cx->CR2 |= (1 << I2C_CR2_ITEVTEN);
		pI2Cx->CR2 |= (1 << I2C_CR2_ITERREN);
	}
	else
	{
		pI2Cx->CR2 &= ~(1 << I2C_CR2_ITBUFEN);
		pI2Cx->CR2 &= ~(1 << I2C_CR2_ITEVTEN);
		pI2Cx->CR2 &= ~(1 << I2C_CR2_ITERREN);
	}
}
