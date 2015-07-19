/* CLOCK - set in system_stm32f0xx.c file -> HSI-> to 1MHz */

#include "FreeRtosFiles/FreeRTOS.h"
#include "FreeRtosFiles/task.h"
#include "FreeRtosFiles/queue.h"
#include "cmsis_boot/stm32f0xx.h"
#include "nRF24L01+.h"
#include "stm_lib/inc/stm32f0xx_spi.h"
#include "stm_lib/inc/stm32f0xx_rcc.h"
#include "stm32_hardware_modules.h"

static void vSetupHardware(void);
static void vTaskGreen(void *pvParameters);
static void vTaskBlue(void *pvParameters);
static void vStartLEDTasks(unsigned portBASE_TYPE uxPriority);
static void vStartSpecialCommandsTask(unsigned portBASE_TYPE uxPriority);
xTaskHandle* xHandleTaskForStackOverlowDetection, xHandleLEDGREEN,
		xHandleLEDBLUE;
xTaskHandle xHandleTaskWaitForSpecialCommands=NULL;
signed char *pcTaskForStackOverlowDetection;

int main(void) {

	//vSetupHardware();
	//vStartUSARTTask(2); // think it overz
	//vStartLEDTasks(1);
	//vStartSPITask(3);
	vStartSpecialCommandsTask(3);
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
/*
 not used - the green led is only turns on/off when the IRQ event happens
 void vTaskGreen(void *pvParameters) {
 portTickType xLastFlashTime;
 xLastFlashTime = xTaskGetTickCount();
 uint16_t x;
 for (;;) {
 vTaskDelayUntil(&xLastFlashTime, 300 / portTICK_RATE_MS);
 GPIOC->ODR ^= (GPIO_ODR_9);

 }
 }
 */
void vTaskBlue(void *pvParameters) {
	//nRF24L01_Init(); //TODO CHANGE THE LOCATION OF IT

	//DEBUG("TASK_BL"); //FOR DEBUG
	portTickType xLastFlashTime;
	xLastFlashTime = xTaskGetTickCount();
	for (;;) {
		vTaskDelayUntil(&xLastFlashTime, 1000 / portTICK_RATE_MS);
		/*DEBUG("BLUE_TASK");*/
		GPIOA->ODR ^= GPIO_ODR_10;

	}
}

void vStartLEDTasks(unsigned portBASE_TYPE uxPriority) {

	xTaskHandle xHandleTaskLedGreen, xHandleTaskLedBlue;
	xHandleLEDBLUE = &xHandleTaskLedBlue;
	xHandleLEDGREEN = &xHandleTaskLedGreen;
	xTaskCreate(vTaskBlue, (signed const char*)"LED_BLUE", 150, NULL,
			uxPriority, &xHandleTaskLedBlue);
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

static void vTaskWaitForSpecialCommands(void *pvParameters) {

	portTickType xLastFlashTime;
	xLastFlashTime = xTaskGetTickCount();
    uint16_t counter=0;
	for (;;) {
		vTaskDelayUntil(&xLastFlashTime, 100 / portTICK_RATE_MS);
		GPIOA->ODR ^= GPIO_ODR_10;
		if (++counter > 100)
			vTaskDelete(xHandleTaskWaitForSpecialCommands);
	}
}

static void vStartSpecialCommandsTask(unsigned portBASE_TYPE uxPriority) {
	vSetupHardware();
		vStartUSARTTask(2); // think it overz
		vStartLEDTasks(1);
		vStartSPITask(3);
	xTaskCreate(vTaskWaitForSpecialCommands,
			(signed const char*)"SPECIAL_COMMANDS", 80, NULL, uxPriority,
			&xHandleTaskWaitForSpecialCommands);
	//DISABLE FOR TESTS

}

