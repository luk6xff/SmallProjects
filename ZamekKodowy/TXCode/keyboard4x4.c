#include "keyboard4x4.h"
#include <cmsis_boot/stm32f0xx.h>
#include "stm_lib/inc/stm32f0xx_rcc.h"
#include "stm_lib/inc/stm32f0xx_syscfg.h"
#include "stm_lib/inc/stm32f0xx_tim.h"
#include "stm32_hardware_modules.h"
#include "string.h"

#define TIME_TO_NEXT_BUTTON_PRESS    100//ms
#define TIME_TO_CANCEL_BUTTON_PRESS_SEQUENCE    5000//ms
#define BINARY_SEMAPHORE_LENGTH	1

volatile uint16_t tickCounterForPressingButtonEvent = 0;

xQueueHandle xKeyboardReceivedQueue;
xQueueHandle xReceivedCorrectDataCodeFromKeyboard;
xSemaphoreHandle xTimer3Semphr = NULL;

xQueueSetHandle xKeyboardQueueSet;
xQueueSetMemberHandle xKeyboardQueueSetMemberHandle;

typedef struct {
	uint8_t abcd_input;
	uint8_t efgh_input;
} InputsPressed;

uint8_t codeToSend[4];
// private methods
//leds

typedef enum {
	GREEN_LED, RED_LED
} eKeyboardLeds;

inline static void keyboardLedsConfig(void) {
	GPIOA->MODER |= (GPIO_MODER_MODER10_0 | GPIO_MODER_MODER11_0); //RED PA10, GREEN PA11  OUTPUTs
	GPIOA->PUPDR |= (GPIO_PUPDR_PUPDR10_0 | GPIO_PUPDR_PUPDR11_0); //PULL UP
}

inline static void redLedOn(void) {
	GPIOA->BSRR = GPIO_BSRR_BS_10;
}
inline static void redLedOff(void) {
	GPIOA->BSRR = GPIO_BSRR_BR_10;
}
inline static void greenLedOn(void) {
	GPIOA->BSRR = GPIO_BSRR_BS_11;
}
inline static void greenLedOff(void) {
	GPIOA->BSRR = GPIO_BSRR_BR_11;
}

//timers
static void enableTimer3(void) {
	TIM3->CR1 |= TIM_CR1_CEN; //enable counter
	TIM3->CNT = 0;
	tickCounterForPressingButtonEvent = 0;
	NVIC_SetPriority(TIM3_IRQn, 2);
	NVIC_EnableIRQ(TIM3_IRQn);
}

static void disableTimer3(void) {
	TIM3->CR1 &= ~(TIM_CR1_CEN);
	NVIC_DisableIRQ(TIM3_IRQn);
}

static void Timer3Configuration(void) {
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	TIM3->ARR = 100; //value that counter counts up to :1ms
	TIM3->BDTR = 0;
	TIM3->PSC = 9; // 1ms  //The counter clock frequency (CK_CNT) is equal to fCK_PSC / (PSC[15:0] + 1) // here fCK_PSC =1MHz  -> 100000 Hz
	TIM3->CR1 = TIM_CR1_ARPE; // ARPE bufferd , upcounter
	TIM3->DIER = TIM_DIER_UIE; //

}

static void Timer3Init(void) {
	xTimer3Semphr = xSemaphoreCreateBinary();
	configASSERT(xTimer3Semphr);
	Timer3Configuration();
}
// ext interrupts
static void enableInterruptsForInputButtons(void) {
	EXTI->IMR = EXTI_IMR_MR0 | EXTI_IMR_MR1 | EXTI_IMR_MR2 | EXTI_IMR_MR3;
	NVIC_SetPriority(EXTI0_1_IRQn, 1);
	NVIC_EnableIRQ(EXTI0_1_IRQn);
	NVIC_SetPriority(EXTI2_3_IRQn, 1);
	NVIC_EnableIRQ(EXTI2_3_IRQn);
}

