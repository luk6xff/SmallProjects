/* CLOCK - set in system_stm32f0xx.c file -> HSI-> to 1MHz */

#include "FreeRtosFiles/FreeRTOS.h"
#include "FreeRtosFiles/task.h"
#include "FreeRtosFiles/queue.h"
#include "cmsis_boot/stm32f0xx.h"
#include "nRF24L01+.h"
#include "stm_lib/inc/stm32f0xx_spi.h"
#include "stm_lib/inc/stm32f0xx_rcc.h"
#include "stm32_hardware_modules.h"
#include "keyboard4x4.h"


void vSetupHardware(void);
void vTaskGreen(void *pvParameters);
void vTaskBlue(void *pvParameters);
void vStartLEDTasks(unsigned portBASE_TYPE uxPriority);
xTaskHandle* xHandleTaskForStackOverlowDetection, xHandleLEDGREEN,
		xHandleLEDBLUE;
signed char *pcTaskForStackOverlowDetection;

/*
 uint8_t SPI2_send(uint8_t data){
 SPI2->DR = data; // write data to be transmitted to the SPI data register
 while( !(SPI2->SR & SPI_I2S_FLAG_TXE) ); // wait until transmit complete
 while( !(SPI2->SR & SPI_I2S_FLAG_RXNE) ); // wait until receive complete
 while( SPI2->SR & SPI_I2S_FLAG_BSY ); // wait until SPI is not busy anymore
 while( !(SPI2->SR & SPI_I2S_FLAG_RXNE) ); // wait until receive complete
 while( SPI2->SR & SPI_I2S_FLAG_BSY ); // wait until SPI is not busy anymore
 return SPI2->DR; // return received data from SPI data register
 }
 */
const uint8_t testData[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
int main(void) {
	/*SystemInit();*/ // it's done in startup file

	vSetupHardware();
	vStartKeyboardTask(3);
	vStartUSARTTask(2); // think it over
	vStartSPITask(3);
	vStartLEDTasks(1);
	vTaskStartScheduler();
	return 0;
}

void vSetupHardware(void) {
	RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOCEN, ENABLE);
	GPIOC->MODER = GPIO_MODER_MODER8_0 | GPIO_MODER_MODER9_0;
	GPIOC->PUPDR = GPIO_PUPDR_PUPDR8_0 | GPIO_PUPDR_PUPDR9_0;
	GPIOC->BSRR = GPIO_BSRR_BS_8;
	GPIOC->BSRR = GPIO_BSRR_BS_9;
}

void vTaskGreen(void *pvParameters) {
	portTickType xLastFlashTime;
	xLastFlashTime = xTaskGetTickCount();
	uint16_t x;
	for (;;) {
		/*vTaskDelay(500 / portTICK_RATE_MS);*/
		vTaskDelayUntil(&xLastFlashTime, 4000 / portTICK_RATE_MS);
		GPIOC->ODR ^= (GPIO_ODR_9);

	}
}

void vTaskBlue(void *pvParameters) {
	nRF24L01_Init();

	//DEBUG("TASK_BL"); //FOR DEBUG
	portTickType xLastFlashTime;
	xLastFlashTime = xTaskGetTickCount();
	for (;;) {
		vTaskDelayUntil(&xLastFlashTime, 2000 / portTICK_RATE_MS);
		GPIOC->ODR ^= (GPIO_ODR_8);
		//DEBUG("(->>>>)");
		nRF24L01_SendDataToFifo((uint8_t*) testData);
		nRF24L01_FlushTx();
		nRF24L01_FlushRx();
		nRF24L01_FlushSTATUSReg();
	}
}

void vStartLEDTasks(unsigned portBASE_TYPE uxPriority) {

	xTaskHandle xHandleTaskLedGreen, xHandleTaskLedBlue;
	xHandleLEDBLUE = &xHandleTaskLedBlue;
	xHandleLEDGREEN = &xHandleTaskLedGreen;
	/*	xTaskCreate(vTaskGreen, (signed const char*)"LED_GREEN", 80, NULL,
	 uxPriority, &xHandleTaskLedGreen);
	 */
	xTaskCreate(vTaskBlue, (signed const char*)"LED_BLUE", 80, NULL, uxPriority,
			&xHandleTaskLedBlue);
	//DISABLE FOR TESTS

}

/*
 * @brief Callback which is invoked when stack overlows
 * @param xTaskHandle  and the name of the task which caused stack overflowed
 * @return NONE
 */
void vApplicationStackOverflowHook(xTaskHandle *pxTask, signed char *pcTaskName) {
	xHandleTaskForStackOverlowDetection = pxTask; // this seems to give me the crashed task handle
	pcTaskForStackOverlowDetection = pcTaskName; // this seems to give me a pointer to the name of the crashed task

	if (pxTask == xHandleLEDGREEN) {
		GPIOC->ODR &= ~(GPIO_ODR_8);
		GPIOC->ODR |= (GPIO_ODR_9);
	} else if (pxTask == xHandleLEDBLUE) {
		GPIOC->ODR &= ~(GPIO_ODR_9);
		GPIOC->ODR |= (GPIO_ODR_8);
	} else {
		GPIOC->ODR &= ~(GPIO_ODR_9 | GPIO_ODR_8);

	}
	for (;;)
		;
}

