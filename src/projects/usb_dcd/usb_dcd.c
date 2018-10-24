#include "usb_dcd.h"

/* Global Variables */
uint8 gu8InterruptFlags=0;

/***************************************************************/
void DCD_Init(void)
{
    // SIM Configuration   
    SIM_SCGC6|=(SIM_SCGC6_USBDCD_MASK);             // USBDCD Clock Gating

    // Enable DCD Interrupt in NVIC
    enable_irq(INT_USBDCD - 16);
    
    USBDCD_CLOCK=(DCD_TIME_BASE<<2)|1;
}

/***************************************************************/

uint8 DCD_Service(void)
{
    //gu8InterruptFlags=0;
    uint32 u8Error;
        
        // DCD results
        if(FLAG_CHK(DCD_FLAG,gu8InterruptFlags))
        {
            u8Error=DCD_GetChargerType();

            if((u8Error&0xF0))
                printf("Oooooops DCD Error");
            else
            {     
                if((u8Error&0x0F)==STANDARD_HOST)
                    printf("\nConnected to a Standard Host");
                if((u8Error&0x0F)==CHARGING_HOST)
                    printf("\nConnected to a Charging Host");
                if((u8Error&0x0F)==DEDICATED_CHARGER)
                    printf("\nConnected to a Dedicated Charger");
            }
            //DCD_ResetModule;  
            FLAG_CLR(DCD_FLAG,gu8InterruptFlags);
            DCD_ResetModule;
            return(1);
        }
        return(0);
}
/***************************************************************/
uint8 DCD_Out_of_reset_test(void)
{
    uint8 u8Error=0;
            
    if(USBDCD_CONTROL != USBDCD_CONTROL_RESET_VALUE)
        u8Error|=1<<0;
    if(USBDCD_CLOCK != USBDCD_CLOCK_RESET_VALUE)
        u8Error|=1<<1;
    if(USBDCD_STATUS != USBDCD_STATUS_RESET_VALUE)
        u8Error|=1<<2;
    if(USBDCD_TIMER0 != USBDCD_TIMER0_RESET_VALUE)
        u8Error|=1<<3;
    if(USBDCD_TIMER1 != USBDCD_TIMER1_RESET_VALUE)
        u8Error|=1<<4;
    if(USBDCD_TIMER2 != USBDCD_TIMER2_RESET_VALUE)
        u8Error|=1<<5;
    
    return(u8Error);
}

/***************************************************************/
uint8 DCD_GetChargerType(void)
{
    uint8 u8ChargerType;
    
    u8ChargerType = (uint8)((USBDCD_STATUS & USBDCD_STATUS_SEQ_RES_MASK)>>16);
    
    u8ChargerType|= (uint8)((USBDCD_STATUS & USBDCD_STATUS_FLAGS_MASK)>>16);

    return(u8ChargerType);
}

/***************************************************************/
uint8 DCD_RegisterAccess_long(void)
{
    uint8 u8Error=0;
    
    // Testing DCD Control Register
    USBDCD_CONTROL=0x00000000;
    if(USBDCD_CONTROL != 0x00000000)
        u8Error|=1<<0;
    USBDCD_CONTROL=0x00010000;
    if(USBDCD_CONTROL != 0x00010000)
        u8Error|=1<<0;
    
    // Testing Clock Register
    USBDCD_CLOCK=0x00000000;
    if(USBDCD_CLOCK != 0x00000000)
        u8Error|=1<<1;
    USBDCD_CLOCK=0x00000FFD;
    if(USBDCD_CLOCK != 0x00000FFD)
        u8Error|=1<<1;
    
    // Testing timer0 Register
    USBDCD_TIMER0=0x00000000;
    if(USBDCD_TIMER0 != 0x00000000)
        u8Error|=1<<2;
    USBDCD_TIMER0=0x03FF0000;
    if(USBDCD_TIMER0 != 0x03FF0000)
        u8Error|=1<<2;
    
    // Testing Timer1 Register
    USBDCD_TIMER1=0x00000000;
    if(USBDCD_TIMER1 != 0x00000000)
        u8Error|=1<<3;
    USBDCD_TIMER1=0x03FF03FF;
    if(USBDCD_TIMER1 != 0x03FF03FF)
        u8Error|=1<<3;
                                   
    // Testing Timer2 Register
    USBDCD_TIMER2=0x00000000;
    if(USBDCD_TIMER2 != 0x00000000)
        u8Error|=1<<4;
    USBDCD_TIMER2=0x03FF000F;
    if(USBDCD_TIMER2 != 0x03FF000F)
        u8Error|=1<<4;
    
    return(u8Error);
}

