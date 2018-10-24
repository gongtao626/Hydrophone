/**
  \mainpage 
  \n Copyright (c) 2011 Freescale
  \n Freescale Confidential Proprietary 
  \brief 
  \author   	Freescale Semiconductor
  \author    
  \version      m.n
  \date         Sep 14, 2011
  
  Put here all the information needed of the Project. Basic configuration as well as information on
  the project definition will be very useful 
*/
/****************************************************************************************************/
/*                                                                                                  */
/* All software, source code, included documentation, and any implied know-how are property of      */
/* Freescale Semiconductor and therefore considered CONFIDENTIAL INFORMATION.                       */
/* This confidential information is disclosed FOR DEMONSTRATION PURPOSES ONLY.                      */
/*                                                                                                  */
/* All Confidential Information remains the property of Freescale Semiconductor and will not be     */
/* copied or reproduced without the express written permission of the Discloser, except for copies  */
/* that are absolutely necessary in order to fulfill the Purpose.                                   */
/*                                                                                                  */
/* Services performed by FREESCALE in this matter are performed AS IS and without any warranty.     */
/* CUSTOMER retains the final decision relative to the total design and functionality of the end    */
/* product.                                                                                         */
/* FREESCALE neither guarantees nor will be held liable by CUSTOMER for the success of this project.*/
/*                                                                                                  */
/* FREESCALE disclaims all warranties, express, implied or statutory including, but not limited to, */
/* implied warranty of merchantability or fitness for a particular purpose on any hardware,         */
/* software ore advise supplied to the project by FREESCALE, and or any product resulting from      */
/* FREESCALE services.                                                                              */
/* In no event shall FREESCALE be liable for incidental or consequential damages arising out of     */
/* this agreement. CUSTOMER agrees to hold FREESCALE harmless against any and all claims demands or */
/* actions by anyone on account of any damage,or injury, whether commercial, contractual, or        */
/* tortuous, rising directly or indirectly as a result of the advise or assistance supplied CUSTOMER*/ 
/* in connectionwith product, services or goods supplied under this Agreement.                      */
/*                                                                                                  */
/****************************************************************************************************/

/*
 * File:		hscmp.c
 * Purpose:		HSCMP example on HSCMP0
 *
 *                      P input is DAC output (49/64 VREF for this example, or 2.53 V)
 *                      M input is IR receiver signal on PTC7
 *
 *                      This example uses IR transmitter on PTD7 to drive the
 *                      IR receiver. They are angled so that the IR transmitter
 *                      can drive the IR receiver on the tower board. This
 *                      requires shunting J7 and J9, and remove J4.
 *                      To enable IR transmit put the board on a white paper and in order to stop 
 *                      transmiting lift the board.
 *                      Alternatively you can point a remote to the IR
 *                      receiver and toggle the HSCMP that way.
 *
 */

/*****************************************************************************************************
* Include files
*****************************************************************************************************/
#include "common.h"


#define GPIO_PIN_MASK            0x1Fu
#define GPIO_PIN(x)              (((1)<<(x & GPIO_PIN_MASK)))

void hscmp_clear();

int main (void)
{

#ifdef KEIL
	start();
#endif
        
//Use HSCMP0 for this example
  CMP_MemMapPtr cmpch = CMP0_BASE_PTR;;

  printf("HSCMP0 Example\n");
  printf("Please remove J4 and shunt J7 and J9 in order to drive the IR Transmitter.\n");  
  printf("Press any key to continue\n");
  in_char(); //wait for keyboard input
  printf("\n\n");

  //Enable HSCMP module clock
 SIM_SCGC4 |= (SIM_SCGC4_CMP_MASK );

  //Enable PTD7 as GPIO output to drive IR transmitter on tower board
  PORTD_PCR7=(PORT_PCR_MUX(1)); //set pin as GPIO
  //PORTD_PCR7 |= (PORT_PCR_DSE_MASK); // Drive Strength Enable
  GPIOD_PDDR= GPIO_PIN(7); //set pin as output	

  //Enable HSCMP0 Compare Out signal on PTC5
  PORTC_PCR5=PORT_PCR_MUX(6);  //CMP0_OUT on PTC5

  printf("CMP0_IN1 input is on PTC7, which is connected to the IR receiver on the tower board\n");
  printf("CMP0_OUT output is on PTC5, which is on the pin3 on J2(GPIO HEADER)\n");

  //Set up HSCMP0 in NVIC. ISR put into vector table in isr.h file
  enable_irq(INT_CMP0-16);   //CMP0 Vector is 39. IRQ# is 39-16=23

  // Configure CMP.
  hscmp_clear(); //clear comparator registers
  CMP_CR0_REG(cmpch) = 0x00;  // Filter and digital hysteresis disabled
  CMP_CR1_REG(cmpch) = 0x15;  // Continuous mode, high-speed compare, unfiltered output, output pin disabled
  CMP_FPR_REG(cmpch) = 0x00;  // Filter disabled
  CMP_SCR_REG(cmpch) = 0x1E;  // Enable rising and falling edge interrupts, clear flags
  CMP_DACCR_REG(cmpch) = 0xF0; // 6-bit reference DAC enabled, output set to vdd/64*(48+1)= 2.53v, (Vin selected is vin2=Vdd)
                        //DACO = (Vin/64) * (VOSEL[5:0] + 1), so the DACO range is from Vin/64 to Vin.
  CMP_MUXCR_REG(cmpch) = 0x0F; // P-input as external channel 1. M-input as 6b DAC = 2.53v, it is adjustable with CMP_DACCR_REG
  CMP_CR1_REG(cmpch) |= CMP_CR1_OPE_MASK; // Enable output pin for HSCMP0.

  while(1)
  {
    //Toggle PTD7 for IR transmitter to drive CMP0_IN1 connected to IR Receiver
    GPIOD_PTOR=GPIO_PIN(7); 

    //Delay for next toggle
    time_delay_ms(500);
  }

}
/********************************************************************/

/*
 * ISR for cmp0
 */
void cmp0_isr(void)
{
  //If rising edge
  if ((CMP0_SCR & CMP_SCR_CFR_MASK)==CMP_SCR_CFR_MASK)
  {
    CMP0_SCR |= CMP_SCR_CFR_MASK;  // Clear the flag
    printf("Rising edge on HSCMP0\n");
  }

  //If falling edge
  if ((CMP0_SCR & CMP_SCR_CFF_MASK)==CMP_SCR_CFF_MASK)
  {
    CMP0_SCR |= CMP_SCR_CFF_MASK;  // Clear the flag
    printf("Falling edge on HSCMP0\n");
  }
}


/*
 * Clear HSCMP0 registers
 */
void hscmp_clear()
{
  CMP_MemMapPtr cmpch = CMP0_BASE_PTR;

  CMP_CR0_REG(cmpch) = 0;
  CMP_CR1_REG(cmpch) = 0;
  CMP_FPR_REG(cmpch) = 0;
  CMP_SCR_REG(cmpch) = 0x06;  // Clear flags if set.
  CMP_DACCR_REG(cmpch) = 0;
  CMP_MUXCR_REG(cmpch) = 0;
}
