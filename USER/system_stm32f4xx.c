/**
  ******************************************************************************
  * @file    system_stm32f4xx.c
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   CMSIS Cortex-M4 Device Peripheral Access Layer System Source File.
  *          This file contains the system clock configuration for STM32F4xx devices.
  *             
  * 1.  This file provides two functions and one global variable to be called from 
  *     user application:
  *      - SystemInit(): Setups the system clock (System clock source, PLL Multiplier
  *                      and Divider factors, AHB/APBx prescalers and Flash settings),
  *                      depending on the configuration made in the clock xls tool. 
  *                      This function is called at startup just after reset and 
  *                      before branch to main program. This call is made inside
  *                      the "startup_stm32f4xx.s" file.
  *
  *      - SystemCoreClock variable: Contains the core clock (HCLK), it can be used
  *                                  by the user application to setup the SysTick 
  *                                  timer or configure other parameters.
  *                                     
  *      - SystemCoreClockUpdate(): Updates the variable SystemCoreClock and must
  *                                 be called whenever the core clock is changed
  *                                 during program execution.
  *
  * 2. After each device reset the HSI (16 MHz) is used as system clock source.
  *    Then SystemInit() function is called, in "startup_stm32f4xx.s" file, to
  *    configure the system clock before to branch to main program.
  *
  * 3. If the system clock source selected by user fails to startup, the SystemInit()
  *    function will do nothing and HSI still used as system clock source. User can 
  *    add some code to deal with this issue inside the SetSysClock() function.
  *
  * 4. The default value of HSE crystal is set to 25MHz, refer to "HSE_VALUE" define
  *    in "stm32f4xx.h" file. When HSE is used as system clock source, directly or
  *    through PLL, and you are using different crystal you have to adapt the HSE
  *    value to your own configuration.
  *
  * 5. This file configures the system clock as follows:
  *=============================================================================
  *=============================================================================
  *                    Supported STM32F40xxx/41xxx devices
  *-----------------------------------------------------------------------------
  *        System Clock source                    | PLL (HSE)
  *-----------------------------------------------------------------------------
  *        SYSCLK(Hz)                             | 168000000
  *-----------------------------------------------------------------------------
  *        HCLK(Hz)                               | 168000000
  *-----------------------------------------------------------------------------
  *        AHB Prescaler                          | 1
  *-----------------------------------------------------------------------------
  *        APB1 Prescaler                         | 4
  *-----------------------------------------------------------------------------
  *        APB2 Prescaler                         | 2
  *-----------------------------------------------------------------------------
  *        HSE Frequency(Hz)                      | 25000000
  *-----------------------------------------------------------------------------
  *        PLL_M                                  | 25
  *-----------------------------------------------------------------------------
  *        PLL_N                                  | 336
  *-----------------------------------------------------------------------------
  *        PLL_P                                  | 2
  *-----------------------------------------------------------------------------
  *        PLL_Q                                  | 7
  *-----------------------------------------------------------------------------
  *=============================================================================
  *=============================================================================
  *                    Supported STM32F42xxx/43xxx devices
  *-----------------------------------------------------------------------------
  *        System Clock source                    | PLL (HSE)
  *-----------------------------------------------------------------------------
  *        SYSCLK(Hz)                             | 180000000
  *-----------------------------------------------------------------------------
  *        HCLK(Hz)                               | 180000000
  *-----------------------------------------------------------------------------
  *        AHB Prescaler                          | 1
  *-----------------------------------------------------------------------------
  *        APB1 Prescaler                         | 4
  *-----------------------------------------------------------------------------
  *        APB2 Prescaler                         | 2
  *-----------------------------------------------------------------------------
  *        HSE Frequency(Hz)                      | 25000000
  *-----------------------------------------------------------------------------
  *        PLL_M                                  | 25
  *-----------------------------------------------------------------------------
  *        PLL_N                                  | 360
  *-----------------------------------------------------------------------------
  *        PLL_P                                  | 2
  *-----------------------------------------------------------------------------
  *        PLL_Q                                  | 7
  *-----------------------------------------------------------------------------
  *=============================================================================
  *=============================================================================
  *                    Supported STM32F401xx devices
  *-----------------------------------------------------------------------------
  *        System Clock source                    | PLL (HSE)
  *-----------------------------------------------------------------------------
  *        SYSCLK(Hz)                             | 84000000
  *-----------------------------------------------------------------------------
  *        HCLK(Hz)                               | 84000000
  *-----------------------------------------------------------------------------
  *        AHB Prescaler                          | 1
  *-----------------------------------------------------------------------------
  *        APB1 Prescaler                         | 2
  *-----------------------------------------------------------------------------
  *        APB2 Prescaler                         | 1
  *-----------------------------------------------------------------------------
  *        HSE Frequency(Hz)                      | 25000000
  *-----------------------------------------------------------------------------
  *        PLL_M                                  | 25
  *-----------------------------------------------------------------------------
  *        PLL_N                                  | 336
  *-----------------------------------------------------------------------------
  *        PLL_P                                  | 8
  *-----------------------------------------------------------------------------
  *        PLL_Q                                  | 7
  *-----------------------------------------------------------------------------
  *=============================================================================
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

/*!< Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

#if !defined  (HSE_VALUE)
  #define HSE_VALUE    ((uint32_t)25000000) /*!< Default value of the External oscillator in Hz */
