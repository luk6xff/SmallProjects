#include "AT24c64.h"
#include "stm32_hardware_modules.h"
__IO uint16_t AT24C64_Address = 0;
__IO uint32_t AT24C64_Timeout = AT24C64_LONG_TIMEOUT;
__IO uint16_t AT24C64_DataNum;

typedef struct {
	uint8_t nrOfStoredCodes;
	uint8_t lengthOfStoredCode;

} DataLocationInMemory;

void AT24C64_DeInit(void) {
	AT24C64_HwDriverDeInit();
}

/**
 * @brief  Initializes peripherals used by the I2C EEPROM driver.
 * @param  None
 * @retval None
 */
void AT24C64_Init(void) {
	I2C_InitTypeDef I2C_InitStructure;

	AT24C64_HwDriverInit();

	/*!< I2C configuration */
	/* sEE_I2C configuration */
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
	I2C_InitStructure.I2C_DigitalFilter = 0x00;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_Timing = AT24C64_I2C_TIMING;

	/* Apply sEE_I2C configuration after enabling it */
	I2C_Init(AT24C64_I2C, &I2C_InitStructure);

	/* sEE_I2C Peripheral Enable */
	I2C_Cmd(AT24C64_I2C, ENABLE);

	/*!< Select the EEPROM address */
	AT24C64_Address = AT24C64_ADDRESS;
}

/**
 * @brief  Reads a block of data from the EEPROM.
 * @param  pBuffer: pointer to the buffer that receives the data read from
 *         the EEPROM.
 * @param  ReadAddr: EEPROM's internal address to start reading from.
 * @param  NumByteToRead: pointer to the variable holding number of bytes to
 *         be read from the EEPROM.
 *
 * @retval sEE_OK (0) if operation is correctly performed, else return value
 *         different from sEE_OK (0) or the timeout user callback.
 */

