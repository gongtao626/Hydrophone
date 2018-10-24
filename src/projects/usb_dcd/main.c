/**
  \main page 
  \n Copyright (c) 2012 Freescale
  \n Freescale Confidential Proprietary 
  \brief 
  \author   	Freescale Semiconductor
  \author    
  \version      m.n
  \date         Jan 3, 2012
  
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

/*****************************************************************************************************
* Include files
*****************************************************************************************************/
#include "common.h"
#include "usb_cdc.h"
#include "usb_reg.h"
#include "usb_dcd.h"
#include "Settings.h"
/*****************************************************************************************************
* Declaration of module wide FUNCTIONs - NOT for use in other modules
*****************************************************************************************************/

/*****************************************************************************************************
* Definition of module wide MACROs / #DEFINE-CONSTANTs - NOT for use in other modules
*****************************************************************************************************/
#define ENABLE_USB_5V   GPIOC_PSOR|=(1<<9);
#define DISABLE_USB_5V  GPIOC_PCOR|=(1<<9);

#define ID_GND_EVENT            0x20
#define ID_FLOAT_EVENT          0x40
#define VBUS_LOW_EVENT          0x10
#define VBUS_HIGH_EVENT         0x01
/*****************************************************************************************************
* Declaration of module wide TYPEs - NOT for use in other modules
*****************************************************************************************************/

/*****************************************************************************************************
* Definition of module wide VARIABLEs - NOT for use in other modules
*****************************************************************************************************/
extern uint8 gu8USB_Flags; 
extern uint8 gu8EP3_OUT_ODD_Buffer[];
extern tBDT tBDTtable[];

volatile uint8  gu8ISR_Flags=0;
uint8 gu8DebugTrace[1000];
uint8 *pu8trace;
/*****************************************************************************************************
* Definition of module wide (CONST-) CONSTANTs - NOT for use in other modules
*****************************************************************************************************/

/*****************************************************************************************************
* Code of project wide FUNCTIONS
*****************************************************************************************************/

/********************************************************************/
void PortC_ISR(void)
{
    uint32 u32Value;
    u32Value = PORTC_ISFR;
    PORTC_ISFR = u32Value;
    
    if (u32Value && (uint32)(1<<11))
    {
      if (GPIOC_PDIR & (uint32)(1<<11))
      {
          FLAG_SET(VBUS_HIGH_EVENT,gu8ISR_Flags);
      }
      else
      {
        USB_DISABLE_PULLUP;
        printf("\nVBUS is Down");
      }

    }
}
/********************************************************************/
void port_config (void)
{

    /* Turn on port clocks */
    SIM_SCGC5|=  SIM_SCGC5_PORTC_MASK;

    /* 5V enable */
    PORTC_PCR9=(0|PORT_PCR_MUX(1));         // configure pin as GPIO
    GPIOC_PDDR|=(1<<9);                     // set as output
    
    /* VBUS detection  */
    enable_irq(INT_PORTC-16);
    PORTC_PCR11=(0|PORT_PCR_MUX(1)|PORT_PCR_PE_MASK|PORT_PCR_PS_MASK|PORT_PCR_IRQC(0x0B));       
}

/**
 * \brief   main
 * \author  
 * \param  none
 * \return none
 * \todo
 * \warning
 */  
int main (void)
{
    pu8trace=gu8DebugTrace;
    
    // Making sure tat Regulator is enable on every low power mode
    USB_REG_SET_ENABLE;
    USB_REG_SET_STDBY_STOP;
    USB_REG_SET_STDBY_VLPx;

    port_config();

    DISABLE_USB_5V;                     // Disable USB 5v (for Host support)
    
    CDC_Init();     // USB CDC Initialization
    DCD_Init();     // Enabling DCD

    /* Here we activate te DCD start sequence. This section must be triggered 
      with the VBUs detection pin, but in this case will work once after
      the board is powered in the Kinetis USB connector */    

    printf("\n********** DCD/USB Device Module Testing **********\n");
    
     while(1)
    {
        
        // USB CDC service routine
        CDC_Engine();
        
        // DCD detection service
        (void)DCD_Service();

        // Check for VBUS event
        if(FLAG_CHK(VBUS_HIGH_EVENT,gu8ISR_Flags))
        {    
            USBDCD_CONTROL=USBDCD_CONTROL_IE_MASK  | USBDCD_CONTROL_IACK_MASK;          
            FLAG_SET(USBDCD_CONTROL_START_SHIFT,USBDCD_CONTROL);
            printf("\nVBUS is Up\n");
            //DCD_Main();
            
            FLAG_CLR(VBUS_HIGH_EVENT,gu8ISR_Flags);
        }
        
     
        // If data transfer arrives
        if(FLAG_CHK(EP_OUT,gu8USB_Flags))
        {
            (void)USB_EP_OUT_SizeCheck(EP_OUT);         
            usbEP_Reset(EP_OUT);
            usbSIE_CONTROL(EP_OUT);
            FLAG_CLR(EP_OUT,gu8USB_Flags);

            // Send it back to the PC
            EP_IN_Transfer(EP2,CDC_OUTPointer,1);
        }        
    }

}
/********************************************************************/