#endif /* HSE_VALUE */

#if !defined  (HSI_VALUE)
  #define HSI_VALUE    ((uint32_t)16000000) /*!< Value of the Internal oscillator in Hz*/
#endif /* HSI_VALUE */

/*!< Uncomment the following line if you need to use external SRAM mounted on STM324xG-EVAL board */
/* #define DATA_IN_ExtSRAM */

/*!< Uncomment the following line if you need to use external SDRAM mounted on STM324xG-EVAL board */
/* #define DATA_IN_ExtSDRAM */


/*!< Definition of PLL parameters ------------------------------------------------*/
#if defined (STM32F40_41xxx) || defined (STM32F427_437xx) || defined (STM32F429_439xx)
  #define PLL_M      8
  #define PLL_N      336
  #define PLL_P      2
  #define PLL_Q      7
#elif defined (STM32F401xx)
  #define PLL_M      25
  #define PLL_N      336
  #define PLL_P      8
  #define PLL_Q      7
#endif /* STM32F40_41xxx || STM32F427_437xx || STM32F429_439xx || STM32F401xx */

/*!< Exported types ------------------------------------------------------------*/
/*!< Exported constants --------------------------------------------------------*/
/*!< Exported macro ------------------------------------------------------------*/
/*!< Exported functions -------------------------------------------------------*/

/*!< Private variables ---------------------------------------------------------*/
uint32_t SystemCoreClock = 16000000;
const uint8_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
const uint8_t APBPrescTable[8]  = {0, 0, 0, 0, 1, 2, 3, 4};

/*!< Private function prototypes -----------------------------------------------*/
static void SetSysClock(void);
#ifdef DATA_IN_ExtSRAM
  static void SystemInit_ExtMemCtl(void); 
#endif /* DATA_IN_ExtSRAM */

/**
  * @brief  Setup the microcontroller system
  *         Initialize the Embedded Flash Interface, the PLL and update the 
  *         SystemFrequency variable.
  * @param  None
  * @retval None
  */
void SystemInit(void)
{
  /* FPU settings ------------------------------------------------------------*/
  #if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */
  #endif
  
  /* Reset the RCC clock configuration to the default reset state ------------*/
  /* Set HSION bit */
  RCC->CR |= (uint32_t)0x00000001;

  /* Reset CFGR register */
  RCC->CFGR = 0x00000000;

  /* Reset HSEON, CSSON and PLLON bits */
  RCC->CR &= (uint32_t)0xFEF6FFFF;

  /* Reset PLLCFGR register */
  RCC->PLLCFGR = 0x24003010;

  /* Reset HSEBYP bit */
  RCC->CR &= (uint32_t)0xFFFBFFFF;

  /* Disable all interrupts */
  RCC->CIR = 0x00000000;

#ifdef DATA_IN_ExtSRAM
  SystemInit_ExtMemCtl(); 
#endif /* DATA_IN_ExtSRAM */

  /* Configure the System clock source, PLL Multiplier and Divider factors,
     AHB/APBx prescalers and Flash settings ----------------------------------*/
  SetSysClock();
}

/**
  * @brief  Update SystemCoreClock variable according to Clock Register Values.
  *         The SystemCoreClock variable contains the core clock (HCLK), it can
  *         be used by the user application to setup the SysTick timer or configure
  *         other parameters.
  *
  * @param  None
  * @retval None
  */
