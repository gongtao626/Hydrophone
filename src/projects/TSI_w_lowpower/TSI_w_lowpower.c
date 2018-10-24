/**
  \mainpage 
  \n Copyright (c) 2012 Freescale
  \n Freescale 
  \brief 
  \author   	Freescale Semiconductor
  \author    
  \version      1.0
  \date         Jan 31, 2012
  
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

/*****************************************************************************************************
* Include files
*****************************************************************************************************/
#include "common.h"
#include "TSI.h"
#include "misc.h"
/*****************************************************************************************************
* Declaration of module wide FUNCTIONS - NOT for use in other modules
*****************************************************************************************************/
void prepareToGetLowestPower(void);
void JTAG_TDO_PullUp_Enable(void);
void clockMonitor(unsigned char state);
void uart(unsigned char state);
/*****************************************************************************************************
* Definition of module wide MACROs / #DEFINE-CONSTANTs - NOT for use in other modules
*****************************************************************************************************/

/*****************************************************************************************************
* Declaration of module wide TYPEs - NOT for use in other modules
*****************************************************************************************************/

/*****************************************************************************************************
* Definition of module wide VARIABLEs - NOT for use in other modules
*****************************************************************************************************/

/*****************************************************************************************************
* Definition of module wide (CONST-) CONSTANTs - NOT for use in other modules
*****************************************************************************************************/

/*****************************************************************************************************
* Code of project wide FUNCTIONS
*****************************************************************************************************/


void GPIO_Init(void);

/********************************************************************/
int main(void)
{
	unsigned char op_mode;
	
    RTC_Init();
    TSI_Init();
    GPIO_Init();
    TSI_SelfCalibration();
    llwu_configure(0,0,LLWU_ME_WUME4_MASK);
    enable_irq(9);
    
    /*Disable Clock Out*/        
    PORTC_PCR3 = ( PORT_PCR_MUX(0));
    
    /*Enable all operation modes because this is a write once register*/  
    SMC_PMPROT =  SMC_PMPROT_AVLLS_MASK |
                  SMC_PMPROT_ALLS_MASK  |    
                  SMC_PMPROT_AVLP_MASK;
    /******************************************************************/
    
    START_SCANNING;
    ENABLE_EOS_INT;
    ENABLE_TSI;

    while(1)
    {
        /* LLS mode */
        printf("\r\n Send any character to go into low power mode LLS \r\n");
        LEDs_Off();
        in_char();
        enable_lpwui();
        TSI_GoToLP();
        prepareToGetLowestPower(); 
        enter_lls();     
        op_mode = what_mcg_mode();
        if(op_mode==PBE)
        {
          pbe_pee(CLK0_FREQ_HZ);
        }
        clockMonitor(ON);
        uart(ON);
        TSI_GoToActive();
        printf("\r\n Woke up from LLS !!! \r\n");
        LEDs_On();

        /* VLLSx mode */
        printf("\r\n Send any character to go into low power mode VLLSx \r\n");
        in_char();
        enable_lpwui();
        prepareToGetLowestPower(); 
        TSI_GoToLP();
#ifdef VLLS3			/*Define either VLLS3 or 2, if none are define, it's VLLS1 */
        enter_vlls3(); 
#elif 	VLLS2
        enter_vlls2();
#else
        enter_vlls1();
#endif
    }

}

/*******************************************************************************************************/
void prepareToGetLowestPower(void)
{
     uart(OFF);
     clockMonitor(OFF);
     JTAG_TDO_PullUp_Enable();          
}

/*******************************************************************************************************/
void JTAG_TDO_PullUp_Enable(void)
{
    PORTA_PCR2 = PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;//JTAG_TDO      
}


/*******************************************************************************************************/
void clockMonitor(unsigned char state)
{
    if(state)
      MCG_C6 |= MCG_C6_CME0_MASK;
    else
      MCG_C6 &= ~MCG_C6_CME0_MASK;
}

/*******************************************************************************************************/
void uart(unsigned char state)
{
    if(state)
    {
       PORTE_PCR0 = PORT_PCR_MUX(3); // UART is alt3 function for this pin
       PORTE_PCR1 = PORT_PCR_MUX(3); // UART is alt3 function for this pin
    }else
    {
       PORTE_PCR0 = PORT_PCR_MUX(0); // Analog is alt0 function for this pin
       PORTE_PCR1 = PORT_PCR_MUX(0); // Analog is alt0 function for this pin
    }
  
 
}


/********************************************************************/

