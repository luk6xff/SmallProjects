/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AT24C64_HW_H
#define __AT24C64_HW_H

#ifdef __cplusplus
extern "C" {
#endif

/*I2C HARDWARE LAYER*/
#include "stm_lib/inc/stm32f0xx_i2c.h"
#include "stm_lib/inc/stm32f0xx_gpio.h"


#define AT24C64_I2C     I2C2



void AT24C64_HwDriverDeInit(void);
void AT24C64_HwDriverInit(void);




#ifdef __cplusplus
}
#endif

#endif /*__AT24C64_HW_H */
