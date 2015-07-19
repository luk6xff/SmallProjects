/**
 * Author Lukasz Uszko 02.07.2014
 *
 *
 *
 */




/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32_HARDWARE_MODULES_H
#define __STM32_HARDWARE_MODULES_H

#ifdef __cplusplus
extern "C" {
#endif
#include <cmsis_boot/stm32f0xx.h>
#include "stdbool.h"
#include "FreeRtosFiles/FreeRTOS.h"
#include "FreeRtosFiles/queue.h"
#include "FreeRtosFiles/task.h"

/*DEBUGER MACROs*/
#define DEBUG(x) vSendDataToUart(x)


/*PORTS*/
#define CE_PORT_FOR_nRF24L01 GPIOC
#define CE_PIN(x,y)  x##6##y

#define CS_PORT_SPI2  GPIOB
#define CS_PIN_SPI2(x,y)  x##12##y


#define IRQ_PORT_FOR_nRF24L01 GPIOC
#define IRQ_PIN(x,y)  x##7##y       //when you're gonna change it , make sure that correct External Interrupt is set
#define IRQ_PIN_Number 7
#define EXTI_PortSourceForIRQ EXTI_PortSourceGPIOC
#define EXTI_PinSourceForIRQ  EXTI_PinSource7
/*SPI */

void SPI2WriteDataViaInterrupt(uint8_t*data); //TODO change to static then and remove
void  SPI2ReadDataViaInterrupt(uint8_t*data);
void SetIRQPinFornRF24L01(void);
void vStartSPITask(unsigned portBASE_TYPE uxPriority);

void SPI2_For_NRF24L01_init(void);

bool SPI_GetFlagStatus(SPI_TypeDef* SPIx, uint16_t SPI_SR_XXX);
/**
 *
 * @brief
 * @retval
 *
 */
static inline void SPI2_TxInterruptEnable(void) {

	SPI2->CR2 |= SPI_CR2_TXEIE;

}
/**
 *
 * @brief
 * @retval
 *
 */
static inline void SPI2_TxInterruptDisable(void) {
	SPI2->CR2 &= ~(SPI_CR2_TXEIE);
}
/**
 *
 * @brief
 * @retval
 *
 */
static inline void SPI2_RxInterruptEnable(void) {

	SPI2->CR2 |= SPI_CR2_RXNEIE;

}
/**
 *
 * @brief
 * @retval
 *
 */
static inline void SPI2_RxInterruptDisable(void) {
	SPI2->CR2 &= ~(SPI_CR2_RXNEIE);
}


/**
 * @name uint16_t SPI2_ReceiveByte(void)
 * @brief
 * @retval
 *
 */
static inline uint8_t SPI2_ReceiveByte(void) {
	return (uint8_t)SPI2->DR;
}

/**
 * @name void SPI2_SendByte(uint16_t*data)
 * @brief
 * @retval
 *
 */
static inline void  SPI2_SendByte(uint8_t*data) {
	SPI2->DR = *data;
}

/*USART1*/
/**
 *
 * @brief
 * @retval
 *
 */
void vStartUSARTTask(unsigned portBASE_TYPE uxPriority);
/**
 *
 * @brief
 * @retval
 *
 */
void vSendDataToUart(char*str);
void vSendIntToUart(int data);

/*GPIOs*/
/**
 *
 * @brief
 * @retval
 *
 */
static inline void CS_PinSPI2_Set(void) {
	 CS_PORT_SPI2->BSRR = CS_PIN_SPI2(GPIO_BSRR_BS_,);
}
/**
 *
 * @brief
 * @retval
 *
 */
static inline void CS_PinSPI2_Reset(void) {
	 CS_PORT_SPI2->BRR = CS_PIN_SPI2(GPIO_BRR_BR_,);
}

/**
 *
 * @brief
 * @retval
 *
 */
static inline void CE_PinForNRF24L01_Set(void) {
	CE_PORT_FOR_nRF24L01->BSRR = CE_PIN(GPIO_BSRR_BS_,);

}
/**
 *
 * @brief
 * @retval
 *
 */
static inline void CE_PinForNRF24L01_Reset(void) {
	CE_PORT_FOR_nRF24L01->BRR = CE_PIN(GPIO_BRR_BR_,);
}



/**
 *
 * @brief
 * @retval
 *
 */
static inline void SetEnabledNRF24L01(void) {
	GPIOC->MODER |= GPIO_MODER_MODER8_0;
	GPIOC->PUPDR |= GPIO_PUPDR_PUPDR8_1; //PULL_Down
	GPIOC->BSRR = GPIO_BSRR_BR_8;
	/*GPIOC->BSRR = GPIO_BSRR_BS_8;*/
}


/**
 *
 * @brief
 * @retval
 *
 */
static inline void SetDisabledNRF24L01(void) {
	GPIOC->BSRR = GPIO_BSRR_BS_8;
}


#ifdef __cplusplus
}
#endif

#endif /*__STM32_HARDWARE_MODULES_H */