void SystemCoreClockUpdate(void)
{
  uint32_t tmp = 0, pllvco = 0, pllp = 2, pllsource = 0, pllm = 2;

  /* Get SYSCLK source -------------------------------------------------------*/
  tmp = RCC->CFGR & RCC_CFGR_SWS;

  switch (tmp)
  {
    case 0x00:  /* HSI used as system clock */
      SystemCoreClock = HSI_VALUE;
      break;
    case 0x04:  /* HSE used as system clock */
      SystemCoreClock = HSE_VALUE;
      break;
    case 0x08:  /* PLL used as system clock */

      /* Get PLL clock source and multiplication factor ----------------------*/
      pllsource = (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) >> 22;
      pllm = RCC->PLLCFGR & RCC_PLLCFGR_PLLM;

      if (pllsource != 0)
      {
        /* HSE used as PLL clock source */
        pllvco = (HSE_VALUE / pllm) * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6);
      }
      else
      {
        /* HSI used as PLL clock source */
        pllvco = (HSI_VALUE / pllm) * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6);
      }

      pllp = (((RCC->PLLCFGR & RCC_PLLCFGR_PLLP) >>16) + 1 ) *2;
      SystemCoreClock = pllvco / pllp;
      break;
    default:
      SystemCoreClock = HSI_VALUE;
      break;
  }
  /* Compute HCLK frequency --------------------------------------------------*/
  /* Get HCLK prescaler */
  tmp = AHBPrescTable[((RCC->CFGR & RCC_CFGR_HPRE) >> 4)];
  /* HCLK frequency */
  SystemCoreClock >>= tmp;
}

/**
  * @brief  Configures the System clock source, PLL Multiplier and Divider factors,
  *         AHB/APBx prescalers and Flash settings
  * @Note   This function should be called only once the RCC clock configuration
  *         is reset to the default reset state (done in SystemInit() function).
  * @param  None
  * @retval None
  */
static void SetSysClock(void)
{
#if defined (STM32F40_41xxx) || defined (STM32F427_437xx) || defined (STM32F429_439xx) || defined (STM32F401xx)
/******************************************************************************/
/*            PLL (clocked by HSE) used as System clock source                */
/******************************************************************************/
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
  
  /* Enable HSE */
  RCC->CR |= ((uint32_t)RCC_CR_HSEON);
 
  /* Wait till HSE is ready and if Time out is reached exit */
  do
  {
    HSEStatus = RCC->CR & RCC_CR_HSERDY;
    StartUpCounter++;
  } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

  if ((RCC->CR & RCC_CR_HSERDY) != RESET)
  {
    HSEStatus = (uint32_t)0x01;
  }
  else
  {
    HSEStatus = (uint32_t)0x00;
  }

  if (HSEStatus == (uint32_t)0x01)
  {
    /* Select regulator voltage output Scale 1 mode */
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR |= PWR_CR_VOS;

    /* HCLK = SYSCLK / 1*/
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

#if defined (STM32F40_41xxx) || defined (STM32F427_437xx) || defined (STM32F429_439xx)      
    /* PCLK2 = HCLK / 2*/
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;
    
    /* PCLK1 = HCLK / 4*/
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;
#endif /* STM32F40_41xxx || STM32F427_437x || STM32F429_439xx */

#if defined (STM32F401xx)
    /* PCLK2 = HCLK / 2*/
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;
    
    /* PCLK1 = HCLK / 4*/
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
#endif /* STM32F401xx */
   
    /* Configure the main PLL */
    RCC->PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) -1) << 16) |
                   (RCC_PLLCFGR_PLLSRC_HSE) | (PLL_Q << 24);

    /* Enable the main PLL */
    RCC->CR |= RCC_CR_PLLON;

    /* Wait till the main PLL is ready */
    while((RCC->CR & RCC_CR_PLLRDY) == 0)
    {
    }
   
#if defined (STM32F427_437xx) || defined (STM32F429_439xx)
    /* Enable the Over-drive to extend the clock frequency to 180 Mhz */
    PWR->CR |= PWR_CR_ODEN;
    while((PWR->CSR & PWR_CSR_ODRDY) == 0)
    {
    }
    PWR->CR |= PWR_CR_ODSWEN;
    while((PWR->CSR & PWR_CSR_ODSWRDY) == 0)
    {
    }      
    /* Configure Flash prefetch, Instruction cache, Data cache and wait state */
    FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_5WS;
#endif /* STM32F427_437x || STM32F429_439xx  */

#if defined (STM32F40_41xxx)     
    /* Configure Flash prefetch, Instruction cache, Data cache and wait state */
    FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_5WS;
#endif /* STM32F40_41xxx  */

#if defined (STM32F401xx)
    /* Configure Flash prefetch, Instruction cache, Data cache and wait state */
    FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_2WS;