static void disableInterruptsForInputButtons(void) {
	EXTI->IMR &= ~(EXTI_IMR_MR0 | EXTI_IMR_MR1 | EXTI_IMR_MR2 | EXTI_IMR_MR3);
	NVIC_DisableIRQ(EXTI0_1_IRQn);
	NVIC_DisableIRQ(EXTI2_3_IRQn);
}

static void setABCDToInputStateDefaultState(void) {

	GPIOA->MODER &= ~(GPIO_MODER_MODER0 | GPIO_MODER_MODER1 | GPIO_MODER_MODER2
			| GPIO_MODER_MODER3); //A,B,C,D - iNPUTS
	GPIOA->PUPDR = (GPIO_PUPDR_PUPDR0_1 | GPIO_PUPDR_PUPDR1_1
			| GPIO_PUPDR_PUPDR2_1 | GPIO_PUPDR_PUPDR3_1); // PULL-DOWN

	GPIOA->MODER |= (GPIO_MODER_MODER4_0 | GPIO_MODER_MODER5_0
			| GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0); //E,F,G,H- OUTPUTS
	GPIOA->PUPDR |= (GPIO_PUPDR_PUPDR4_0 | GPIO_PUPDR_PUPDR5_0
			| GPIO_PUPDR_PUPDR6_0 | GPIO_PUPDR_PUPDR7_0); // PULL-UP
	GPIOA->BSRR = (GPIO_BSRR_BS_4 | GPIO_BSRR_BS_5 | GPIO_BSRR_BS_6
			| GPIO_BSRR_BS_7);

}

static void setEFGHToInputStateWhenInterruptOccurs(void) {

	GPIOA->MODER &= ~(GPIO_MODER_MODER4 | GPIO_MODER_MODER5 | GPIO_MODER_MODER6 //E,F,G,H - INPUT
			| GPIO_MODER_MODER7);
	GPIOA->PUPDR = GPIO_PUPDR_PUPDR4_1 | GPIO_PUPDR_PUPDR5_1
			| GPIO_PUPDR_PUPDR6_1 | GPIO_PUPDR_PUPDR7_1; // PULL-DOWN

	GPIOA->MODER |= (GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0 //A,B,C,D - OUTPUT
			| GPIO_MODER_MODER2_0 | GPIO_MODER_MODER3_0);
	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR0_0 | GPIO_PUPDR_PUPDR1_0
			| GPIO_PUPDR_PUPDR2_0 | GPIO_PUPDR_PUPDR3_0; // PULL-UP

	GPIOA->BSRR = (GPIO_BSRR_BS_0 | GPIO_BSRR_BS_1 | GPIO_BSRR_BS_2
			| GPIO_BSRR_BS_3);
}

static void KeyboardInit(void) {

	xKeyboardQueueSet = xQueueCreateSet(
			sizeof(InputsPressed) + BINARY_SEMAPHORE_LENGTH);
	configASSERT(xKeyboardQueueSet);
	xKeyboardReceivedQueue = xQueueCreate(1,sizeof(InputsPressed));
	configASSERT(xKeyboardReceivedQueue);
	xReceivedCorrectDataCodeFromKeyboard = xQueueCreate(1,sizeof(codeToSend));
	configASSERT( xReceivedCorrectDataCodeFromKeyboard);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2ENR_SYSCFGEN, ENABLE); // Enable SYSCFG clock
	keyboardLedsConfig();
	setABCDToInputStateDefaultState();
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,
			EXTI_PinSource0 | EXTI_PinSource1 | EXTI_PinSource2
					| EXTI_PinSource3);
	EXTI->IMR |= EXTI_IMR_MR0 | EXTI_IMR_MR1 | EXTI_IMR_MR2 | EXTI_IMR_MR3; //Interrupt request from these pins is not masked
	EXTI->RTSR |= EXTI_RTSR_TR0 | EXTI_RTSR_TR1 | EXTI_RTSR_TR2 | EXTI_RTSR_TR3; // Rising trigger edge
	Timer3Init();
	xQueueAddToSet(xKeyboardReceivedQueue, xKeyboardQueueSet);
	xQueueAddToSet(xTimer3Semphr, xKeyboardQueueSet);

	enableInterruptsForInputButtons();

}

