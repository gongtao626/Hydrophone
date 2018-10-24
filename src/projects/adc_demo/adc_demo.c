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
 * File:		adc_demo.c
 * Purpose:		Demo the Analog to Digtal Converter triggered by the Programable Delay Block
 *                      Call the function Hw_Trig_Test which demonstrates and tests this function.
 *                      Runs on the Tower Kinetis Card, using the potentiometer as input.
 *                      The operator will be prompted on the serial port at 115200 baud, 8n1.
 *                      The LED's will also indicate the timing of the conversions.
 *
 */

/*****************************************************************************************************
* Include files
*****************************************************************************************************/
#include "common.h"
#include "adc16.h"     // ADC driver defines
#include "adc_demo.h"   // For this function


/*    global variable    */
tADC_Config Master_Adc_Config;  // This is the structure that contains the desired ADC/PGA configuration.


extern uint8_t  Hw_Trig_Test(void);

/********************************************************************/

int main (void)
{

#ifdef KEIL
	start();
#endif
        
  	printf("\nRunning the adc_demo project in K2050MHz family\n");
        
        Init_Gpio(); 
        Hw_Trig_Test(); //  demo the adc/pdb
         
}
/********************************************************************/

//******************************************************************************
// setup an output pin, "PIN",  to indirectly observe adc status changes
// 
//******************************************************************************

void Init_Gpio(void)

{

 //the "PIN" will be visible as PORTC7 on TOWER board
 // setup PORTC7  for output "PIN" as called in test programs as on TOWER board
  PORTC_PCR7 = PORT_PCR_MUX(1) ;        // selec GPIO function
  GPIOC_PDDR|=(1<<7);                     // set as output 
  GPIOC_PCOR = 0x01 << 7 ;              // initial out low 
}