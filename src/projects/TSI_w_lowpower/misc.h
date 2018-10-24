
#include "common.h"
#include "pmc.h"
#include "smc.h"
#include "llwu.h"

void delay(uint32);
void GPIO_Init(void);
void LEDs_On(void);
void LEDs_Off(void);
void LED_Dir_Out(void);
void RTC_Init(void);

#define ENABLE_GPIO_CLOCKS (SIM_SCGC5 |= (SIM_SCGC5_PORTC_MASK))
#define LED0_EN (PORTC_PCR10 = PORT_PCR_MUX(1))
#define LED1_EN (PORTC_PCR9 = PORT_PCR_MUX(1))
#define LED2_EN (PORTC_PCR7 = PORT_PCR_MUX(1))
#define LED3_EN (PORTC_PCR8 = PORT_PCR_MUX(1))

#define LED0_TOGGLE (GPIOC_PTOR = (1<<10))
#define LED1_TOGGLE (GPIOC_PTOR = (1<<9))
#define LED3_TOGGLE (GPIOC_PTOR = (1<<7))
#define LED2_TOGGLE (GPIOC_PTOR = (1<<8))
