#include "stm32_hardware_modules.h"
#include "stm_lib/inc/stm32f0xx_rcc.h"
#include "stm_lib/inc/stm32f0xx_usart.h"
#include "stm_lib/inc/stm32f0xx_spi.h"
#include "string.h"
#include "nRF24L01+.h"
#include "stdio.h"
#include "AT24c64.h"
#include "stm_lib/inc/stm32f0xx_syscfg.h"

#define DEBUG_MACRO

/*Private variables for USART1*/
#define USART_TXRXBUFFERSIZE   (33)    //+1 for command and status
#define NUMBER_OF_DATA_IN_USART_QUEUE (3)

typedef struct {
	uint8_t uiLengthOfString;
	char uiMessage[USART_TXRXBUFFERSIZE];
} xUSARTMessage;

uint8_t USART_TxBuffer[USART_TXRXBUFFERSIZE],
		USART_RxBuffer[USART_TXRXBUFFERSIZE];
xQueueHandle xUsartQueueHandle;
xUSARTMessage USART_TxData, USART_RxData;
/*Private variables for USART END Of declaration*/

/*Private variables for SPI2*/
#define SPI_TXRXBUFFERSIZE (NUMBER_OF_BYTES_IN_FIFO+1) //+1 for status register or command
#define NUMBER_OF_DATA_IN_SPI2_QUEUE (2)

typedef enum {
	SPI_WRITE, SPI_READ
} SPI_WRITE_READ; // useful to indicate when we send or read data to avoid unnecesaryy receiving messages

typedef struct {
	uint8_t uiNumberOfTxRxData;
	SPI_WRITE_READ eWriteReadData;
	uint8_t uiMessage[SPI_TXRXBUFFERSIZE];
} xSPIMessage;

xQueueHandle xSPI2WriteQueueHandle, xSPI2ReadQueueHandle;
xSPIMessage SPI_TxData, SPI_RxData;
/*Private variables for SPI2 END Of declaration*/

extern xTaskHandle xHandleTaskWaitForSpecialCommands; //handle for programming task
static const char* specialCommands[] = { "PROGRM", "DELETE" };

/*Private variables for IRQ HANDLING*/

xSemaphoreHandle nRF24L01PinIrqSemaphore = NULL;
/*Private variables for IRQ handling END Of declaration*/

/*private methods*/
//LED
static inline void initLED(void) {
	RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOAEN, ENABLE);
	GPIOA->MODER |= GPIO_MODER_MODER10_0;
	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR10_0;
}
static inline void LEDOff(void) {
	GPIOA->BSRR = GPIO_BSRR_BR_10;
}

static inline void LEDOn(void) {
	GPIOA->BSRR = GPIO_BSRR_BS_10;
}

static inline void LEDToogle(void) {
	GPIOA->ODR ^= GPIO_ODR_10;
}

/**
 *@name void SPI2_For_NRF24L01_init(void)
 *@brief Initialization of the required SPI2 module registers
 *@brief PINOUT:  PB12(SPI2_NSS),  PB13(SPI2_SCK),  PB14(SPI2_MISO),  PB15(SPI2_MOSI)
 *@param NONE
 *@retval NONE
 *
 */
void SPI2_For_NRF24L01_init(void) {

//register all necessary callbacks
	if (!nRF24L01_RegisterCallbacks(SPI2ReadDataViaInterrupt,
			SPI2WriteDataViaInterrupt, CE_PinForNRF24L01_Set,
			CE_PinForNRF24L01_Reset, SetIRQPinFornRF24L01))
		return; // if failed -> stop further initialization
//RCC
	RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOCEN, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOBEN, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1ENR_SPI2EN, ENABLE);
// GPIO

	GPIOB->MODER |= GPIO_MODER_MODER13_1 | GPIO_MODER_MODER14_1
			| GPIO_MODER_MODER15_1; // PB13(SPI2_SCK) , PB14(SPI2_MISO) , PB15(SPI2_MOSI) -> alternate function

	CS_PORT_SPI2->MODER |= CS_PIN_SPI2(GPIO_MODER_MODER,_0); //PB12(SPI2_NSS (CS) ) // OUTPUT
	CS_PORT_SPI2->PUPDR |= CS_PIN_SPI2(GPIO_PUPDR_PUPDR,_0); // PULL-UP

	CE_PORT_FOR_nRF24L01->MODER |= CE_PIN(GPIO_MODER_MODER,_0)/*6_0*/; // PC6 (PIN_CE) - OUTPUT
	CE_PORT_FOR_nRF24L01->PUPDR |= CE_PIN(GPIO_PUPDR_PUPDR,_0); // PC6 (PIN_CE) - PULL_UP
	//TODO -> PC7 IRQ INPUT