#endif /* STM32F401xx */

    /* Select the main PLL as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    /* Wait till the main PLL is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL)
    {
    }
  }
  else
  {
    /* If HSE fails to start-up, the application will have wrong clock
         configuration. User can add here some code to deal with this error */
  }
#elif defined (STM32F411xE)
#if defined (USE_HSE_BYPASS)
/******************************************************************************/
/*            PLL (clocked by HSE) used as System clock source                */
/******************************************************************************/
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0;

  /* Enable HSE with Bypass */
  RCC->CR |= ((uint32_t)RCC_CR_HSEON | RCC_CR_HSEBYP);

  /* Wait till HSE is ready and if Time out is reached exit */
  do
  {
    HSEStatus = RCC->CR & RCC_CR_HSERDY;
    StartUpCounter++;
  } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

  if ((RCC->CR & RCC_CR_HSERDY) != RESET)
  {
    HSEStatus = (uint32_t)0x01;
  }
  else
  {
    HSEStatus = (uint32_t)0x00;
  }

  if (HSEStatus == (uint32_t)0x01)
  {
    /* Select regulator voltage output Scale 1 mode */
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR |= PWR_CR_VOS;

    /* HCLK = SYSCLK / 1*/
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

    /* PCLK2 = HCLK / 2*/
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;

    /* PCLK1 = HCLK / 4*/
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;

    /* Configure the main PLL */
    RCC->PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) -1) << 16) |
                   (RCC_PLLCFGR_PLLSRC_HSE) | (PLL_Q << 24);

    /* Enable the main PLL */
    RCC->CR |= RCC_CR_PLLON;

    /* Wait till the main PLL is ready */
    while((RCC->CR & RCC_CR_PLLRDY) == 0)
    {
    }

    /* Configure Flash prefetch, Instruction cache, Data cache and wait state */
    FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_5WS;

    /* Select the main PLL as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    /* Wait till the main PLL is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL)
    {
    }
  }
  else
  {
    /* If HSE fails to start-up, the application will have wrong clock
         configuration. User can add here some code to deal with this error */
  }
#else
/******************************************************************************/
/*            PLL (clocked by HSI) used as System clock source                */
/******************************************************************************/
  /* Select regulator voltage output Scale 1 mode */
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;
  PWR->CR |= PWR_CR_VOS;

  /* HCLK = SYSCLK / 1*/
  RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

  /* PCLK2 = HCLK / 2*/
  RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;

  /* PCLK1 = HCLK / 4*/
  RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;

  /* Configure the main PLL */
  RCC->PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) -1) << 16) |
                 (RCC_PLLCFGR_PLLSRC_HSI) | (PLL_Q << 24);

  /* Enable the main PLL */
  RCC->CR |= RCC_CR_PLLON;

  /* Wait till the main PLL is ready */
  while((RCC->CR & RCC_CR_PLLRDY) == 0)
  {
  }

  /* Configure Flash prefetch, Instruction cache, Data cache and wait state */
  FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_5WS;

  /* Select the main PLL as system clock source */
  RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
  RCC->CFGR |= RCC_CFGR_SW_PLL;

  /* Wait till the main PLL is used as system clock source */
  while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL)
  {
  }
#endif /* USE_HSE_BYPASS */
#endif /* STM32F40_41xxx || STM32F427_437xx || STM32F429_439xx || STM32F401xx */
}

#ifdef DATA_IN_ExtSRAM
/**
  * @brief  Setup the external memory controller. Called in startup_stm32f4xx.s 
  *          before jump to __main
  * @param  None
  * @retval None
  */
static void SystemInit_ExtMemCtl(void)
{
  __IO uint32_t tmpreg;
  
  /* Enable FMC clock */
  RCC->AHB3ENR |= RCC_AHB3ENR_FMCEN;

  /* Configure and enable Bank1_SRAM2 */
  FMC_Bank1->BTCR[2] = FMC_BTR2_ADDSET_0 | FMC_BTR2_ADDHLD_0 | FMC_BTR2_DATAST_3 |
                        FMC_BTR2_BUSTURN_0 | FMC_BTR2_CLKDIV_0 | FMC_BTR2_DATLAT_0 |
                        FMC_BTR2_ACCMOD_A;
  
  FMC_Bank1->BTCR[3] = FMC_BCR2_MBKEN | FMC_BCR2_WREN;

  /* Delay to let the memory be initialized */
  for (tmpreg = 0; tmpreg < 1000; tmpreg++);
}
#endif /* DATA_IN_ExtSRAM */
