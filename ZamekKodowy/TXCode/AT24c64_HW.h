/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AT24C64_HW_H
#define __AT24C64_HW_H

#ifdef __cplusplus
extern "C" {
#endif

/*I2C HARDWARE LAYER*/
#include "stm_lib/inc/stm32f0xx_i2c.h"
#include "stm_lib/inc/stm32f0xx_gpio.h"


#define AT24C64_I2C                          I2C2
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






void AT24C64_DriverDeInit(void);
void AT24C64_DriverInit(void);

















#ifdef __cplusplus
}
#endif

#endif /*__AT24C64_HW_H */