//SPI -> fpclk/4 ; MSB first ; NSS software
	//SPI2->CR1 = SPI_CR1_BR_0 | SPI_CR1_MSTR | SPI_CR1_SSM;
	//-> 8bit ; fifo 8bit , tx/rx dma enabled // interrupts for TX and RX
	//SPI2->CR2 = SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2 | SPI_CR2_FRXTH
	/*| SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN;SPI_CR2_TXEIE
	 | SPI_CR2_RXNEIE*/; // interrupts for test , then DMA
	///////////////////////////////////////////////////////////////SPI2->CR1 |= SPI_CR1_SPE; // enable SPI

	/* Wybranie parametrów pracy kontrolera SPI1 */
	SPI_InitTypeDef SPI_InitStructure;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_Init(SPI2, &SPI_InitStructure);
	//SPI_RxFIFOThresholdConfig(SPI2, SPI_RxFIFOThreshold_QF);

	SPI_CalculateCRC(SPI2, DISABLE);
	SPI_Cmd(SPI2, ENABLE);
	/* ze snippetsow for STM32F0
	 SPI2->CR1 = SPI_CR1_MSTR | SPI_CR1_BR;
	 SPI2->CR2 = SPI_CR2_SSOE | SPI_CR2_RXNEIE | SPI_CR2_FRXTH | SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0;
	 SPI2->CR1 |= SPI_CR1_SPE;
	 */
	//SPI2_RxInterruptEnable(); //ENABLE RX interrupts
	//NVIC
	NVIC_SetPriority(SPI2_IRQn, 4);
	NVIC_EnableIRQ(SPI2_IRQn);

	xSPI2WriteQueueHandle =
			xQueueCreate(NUMBER_OF_DATA_IN_SPI2_QUEUE,sizeof(xSPIMessage)); // create a queue for SPI transmit
	if (xSPI2WriteQueueHandle == 0) {
		while (1)
			; // error:  queue has not been created
	}

	xSPI2ReadQueueHandle =
			xQueueCreate(NUMBER_OF_DATA_IN_SPI2_QUEUE,sizeof(xSPIMessage)); // create a queue for SPI reception
	if (xSPI2ReadQueueHandle == 0) {
		while (1)
			; // error:  queue has not been created
	}

}
/**
 * @name  bool SPI_GetFlagStatus(SPI_TypeDef* SPIx, uint16_t SPI_SR_XXX)
 * @brief  Returns the status of SPI flags from SPI_SR register
 * @retval content of SPI_SR register
 */bool SPI_GetFlagStatus(SPI_TypeDef* SPIx, uint16_t SPI_SR_XXX) {
	if (SPI_SR_XXX > SPI_SR_FRE)
		return false;
	bool status;
	if (SPIx->SR & SPI_SR_XXX) {
		status = true;
	} else
		status = false;
	return status;
}
/**
 * @name void SendDataToSPI2(uint8_t*data)
 * @brief
 * @retval
 *
 */
void SPI2WriteDataViaInterrupt(uint8_t*data) {
	CS_PinSPI2_Set();
	xSPIMessage TxData;
	TxData.uiNumberOfTxRxData = data[0];
	TxData.eWriteReadData = SPI_WRITE;
	memcpy(TxData.uiMessage, &data[1], TxData.uiNumberOfTxRxData);
	xQueueSend(xSPI2WriteQueueHandle, &TxData, 0);
	CS_PinSPI2_Reset();
	SPI2_RxInterruptEnable();
	SPI2_TxInterruptEnable();

}

void SPI2ReadDataViaInterrupt(uint8_t*data) {
	CS_PinSPI2_Set();
	xSPIMessage RxData;
	RxData.uiNumberOfTxRxData = data[0];
	RxData.eWriteReadData = SPI_READ;
	memcpy(RxData.uiMessage, &data[1], RxData.uiNumberOfTxRxData);
	xQueueSend(xSPI2WriteQueueHandle, &RxData, 0);
	CS_PinSPI2_Reset();
	SPI2_RxInterruptEnable();
	SPI2_TxInterruptEnable();

}

/**
 * @name void SPI2_IRQHandler(void)
 * @brief
 * @retval
 *
 */