uint32_t AT24C64_ReadBuffer(uint8_t* pBuffer, uint16_t ReadAddr,
		uint16_t* NumByteToRead) {
	uint32_t NumbOfSingle = 0, Count = 0, DataNum = 0, StartCom = 0;

	/* Get number of reload cycles */
	Count = (*NumByteToRead) / 255;
	NumbOfSingle = (*NumByteToRead) % 255;

	/* Configure slave address, nbytes, reload and generate start */
	I2C_TransferHandling(AT24C64_I2C, AT24C64_Address, 2, I2C_SoftEnd_Mode,
			I2C_Generate_Start_Write);

	/* Wait until TXIS flag is set */
	AT24C64_Timeout = AT24C64_LONG_TIMEOUT;
	while (I2C_GetFlagStatus(AT24C64_I2C, I2C_ISR_TXIS) == RESET) {
		if ((AT24C64_Timeout--) == 0)
			return AT24C64_TIMEOUT_UserCallback();
	}

	/* Send MSB of memory address */
	I2C_SendData(AT24C64_I2C, (uint8_t)((ReadAddr & 0xFF00) >> 8));

	/* Wait until TXIS flag is set */
	AT24C64_Timeout = AT24C64_LONG_TIMEOUT;
	while (I2C_GetFlagStatus(AT24C64_I2C, I2C_ISR_TXIS) == RESET) {
		if ((AT24C64_Timeout--) == 0)
			return AT24C64_TIMEOUT_UserCallback();
	}

	/* Send LSB of memory address  */
	I2C_SendData(AT24C64_I2C, (uint8_t)(ReadAddr & 0x00FF));

	/* Wait until TC flag is set */
	AT24C64_Timeout = AT24C64_LONG_TIMEOUT;
	while (I2C_GetFlagStatus(AT24C64_I2C, I2C_ISR_TC) == RESET) {
		if ((AT24C64_Timeout--) == 0)
			return AT24C64_TIMEOUT_UserCallback();
	}

	/* If number of Reload cycles is not equal to 0 */
	if (Count != 0) {
		/* Starting communication */
		StartCom = 1;

		/* Wait until all reload cycles are performed */
		while (Count != 0) {
			/* If a read transfer is performed */
			if (StartCom == 0) {
				/* Wait until TCR flag is set */
				AT24C64_Timeout = AT24C64_LONG_TIMEOUT;
				while (I2C_GetFlagStatus(AT24C64_I2C, I2C_ISR_TCR) == RESET) {
					if ((AT24C64_Timeout--) == 0)
						return AT24C64_TIMEOUT_UserCallback();
				}
			}

			/* if remains one read cycle */
			if ((Count == 1) && (NumbOfSingle == 0)) {
				/* if starting communication */
				if (StartCom != 0) {
					/* Configure slave address, end mode and start condition */
					I2C_TransferHandling(AT24C64_I2C, AT24C64_Address, 255,
							I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
				} else {
					/* Configure slave address, end mode */
					I2C_TransferHandling(AT24C64_I2C, AT24C64_Address, 255,
							I2C_AutoEnd_Mode, I2C_No_StartStop);
				}
			} else {
				/* if starting communication */
				if (StartCom != 0) {
					/* Configure slave address, end mode and start condition */
					I2C_TransferHandling(AT24C64_I2C, AT24C64_Address, 255,
							I2C_Reload_Mode, I2C_Generate_Start_Read);
				} else {
					/* Configure slave address, end mode */
					I2C_TransferHandling(AT24C64_I2C, AT24C64_Address, 255,
							I2C_Reload_Mode, I2C_No_StartStop);
				}
			}

			/* Update local variable */
			StartCom = 0;
			DataNum = 0;

			/* Wait until all data are received */
			while (DataNum != 255) {
				/* Wait until RXNE flag is set */
				AT24C64_Timeout = AT24C64_LONG_TIMEOUT;
				while (I2C_GetFlagStatus(AT24C64_I2C, I2C_ISR_RXNE) == RESET) {
					if ((AT24C64_Timeout--) == 0)
						return AT24C64_TIMEOUT_UserCallback();
				}

				/* Read data from RXDR */
				pBuffer[DataNum] = I2C_ReceiveData(AT24C64_I2C);

				/* Update number of received data */
				DataNum++;
				(*NumByteToRead)--;
			}
			/* Update Pointer of received buffer */
			pBuffer += DataNum;

			/* update number of reload cycle */
			Count--;
		}

		/* If number of single data is not equal to 0 */
		if (NumbOfSingle != 0) {
			/* Wait until TCR flag is set */
			AT24C64_Timeout = AT24C64_LONG_TIMEOUT;
			while (I2C_GetFlagStatus(AT24C64_I2C, I2C_ISR_TCR) == RESET) {
				if ((AT24C64_Timeout--) == 0)
					return AT24C64_TIMEOUT_UserCallback();
			}

			/* Update CR2 : set Nbytes and end mode */
			I2C_TransferHandling(AT24C64_I2C, AT24C64_Address,
					(uint8_t)(NumbOfSingle), I2C_AutoEnd_Mode,
					I2C_No_StartStop);

			/* Reset local variable */
			DataNum = 0;

			/* Wait until all data are received */
			while (DataNum != NumbOfSingle) {
				/* Wait until RXNE flag is set */
				AT24C64_Timeout = AT24C64_LONG_TIMEOUT;
				while (I2C_GetFlagStatus(AT24C64_I2C, I2C_ISR_RXNE) == RESET) {
					if ((AT24C64_Timeout--) == 0)
						return AT24C64_TIMEOUT_UserCallback();
				}

				/* Read data from RXDR */
				pBuffer[DataNum] = I2C_ReceiveData(AT24C64_I2C);

				/* Update number of received data */
				DataNum++;
				(*NumByteToRead)--;
			}
		}
	} else {
		/* Update CR2 : set Slave Address , set read request, generate Start and set end mode */
		I2C_TransferHandling(AT24C64_I2C, AT24C64_Address,
				(uint32_t) (NumbOfSingle), I2C_AutoEnd_Mode,
				I2C_Generate_Start_Read);

		/* Reset local variable */
		DataNum = 0;

		/* Wait until all data are received */
		while (DataNum != NumbOfSingle) {
			/* Wait until RXNE flag is set */
			AT24C64_Timeout = AT24C64_LONG_TIMEOUT;
			while (I2C_GetFlagStatus(AT24C64_I2C, I2C_ISR_RXNE) == RESET) {
				if ((AT24C64_Timeout--) == 0)
					return AT24C64_TIMEOUT_UserCallback();
			}

			/* Read data from RXDR */
			pBuffer[DataNum] = I2C_ReceiveData(AT24C64_I2C);

			/* Update number of received data */
			DataNum++;
			(*NumByteToRead)--;
		}
	}

	/* Wait until STOPF flag is set */
	AT24C64_Timeout = AT24C64_LONG_TIMEOUT;
	while (I2C_GetFlagStatus(AT24C64_I2C, I2C_ISR_STOPF) == RESET) {
		if ((AT24C64_Timeout--) == 0)
			return AT24C64_TIMEOUT_UserCallback();
	}

	/* Clear STOPF flag */
	I2C_ClearFlag(AT24C64_I2C, I2C_ICR_STOPCF);

	/* If all operations OK, return sEE_OK (0) */
	return AT24C64_OK;
}

/**
 * @brief  Writes more than one byte to the EEPROM with a single WRITE cycle.
 *
 * @note   The number of bytes (combined to write start address) must not
 *         cross the EEPROM page boundary. This function can only write into
 *         the boundaries of an EEPROM page.
 * @note   This function doesn't check on boundaries condition (in this driver
 *         the function sEE_WriteBuffer() which calls sEE_WritePage() is
 *         responsible of checking on Page boundaries).
 *
 * @param  pBuffer: pointer to the buffer containing the data to be written to
 *         the EEPROM.
 * @param  WriteAddr: EEPROM's internal address to write to.
 * @param  NumByteToWrite: pointer to the variable holding number of bytes to
 *         be written into the EEPROM.
 *
 * @retval sEE_OK (0) if operation is correctly performed, else return value
 *         different from sEE_OK (0) or the timeout user callback.
 */
/*********************************************************************************************************************************************************************************/
uint32_t AT24C64_WritePage(uint8_t* pBuffer, uint16_t WriteAddr,
		uint8_t* NumByteToWrite) {
	uint32_t DataNum = 0;
	/* Configure slave address, nbytes, reload and generate start */
	I2C_TransferHandling(AT24C64_I2C, AT24C64_Address, 2, I2C_Reload_Mode,
			I2C_Generate_Start_Write);
	/* Wait until TXIS flag is set */
	AT24C64_Timeout = AT24C64_LONG_TIMEOUT;
	while (I2C_GetFlagStatus(AT24C64_I2C, I2C_ISR_TXIS) == RESET) {
		if ((AT24C64_Timeout--) == 0)
			return AT24C64_TIMEOUT_UserCallback();
	}
	/* Send MSB of memory address */
	I2C_SendData(AT24C64_I2C, (uint8_t)((WriteAddr & 0xFF00) >> 8));
	/* Wait until TXIS flag is set */
	AT24C64_Timeout = AT24C64_LONG_TIMEOUT;
	while (I2C_GetFlagStatus(AT24C64_I2C, I2C_ISR_TXIS) == RESET) {
		if ((AT24C64_Timeout--) == 0)
			return AT24C64_TIMEOUT_UserCallback();
	}
	/* Send LSB of memory address  */
	I2C_SendData(AT24C64_I2C, (uint8_t)(WriteAddr & 0x00FF));
	/* Wait until TCR flag is set */
	AT24C64_Timeout = AT24C64_LONG_TIMEOUT;
	while (I2C_GetFlagStatus(AT24C64_I2C, I2C_ISR_TCR) == RESET) {
		if ((AT24C64_Timeout--) == 0)
			return AT24C64_TIMEOUT_UserCallback();
	}
	/* Update CR2 : set Slave Address , set write request, generate Start and set end mode */
	I2C_TransferHandling(AT24C64_I2C, AT24C64_Address,
			(uint8_t)(*NumByteToWrite), I2C_AutoEnd_Mode, I2C_No_StartStop);
	while (DataNum != (*NumByteToWrite)) {
		/* Wait until TXIS flag is set */
		AT24C64_Timeout = AT24C64_LONG_TIMEOUT;
		while (I2C_GetFlagStatus(AT24C64_I2C, I2C_ISR_TXIS) == RESET) {
			if ((AT24C64_Timeout--) == 0)
				return AT24C64_TIMEOUT_UserCallback();
		}
		/* Write data to TXDR */
		I2C_SendData(AT24C64_I2C, (uint8_t)(pBuffer[DataNum]));
		/* Update number of transmitted data */
		DataNum++;
	}
	/* Wait until STOPF flag is set */
	AT24C64_Timeout = AT24C64_LONG_TIMEOUT;
	while (I2C_GetFlagStatus(AT24C64_I2C, I2C_ISR_STOPF) == RESET) {
		if ((AT24C64_Timeout--) == 0)
			return AT24C64_TIMEOUT_UserCallback();
	}
	/* Clear STOPF flag */
	I2C_ClearFlag(AT24C64_I2C, I2C_ICR_STOPCF);
	/* If all operations OK, return sEE_OK (0) */
	return AT24C64_OK;
}

/**
 * @brief  Writes buffer of data to the I2C EEPROM.
 * @param  pBuffer: pointer to the buffer  containing the data to be written
 *         to the EEPROM.
 * @param  WriteAddr: EEPROM's internal address to write to.
 * @param  NumByteToWrite: number of bytes to write to the EEPROM.
 * @retval None
 */
void AT24C64_WriteBuffer(uint8_t* pBuffer, uint16_t WriteAddr,
		uint16_t NumByteToWrite) {
	uint16_t NumOfPage = 0, NumOfSingle = 0, count = 0;
	uint16_t Addr = 0;

	Addr = WriteAddr % AT24C64_PAGE_SIZE;
	count = AT24C64_PAGE_SIZE - Addr;
	NumOfPage = NumByteToWrite / AT24C64_PAGE_SIZE;
	NumOfSingle = NumByteToWrite % AT24C64_PAGE_SIZE;

	/*!< If WriteAddr is sEE_PAGESIZE aligned  */
	if (Addr == 0) {
		/*!< If NumByteToWrite < sEE_PAGESIZE */
		if (NumOfPage == 0) {
			/* Store the number of data to be written */
			AT24C64_DataNum = NumOfSingle;
			/* Start writing data */
			AT24C64_WritePage(pBuffer, WriteAddr,
					(uint8_t*) (&AT24C64_DataNum));
			AT24C64_WaitEepromStandbyState();
		}
		/*!< If NumByteToWrite > sEE_PAGESIZE */
		else {
			while (NumOfPage--) {
				/* Store the number of data to be written */
				AT24C64_DataNum = AT24C64_PAGE_SIZE;
				AT24C64_WritePage(pBuffer, WriteAddr,
						(uint8_t*) (&AT24C64_DataNum));
				AT24C64_WaitEepromStandbyState();

				WriteAddr += AT24C64_PAGE_SIZE;
				pBuffer += AT24C64_PAGE_SIZE;
			}

			if (NumOfSingle != 0) {
				/* Store the number of data to be written */
				AT24C64_DataNum = NumOfSingle;
				AT24C64_WritePage(pBuffer, WriteAddr,
						(uint8_t*) (&AT24C64_DataNum));
				AT24C64_WaitEepromStandbyState();
			}
		}
	}
	/*!< If WriteAddr is not sEE_PAGESIZE aligned  */
	else {
		/*!< If NumByteToWrite < sEE_PAGESIZE */
		if (NumOfPage == 0) {
			/*!< If the number of data to be written is more than the remaining space
			 in the current page: */
			if (NumByteToWrite > count) {
				/* Store the number of data to be written */
				AT24C64_DataNum = count;
				/*!< Write the data conained in same page */
				AT24C64_WritePage(pBuffer, WriteAddr,
						(uint8_t*) (&AT24C64_DataNum));
				AT24C64_WaitEepromStandbyState();

				/* Store the number of data to be written */
				AT24C64_DataNum = (NumByteToWrite - count);
				/*!< Write the remaining data in the following page */
				AT24C64_WritePage((uint8_t*) (pBuffer + count),
						(WriteAddr + count), (uint8_t*) (&AT24C64_DataNum));
				AT24C64_WaitEepromStandbyState();
			} else {
				/* Store the number of data to be written */
				AT24C64_DataNum = NumOfSingle;
				AT24C64_WritePage(pBuffer, WriteAddr,
						(uint8_t*) (&AT24C64_DataNum));
				AT24C64_WaitEepromStandbyState();
			}
		}
		/*!< If NumByteToWrite > sEE_PAGESIZE */
		else {
			NumByteToWrite -= count;
			NumOfPage = NumByteToWrite / AT24C64_PAGE_SIZE;
			NumOfSingle = NumByteToWrite % AT24C64_PAGE_SIZE;

			if (count != 0) {
				/* Store the number of data to be written */
				AT24C64_DataNum = count;
				AT24C64_WritePage(pBuffer, WriteAddr,
						(uint8_t*) (&AT24C64_DataNum));
				AT24C64_WaitEepromStandbyState();
				WriteAddr += count;
				pBuffer += count;
			}

			while (NumOfPage--) {
				/* Store the number of data to be written */
				AT24C64_DataNum = AT24C64_PAGE_SIZE;
				AT24C64_WritePage(pBuffer, WriteAddr,
						(uint8_t*) (&AT24C64_DataNum));
				AT24C64_Timeout = AT24C64_LONG_TIMEOUT;
				AT24C64_WaitEepromStandbyState();
				WriteAddr += AT24C64_PAGE_SIZE;
				pBuffer += AT24C64_PAGE_SIZE;
			}
			if (NumOfSingle != 0) {
				/* Store the number of data to be written */
				AT24C64_DataNum = NumOfSingle;
				AT24C64_WritePage(pBuffer, WriteAddr,
						(uint8_t*) (&AT24C64_DataNum));
				AT24C64_WaitEepromStandbyState();
			}
		}
	}
}

/**
 * @brief  Wait for EEPROM Standby state.
 *
 * @note  This function allows to wait and check that EEPROM has finished the
 *        last operation. It is mostly used after Write operation: after receiving
 *        the buffer to be written, the EEPROM may need additional time to actually
 *        perform the write operation. During this time, it doesn't answer to
 *        I2C packets addressed to it. Once the write operation is complete
 *        the EEPROM responds to its address.
 *
 * @param  None
 *
 * @retval sEE_OK (0) if operation is correctly performed, else return value
 *         different from sEE_OK (0) or the timeout user callback.
 */
uint32_t AT24C64_WaitEepromStandbyState(void) {
	__IO uint32_t sEETrials = 0;

	/* Keep looping till the slave acknowledge his address or maximum number
	 of trials is reached (this number is defined by sEE_MAX_TRIALS_NUMBER define
	 in stm32373c_eval_i2c_ee.h file) */

	/* Configure CR2 register : set Slave Address and end mode */
	I2C_TransferHandling(AT24C64_I2C, AT24C64_Address, 0, I2C_AutoEnd_Mode,
			I2C_No_StartStop);

	do {
		/* Initialize sEETimeout */
		AT24C64_Timeout = AT24C64_FLAG_TIMEOUT;

		/* Clear NACKF */
		I2C_ClearFlag(AT24C64_I2C, I2C_ICR_NACKCF | I2C_ICR_STOPCF);

		/* Generate start */
		I2C_GenerateSTART(AT24C64_I2C, ENABLE);

		/* Wait until timeout elapsed */
		while (AT24C64_Timeout-- != 0)
			;

		/* Check if the maximum allowed numbe of trials has bee reached */
		if (sEETrials++ == AT24C64_MAX_TRIALS_NUMBER) {
			/* If the maximum number of trials has been reached, exit the function */
			return AT24C64_TIMEOUT_UserCallback();
		}
	} while (I2C_GetFlagStatus(AT24C64_I2C, I2C_ISR_NACKF) != RESET);

	/* Clear STOPF */
	I2C_ClearFlag(AT24C64_I2C, I2C_ICR_STOPCF);

	/* Return sEE_OK if device is ready */
	return AT24C64_OK;
}

/**
 * @brief  Basic management of the timeout situation.
 * @param  None.
 * @retval None.
 */
uint32_t AT24C64_TIMEOUT_UserCallback(void) {
	/* Block communication and all processes */
	vSendIntToUart(6);
	GPIOA->ODR ^= GPIO_ODR_10;
}

/**
 * @brief  clears memory what means put '0' char to each byte.
 * @param  None.
 * @retval success or not
 */
uint8_t AT24C64_ClearMemory(void) {
	/* Initialize the I2C EEPROM driver ----------------------------------------*/
	AT24C64_Init();
	//if (nrOfBytes > (AT24C64_PAGE_SIZE * AT24C64_NR_OF_PAGES))
	char rxBuffer[6];
	uint16_t NumDataRead = 5;
	AT24C64_ReadBuffer(rxBuffer, addressToRead, (uint16_t *) (&NumDataRead));
	rxBuffer[5] = '\0';
	vSendDataToUart(rxBuffer);
	/* Free all used resources */
	AT24C64_DeInit();

}
////////////////////////////////////////////TEST METHODS ////////////////////////////////////////////
typedef enum {
	FAILED = 0, PASSED = !FAILED
} TestStatus;

#define sEE_WRITE_ADDRESS1        0x00
#define sEE_READ_ADDRESS1         0x00
#define BUFFER_SIZE1             (countof(Tx1Buffer)-1)

/* Private macro -------------------------------------------------------------*/
#define countof(a) (sizeof(a) / sizeof(*(a)))

/* Private variables ---------------------------------------------------------*/
const uint8_t Tx1Buffer[] =
		"STM32F0xx I2C Firmware Library EEPROM driver example";

uint8_t Rx1Buffer[BUFFER_SIZE1];
volatile TestStatus TransferStatus = FAILED, TransferStatus2 = FAILED;
volatile uint16_t NumDataRead = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2,
		uint16_t BufferLength);

