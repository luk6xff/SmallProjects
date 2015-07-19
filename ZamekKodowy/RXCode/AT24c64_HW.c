#include "AT24c64_HW.h"
#include "stm_lib/inc/stm32f0xx_rcc.h"
#include "stm_lib/inc/stm32f0xx_i2c.h"



#define AT24C64_I2C_CLK                      RCC_APB1Periph_I2C2

#define AT24C64_I2C_SCL_PIN                  GPIO_Pin_6                  /* PF.06 */
#define AT24C64_I2C_SCL_GPIO_PORT            GPIOF                       /* GPIOF */
#define AT24C64_I2C_SCL_GPIO_CLK             RCC_AHBPeriph_GPIOF
#define AT24C64_I2C_SCL_SOURCE               GPIO_PinSource6
#define AT24C64_I2C_SCL_AF                   GPIO_AF_0

#define AT24C64_I2C_SDA_PIN                  GPIO_Pin_7                  /* PF.07 */
#define AT24C64_I2C_SDA_GPIO_PORT            GPIOF                       /* GPIOF */
#define AT24C64_I2C_SDA_GPIO_CLK             RCC_AHBPeriph_GPIOF
#define AT24C64_I2C_SDA_SOURCE               GPIO_PinSource7
#define AT24C64_I2C_SDA_AF                   GPIO_AF_0

#define AT24C64_WP_PIN                       GPIO_Pin_12                 /* PA.12 WRITE Protection Pin ->VCC-DISABLE ,GND-ENABLE*/
#define AT24C64_WP_PORT           			 GPIOA                       /* GPIOA */
#define AT24C64_WP_GPIO_CLK            		 RCC_AHBPeriph_GPIOA
#define AT24C64_WP_SOURCE               	 GPIO_PinSource12
#define AT24C64_WP_GPIO_MODER                GPIO_MODER_MODER12_0
#define AT24C64_WP_GPIO_PUPDR                GPIO_PUPDR_PUPDR12_0
#define AT24C64_WP_GPIO_BSRR_SET             GPIO_BSRR_BS_12
#define AT24C64_WP_GPIO_BSRR_RESET           GPIO_BSRR_BR_12





static inline void AT24C64_WriteProtection_ENABLE(void) {

	RCC_AHBPeriphClockCmd(AT24C64_WP_GPIO_CLK, ENABLE);
	AT24C64_WP_PORT->MODER |= AT24C64_WP_GPIO_MODER;
	AT24C64_WP_PORT->PUPDR |= AT24C64_WP_GPIO_PUPDR;

	AT24C64_WP_PORT->BSRR = AT24C64_WP_GPIO_BSRR_RESET;

}

static inline void AT24C64_WriteProtection_DISABLE(void) {
	AT24C64_WP_PORT->BSRR = AT24C64_WP_GPIO_BSRR_SET;
}

/**
 * @brief  DeInitializes peripherals used by the I2C EEPROM driver.
 * @param  None
 * @retval None
 */
void AT24C64_HwDriverDeInit(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_Cmd(AT24C64_I2C, DISABLE);
	I2C_DeInit(AT24C64_I2C);
	RCC_APB1PeriphClockCmd(AT24C64_I2C_CLK, DISABLE);

	GPIO_InitStructure.GPIO_Pin = AT24C64_I2C_SCL_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(AT24C64_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = AT24C64_I2C_SDA_PIN;
	GPIO_Init(AT24C64_I2C_SDA_GPIO_PORT, &GPIO_InitStructure);
	//AT24C64_WriteProtection_DISABLE();
}

void AT24C64_HwDriverInit(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_I2CCLKConfig(RCC_I2C1CLK_HSI);

	RCC_AHBPeriphClockCmd(AT24C64_I2C_SCL_GPIO_CLK | AT24C64_I2C_SDA_GPIO_CLK,
			ENABLE);

	RCC_APB1PeriphClockCmd(AT24C64_I2C_CLK, ENABLE);

	GPIO_PinAFConfig(AT24C64_I2C_SCL_GPIO_PORT, AT24C64_I2C_SCL_SOURCE,
			AT24C64_I2C_SCL_AF);

	GPIO_PinAFConfig(AT24C64_I2C_SDA_GPIO_PORT, AT24C64_I2C_SDA_SOURCE,
			AT24C64_I2C_SDA_AF);

	GPIO_InitStructure.GPIO_Pin = AT24C64_I2C_SCL_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_1;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_Init(AT24C64_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = AT24C64_I2C_SDA_PIN;
	GPIO_Init(AT24C64_I2C_SDA_GPIO_PORT, &GPIO_InitStructure);
	AT24C64_WriteProtection_ENABLE();
}