static EButtonInputs getInputStateFromEFGH(void) {

	if (GPIOA->IDR & GPIO_IDR_4) {
		return BUTTON_INPUT_E;
	} else if (GPIOA->IDR & GPIO_IDR_5) {
		return BUTTON_INPUT_F;
	} else if (GPIOA->IDR & GPIO_IDR_6) {
		return BUTTON_INPUT_G;
	} else if (GPIOA->IDR & GPIO_IDR_7) {
		return BUTTON_INPUT_H;
	} else {
		return BUTTON_UNKNOWN_ERROR;
	}

}
// INTERRUPTS
void EXTI0_1_IRQHandler() {
	disableInterruptsForInputButtons();
	disableTimer3();
	enableTimer3(); //after first number was received , fire up the counter that counts to
	InputsPressed currentInputsPressed = { 255, 255 };
	setEFGHToInputStateWhenInterruptOccurs();
	if (EXTI->PR & EXTI_PR_PR0) {
		EXTI->PR |= EXTI_PR_PR0; // clear flag
		currentInputsPressed.abcd_input = BUTTON_INPUT_A;
	} else if (EXTI->PR & EXTI_PR_PR1) {
		EXTI->PR |= EXTI_PR_PR1; //clear flag
		currentInputsPressed.abcd_input = BUTTON_INPUT_B;
	} else
		currentInputsPressed.abcd_input = BUTTON_UNKNOWN_ERROR;
	currentInputsPressed.efgh_input = getInputStateFromEFGH();
	if (currentInputsPressed.efgh_input != BUTTON_UNKNOWN_ERROR
			&& currentInputsPressed.abcd_input != BUTTON_UNKNOWN_ERROR) {
		greenLedOn();
		xQueueSendToBackFromISR(xKeyboardReceivedQueue, &currentInputsPressed,
				0);

	}
	setABCDToInputStateDefaultState();
}

void EXTI2_3_IRQHandler() {
	disableInterruptsForInputButtons();
	disableTimer3();
	enableTimer3(); //after first number was received , fire up the counter that counts to
	InputsPressed currentInputsPressed = { 255, 255 };
	setEFGHToInputStateWhenInterruptOccurs();
	if (EXTI->PR & EXTI_PR_PR2) {
		EXTI->PR |= EXTI_PR_PR2; // clear flag
		currentInputsPressed.abcd_input = BUTTON_INPUT_C;
	} else if (EXTI->PR & EXTI_PR_PR3) {
		EXTI->PR |= EXTI_PR_PR3; //clear flag
		currentInputsPressed.abcd_input = BUTTON_INPUT_D;
	} else
		currentInputsPressed.abcd_input = BUTTON_UNKNOWN_ERROR;
	currentInputsPressed.efgh_input = getInputStateFromEFGH();
	if (currentInputsPressed.efgh_input != BUTTON_UNKNOWN_ERROR
			&& currentInputsPressed.abcd_input != BUTTON_UNKNOWN_ERROR) {
		greenLedOn();
		xQueueSendToBackFromISR(xKeyboardReceivedQueue, &currentInputsPressed,
				0);

	}
	setABCDToInputStateDefaultState();
}

void TIM3_IRQHandler(void) {
	if (TIM3->SR & TIM_SR_UIF) {
		TIM3->SR &= ~(TIM_SR_UIF);
		tickCounterForPressingButtonEvent++;
		if (tickCounterForPressingButtonEvent == TIME_TO_NEXT_BUTTON_PRESS) {
			//DEBUG("=");
			//DEBUG
			enableInterruptsForInputButtons();
		} else if (tickCounterForPressingButtonEvent
				== TIME_TO_CANCEL_BUTTON_PRESS_SEQUENCE) {
			xSemaphoreGiveFromISR(xTimer3Semphr, pdFALSE);
			disableTimer3();
		}
	}
}

