/*
 * File:		uart_baud_rate_tests.c
 * Purpose:		SBR and BRFA baud rate divider tests.
 *
 */

#include "common.h"
#include "uart_tests.h"

/* Globals from sysinit.c */
extern int core_clk_khz;
extern int core_clk_mhz;
extern int periph_clk_khz;

/********************************************************************/
/* UART module sbr buad rate divider test.
 * Tests multiple baud rates on one of the high speed UARTs and on one
 * of the normal speed UARTs. This test will use the SBR baud rate divider.
 * A scope will be used to verify the frequency of the output transmit
 * data. Since the test is not self checking it will always return 0 (pass).
 */
int uart_sbr_test()
{
    UART_MemMapPtr module;
    uint32 freq, sbr;

    /* Determine which high speed UART to use for testing. UART0 or UART1 could
     * be used. We'll use UART0 as long as it isn't used as the TERM_PORT.
     */
    if (TERM_PORT != UART0_BASE_PTR)
    {      
       module = UART0_BASE_PTR;   // set the module pointer
        
       printf("\nStarting high-speed UART baud rate test on UART0.\n");
       printf("\nPlease connect a scope to PTE0/UART0_TX (J53 pin 1 on EDSNDC-256).\n");
       printf("Set scope for single edge trigger (rising or falling).\n");
       printf("Press any key when ready.\n");
       (void)in_char();
   }      
    else
    {      
       module = UART1_BASE_PTR;   // set the module pointer
       
       printf("\nStarting high-speed UART baud rate test on UART1.\n");
       printf("\nPlease connect a scope to PTC4/UART1_TX (J7 pin 2 on EDSNDC-256).\n");
       printf("Set scope for single edge trigger (rising or falling).\n");
       printf("Press any key when ready.\n");
       (void)in_char();
    }
    
    /* Clear the UART_C4 register to make sure the fractional divider is 
     * disabled. If this isn't cleared it throws off the calculations for
     * the expected frequency.
     */
    UART_C4_REG(module) = 0;
    

    /* Test each bit of the SBR. By walking a one through the SBR value we will
     * hit all of the possible bits in the divider. This gives better test 
     * coverage than randombly picked intermediate values for the divider.
     */

    /* Set sbr to 0x1 (the minimum divide value) */
    sbr = 1;
    
    /* Test each bit in the BDL */
    while (sbr < 0x100)
    {   
       /* Calculate the expected UART_TXD toggle rate with the current sbr */
       freq = ((core_clk_khz * 1000)/(sbr * 32));
       
       /* Output the expected value and wait for scope ready */
       printf("\nExpected data toggle frequency is %d Hz\n", freq);
       printf("Press any key when scope is ready.\n");
       (void)in_char();    
       
       /* Set BDL register to the current sbr value we are testing */
       UART_BDL_REG(module) = sbr;
       
       /* Send 0x55, so that UARTn_TX will toggle at a constant rate during transmission */
       uart_putchar (module, 0x55); 
       
       /* Shift the 1 in the sbr value to get next value to test */
       sbr = sbr<<1;
    }       

    /* Test each bit in the BDH */
    while (sbr < 0x1000)
    {   
       /* Calculate the expected UART_TXD toggle rate with the current sbr */
       freq = ((core_clk_khz * 1000)/(sbr * 32));
       
       /* Output the expected value and wait for scope ready */
       printf("\nExpected data toggle frequency is %d Hz\n", freq);
       printf("Press any key when scope is ready.\n");
       (void)in_char();    
       
       /* Set BDH register to the current sbr value we are testing */
        UART_BDH_REG(module) = sbr>>8;

       /* Set BDL register to zero so the BDH write will take effect */
       UART_BDL_REG(module) = 0;
       
       /* Send 0x55, so that UARTn_TX will toggle at a constant rate during transmission */
       uart_putchar (module, 0x55); 
       
       /* Shift the 1 in the sbr value to get next value to test */
       sbr = sbr<<1;
    }       

    printf("\nHigh speed UART baud rate tests complete. Press any key to continue to next test.\n");
    (void)in_char();    

    /* Reinitialize the UART back to default settings before moving on to next test */
    uart_init (module, core_clk_khz, TERMINAL_BAUD);                  


    /* Determine which normal speed UART to use for testing. UART2 or UART3 could
     * be used. We'll use UART2 as long as it isn't used as the TERM_PORT.
     */
    if (TERM_PORT != UART2_BASE_PTR)
    {      
       module = UART2_BASE_PTR;   // set the module pointer
       uart_init (module, periph_clk_khz, TERMINAL_BAUD);   // init the uart with default settings
       
       printf("\nStarting UART baud rate test on UART2.\n");
       printf("\nPlease connect a scope to PTE16/UART2_TX (J4 pin 2 on EDSNDC-256.\n");
       printf("Set scope for single edge trigger (rising or falling).\n");
       printf("Press any key when ready.\n");
       (void)in_char();
   }      
    else
    {      
       printf("\nNo more UARTs availables\n");
       (void)in_char();
    }

    /* Clear the UART_C4 register to make sure the fractional divider is 
     * disabled. If this isn't cleared it throws off the calculations for
     * the expected frequency.
     */
    UART_C4_REG(module) = 0;

    /* Test each bit of the SBR. By walking a one through the SBR value we will
     * hit all of the possible bits in the divider. This gives better test 
     * coverage than randombly picked intermediate values for the divider.
     */
    
    /* Set sbr to 0x1 (the minimum divide value) */
    sbr = 1;
    
    /* Test each bit in the BDL */
    while (sbr < 0x100)
    {   
       /* Calculate the expected UART_TXD toggle rate with the current sbr */
       freq = ((periph_clk_khz * 1000)/(sbr * 32));
       
       /* Output the expected value and wait for scope ready */
       printf("\nExpected data toggle frequency is %d Hz\n", freq);
       printf("Press any key when scope is ready.\n");
       (void)in_char();    
       
       /* Set BDL register to the current sbr value we are testing */
       UART_BDL_REG(module) = sbr;
       
       /* Send 0x55, so that UARTn_TX will toggle at a constant rate during transmission */
       uart_putchar (module, 0x55); 
       
       /* Shift the 1 in the sbr value to get next value to test */
       sbr = sbr<<1;
    }       

    /* Test each bit in the BDH */
    while (sbr < 0x1000)
    {   
       /* Calculate the expected UART_TXD toggle rate with the current sbr */
       freq = ((periph_clk_khz * 1000)/(sbr * 32));
       
       /* Output the expected value and wait for scope ready */
       printf("\nExpected data toggle frequency is %d Hz\n", freq);
       printf("Press any key when scope is ready.\n");
       (void)in_char();    
       
       /* Set BDH register to the current sbr value we are testing */
        UART_BDH_REG(module) = sbr>>8;

       /* Set BDL register to zero so the BDH write will take effect */
       UART_BDL_REG(module) = 0;
       
       /* Send 0x55, so that UARTn_TX will toggle at a constant rate during transmission */
       uart_putchar (module, 0x55); 
       
       /* Shift the 1 in the sbr value to get next value to test */
       sbr = sbr<<1;
    }       

    printf("\nLow speed UART baud rate tests complete. Press any key to continue to next test.\n");
    (void)in_char();    

    /* Reinitialize the UART back to default settings before moving on to next test */
    uart_init (module, core_clk_khz, TERMINAL_BAUD);                  
    
    return 0;
}

/********************************************************************/

