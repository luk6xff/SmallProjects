#include "AT24c64_HW.h"
#include "stm_lib/inc/stm32f0xx_rcc.h"
#include "stm_lib/inc/stm32f0xx_i2c.h"




/**
  * @brief  DeInitializes peripherals used by the I2C EEPROM driver.
  * @param  None
  * @retval None
  */
void AT24C64_I2cDriverDeInit(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  I2C_Cmd(AT24C64_I2C, DISABLE);
  I2C_DeInit(AT24C64_I2C);
  RCC_APB1PeriphClockCmd(AT24C64_I2C_CLK, DISABLE);


  GPIO_InitStructure.GPIO_Pin = AT24C64_I2C_SCL_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(AT24C64_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = AT24C64_I2C_SDA_PIN;
  GPIO_Init(AT24C64_I2C_SDA_GPIO_PORT, &GPIO_InitStructure);
}


void AT24C64_I2cDriverInit(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_I2CCLKConfig(RCC_I2C1CLK_HSI);

  RCC_AHBPeriphClockCmd(AT24C64_I2C_SCL_GPIO_CLK | AT24C64_I2C_SDA_GPIO_CLK, ENABLE);

  RCC_APB1PeriphClockCmd(AT24C64_I2C_CLK, ENABLE);

  GPIO_PinAFConfig(AT24C64_I2C_SCL_GPIO_PORT, AT24C64_I2C_SCL_SOURCE, AT24C64_I2C_SCL_AF);

  GPIO_PinAFConfig(AT24C64_I2C_SDA_GPIO_PORT, AT24C64_I2C_SDA_SOURCE, AT24C64_I2C_SDA_AF);

  GPIO_InitStructure.GPIO_Pin = AT24C64_I2C_SCL_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_1;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_Init( AT24C64_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin =  AT24C64_I2C_SDA_PIN;
  GPIO_Init( AT24C64_I2C_SDA_GPIO_PORT, &GPIO_InitStructure);
}