void SPI2_IRQHandler(void) {
	static uint8_t RxCount;
	static uint8_t TxCount;
	static SPI_WRITE_READ state = SPI_WRITE;

	if (xQueueReceive(xSPI2WriteQueueHandle, &SPI_TxData, 0)) {
		state = SPI_TxData.eWriteReadData;
	}

	if (SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_RXNE) == SET) {
		//	if (RxCount == 0) {        // to eliminate STATUS register Value from the received data
		//		SPI_ReceiveData8(SPI2);
		//	} else
		SPI_RxData.uiMessage[RxCount++] = SPI_ReceiveData8(SPI2);
		if (RxCount >= SPI_TxData.uiNumberOfTxRxData) { // +1 because additional STATUS register value
			SPI_RxData.uiNumberOfTxRxData = RxCount;
			RxCount = 0;

			if (state == SPI_READ) {
				xQueueReset(xSPI2ReadQueueHandle);
				xQueueSendFromISR(xSPI2ReadQueueHandle, &SPI_RxData, 0);
				// only data to read is sent
				state = SPI_WRITE;
			} else {
				xQueueReset(xSPI2ReadQueueHandle);
			}
			SPI2_RxInterruptDisable();
		}
	}
	if (SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_TXE) == SET) { // it means that the fifo buffer is empty
		SPI_SendData8(SPI2, SPI_TxData.uiMessage[TxCount++]);
		if (TxCount >= SPI_TxData.uiNumberOfTxRxData) {
			SPI2_TxInterruptDisable();
			TxCount = 0;
		}
	}
}
//////////////////////////////////TASK DO ODBIORU DANYCH ??????????????????????????????????????????
/**
 * @name  static void vTaskSPI2(void* pvParameters)
 * @brief
 * @retval
 *
 */
static void vTaskReceiveDataSPI2(void* pvParameters) {
	SPI2_For_NRF24L01_init();
	nRF24L01_Init(); //TODO CHANGE THE LOCATION OF IT
	xSPIMessage spiMessage;
	bool programmingFlag = false;
	bool deletingFlag = false;

	const uint8_t testData[NUMBER_OF_BYTES_IN_FIFO] = { 0, 1, 2, 3, 4, 5, 6, 7,
			8, 9 };

	for (;;) {

		while (xQueueReceive(xSPI2ReadQueueHandle,&spiMessage,portMAX_DELAY)
				!= pdPASS)
			;
#ifdef DEBUG_MACRO
		if (xSPI2ReadQueueHandle != NULL) {

			DEBUG("LenReceived:");
			vSendIntToUart(spiMessage.uiNumberOfTxRxData);
			DEBUG("Data:");
			for (uint8_t i = 0; i < spiMessage.uiNumberOfTxRxData; i++)
				vSendIntToUart(spiMessage.uiMessage[i]);
		}
#endif

		if (xHandleTaskWaitForSpecialCommands != NULL) {

			if (!programmingFlag && !deletingFlag) {
				for (uint8_t i = 0; i < 2; i++) {

					if (strncmp(specialCommands[0], &spiMessage.uiMessage[1], 6)
							== 0) {
						char tempBuf[2];
						switch (i) {
						case 0:
#ifdef DEBUG_MACRO
							programmingFlag = true;
							DEBUG("PROGRMING");
#endif
							break;
						case 1:
#ifdef DEBUG_MACRO
							deletingFlag = true;
							DEBUG("DELETING");
#endif
							break;
						default:
							DEBUG("CMD INCORRECT");
							break;
						}
					}
				}
			} else {

				if (programmingFlag) {
//PROGRAMMING FRAME: [0 byte: status register][1: length of FRAME (new code) ]-[ 2-4: "PRG"]-[5:20: "NEW CODE TO BE SAVED TO EEPROM"] [21:'x' delimeter]

					if (strncmp("PRG", &spiMessage.uiMessage[2], 3) == 0) {
						uint8_t * buf = (uint8_t*) pvPortMalloc(
								spiMessage.uiMessage[1]);
						if (buf == NULL) {
							programmingFlag = false;
							break;
						}
						memcpy(buf, &spiMessage.uiMessage[5],
								spiMessage.uiMessage[1]);
					}
				}

				else if (deletingFlag) {

				}

			}
			///TODO handling adding new device
			///void vTaskSuspend( TaskHandle_t xTaskToSuspend );
			///void vTaskResume( TaskHandle_t xTaskToResume );
		} else if (spiMessage.uiNumberOfTxRxData
				== NUMBER_OF_BYTES_IN_FIFO + 1) {
			/*
			 if (!strcmp(testData, &spiMessage.uiMessage[1])) {

			 DEBUG("Correct Data was received!");
			 }

			 else
			 DEBUG("Incorrect Data!");
			 */
		}

		nRF24L01_FlushSTATUSReg();
		nRF24L01_FlushRx(); //after receiving flush fifo
	}
}