/***************************************************************/
void DCD_Main(void)
{
    uint32 u32Error;
    
    gu8InterruptFlags=0;
        
    /* SIM Configuration */   
    SIM_SCGC4|=(SIM_SCGC4_USBOTG_MASK);             // USB FS/LS Clock Gating
    SIM_SCGC6|=(SIM_SCGC6_USBDCD_MASK);             // USB DCD Clock Gating

    // Enable DCD Interrupt in NVIC
    enable_irq(INT_USBDCD -16);
  
    // USB pre-initialization 
    USB0_USBTRC0 |= USB_USBTRC0_USBRESET_MASK;
    while(FLAG_CHK(USB_USBTRC0_USBRESET_SHIFT,USB0_USBTRC0)){};
    FLAG_SET(USB_ISTAT_USBRST_MASK,USB0_ISTAT);

    // Enable USB Reset Interrupt
    FLAG_SET(USB_INTEN_USBRSTEN_SHIFT,USB0_INTEN);
    USB0_USBCTRL=0x00;
    USB0_USBTRC0|=0x40;
    USB0_CTL|=0x01;        
        
    USBDCD_CLOCK=(DCD_TIME_BASE<<2)|1;
    
    /** Configure VBUS detection pin */
    PORTC_PCR11 = PORT_PCR_MUX(1);
    FLAG_CLR(VBUS_FLAG,gu8InterruptFlags);
   
    for(;;)
    {
        /* Waiting for VBUS */
       if(FLAG_CHK(11,GPIOC_PDIR) && !FLAG_CHK(VBUS_FLAG,gu8InterruptFlags))
       {
          #ifdef DCD_INTERRUPT_METHOD
          USBDCD_CONTROL = USBDCD_CONTROL_IE_MASK  | USBDCD_CONTROL_IACK_MASK;          
          FLAG_SET(USBDCD_CONTROL_START_SHIFT,USBDCD_CONTROL);
          #endif
          
          #ifdef DCD_POLLING_METHOD
          DCD_Start_Charger_Detection_Sequence_PollMode;  
          #endif
          
          FLAG_SET(VBUS_FLAG,gu8InterruptFlags);
        }         
        
        // Polling for enable Pullup resistor 
        #ifdef DCD_POLLING_METHOD
        if(USBDCD_STATUS & USBDCD_STATUS_SEQ1_RES_MASK)
            USB0_CONTROL|= USB_CONTROL_DPPULLUPNONOTG_MASK;   
        
        if (FLAG_CHK(VBUS_FLAG,gu8InterruptFlags))
        {
          if((!(USBDCD_STATUS & 0x00400000)) || (USBDCD_STATUS & 0x00300000))
            FLAG_SET(DCD_FLAG,gu8InterruptFlags);                 
        }

        #endif
        
        // DCD results
        if(FLAG_CHK(DCD_FLAG,gu8InterruptFlags))
        {
            u32Error=DCD_GetChargerType();

            if((u32Error&0xF0))
                printf("Oooooops DCD Error");
            else
            {     
                if((u32Error&0x0F)==STANDARD_HOST)
                    printf("Connected to a Standard Host");
                if((u32Error&0x0F)==CHARGING_HOST)
                    printf("Connected to a Charging Host");
                if((u32Error&0x0F)==DEDICATED_CHARGER)
                    printf("Connected to a Dedicated Charger");
            }
            //DCD_ResetModule;  
            FLAG_CLR(DCD_FLAG,gu8InterruptFlags);
            FLAG_CLR(VBUS_FLAG,gu8InterruptFlags);
            FLAG_SET(DEBUG_FLAG,gu8InterruptFlags);
        }
    }

}

/***************************************************************/
void DCD_ISR(void)
{
    USBDCD_CONTROL|= USBDCD_CONTROL_IACK_MASK;
    
    if((USBDCD_STATUS&0x000C0000) == 0x00080000)
        FLAG_SET(USB_CONTROL_DPPULLUPNONOTG_SHIFT,USB0_CONTROL); 

    if((!(USBDCD_STATUS & 0x00400000)) || (USBDCD_STATUS & 0x00300000))
        FLAG_SET(DCD_FLAG,gu8InterruptFlags);        

}

   
