/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AT24C64_H
#define __AT24C64_H

#ifdef __cplusplus
extern "C" {
#endif

/*I2C HARDWARE LAYER*/

#include "AT24c64_HW.h"
#define AT24_PAGE_SIZE PAGE_SIZE

#define AT_DELAY 20

#define AT24C64_ADDRESS ((0xA0)) /* |1|0|1|0|A2|A1|A0|R/W| */
#define AT24C64_PAGE_SIZE 32 /*bytes*/
#define AT24C64_NR_OF_PAGES 256

#define AT24C64_OK                    0
#define AT24C64_FAIL                  1

#define AT24C64_I2C_TIMING       0x00100508//   0x00210507  // 0x00000103


/* Maximum Timeout values for flags and events waiting loops. These timeouts are
   not based on accurate values, they just guarantee that the application will
   not remain stuck if the I2C communication is corrupted.
   You may modify these timeout values depending on CPU frequency and application
   conditions (interrupts routines ...). */
#define AT24C64_FLAG_TIMEOUT         ((uint32_t)0x8000)
#define AT24C64_LONG_TIMEOUT         ((uint32_t)(4 * AT24C64_FLAG_TIMEOUT))

/* Maximum number of trials for sEE_WaitEepromStandbyState() function */
#define AT24C64_MAX_TRIALS_NUMBER     300





void AT24C64_DeInit(void);
void AT24C64_Init(void);
uint32_t AT24C64_ReadBuffer(uint8_t* pBuffer, uint16_t ReadAddr,
		uint16_t* NumByteToRead);
uint32_t AT24C64_WritePage(uint8_t* pBuffer, uint16_t WriteAddr,
		uint8_t* NumByteToWrite);
void AT24C64_WriteBuffer(uint8_t* pBuffer, uint16_t WriteAddr,
		uint16_t NumByteToWrite);
uint8_t I2C_TestMethod(void);

uint32_t AT24C64_WaitEepromStandbyState(void);
uint32_t AT24C64_TIMEOUT_UserCallback(void);
void displayDataOfEEpromMemory(uint16_t addressToRead);

#ifdef __cplusplus
}
#endif

#endif /*__AT24C64_H */
