/****************************************************************************************************/
/**
Copyright (c) 2011 Freescale Semiconductor
Freescale Confidential Proprietary
\file     irda_test.c
\brief    UART0 configured as IRDA interface  CMT output enabled (doubled buffer)

\author     
\author     
\version    1.0
\date       Sep 26, 2011

*/


#include "common.h"
#include "uart.h"
#include "irda_test.h"

#define IRDA_UART_PORT     UART0_BASE_PTR



/**   buzzer_ftm_init
 * \brief    Initialize UART for irda Test 
 * \brief    PIT1 is used for tone/buzzer time control
 * \author   b01252
 * \param    none
 * \return   none
 */  
void irda_test_init(void )
{


  SIM_SOPT5 |= SIM_SOPT5_UART0RXSRC(1);     // CMP0 as RX source  
  //SIM_SOPT2 |= SIM_SOPT2_CMTUARTPAD_MASK;   // Enable CMT_OUTPUT double buffer SOPT2[CMTUARTPAD]
  SIM_SOPT2 |= SIM_SOPT2_PTD7PAD_MASK;
// Pin multilplexing configuration
  
  
  
 // PORTD_PCR7 |= PORT_PCR_MUX(3);  // pin for   UART0_TX  Transmission by CMT
    
   // Pin multilplexing configuration
  PORTC_PCR7 &= ~PORT_PCR_MUX_MASK;  
  PORTC_PCR7 |= PORT_PCR_MUX(0);  // pin for   PTC7_EBI_AD8/CMP0_IN1  from IR receiver
  
  PORTC_PCR5 |= PORT_PCR_MUX(6);  // pin for   CMP0_OUT
  
  
// initialize comparator 0 to receive input from IR receiver, dac is set to 
  
  SIM_SCGC4 |= SIM_SCGC4_CMP_MASK;
  CMP0_CR1 |= CMP_CR1_EN_MASK | CMP_CR1_INV_MASK | CMP_CR1_COS_MASK | CMP_CR1_OPE_MASK | CMP_CR1_PMODE_MASK;
  
  CMP0_DACCR =  CMP_DACCR_DACEN_MASK | CMP_DACCR_VRSEL_MASK | CMP_DACCR_VOSEL(0x38); 
  
  CMP0_MUXCR = CMP_MUXCR_MEN_MASK | CMP_MUXCR_PEN_MASK |  CMP_MUXCR_MSEL(DAC_INPUT) | CMP_MUXCR_PSEL(IRDA_CMP_INPUT); 
  
  
  
  uart_init (IRDA_UART_PORT, 50000,600);
  UART_S2_REG(IRDA_UART_PORT) |= !UART_S2_RXINV_MASK;
  
  // configure as irda
  

}



void irda_tx(char ch)
{
 
  uart_putchar(IRDA_UART_PORT, ch);

}


char irda_get_char(void)
{
   char ch=0;
   if (uart_getchar_present (IRDA_UART_PORT))
   {
   ch = uart_getchar (IRDA_UART_PORT);
   }
   return ch;
}