static void blinkGivenLed(eKeyboardLeds ledColour, uint8_t nrOfBlinks,
		uint16_t delayBetweenBlinks_ms) {

	void (*ledOnCallback)(void);
	void (*ledOffCallback)(void);
	switch (ledColour) {
	case GREEN_LED:
		ledOnCallback = greenLedOn;
		ledOffCallback = greenLedOff;
		break;
	case RED_LED:
		ledOnCallback = redLedOn;
		ledOffCallback = redLedOff;
		break;
	default:
		return;
	}
	for (uint8_t i = 0; i < nrOfBlinks; i++) {
		(*ledOnCallback)();
		vTaskDelay(delayBetweenBlinks_ms / portTICK_RATE_MS);
		(*ledOffCallback)();
		vTaskDelay(delayBetweenBlinks_ms / portTICK_RATE_MS);
	}
}

static void vTaskKeyboard(void* pvParameters) {

	KeyboardInit();
	InputsPressed receivedInputState;
	const uint8_t nrOfDatatowaitFor = 4;
	uint8_t receivedDataBuffer[nrOfDatatowaitFor];
	static uint8_t numberOfReceivedFrames;
	portTickType xLastFlashTime;
	xLastFlashTime = xTaskGetTickCount();
	for (;;) {
		xKeyboardQueueSetMemberHandle = xQueueSelectFromSet(xKeyboardQueueSet,
				10 / portTICK_RATE_MS);

		if (xKeyboardQueueSetMemberHandle == xTimer3Semphr) {
			//	DEBUG("*");
			if (xTimer3Semphr != NULL) {
				if (xSemaphoreTake(xTimer3Semphr,(portTickType)0)) {
					numberOfReceivedFrames = 0;
					blinkGivenLed(GREEN_LED, 3, 100);
					memset(receivedDataBuffer, 0, sizeof(receivedDataBuffer));
				}
			}
		} else if (xKeyboardQueueSetMemberHandle == xKeyboardReceivedQueue) {
			xQueueReceive(xKeyboardReceivedQueue, &receivedInputState, 0);
			//GPIOC->ODR ^= (GPIO_ODR_9); //DEBUG
			receivedDataBuffer[numberOfReceivedFrames] =
					(receivedInputState.abcd_input
							+ receivedInputState.efgh_input);
			greenLedOff();
			numberOfReceivedFrames++;
			if (numberOfReceivedFrames == nrOfDatatowaitFor) {
				numberOfReceivedFrames = 0;
				//here checking if in eeprom such value exists
				blinkGivenLed(RED_LED, 3, 300);
				//memcpy(codeToSend, receivedDataBuffer,
				//		sizeof(nrOfDatatowaitFor));
				//codeToSend[nrOfDatatowaitFor] = '\0';
				//	if(strcmp(codeToSend,))
			}
		} else { /*do nothing just wait 10ms  for any data*/
		}

	}

}

void vStartKeyboardTask(unsigned portBASE_TYPE uxPriority) {
	xTaskCreate(vTaskKeyboard, (signed const char*)"KEY", 160, NULL, uxPriority,
			NULL);

}

/*switch (receivedDataBuffer[numberOfReceivedFrames]) { //FOR DEBUG
 case BUTTON_0:
 //DEBUG("0");
 break;
 case BUTTON_1:
 //DEBUG("1");
 break;
 case BUTTON_2:
 //DEBUG("2");
 break;
 case BUTTON_3:
 //DEBUG("3");
 break;
 case BUTTON_4:
 //DEBUG("4");
 break;
 case BUTTON_5:
 //DEBUG("5");
 break;
 case BUTTON_6:
 //DEBUG("6");
 break;
 case BUTTON_7:
 //DEBUG("7");
 break;
 case BUTTON_8:
 //DEBUG("8");
 break;
 case BUTTON_9:
 //DEBUG("9");
 break;
 case BUTTON_P:
 //DEBUG("P");
 break;
 case BUTTON_M:
 //DEBUG("M");
 break;
 case BUTTON_ESC:
 //DEBUG("ES");
 break;
 case BUTTON_ARROW_UP:
 //DEBUG("AU");
 break;
 case BUTTON_ARROW_DOWN:
 //DEBUG("AD");
 break;
 default:
 DEBUG("GOW");
 break;

 }
 */