/**
 * @name  void vStartSPITask(unsigned portBASE_TYPE uxPriority)
 * @brief
 * @retval
 *
 */
void vStartSPITask(unsigned portBASE_TYPE uxPriority) {
//xTaskHandle xHandleTaskSPI;
	xTaskCreate(vTaskReceiveDataSPI2, (signed const char*)"SPI", 160, NULL,
			uxPriority, NULL);

}

/**
 * @name  void vIrqNrfTask(unsigned portBASE_TYPE uxPriority)
 * @brief
 * @retval
 *
 */
static void vIrqNrfTask(void* pvParameters) {
	for (;;) {
		if (nRF24L01PinIrqSemaphore != NULL) {
			xSemaphoreTake(nRF24L01PinIrqSemaphore, portMAX_DELAY);
			//take the semaphore, IRQ event available
			nRF24L01_ReadFromFifo();

		}
	}
}

/**
 * @name  void vStartIrqSignalFromNrf24L01Task(unsigned portBASE_TYPE uxPriority)
 * @brief
 * @retval
 *
 */
void vStartIrqSignalFromNrf24L01Task(unsigned portBASE_TYPE uxPriority) {
//xTaskHandle xTaskIrqNrfReceived;
	xTaskCreate(vIrqNrfTask, (signed const char*)"NRF_IRQ", 200, NULL,
			uxPriority, NULL);
}

////////////////////////////////////////////////////////////IRQ_PIN-EXTERNAL INTERRUPT///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////IRQ_PIN-EXTERNAL INTERRUPT///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////IRQ_PIN-EXTERNAL INTERRUPT///////////////////////////////////////////////////////
void SetIRQPinFornRF24L01(void) {
	initLED();
	RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOCEN, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2ENR_SYSCFGEN, ENABLE); // Enable SYSCFG clock
	IRQ_PORT_FOR_nRF24L01->MODER &= ~(IRQ_PIN(GPIO_MODER_MODER,)); //PC7(IRQ ) // INPUT
	IRQ_PORT_FOR_nRF24L01->PUPDR |= IRQ_PIN(GPIO_PUPDR_PUPDR,_0); // PULL-UP
	SYSCFG_EXTILineConfig(EXTI_PortSourceForIRQ, EXTI_PinSourceForIRQ);
	EXTI->IMR |= EXTI_IMR_MR7; //Interrupt request from PC7 is not masked
	EXTI->FTSR |= EXTI_FTSR_TR7; // Falling trigger edge

	vSemaphoreCreateBinary(nRF24L01PinIrqSemaphore);
	if (nRF24L01PinIrqSemaphore == NULL) {
		while (1)
			; // error:  semaphore has not been created
	}

	/*NVIC for EXTI enable	*/
	NVIC_SetPriority(EXTI4_15_IRQn, 4);
	NVIC_EnableIRQ(EXTI4_15_IRQn);
	vStartIrqSignalFromNrf24L01Task(1); // create the task for an indication of waiting data in FIFO  to read

}

void EXTI4_15_IRQHandler(void) {
	if (EXTI->PR & EXTI_PR_PR7) {
		EXTI->PR |= EXTI_PR_PR7; //Clear the EXTI line (PC) 7 pending bit
		LEDToogle();
		//	nRF24L01_ReadFromFifo();
		xSemaphoreGiveFromISR(nRF24L01PinIrqSemaphore, 0);

	}
}
////////////////////////////////////////////////////////////USART///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////USART///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////USART///////////////////////////////////////////////////////
/**
 * @name   static void USART1Init(void)
 * @brief  Configures USART2 com port. PB6 (USART1_TX) , PB7(USART1_RX)
 * @retval None
 */

static void USART1Init(void) {
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	GPIOB->MODER |= GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1; //AF
	GPIOB->PUPDR = GPIO_PUPDR_PUPDR6_0 | GPIO_PUPDR_PUPDR7_0; // PULL-UP
	/* Enable GPIO clock */

	/* Enable USART clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 19200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl =
			USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	/* USART configuration */
	USART_Init(USART1, &USART_InitStructure);

	/*USART Enable */
	USART_Cmd(USART1, ENABLE);
	/*NVIC for USART enable	*/
	NVIC_SetPriority(USART1_IRQn, 7);
	NVIC_EnableIRQ(USART1_IRQn);

	xUsartQueueHandle =
			xQueueCreate(NUMBER_OF_DATA_IN_USART_QUEUE,sizeof(xUSARTMessage));
	if (xUsartQueueHandle == 0) {
		while (1)
			; // error:  queue has not been created

	}

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // enable USART1 RXNE Interrupt

}

