#include "misc.h"

/********************************************************************************
 *   delay: delay
 * Notes:
 *    -
 ********************************************************************************/
void delay(uint32 i)
{
  for(i;i;i--)  //delay
  {
  }
}

/********************************************************************************
 *   GPIO_Init: Initializes GPIO controlling LED
 * Notes:
 *    -
 ********************************************************************************/
void GPIO_Init(void)
{
  ENABLE_GPIO_CLOCKS;

  LED0_EN;
  LED1_EN;
/* These LEDs may be used, initialization is commented out because they
  source current from the MCU and this impact low power measurements */
/*
  LED2_EN;
  LED3_EN;
*/
  LEDs_On();
  LED_Dir_Out();
}

void LEDs_On(void)
{
  GPIOC_PDDR |= (/*(1<<7)|(1<<8)|*/(1<<9)|(1<<10));
}

void LEDs_Off(void)
{
  GPIOC_PSOR |= (/*(1<<7)|(1<<8)|*/(1<<9)|(1<<10));
}

void LED_Dir_Out(void)
{
  GPIOC_PDOR &= ~(/*(1<<7)|(1<<8)|*/(1<<9)|(1<<10));
}

void RTC_Init(void)
{
  SIM_SOPT1 |= SIM_SOPT1_OSC32KSEL_MASK;
  SIM_SCGC6 |= SIM_SCGC6_RTC_MASK;
}
