/**
  ******************************************************************************
  * @file    08_WaitMode/main.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    05-March-2014
  * @brief   This code example shows how to configure the ADC to launch manually  
  *          a continuous conversion using the wait mode. 
  *
 ===============================================================================
                    #####       MCU Resources     #####
 ===============================================================================
   - RCC
   - ADC on PA1, PB1, PC0 and VREFINT
   - HSI 14 MHz for ADC
   - GPIO PA0, PC8 and PC9
   - EXTI0
   - Low power (WFI)

 ===============================================================================
                    ##### How to use this example #####
 ===============================================================================
    - this file must be inserted in a project containing  the following files :
      o system_stm32f0xx.c, startup_stm32f0xx.s
      o stm32f0xx.h to get the register definitions
      o CMSIS files
 
 ===============================================================================
                    ##### How to test this example #####
 ===============================================================================
    - This example configures the ADC to convert 4 channels (CH1, 9, 10 and 17)
    - The code launches a continuous conversion.
    - The overrun interrupt is set.
    - The ADC data register is only read while the USER push-button is pressed.
    - The green is switched on till an error occurs.
    - In case of failure, overrun particularly, the ADC is disabled 
      and the orange led blinks coding the error type.
    - The feature can be checked by watching ADC_array in live. 
      The values are only modified while the USER push-button is pressed. 
      Sometimes few data can be modified due to the mechanical rebounds generating 
      successive front edges.
  *    
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"

/** @addtogroup STM32F0_Snippets
  * @{
  */



/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Delay value : short one is used for the error coding, long one (~1s) in case 
   of no error or between two bursts */
#define SHORT_DELAY 200
#define LONG_DELAY 2000

/* Error codes used to make the orange led blinking */
#define ERROR_OVERRUN_ADC_IT 0x01
#define ERROR_ADC_DISABLED 0x02
#define ERROR_UNEXPECTED_ADC_IT 0x04
#define ERROR_UNEXPECTED_EXT_IT 0x08

#define NUMBER_OF_ADC_CHANNEL 4

/* Internal voltage reference calibration value address */
#define VREFINT_CAL_ADDR ((uint16_t*) ((uint32_t) 0x1FFFF7BA))

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
volatile uint16_t error = 0xFF;  //initialized at 0 and modified by the functions 

volatile uint16_t ADC_array[NUMBER_OF_ADC_CHANNEL]; //Array to store the values coming from the ADC and copied by DMA
uint32_t CurrentChannel; //index on the ADC_array
/* Private function prototypes -----------------------------------------------*/
void ConfigureGPIO(void);
void ConfigureExternalIT(void);
void SetClockForADC(void);
void CalibrateADC(void);
void ConfigureADC(void);
void ConfigureGPIOforADC(void);
void EnableADC(void);
void DisableADC(void);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f0xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f0xx.c file
  
     PLL is set to x12 with  a PREDIV /2 so the system clock SYSCLK = 48MHz
     */

  ConfigureGPIO();
  ConfigureExternalIT();
  SetClockForADC();
  CalibrateADC();
  ConfigureGPIOforADC();
  EnableADC();
  ConfigureADC();
  CurrentChannel = 0; /* Initializes the CurrentChannel */
  ADC1->CR |= ADC_CR_ADSTART; /* start the ADC conversions */
  GPIOC->BSRR = (1<<9); /* Switch on green led on PC9  */  
  while (error == 0xFF) /* loop should never be exited */
  {
    __WFI();
  }  
  DisableADC();
  SysTick_Config(3000);/* 1ms config */
  while (1) /* Infinite loop */
  {
  }
}


/**
  * @brief  This function enables the peripheral clocks on GPIO port C,
  *         configures GPIO PC9 in output mode for the Green LED pin,
  *         configures GPIO PC8 in output mode for the orange LED pin,
  * @param  None
  * @retval None
  */
__INLINE void  ConfigureGPIO(void)
{  
  /* (1) Enable the peripheral clock of GPIOC */
  /* (2) Select output mode (01) on GPIOC pin 8 and 9 */
  RCC->AHBENR |= RCC_AHBENR_GPIOCEN; /* (1) */  
  GPIOC->MODER = (GPIOC->MODER & ~(GPIO_MODER_MODER8|GPIO_MODER_MODER9)) \
               | (GPIO_MODER_MODER8_0|GPIO_MODER_MODER9_0); /* (2) */  
}


/**
  * @brief  This function enables the peripheral clocks on GPIO ports A,B,C
  *         configures PA1, PB1 and PC0 in Analog mode.
  *         For portability, some GPIO are again enabled.
  * @param  None
  * @retval None
  */