/**
 * @name void USART1_IRQHandler(void)
 * @brief
 * @retval
 *
 */

void USART1_IRQHandler(void) {

	static uint8_t RxCount;
	static uint8_t TxCount;

	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
		USART_RxData.uiMessage[RxCount++] = (char) (USART_ReceiveData(USART1));
		if ((USART_RxData.uiMessage[RxCount - 1] == 'x')) { //let's assume  that lowercase letter 'x' is a delimeter

			USART_RxData.uiLengthOfString = strlen(USART_RxData.uiMessage) - 1;
			xQueueSendToBackFromISR(xUsartQueueHandle, &USART_RxData, pdFALSE);
			RxCount = 0;
		}
		if (RxCount == USART_TXRXBUFFERSIZE)
			RxCount = 0;
	}
	if (USART_GetITStatus(USART1, USART_IT_TXE) != RESET) {
		xQueueReceiveFromISR(xUsartQueueHandle, &USART_TxData, pdFALSE);
		USART_SendData(USART1, (uint16_t)(USART_TxData.uiMessage[TxCount++]));
		if (TxCount == USART_TxData.uiLengthOfString) {
			USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
			TxCount = 0;
		}
	}
}

/**
 * @name  static void vTaskUSART1(void* pvParameters)
 * @brief
 * @retval
 *
 */
static void vTaskUSART1(void* pvParameters) {

	USART1Init();
	const char*commands[] = { "GREEN", "BLUE", "BOTH", "PWRSET", "PWRRESET", //commands must be equal or longer than 4
			"CESET", "CERESET", "NOPP", "STATUS", "CONFIG", "I2CTEST", "EEMM" }; // once you've added a new command , change increase loop counter
	xUSARTMessage usartMessage;
	for (;;) {

		while (xQueueReceive(xUsartQueueHandle,&usartMessage,portMAX_DELAY)
				!= pdPASS)
			;

		for (uint8_t i = 0; i < 12; i++) {

			if (strncmp(commands[i], usartMessage.uiMessage, 4) == 0) {
				char tempBuf[2];
				switch (i) {
				case 0:
					GPIOC->ODR ^= (GPIO_ODR_9);
					break;
				case 1:
					GPIOC->ODR ^= (GPIO_ODR_8);
					break;

				case 2:
					GPIOC->ODR ^= (GPIO_ODR_9);
					GPIOC->ODR ^= (GPIO_ODR_8);
					break;
				case 3:
					nRF24L01_PowerUp();
					break;
				case 4:
					nRF24L01_PowerDown();
					break;
				case 5:
					nRF24L01_SetCE_PIN();
					break;
				case 6:
					nRF24L01_ResetCE_PIN();
					break;
				case 7:
					nRF24L01_NOP();
					break;
				case 8:
					nRF24L01_ReadSTATUS();
					break;
				case 9:
					nRF24L01_ReadCONFIGReg();
					break;
				case 10:
					I2C_TestMethod();
					break;
				case 11:
					strncpy(tempBuf, usartMessage.uiMessage[4], 2);
					int addr = 0;
					addr = atoi(tempBuf);
					displayDataOfEEpromMemory(addr);
					break;
				default:
					break;
				}
			}
		}
	}
}

/**
 * @name void vStartUSARTTask(unsigned portBASE_TYPE uxPriority)
 * @brief
 * @retval
 */
void vStartUSARTTask(unsigned portBASE_TYPE uxPriority) {
	xTaskHandle xHandleTaskUSART;
	xTaskCreate(vTaskUSART1, (signed const char*)"USART1", 300, NULL,
			uxPriority, &xHandleTaskUSART);
}

/**
 * @name void vSendDataToUart(char*str)
 * @brief
 * @retval
 */
void vSendDataToUart(char*str) {
	xUSARTMessage TxData;
	TxData.uiLengthOfString = strlen(str); // + 1;
	if (TxData.uiLengthOfString > USART_TXRXBUFFERSIZE)
		return;
	strcpy(TxData.uiMessage, str);
//TxData.uiMessage[TxData.uiLengthOfString] = '\n';
	xQueueReset(xUsartQueueHandle);
	xQueueSend(xUsartQueueHandle, &TxData, pdFALSE);
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);

}

void vSendIntToUart(int data) {

	char buffer[10];
	snprintf(buffer, 6, "%d ", data);
	vSendDataToUart(buffer);
}
////////////////////////////////END OF USART///////////////////////

