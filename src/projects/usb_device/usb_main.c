/*
 * File:		usb_main.c
 * Purpose:		Main process
 *
 */

/* Includes */
#include "common.h"
#include "usb_cdc.h"
#include "usb_reg.h"
#include "Settings.h"

volatile uint8  gu8ISR_Flags=0;

/* Extern Variables */
extern uint8 gu8USB_Flags; 
extern uint8 gu8EP3_OUT_ODD_Buffer[];
extern tBDT tBDTtable[];

void PORTC_ISR (void);

/********************************************************************/
int main (void)
{
    printf("\n********** USB Device Module Testing **********\n");
    
    USB_REG_SET_ENABLE;
    USB_REG_SET_STDBY_STOP;      
    USB_REG_SET_STDBY_VLPx;
    
    /* VBUS detection  */
	SIM_SCGC5|= SIM_SCGC5_PORTC_MASK;
	enable_irq(INT_PORTC-16);
	PORTC_PCR11=(0|PORT_PCR_MUX(1)|PORT_PCR_PE_MASK|PORT_PCR_PS_MASK|PORT_PCR_IRQC(0x0B));  
     
    
    // USB CDC Initialization
    CDC_Init();
    
    while(1)
    {
        
       // USB CDC service routine 
          CDC_Engine();

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

/*******************************************************************/
/*******************************************************************/

void PORTC_ISR (void)
{
    uint32 u32Value;
    u32Value = PORTC_ISFR;
    PORTC_ISFR = u32Value;
    
    if (u32Value && (uint32)(1<<11))
    {
      if (GPIOC_PDIR & (uint32)(1<<11))
      {
          FLAG_SET(VBUS_HIGH_EVENT,gu8ISR_Flags);		
          printf("\n\nVBUS is Up");   
      }
      else
      {
        FLAG_SET(VBUS_LOW_EVENT,gu8ISR_Flags);
        FLAG_CLR(USB_CTL_USBENSOFEN_MASK,USB0_CTL);		//If USB cable detach turn-off the USB peripheral
        printf("\nVBUS is Down");
      }

    }
}
