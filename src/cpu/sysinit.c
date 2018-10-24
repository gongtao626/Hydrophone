/*
 * File:        sysinit.c
 * Purpose:     Kinetis Configuration
 *              Initializes processor to a default state
 *
 * Notes:
 *
 */

#include "common.h"
#include "sysinit.h"
#include "uart.h"
#include "mcg.h"

/********************************************************************/
/* Actual system clock frequency */
int mcg_clk_hz;
int mcg_clk_khz;
int core_clk_khz;
int periph_clk_khz;
int pll_clk_khz;

int core_clk_mhz;

/********************************************************************/
void sysinit (void)
{
        /*
         * Enable all of the port clocks. These have to be enabled to configure
         * pin muxing options, so most code will need all of these on anyway.
         */
        SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK
                      | SIM_SCGC5_PORTB_MASK
                      | SIM_SCGC5_PORTC_MASK
                      | SIM_SCGC5_PORTD_MASK
                      | SIM_SCGC5_PORTE_MASK );
        
            // releases hold with ACKISO:  Only has an effect if recovering from VLLS1, VLLS2, or VLLS3
    // if ACKISO is set you must clear ackiso before calling pll_init 
    //    or pll init hangs waiting for OSC to initialize
    // if osc enabled in low power modes - enable it first before ack
    // if I/O needs to be maintained without glitches enable outputs and modules first before ack.
//LP 10/25 #ifdef MCU_MK20DZ50        
#ifdef MCU_MK20DZ50 
       // printf(" PMC_REGSC = %x", PMC_REGSC);
    if (PMC_REGSC &  PMC_REGSC_ACKISO_MASK)
        PMC_REGSC |= PMC_REGSC_ACKISO_MASK;
#endif
    
        
#if defined(NO_PLL_INIT)
    // ON REV 1.0 THE SLOW IRC IS NON-FUNCTIONAL AT < 2.05V. THE DEFAULT FREQUENCY WILL VARY FROM PART TO PART IN FEI MODE
          mcg_clk_hz = 21000000; //FEI mode
#else 
       /* Ramp up the system clock */
       /* Set the system dividers */
       /* NOTE: The PLL init will not configure the system clock dividers,
        * so they must be configured appropriately before calling the PLL
        * init function to ensure that clocks remain in valid ranges.
        */  
        SIM_CLKDIV1 = ( 0
                        | SIM_CLKDIV1_OUTDIV1(0)
                        | SIM_CLKDIV1_OUTDIV2(1)
                        | SIM_CLKDIV1_OUTDIV4(3) );
    // releases hold with ACKISO:  Only has an effect if recovering from VLLS1, VLLS2, or VLLS3
    // if ACKISO is set you must clear ackiso before calling pll_init 
    //    or pll init hangs waiting for OSC to initialize
    // if osc enabled in low power modes - enable it first before ack
    // if I/O needs to be maintained without glitches enable outputs and modules first before ack.
    if (PMC_REGSC &  PMC_REGSC_ACKISO_MASK)
        PMC_REGSC |= PMC_REGSC_ACKISO_MASK;

       /* Initialize PLL */
       /* PLL will be the source for MCG CLKOUT so the core, system, and flash clocks are derived from it */ 
       mcg_clk_hz = pll_init(CLK0_FREQ_HZ,  /* CLKIN0 frequency */
                             LOW_POWER,     /* Set the oscillator for low power mode */
                             CLK0_TYPE,     /* Crystal or canned oscillator clock input */
                             PLL0_PRDIV,    /* PLL predivider value */
                             PLL0_VDIV,     /* PLL multiplier */
                             MCGOUT);       /* Use the output from this PLL as the MCGOUT */

       /* Check the value returned from pll_init() to make sure there wasn't an error */
       if (mcg_clk_hz < 0x100)
         while(1);
