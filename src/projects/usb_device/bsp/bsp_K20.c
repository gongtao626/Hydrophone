#include "bsp_K20.h"

void vfnfll_init (void);
void fll_init (void);

void vfnInitUSBClock (uint8 u8ClkOption)
{

  switch (u8ClkOption)
  {
    case MCGPLL0:
    {
      SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK     /** PLL reference */   
                |  SIM_SOPT2_USBSRC_MASK;       /** USB fractional divider like USB reference clock */  
      /** Divide reference clock by one to obtain 48MHz */
      SIM_CLKDIV2 &= ~(SIM_CLKDIV2_USBDIV_MASK |SIM_CLKDIV2_USBFRAC_MASK);    
      break;     
    }
    
  
    case MCGFLL:
    {
        /** Configure FLL to generate a 48MHz core clock */
       fll_init();                            
          
        SIM_SOPT2 &= ~SIM_SOPT2_PLLFLLSEL_MASK;       /** FLL reference */   
        SIM_SOPT2 |=  SIM_SOPT2_USBSRC_MASK;          /** USB fractional divider like USB reference clock */ 
        /** Divide reference clock by one to obtain 48MHz */
        SIM_CLKDIV2 &= ~(SIM_CLKDIV2_USBDIV_MASK |SIM_CLKDIV2_USBFRAC_MASK);      
        break;
    }
    
    case CLKIN:
    {
      SIM_SOPT2 &= (uint32)(~SIM_SOPT2_USBSRC_MASK);    /** PTA5 selected as USBFS CLK source */ 
      FLAG_SET(SIM_SCGC5_PORTA_SHIFT,SIM_SCGC5); 
      PORTA_PCR25=(0|PORT_PCR_MUX(2));                   // Enabling PTA5 as CLK input    
    }
  }
  
  SIM_SCGC4|=(SIM_SCGC4_USBOTG_MASK);             // USB Clock Gating
}



void fll_init(void)
{
 /**  pee_pbe(CLK0_FREQ_HZ);
  pbe_fbe(CLK0_FREQ_HZ);
  fbe_fee(CLK0_FREQ_HZ);*/

}

/**FLL output like bus clock source, divide reference by 256 (8MHz /256 = 31.25kHz)
	MCG_C1 = MCG_C1_CLKS(0) | MCG_C1_FRDIV(3);
	
	while (!(MCG_S & MCG_S_OSCINIT_MASK)){};  // wait for oscillator to initialize
	while (MCG_S & MCG_S_IREFST_MASK){}; 	  // wait for Reference clock Status bit to clear  */

        /**Fixed DCO multiplier of 1464, MCGOUT = 32.768 kHz * 1464 / 1 = 48 MHz
        MCG_C4 = MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(1);		       
        while (MCG_S & MCG_S_CLKST_MASK){};    	  // Wait for output of FLL */
	