__INLINE void  ConfigureGPIOforADC(void)
{
  /* (1) Enable the peripheral clock of GPIOA, GPIOB and GPIOC */
  /* (2) Select analog mode for PA1 */
  /* (3) Select analog mode for PB1 */
  /* (4) Select analog mode for PC0 */
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOCEN; /* (1) */
  GPIOA->MODER |= GPIO_MODER_MODER1; /* (2) */
  GPIOB->MODER |= GPIO_MODER_MODER1; /* (3) */
  GPIOC->MODER |= GPIO_MODER_MODER0; /* (4) */ 
}


/**
  * @brief  This function enables the peripheral clocks on GPIO port A,
  *         configures the EXTI register and NVIC IRQ.
  *         PA0 is kept in the default configuration (input,no pull-up, no pull-down)
  *         SYSCFG_EXTICR1 is kept at its reset value to select Port A for EXTI0 
  * @param  None
  * @retval None
  */
__INLINE void  ConfigureExternalIT(void)
{  
  /* (1) Enable the peripheral clock of GPIOA */
  /* (2) Configure the corresponding mask bit in the EXTI_IMR register */ 
  /* (3) Configure the Trigger Selection bits of the Interrupt line on rising edge*/
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN; /* (1) */  
  EXTI->IMR = 0x0001; /* (2) */ 
  EXTI->RTSR = 0x0001; /* (3)*/
  
  /* Configure NVIC for External Interrupt */
  /* (4) Enable Interrupt on EXTI0_1 */
  /* (5) Set priority for EXTI0_1 */
  NVIC_EnableIRQ(EXTI0_1_IRQn); /* (4) */
  NVIC_SetPriority(EXTI0_1_IRQn,1); /* (5) */
}


/**
  * @brief  This function enables the clock in the RCC for the ADC
  *         and set the Core clock (HCLK) prescaler to 16.
  *         The HSI14 will be automatically starts by the ADC.
  * @param  None
  * @retval None
  */
__INLINE void SetClockForADC(void)
{
  /* (1) Enable the peripheral clock of the ADC */
  /* (2) Set Core clock prescaler to /16 so HCLK = PLLCLK/16 = 3MHz
          The HSI14MHz will be selected to feed the ADC */
  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; /* (1) */
  RCC->CFGR |= RCC_CFGR_HPRE_DIV16; /* (2) */
}


/**
  * @brief  This function performs a self-calibration of the ADC
  * @param  None
  * @retval None
  */
__INLINE void  CalibrateADC(void)
{
  /* (1) Ensure that ADEN = 0 */
  /* (2) Clear ADEN */ 
  /* (3) Launch the calibration by setting ADCAL */
  /* (4) Wait until ADCAL=0 */
  if ((ADC1->CR & ADC_CR_ADEN) != 0) /* (1) */
  {
    ADC1->CR &= (uint32_t)(~ADC_CR_ADEN);  /* (2) */  
  }
  ADC1->CR |= ADC_CR_ADCAL; /* (3) */
  while ((ADC1->CR & ADC_CR_ADCAL) != 0) /* (4) */
  {
    /* For robust implementation, add here time-out management */
  }
  __NOP();__NOP();   /* This 2 NOPs are to ensure 2 ADC Cycles 
                        before setting ADEN bit  */
}


/**
  * @brief  This function configures the ADC to convert sequentially 4 channels
            in continuous mode.
  *         The conversion frequency is 14MHz using HSI14
  *         The interrupt on overrun is enabled and the NVIC is configured
  * @param  None
  * @retval None
  */
__INLINE void ConfigureADC(void)
{
  /* (1) Select HSI14 by writing 00 in CKMODE (reset value) */ 
  /* (2) Select the continuous mode and the wait mode */
  /* (3) Select CHSEL1, CHSEL9, CHSEL10 and CHSEL17 */
  /* (4) Select a sampling mode of 111 i.e. 239.5 ADC clk to be greater than 17.1us */
  /* (5) Enable interrupts on overrrun */
  /* (6) Wake-up the VREFINT (only for VBAT, Temp sensor and VRefInt) */
  //ADC1->CFGR2 &= ~ADC_CFGR2_CKMODE; /* (1) */   
  ADC1->CFGR1 |= ADC_CFGR1_CONT | ADC_CFGR1_WAIT; /* (2) */
  ADC1->CHSELR = ADC_CHSELR_CHSEL1 | ADC_CHSELR_CHSEL9 \
               | ADC_CHSELR_CHSEL10 | ADC_CHSELR_CHSEL17; /* (3)*/
  ADC1->SMPR |= ADC_SMPR_SMP_0 | ADC_SMPR_SMP_1 | ADC_SMPR_SMP_2; /* (4) */
  ADC1->IER = ADC_IER_OVRIE; /* (5) */
  ADC->CCR |= ADC_CCR_VREFEN; /* (6) */

  /* Configure NVIC for ADC */
  /* (7) Enable Interrupt on ADC */
  /* (8) Set priority for ADC */
  NVIC_EnableIRQ(ADC1_COMP_IRQn); /* (7) */
  NVIC_SetPriority(ADC1_COMP_IRQn,0); /* (8) */
}