#define ENABLE_UART_DEBUG

uint8_t I2C_TestMethod(void) {
	uint8_t numOfBytes = countof(Tx1Buffer);

	/* Initialize the I2C EEPROM driver ----------------------------------------*/
	AT24C64_Init();
	//AT24C64_WritePage(Tx1Buffer, 0x00, &numOfBytes);

	/* First write in the memory followed by a read of the written data --------*/
	/* Write on I2C EEPROM from sEE_WRITE_ADDRESS1 */
	AT24C64_WriteBuffer(Tx1Buffer, sEE_WRITE_ADDRESS1, BUFFER_SIZE1);

	/* Set the Number of data to be read */
	NumDataRead = BUFFER_SIZE1;
	/* Read from I2C EEPROM from sEE_READ_ADDRESS1 */
	AT24C64_ReadBuffer(Rx1Buffer, sEE_READ_ADDRESS1,
			(uint16_t *) (&NumDataRead));

	TransferStatus = Buffercmp(Tx1Buffer, Rx1Buffer, BUFFER_SIZE1);
#ifdef ENABLE_UART_DEBUG
	if (TransferStatus == PASSED) {
		vSendIntToUart(9);
	} else {
		vSendIntToUart(8);
	}
#endif /* ENABLE_UART_DEBUG */

	/* Free all used resources */
	AT24C64_DeInit();

}