#endif

	/*
         * Use the value obtained from the pll_init function to define variables
	 * for the core clock in kHz and also the peripheral clock. These
	 * variables can be used by other functions that need awareness of the
	 * system frequency.
	 */
        mcg_clk_khz = mcg_clk_hz / 1000;
	core_clk_khz = mcg_clk_khz / (((SIM_CLKDIV1 & SIM_CLKDIV1_OUTDIV1_MASK) >> 28)+ 1);
  	periph_clk_khz = mcg_clk_khz / (((SIM_CLKDIV1 & SIM_CLKDIV1_OUTDIV2_MASK) >> 24)+ 1);
        
  	/* For debugging purposes, enable the trace clock and/or FB_CLK so that
  	 * we'll be able to monitor clocks and know the PLL is at the frequency
  	 * that we expect.
  	 */
	//trace_clk_init();
#ifndef NO_CLKOUT_SUPPORT
  	/* Enable the pins for the clock out pins */
	trace_clk_init();
  	fb_clk_init();
#endif
        
#ifndef CONSOLE_SUPPORT
  	/* Enable the pins for the selected UART */
         if (TERM_PORT == UART0_BASE_PTR)
         {
#ifdef TWR_K20DX256
           /* Enable the UART0_TXD function on PTB17 */
            PORTB_PCR17 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin

            /* Enable the UART0_RXD function on PTB16 */
            PORTB_PCR16 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin    
#elif 0//P0_DC_64LQFP
           /* Enable the UART0_TXD function on PTA2*/
            PORTA_PCR2 = PORT_PCR_MUX(0x2); // UART is alt2 function for this pin

            /* Enable the UART0_RXD function on PTA1 */
            PORTA_PCR1 = PORT_PCR_MUX(0x2); // UART is alt2 function for this pin            
#else
            /* Enable the UART0_TXD function on PTD6 */
            PORTD_PCR6 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin

            /* Enable the UART0_RXD function on PTD7 */
            PORTD_PCR7 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
#endif
         }

         if (TERM_PORT == UART1_BASE_PTR)
  	 {
           /* Enable the UART0_TXD function on PTE0 (1) */
            PORTE_PCR0 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin

            /* Enable the UART0_RXD function on PTE1 (2) */
            PORTE_PCR1 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin   
  	}

  	if (TERM_PORT == UART2_BASE_PTR)
  	{
                 /* Enable the UART2_TXD function on PTD3 */
  		PORTD_PCR3 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin

  		/* Enable the UART2_RXD function on PTD2 */
  		PORTD_PCR2 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
  	}
        
  	/* UART0 and UART1 are clocked from the core clock, but all other UARTs are
         * clocked from the peripheral clock. So we have to determine which clock
         * to send to the uart_init function.
         */
        if ((TERM_PORT == UART0_BASE_PTR) | (TERM_PORT == UART1_BASE_PTR))
            uart_init (TERM_PORT, core_clk_khz, TERMINAL_BAUD);
        else
  	    uart_init (TERM_PORT, periph_clk_khz, TERMINAL_BAUD);
#endif
}
/********************************************************************/
void trace_clk_init(void)
{
	/* Set the trace clock to the core clock frequency */
	SIM_SOPT2 |= SIM_SOPT2_TRACECLKSEL_MASK;

	/* Enable the TRACE_CLKOUT pin function on PTA6 (alt7 function) */
	PORTA_PCR6 = ( PORT_PCR_MUX(0x7));
}
/********************************************************************/
void fb_clk_init(void)
{
 	/* Enable the FB_CLKOUT function on PTC3 (alt5 function) */
        SIM_SOPT2 &= ~SIM_SOPT2_CLKOUTSEL_MASK; // clear clkoout field
        SIM_SOPT2 |= SIM_SOPT2_CLKOUTSEL(2);    // select flash clock
	PORTC_PCR3 = ( PORT_PCR_MUX(0x5) | PORT_PCR_DSE_MASK );
}
/********************************************************************/