/**
  * @brief  This function enables the ADC
  * @param  None
  * @retval None
  */
__INLINE void EnableADC(void)
{
  /* (1) Enable the ADC */
  /* (2) Wait until ADC ready */
  ADC1->CR |= ADC_CR_ADEN; /* (1) */
  while ((ADC1->ISR & ADC_ISR_ADRDY) == 0) /* (2) */
{
    /* For robust implementation, add here time-out management */
  }  
}


/**
  * @brief  This function disables the ADC
  * @param  None
  * @retval None
  */
__INLINE void DisableADC(void)
{
  /* (1) Ensure that no conversion on going */
  /* (2) Stop any ongoing conversion */
  /* (3) Wait until ADSTP is reset by hardware i.e. conversion is stopped */
  /* (4) Disable the ADC */
  /* (5) Wait until the ADC is fully disabled */
  if ((ADC1->CR & ADC_CR_ADSTART) != 0) /* (1) */
  {
    ADC1->CR |= ADC_CR_ADSTP; /* (2) */
  }
  while ((ADC1->CR & ADC_CR_ADSTP) != 0) /* (3) */
  {
     /* For robust implementation, add here time-out management */
  }
  ADC1->CR |= ADC_CR_ADDIS; /* (4) */
  while ((ADC1->CR & ADC_CR_ADEN) != 0) /* (5) */
  {
    /* For robust implementation, add here time-out management */
  }  
}

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  *         It toggles the green led if the action has been performed correctly
  *         and toggles the orange led coding the error number
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  static uint32_t long_counter = LONG_DELAY;
  static uint32_t short_counter = SHORT_DELAY;  
  static uint16_t error_temp = 0;
  
  if (long_counter-- == 0) 
  {
    if(error == 0)
    {
      /* the following instruction can only be used if no ISR modifies GPIOC ODR
         either by writing directly it or by using GPIOC BSRR or BRR 
         else a toggle mechanism must be implemented using GPIOC BSRR and/or BRR
      */
      GPIOC->ODR ^= (1<<9);//toggle green led on PC9
      long_counter = LONG_DELAY;
    }
    else if (error != 0xFF)
    {
      /* orange led blinks according to the code error value */
      error_temp = (error << 1) - 1;
      short_counter = SHORT_DELAY;
      long_counter = LONG_DELAY << 1;
      GPIOC->BSRR = (1<<8); //set orange led on PC8
      GPIOC->BRR = (1<<9); //switch off green led on PC9
    }
  }
  if (error_temp > 0)
  {
    if (short_counter-- == 0) 
    {
      GPIOC->ODR ^= (1 << 8); //toggle orange led
      short_counter = SHORT_DELAY;
      error_temp--;
    }  
  }
}


/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f0xx.s).                                               */
/******************************************************************************/


/**
  * @brief  This function handles ADC interrupt request.
  *         In case of Overrun, the ADC is stopped but not disabled,
  * @param  None
  * @retval None
  */
void ADC1_COMP_IRQHandler(void)
{
  if ((ADC1->ISR & ADC_ISR_OVR) != 0)  /* Check OVR has triggered the IT */
  {
    GPIOC->BSRR = (1<<8); /* Switch on orange led to report a resume of the conversion  */
    GPIOC->BSRR = (1<<(9+16)); /* Switch off green led to reportit is due to overrun  */
    ADC1->ISR |= ADC_ISR_OVR; /* Clear all pending flags */
    ADC1->CR |= ADC_CR_ADSTP; /* Stop the sequence conversion */
    /* the data in the DR is considered as not valid */
    error |= ERROR_OVERRUN_ADC_IT; /* Report an error */
  }
  else
  {
    error |= ERROR_UNEXPECTED_ADC_IT; /* Report unexpected ADC interrupt occurrence */
  }
}


/**
  * @brief  This function handles EXTI0_1 interrupt request.
  *         It toggles the conversion sequence if PA0 is pressed
  * @param  None
  * @retval None
  */
void EXTI0_1_IRQHandler(void)
{
  if ((EXTI->PR & 0x0001) != 0)  /* Check line 0 has triggered the IT */
  {
    EXTI->PR |= 0x0001; /* Clear the pending bit */
    if ((ADC1->CR & ADC_CR_ADSTART) != 0) /* Check if conversion on going */
    {
      if ((ADC1->ISR & ADC_ISR_EOC) != 0)  /* Check conversion is completed */
      {
        ADC_array[CurrentChannel] = ADC1->DR; /* Read data and clears EOC flag */
        CurrentChannel++;  /* Increment the index on ADC_array */ 
        if (CurrentChannel >= NUMBER_OF_ADC_CHANNEL)
        {
          CurrentChannel = 0;
        }
      }
    }
    else
    {
      error |= ERROR_ADC_DISABLED; /* Report an error */
    }   
  }
  else
  {
    error |= ERROR_UNEXPECTED_EXT_IT; /* Report unexpected external interrupt occurrence */   
  }
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
