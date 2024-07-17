/**
 ******************************************************************************
 * @file    main.c
 * @author  MCU Application Team
 * @brief   Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) Puya Semiconductor Co.
 * All rights reserved.</center></h2>
 *
 * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

#pragma GCC optimize("O3")
#define BREAK() __asm__ __volatile__("bkpt #0")

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "py32f0xx.h"
#include <string.h>

static void APP_SystemClockConfig(void);

static char aIsPrime[1000];
static unsigned int NumPrimes;

/*********************************************************************
 *
 *       _CalcPrimes()
 */
static void _CalcPrimes(unsigned int NumItems) {
  unsigned int i;
  unsigned int j;

  //
  // Mark all as potential prime numbers
  //
  memset(aIsPrime, 1, NumItems);
  //
  // 2 deserves a special treatment
  //
  for (i = 4; i < NumItems; i += 2) {
    aIsPrime[i] = 0; // Cross it out: not a prime
  }
  //
  // Cross out multiples of every prime starting at 3. Crossing out starts at
  // i^2.
  //
  for (i = 3; i * i < NumItems; i++) {
    if (aIsPrime[i]) {
      j = i * i; // The square of this prime is the first we need to cross out
      do {
        aIsPrime[j] = 0; // Cross it out: not a prime
        j += 2 * i;      // Skip even multiples (only 3*, 5*, 7* etc)
      } while (j < NumItems);
    }
  }
  //
  // Count prime numbers
  //
  NumPrimes = 0;
  for (i = 2; i < NumItems; i++) {
    if (aIsPrime[i]) {
      NumPrimes++;
    }
  }
}

int main(void) {
  APP_SystemClockConfig();

  while (1) {
    unsigned int ms = 1000;
    volatile unsigned int Cnt = 0;

    __IO uint32_t tmp = SysTick->CTRL; /* Clear the COUNTFLAG first */
    /* Add this code to indicate that local variable is not used */
    ((void)tmp);

    while (ms) {
      if ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) != 0U) {
        ms--;
      }
      _CalcPrimes(sizeof(aIsPrime));
      Cnt++;
    }
    BREAK(); // look at Cnt with your debugger
  }
}

static void APP_SystemClockConfig(void) {
  LL_UTILS_ClkInitTypeDef UTILS_ClkInitStruct;

  LL_RCC_HSI_Enable();
  /* Change this value to adjust frequency */
  LL_RCC_HSI_SetCalibFreq(LL_RCC_HSICALIBRATION_24MHz);
  while (LL_RCC_HSI_IsReady() != 1)
    ;

  UTILS_ClkInitStruct.AHBCLKDivider = LL_RCC_SYSCLK_DIV_1;
  UTILS_ClkInitStruct.APB1CLKDivider = LL_RCC_APB1_DIV_1;
  LL_PLL_ConfigSystemClock_HSI(&UTILS_ClkInitStruct);

  /* Re-init frequency of SysTick source, reload = freq/ticks = 48000000/1000 =
   * 48000 */
  LL_InitTick(48000000, 1000U);
}

void APP_ErrorHandler(void) {
  while (1)
    ;
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line) {
  while (1)
    ;
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT Puya *****END OF FILE******************/