/**
 * @brief  Compares two buffers.
 * @param  pBuffer1, pBuffer2: buffers to be compared.
 * @param  BufferLength: buffer's length
 * @retval PASSED: pBuffer1 identical to pBuffer2
 *         FAILED: pBuffer1 differs from pBuffer2
 */
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2,
		uint16_t BufferLength) {
	while (BufferLength--) {
		if (*pBuffer1 != *pBuffer2) {
			return FAILED;
		}

		pBuffer1++;
		pBuffer2++;
	}

	return PASSED;
}

void displayDataOfEEpromMemory(uint16_t addressToRead) {
	/* Initialize the I2C EEPROM driver ----------------------------------------*/
	AT24C64_Init();
	//if (nrOfBytes > (AT24C64_PAGE_SIZE * AT24C64_NR_OF_PAGES))
	char rxBuffer[6];
	uint16_t NumDataRead = 5;
	AT24C64_ReadBuffer(rxBuffer, addressToRead, (uint16_t *) (&NumDataRead));
	rxBuffer[5] = '\0';
	vSendDataToUart(rxBuffer);
	/* Free all used resources */
	AT24C64_DeInit();

}

/*


 void AT24CXX_WriteBuffer(unsigned short Address, unsigned char * Buffer,
 unsigned short Length) {
 unsigned short Counter;
 unsigned char FirstPageLeft = PAGE_SIZE - Address % PAGE_SIZE;
 unsigned char LastPageUsed = (Address + Length) % PAGE_SIZE;
 unsigned short FullPageNums = (Length - FirstPageLeft - LastPageUsed)
 / PAGE_SIZE;
 if (FirstPageLeft >= Length) {
 AT24CXX_WriteCommand(Address, Buffer, Length);
 } else {
 if (Length <= (FirstPageLeft + LastPageUsed)) {
 AT24CXX_WriteCommand(Address, Buffer, FirstPageLeft);

 AT24CXX_Delay(AT_DELAY);

 AT24CXX_WriteCommand(Address + FirstPageLeft,
 Buffer + FirstPageLeft, LastPageUsed);
 }
 }

 if (FullPageNums > 0) {
 if (FirstPageLeft > 0) {
 AT24CXX_WriteCommand(Address, Buffer, FirstPageLeft);
 AT24CXX_Delay(AT_DELAY);
 }
 for (Counter = 0; Counter < FullPageNums; Counter++) {
 AT24CXX_WriteCommand(Address + FirstPageLeft + Counter * PAGE_SIZE,
 Buffer + FirstPageLeft + Counter * PAGE_SIZE, PAGE_SIZE);
 AT24CXX_Delay(AT_DELAY);
 }
 if (LastPageUsed > 0) {
 AT24CXX_Delay(AT_DELAY);
 AT24CXX_WriteCommand(
 (Address + FirstPageLeft + FullPageNums * PAGE_SIZE),
 (Buffer + FirstPageLeft + FullPageNums * PAGE_SIZE),
 LastPageUsed);
 }
 }
 }

 void AT24CXX_ReadBuffer(unsigned short Address, unsigned char * Buffer,
 unsigned short Length) {
 unsigned short Counter;
 unsigned char FirstPageLeft = PAGE_SIZE - Address % PAGE_SIZE;
 unsigned char LastPageUsed = (Address + Length) % PAGE_SIZE;
 unsigned short FullPageNums = (Length - FirstPageLeft - LastPageUsed)
 / PAGE_SIZE;
 if (FirstPageLeft >= Length) {
 AT24CXX_ReadCommand(Address, Buffer, Length);
 } else {
 if (Length <= (FirstPageLeft + LastPageUsed)) {
 AT24CXX_ReadCommand(Address, Buffer, FirstPageLeft);

 AT24CXX_Delay(AT_DELAY);

 AT24CXX_ReadCommand(Address + FirstPageLeft, Buffer + FirstPageLeft,
 LastPageUsed);
 }
 }

 if (FullPageNums > 0) {
 if (FirstPageLeft > 0) {
 AT24CXX_ReadCommand(Address, Buffer, FirstPageLeft);
 AT24CXX_Delay(AT_DELAY);
 }
 for (Counter = 0; Counter < FullPageNums; Counter++) {
 AT24CXX_ReadCommand(Address + FirstPageLeft + Counter * PAGE_SIZE,
 Buffer + FirstPageLeft + Counter * PAGE_SIZE, PAGE_SIZE);
 AT24CXX_Delay(AT_DELAY);
 }
 if (LastPageUsed > 0) {
 AT24CXX_Delay(AT_DELAY);
 AT24CXX_ReadCommand(
 (Address + FirstPageLeft + FullPageNums * PAGE_SIZE),
 (Buffer + FirstPageLeft + FullPageNums * PAGE_SIZE),
 LastPageUsed);
 }
 }
 }
 */
