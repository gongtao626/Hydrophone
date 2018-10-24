/*
 * File:		uart_tx_tests.c
 * Purpose:		UART transmit tests. All of these tests use a scope 
 *              to verify correct bit toggling/timing.
 *
 */

#include "common.h"
#include "uart.h"
#include "uart_tests.h"


/********************************************************************/
/* UART most significant bit first test.
 * Tests operation of the UARTn_S2[MSBF] bit on a single UART. This bit
 * is the same across UART instantiations, so only one UART needs to be
 * tested. The test will use both values for MSBF and then transmit specific
 * bytes. A scope should be used to verify that the values transmitted
 * match the expected value (either lsb or msb first).
 *
 * Since the test is not self checking it will always return 0 (pass).
 */
int uart_msbf_test()
{
    UART_MemMapPtr module;
    volatile char ch;
    
    /* Determine which UART to use for testing. UART2 or UART3 could
     * be used. We'll use UART2 as long as it isn't used as the TERM_PORT.
     */
    if (TERM_PORT != UART2_BASE_PTR)
    {      
       module = UART2_BASE_PTR;   // set the module pointer
       
       printf("\nStarting UART MSBF test on UART2.\n");
       printf("\nPlease connect a scope to PTE16/UART2_TX (J4 pin 2 on EDSNDC-256.\n");
       printf("Set scope for single edge trigger (rising or falling).\n");
       printf("Press any key when ready.\n");
       ch = in_char();
   }      
    else
    {
#warning "change in the UART port"      
       module = UART1_BASE_PTR;   // set the module pointer
       
       printf("\nStarting UART MSBF test on UART1.\n");
       printf("\nPlease connect a scope to PTC17/UART3_TX (J43 pin 2 on EDSNDC-256).\n");
       printf("Set scope for single edge trigger (rising or falling).\n");
       printf("Press any key when ready.\n");
       ch = in_char();
    }
    
    /* Make sure that the MSBF bit is cleared to start */
    UART_S2_REG(module) &= ~UART_S2_MSBF_MASK;
    
    printf("\n\nUART configured for lsb first.\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();
    
    printf("Sending 0xAA...\n");
    uart_putchar (module, 0xAA); 
    printf("Press any key when scope is ready.\n");
    ch = in_char();
    
    printf("Sending 0x55...\n");
    uart_putchar (module, 0x55); 
    printf("Press any key when scope is ready.\n");
    ch = in_char();


    /* Now send the same data but with MSBF bit set */
    UART_S2_REG(module) |= UART_S2_MSBF_MASK;
    
    printf("\n\nUART configured for msb first.\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();
    
    printf("Sending 0xAA...\n");
    uart_putchar (module, 0xAA); 
    printf("Press any key when scope is ready.\n");
    ch = in_char();
    
    printf("Sending 0x55...\n");
    uart_putchar (module, 0x55); 
    printf("Press any key when scope is ready.\n");
    ch = in_char();
    
    /* Clear MSBF again before executing any other tests */
    UART_S2_REG(module) &= ~UART_S2_MSBF_MASK;    

    return 0;
}
/********************************************************************/
/* UART transmit data inversion test.
 * Tests operation of the UARTn_C3[TXINV] bit on a single UART. This bit
 * is the same across UART instantiations, so only one UART needs to be
 * tested. The test will use both values for TXINV and then transmit specific
 * bytes. A scope should be used to verify that the values transmitted
 * match the expected value (either lsb or msb first).
 *
 * Since the test is not self checking it will always return 0 (pass).
 */
int uart_txinv_test()
{
    UART_MemMapPtr module;
    volatile char ch;
    
    /* Determine which UART to use for testing. UART2 or UART3 could
     * be used. We'll use UART2 as long as it isn't used as the TERM_PORT.
     */
    if (TERM_PORT != UART2_BASE_PTR)
    {      
       module = UART2_BASE_PTR;   // set the module pointer
       
       printf("\nStarting UART transmit data inversion test on UART2.\n");
       printf("\nPlease connect a scope to PTE16/UART2_TX (J4 pin 2 on EDSNDC-256.\n");
       printf("Set scope for single edge trigger (rising or falling).\n");
       printf("Press any key when ready.\n");
       ch = in_char();
   }      
    else
    {   
#warning "change in the UART port"   
       module = UART1_BASE_PTR;   // set the module pointer
       
       printf("\nStarting UART transmit data inversion test on UART1.\n");
       printf("\nPlease connect a scope to PTC17/UART3_TX (J43 pin 2 on EDSNDC-256).\n");
       printf("Set scope for single edge trigger (rising or falling).\n");
       printf("Press any key when ready.\n");
       ch = in_char();
    }
 
    /* Make sure that the TXINV bit is cleared to start */
    UART_C3_REG(module) &= ~UART_C3_TXINV_MASK;
    
    printf("\n\nUART configured for non-inverted data.\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();
    
    printf("Sending 0xAA...\n");
    uart_putchar (module, 0xAA); 
    printf("Press any key when scope is ready.\n");
    ch = in_char();
    
    printf("Sending 0x55...\n");
    uart_putchar (module, 0x55); 
    printf("Press any key when scope is ready.\n");
    ch = in_char();


    /* Now send the same data but with TXINV bit set */
    UART_C3_REG(module) |= UART_C3_TXINV_MASK;
    
    printf("\n\nUART configured for inverted data.\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();
    
    printf("Sending 0xAA...\n");
    uart_putchar (module, 0xAA); 
    printf("Press any key when scope is ready.\n");
    ch = in_char();
    
    printf("Sending 0x55...\n");
    uart_putchar (module, 0x55); 
    printf("Press any key when scope is ready.\n");
    ch = in_char();
    
    /* Clear TXINV again before executing any other tests */
    UART_C3_REG(module) &= ~UART_C3_TXINV_MASK;

    return 0;
}
/********************************************************************/
/* UART 9-bit mode transmit test.
 * Tests operation of the UARTn_C1[M] bit on a single UART. This bit
 * is the same across UART instantiations, so only one UART needs to be
 * tested. The test will use both values for UARTn_C1[M] and then transmit
 * specific data. A scope should be used to verify that the values transmitted
 * match the expected value.
 *
 * Since the test is not self checking it will always return 0 (pass).
 */
int uart_9bit_tx_test()
{
    UART_MemMapPtr module;
    volatile char ch;
    
    /* Determine which UART to use for testing. UART2 or UART3 could
     * be used. We'll use UART2 as long as it isn't used as the TERM_PORT.
     */
    if (TERM_PORT != UART2_BASE_PTR)
    {      
       module = UART2_BASE_PTR;   // set the module pointer
       
       printf("\nStarting UART 9-bit transmit test on UART2.\n");
       printf("\nPlease connect a scope to PTE16/UART2_TX (J4 pin 2 on EDSNDC-256.\n");
       printf("Set scope for single edge trigger (rising or falling).\n");
       printf("Press any key when ready.\n");
       ch = in_char();
   }      
    else
    {
#warning "change in the UART port"
       module = UART1_BASE_PTR;   // set the module pointer
       
       printf("\nStarting UART 9-bit transmit test on UART1.\n");
       printf("\nPlease connect a scope to PTC17/UART3_TX (J43 pin 2 on EDSNDC-256).\n");
       printf("Set scope for single edge trigger (rising or falling).\n");
       printf("Press any key when ready.\n");
       ch = in_char();
    }
    
    /* Make sure that the M bit is cleared to start */
    UART_C1_REG(module) &= ~UART_C1_M_MASK;
    
    printf("\n\nUART configured for 8-bit mode.\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();
    
    printf("Sending 0x02A...\n");
    
    /* Make sure T8 (the 9th bit) is cleared */
    UART_C3_REG(module) &= ~UART_C3_T8_MASK;   
    uart_putchar (module, 0x2A); 
    printf("Press any key when scope is ready.\n");
    ch = in_char();
    
    printf("Sending 0x115...\n");

    /* Set T8 (the 9th bit) */
    UART_C3_REG(module) |= UART_C3_T8_MASK;   
    uart_putchar (module, 0x15); 
    printf("Press any key when scope is ready.\n");
    ch = in_char();


    /* Now send the same data but in 9-bit mode */
    UART_C1_REG(module) |= UART_C1_M_MASK;
    
    printf("\n\nUART configured for 9-bit mode lsb first.\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();
    
    printf("Sending 0x02A...\n");
    
    /* Make sure T8 (the 9th bit) is cleared */
    UART_C3_REG(module) &= ~UART_C3_T8_MASK;   
    uart_putchar (module, 0x2A); 
    printf("Press any key when scope is ready.\n");
    ch = in_char();
    
    printf("Sending 0x115...\n");

    /* Set T8 (the 9th bit) */
    UART_C3_REG(module) |= UART_C3_T8_MASK;   
    uart_putchar (module, 0x15); 
    printf("Press any key when scope is ready.\n");
    ch = in_char();

    /* Now send the same data but in 9-bit mode msb first*/
    UART_S2_REG(module) |= UART_S2_MSBF_MASK;
    
    printf("\n\nUART configured for 9-bit mode msb first.\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();
    
    printf("Sending 0x02A...\n");
    
    /* Make sure T8 (the 9th bit) is cleared */
    UART_C3_REG(module) &= ~UART_C3_T8_MASK;   
    uart_putchar (module, 0x2A); 
    printf("Press any key when scope is ready.\n");
    ch = in_char();
    
    printf("Sending 0x115...\n");

    /* Set T8 (the 9th bit) */
    UART_C3_REG(module) |= UART_C3_T8_MASK;   
    uart_putchar (module, 0x15); 
    printf("Press any key when scope is ready.\n");
    ch = in_char();
 
    /* Clear M and MSBF to go back to 8-bit lsb first mode before starting next test */
    UART_C1_REG(module) &= ~UART_C1_M_MASK;
    UART_S2_REG(module) &= ~UART_S2_MSBF_MASK;

    return 0;
}
/********************************************************************/
/* UART 10-bit mode transmit test.
 * Tests operation of the 10-bit mode on a single UART. This feature
 * is the same across UART instantiations, so only one UART needs to be
 * tested. A scope should be used to verify that the values transmitted
 * match the expected value.
 *
 * Since the test is not self checking it will always return 0 (pass).
 */
int uart_10bit_tx_test()
{
    UART_MemMapPtr module;
    volatile char ch;
    
    /* Determine which UART to use for testing. UART2 or UART3 could
     * be used. We'll use UART2 as long as it isn't used as the TERM_PORT.
     */
    if (TERM_PORT != UART2_BASE_PTR)
    {      
       module = UART2_BASE_PTR;   // set the module pointer
       
       printf("\nStarting UART 10-bit transmit test on UART2.\n");
       printf("\nPlease connect a scope to PTE16/UART2_TX (J4 pin 2 on EDSNDC-256.\n");
       printf("Set scope for single edge trigger (rising or falling).\n");
       printf("Press any key when ready.\n");
       ch = in_char();
   }      
    else
    {
#warning "change in the UART port"   
       module = UART1_BASE_PTR;   // set the module pointer
       
       printf("\nStarting UART 10-bit transmit test on UART1.\n");
       printf("\nPlease connect a scope to PTC17/UART3_TX (J43 pin 2 on EDSNDC-256).\n");
       printf("Set scope for single edge trigger (rising or falling).\n");
       printf("Press any key when ready.\n");
       ch = in_char();
    }
    
    /* 10-bit mode requires setting the M bit, M10 bit, and enabling parity */
    UART_C1_REG(module) |= UART_C1_M_MASK | UART_C1_PE_MASK;
    UART_C4_REG(module) |= UART_C4_M10_MASK;
    

    /* Make sure MSBF is cleared (lsb first) to start */
    UART_S2_REG(module) &= ~UART_S2_MSBF_MASK;
    
    /* Make sure PT is cleared (even parity) to start */
    UART_C1_REG(module) &= ~UART_C1_PT_MASK;
      
    printf("\n\nUART configured for LSB first, even parity mode.\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();
    
    printf("Sending 0x02A...\n");
    
    /* Make sure T8 (the 9th bit) is cleared */
    UART_C3_REG(module) &= ~UART_C3_T8_MASK;   
    uart_putchar (module, 0x2A); 
    printf("Press any key when scope is ready.\n");
    ch = in_char();
    
    printf("Sending 0x115...\n");

    /* Set T8 (the 9th bit) */
    UART_C3_REG(module) |= UART_C3_T8_MASK;   
    uart_putchar (module, 0x15); 
    printf("Press any key when scope is ready.\n");
    ch = in_char();


    /* Now test msb first, even parity */
    UART_S2_REG(module) |= UART_S2_MSBF_MASK;
      
    printf("\n\nUART configured for MSB first, even parity mode.\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();
    
    printf("Sending 0x02A...\n");
    
    /* Make sure T8 (the 9th bit) is cleared */
    UART_C3_REG(module) &= ~UART_C3_T8_MASK;   
    uart_putchar (module, 0x2A); 
    printf("Press any key when scope is ready.\n");
    ch = in_char();
    
    printf("Sending 0x115...\n");

    /* Set T8 (the 9th bit) */
    UART_C3_REG(module) |= UART_C3_T8_MASK;   
    uart_putchar (module, 0x15); 
    printf("Press any key when scope is ready.\n");
    ch = in_char();    


    /* Test LSB first, odd parity */
    UART_S2_REG(module) &= ~UART_S2_MSBF_MASK;
    
    /* Set PT for odd parity */
    UART_C1_REG(module) |= UART_C1_PT_MASK;
      
    printf("\n\nUART configured for LSB first, odd parity mode.\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();
    
    printf("Sending 0x02A...\n");
    
    /* Make sure T8 (the 9th bit) is cleared */
    UART_C3_REG(module) &= ~UART_C3_T8_MASK;   
    uart_putchar (module, 0x2A); 
    printf("Press any key when scope is ready.\n");
    ch = in_char();
    
    printf("Sending 0x115...\n");

    /* Set T8 (the 9th bit) */
    UART_C3_REG(module) |= UART_C3_T8_MASK;   
    uart_putchar (module, 0x15); 
    printf("Press any key when scope is ready.\n");
    ch = in_char();    
    
    
    /* Last test is MSB first, odd parity */
    UART_S2_REG(module) |= UART_S2_MSBF_MASK;
      
    printf("\n\nUART configured for MSB first, odd parity mode.\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();
    
    printf("Sending 0x02A...\n");
    
    /* Make sure T8 (the 9th bit) is cleared */
    UART_C3_REG(module) &= ~UART_C3_T8_MASK;   
    uart_putchar (module, 0x2A); 
    printf("Press any key when scope is ready.\n");
    ch = in_char();
    
    printf("Sending 0x115...\n");

    /* Set T8 (the 9th bit) */
    UART_C3_REG(module) |= UART_C3_T8_MASK;   
    uart_putchar (module, 0x15); 
    printf("Press any key when scope is ready.\n");
    ch = in_char();    
    
    /* Clear M, M10, PE and MSBF to go back to default mode before starting next test */
    UART_C1_REG(module) = 0;
    UART_C4_REG(module) = 0;
    UART_S2_REG(module) = 0;

    return 0;
}
/********************************************************************/
/* UART 8-bit parity transmit test.
 * Tests operation of the UART in 8-bit mode with parity on a single UART
 * (7 data bits + 1 parity bit). This function is the same across UART 
 * instantiations, so only one UART needs to be tested. This function tests 
 * both even and odd parity modes. A scope should be used to verify that the
 * values transmitted match the expected value.
 *
 * Since the test is not self checking it will always return 0 (pass).
 */
int uart_parity_test()
{
    UART_MemMapPtr module;
    volatile char ch;
    
    /* Determine which UART to use for testing. UART2 or UART3 could
     * be used. We'll use UART2 as long as it isn't used as the TERM_PORT.
     */
    if (TERM_PORT != UART2_BASE_PTR)
    {      
       module = UART2_BASE_PTR;   // set the module pointer
       
       printf("\nStarting UART 8-bit parity transmit test on UART2.\n");
       printf("\nPlease connect a scope to PTE16/UART2_TX (J4 pin 2 on EDSNDC-256.\n");
       printf("Set scope for single edge trigger (rising or falling).\n");
       printf("Press any key when ready.\n");
       ch = in_char();
   }      
    else
    {
#warning "change in the UART port"
       module = UART1_BASE_PTR;   // set the module pointer
       
       printf("\nStarting UART 8-bit parity transmit test on UART1.\n");
       printf("\nPlease connect a scope to PTC17/UART3_TX (J43 pin 2 on EDSNDC-256).\n");
       printf("Set scope for single edge trigger (rising or falling).\n");
       printf("Press any key when ready.\n");
       ch = in_char();
    }
    
    /* Configure the UART for even parity */
    UART_C1_REG(module) |= UART_C1_PE_MASK; // enable parity
    UART_C1_REG(module) &= ~UART_C1_PT_MASK; // even parity
    
    printf("\n\nUART configured for even parity.\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();
    
    printf("Sending 0x41...\n");
    uart_putchar (module, 0x41); 
    printf("Press any key when scope is ready.\n");
    ch = in_char();
    
    printf("Sending 0x25...\n");
    uart_putchar (module, 0x25); 
    printf("Press any key when scope is ready.\n");
    ch = in_char();

    
    /* Configure the UART for odd parity */
    UART_C1_REG(module) |= UART_C1_PE_MASK; // enable parity
    UART_C1_REG(module) |= UART_C1_PT_MASK; // odd parity
    
    printf("\n\nUART configured for odd parity.\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();
    
    printf("Sending 0x41...\n");
    uart_putchar (module, 0x41); 
    printf("Press any key when scope is ready.\n");
    ch = in_char();
    
    printf("Sending 0x25...\n");
    uart_putchar (module, 0x25); 
    printf("Press any key when scope is ready.\n");
    ch = in_char();

 
    /* Clear parity bits again before executing any other tests */
    UART_C1_REG(module) &= ~(UART_C1_PE_MASK | UART_C1_PT_MASK);    

    return 0;
}
/********************************************************************/
/* UART 9-bit parity transmit test.
 * Tests operation of the UART in 9-bit mode with parity on a single UART
 * (8 data bits + 1 parity bit). This function is the same across UART 
 * instantiations, so only one UART needs to be tested. This function tests 
 * both even and odd parity modes. A scope should be used to verify that the
 * values transmitted match the expected value.
 *
 * Since the test is not self checking it will always return 0 (pass).
 */
int uart_9bit_parity_test()
{
    UART_MemMapPtr module;
    volatile char ch;
    
    /* Determine which UART to use for testing. UART2 or UART3 could
     * be used. We'll use UART2 as long as it isn't used as the TERM_PORT.
     */
    if (TERM_PORT != UART2_BASE_PTR)
    {      
       module = UART2_BASE_PTR;   // set the module pointer
       
       printf("\nStarting UART 9-bit parity transmit test on UART2.\n");
       printf("\nPlease connect a scope to PTE16/UART2_TX (J4 pin 2 on EDSNDC-256.\n");
       printf("Set scope for single edge trigger (rising or falling).\n");
       printf("Press any key when ready.\n");
       ch = in_char();
   }      
    else
    {
#warning "change in the UART port"   
       module = UART1_BASE_PTR;   // set the module pointer
       
       printf("\nStarting UART 9-bit parity transmit test on UART1.\n");
       printf("\nPlease connect a scope to PTC17/UART3_TX (J43 pin 2 on EDSNDC-256).\n");
       printf("Set scope for single edge trigger (rising or falling).\n");
       printf("Press any key when ready.\n");
       ch = in_char();
    }
    
    /* Configure the UART for 9-bit mode */
    UART_C1_REG(module) |= UART_C1_M_MASK;
    
    /* Configure the UART for even parity */
    UART_C1_REG(module) |= UART_C1_PE_MASK; // enable parity
    UART_C1_REG(module) &= ~UART_C1_PT_MASK; // even parity
    
    printf("\n\nUART configured for even parity.\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();
    
    printf("Sending 0x81...\n");
    uart_putchar (module, 0x81); 
    printf("Press any key when scope is ready.\n");
    ch = in_char();
    
    printf("Sending 0x45...\n");
    uart_putchar (module, 0x45); 
    printf("Press any key when scope is ready.\n");
    ch = in_char();

    
    /* Configure the UART for odd parity */
    UART_C1_REG(module) |= UART_C1_PE_MASK; // enable parity
    UART_C1_REG(module) |= UART_C1_PT_MASK; // odd parity
    
    printf("\n\nUART configured for odd parity.\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();
    
    printf("Sending 0x81...\n");
    uart_putchar (module, 0x81); 
    printf("Press any key when scope is ready.\n");
    ch = in_char();
    
    printf("Sending 0x45...\n");
    uart_putchar (module, 0x45); 
    printf("Press any key when scope is ready.\n");
    ch = in_char();

 
    /* Clear M and parity bits again before executing any other tests */
    UART_C1_REG(module) = 0;    

    return 0;
}
/********************************************************************/
/* UART break character transmit test.
 * Tests transmission of all supported break character lengths (10, 11,
 * 12, 13, or 14). This function is the same across UART instantiations, 
 * so only one UART needs to be tested. A scope should be used to verify 
 * that the break characters are transmitted and have the expected length.
 *
 * Since the test is not self checking it will always return 0 (pass).
 */
int uart_break_char_test()
{
    UART_MemMapPtr module;
    volatile char ch;
    int i;

    /* Determine which UART to use for testing. UART2 or UART3 could
     * be used. We'll use UART2 as long as it isn't used as the TERM_PORT.
     */
    if (TERM_PORT != UART2_BASE_PTR)
    {      
       module = UART2_BASE_PTR;   // set the module pointer
       
       printf("\nStarting UART break character transmit test on UART2.\n");
       printf("\nPlease connect a scope to PTE16/UART2_TX (J4 pin 2 on EDSNDC-256.\n");
       printf("Set scope for single edge trigger (falling).\n");
       printf("Press any key when ready.\n");
       ch = in_char();
   }      
    else
    {
#warning "change in the UART port"   
       module = UART1_BASE_PTR;   // set the module pointer
       
       printf("\nStarting UART break character transmit test on UART1.\n");
       printf("\nPlease connect a scope to PTC17/UART3_TX (J43 pin 2 on EDSNDC-256).\n");
       printf("Set scope for single edge trigger (falling).\n");
       printf("Press any key when ready.\n");
       ch = in_char();
    }
    
    /* BRK13 = 0, M = 0, M10 = X, PE = X  gives you a 10-bit break char
     * These are all default register settings, so no init is needed to 
     * setup for the first break character.
     */
    
    printf("\nReady to send 10-bit break character...\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();

    /* Set C2[SBK] bit to queue a break character for transmission */
    UART_C2_REG(module) |= UART_C2_SBK_MASK; 
    
    /* Clear C2[SBK] so that only one break character is sent */
    UART_C2_REG(module) &= ~UART_C2_SBK_MASK;
    
       
    printf("\nReady to send 11-bit break character...\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();

    /* BRK13 = 0, M = 1, M10 = 0, PE = X  gives you a 11-bit break char */
    UART_C1_REG(module) |= UART_C1_M_MASK;   

    /* Set C2[SBK] bit to queue a break character for transmission */
    UART_C2_REG(module) |= UART_C2_SBK_MASK; 
    
    /* Clear C2[SBK] so that only one break character is sent */
    UART_C2_REG(module) &= ~UART_C2_SBK_MASK;
    
    
    printf("\nReady to send a second 11-bit break character...\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();

    /* BRK13 = 0, M = 1, M10 = 1, PE = 0  gives you a 11-bit break char */
    UART_C4_REG(module) |= UART_C4_M10_MASK;   

    /* Set C2[SBK] bit to queue a break character for transmission */
    UART_C2_REG(module) |= UART_C2_SBK_MASK; 
    
    /* Clear C2[SBK] so that only one break character is sent */
    UART_C2_REG(module) &= ~UART_C2_SBK_MASK;
        

    printf("\nReady to send a 12-bit break character...\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();

    /* BRK13 = 0, M = 1, M10 = 1, PE = 1  gives you a 12-bit break char */
    UART_C1_REG(module) |= UART_C1_PE_MASK;   

    /* Set C2[SBK] bit to queue a break character for transmission */
    UART_C2_REG(module) |= UART_C2_SBK_MASK; 
    
    /* Clear C2[SBK] so that only one break character is sent */
    UART_C2_REG(module) &= ~UART_C2_SBK_MASK;
    
    
    printf("\nReady to send a 13-bit break character...\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();

    /* BRK13 = 1, M = 0, M10 = X, PE = X  gives you a 13-bit break char */
    UART_C1_REG(module) &= ~UART_C1_M_MASK;   // clear M bit 
    UART_S2_REG(module) |= UART_S2_BRK13_MASK;   

    /* Set C2[SBK] bit to queue a break character for transmission */
    UART_C2_REG(module) |= UART_C2_SBK_MASK; 
    
    /* Clear C2[SBK] so that only one break character is sent */
    UART_C2_REG(module) &= ~UART_C2_SBK_MASK;

    
    printf("\nReady to send a 14-bit break character...\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();

    /* BRK13 = 1, M = 1, M10 = X, PE = X  gives you a 14-bit break char */
    UART_C1_REG(module) |= UART_C1_M_MASK;    

    /* Set C2[SBK] bit to queue a break character for transmission */
    UART_C2_REG(module) |= UART_C2_SBK_MASK; 
    
    /* Clear C2[SBK] so that only one break character is sent */
    UART_C2_REG(module) &= ~UART_C2_SBK_MASK;
    
    /* Delay to give last break character time to transmit before
     * seting the UART back to default values.
     */
    for( i=0; i < 0x100000; i++);
    
    
    /* All tests complete. Clear any bits that were set during the test */
    UART_C1_REG(module) &= ~UART_C1_M_MASK;    // clear M bit
    UART_C4_REG(module) &= ~UART_C4_M10_MASK;   // clear M10 bit    
    UART_C1_REG(module) &= ~UART_C1_PE_MASK;  // clear PE bit 
    UART_S2_REG(module) &= ~UART_S2_BRK13_MASK;  // clear BRK13 bit 

    
    return 0;    
}
/********************************************************************/
/* UART idle character transmit test.
 * Tests transmission of all supported idle character lengths (10, 11,
 * or 12). This function is the same across UART instantiations, 
 * so only one UART needs to be tested. A scope should be used to verify 
 * that the idle characters are transmitted and have the expected length.
 *
 * Since the test is not self checking it will always return 0 (pass).
 */
int uart_idle_char_test()
{
    UART_MemMapPtr module;
    volatile char ch;
    int i;

    /* Determine which UART to use for testing. UART2 or UART3 could
     * be used. We'll use UART2 as long as it isn't used as the TERM_PORT.
     */
    if (TERM_PORT != UART2_BASE_PTR)
    {      
       module = UART2_BASE_PTR;   // set the module pointer
       
       printf("\nStarting UART idle character transmit test on UART2.\n");
       printf("\nPlease connect a scope to PTE16/UART2_TX (J4 pin 2 on EDSNDC-256.\n");
       printf("Set scope for single edge trigger (rising).\n");
       printf("Press any key when ready.\n");
       ch = in_char();
   }      
    else
    {
#warning "change in the UART port"   
       module = UART1_BASE_PTR;   // set the module pointer
       
       printf("\nStarting UART idle character transmit test on UART1.\n");
       printf("\nPlease connect a scope to PTC17/UART3_TX (J43 pin 2 on EDSNDC-256).\n");
       printf("Set scope for single edge trigger (rising).\n");
       printf("Press any key when ready.\n");
       ch = in_char();
    }
    
    /* M = 0, M10 = X, PE = X  gives you a 10-bit idle char
     * These are all default register settings, so no init is needed to 
     * setup for the first idle character.
     */
    
    printf("\nReady to send 10-bit idle character...\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();

    UART_D_REG(module) = 0x55;
    
    for(i=0; i< 0x400; i++); // delay to allow transmission to start
    
    /* Clear C2[TE] bit to queue an idle character for transmission */
    UART_C2_REG(module) &= ~UART_C2_TE_MASK; 
    
    /* Set C2[TE] to re-enable transmitter - sends an idle character */
    UART_C2_REG(module) |= UART_C2_TE_MASK;
    
    UART_D_REG(module) = 0x55;

    
    printf("\nReady to send 11-bit idle character...\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();

    /* M = 1, M10 = 0, PE = X  gives you a 11-bit idle char */
    UART_C1_REG(module) |= UART_C1_M_MASK;    

    UART_D_REG(module) = 0x55;
    
    for(i=0; i< 0x400; i++); // delay to allow transmission to start
    
    /* Clear C2[TE] bit to queue an idle character for transmission */
    UART_C2_REG(module) &= ~UART_C2_TE_MASK; 
    
    /* Set C2[TE] to re-enable transmitter - sends an idle character */
    UART_C2_REG(module) |= UART_C2_TE_MASK;
    
    UART_D_REG(module) = 0x55;

    
    printf("\nReady to send 11-bit idle character...\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();

    /* M = 1, M10 = 1, PE = 0  gives you a 11-bit idle char */
    UART_C4_REG(module) |= UART_C4_M10_MASK;     

    UART_D_REG(module) = 0x55;
    
    for(i=0; i< 0x400; i++); // delay to allow transmission to start
    
    /* Clear C2[TE] bit to queue an idle character for transmission */
    UART_C2_REG(module) &= ~UART_C2_TE_MASK; 
    
    /* Set C2[TE] to re-enable transmitter - sends an idle character */
    UART_C2_REG(module) |= UART_C2_TE_MASK;
    
    UART_D_REG(module) = 0x55;

        
    printf("\nReady to send 12-bit idle character...\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();

    /* M = 1, M10 = 1, PE = 1 gives you a 12-bit idle char */
    UART_C1_REG(module) |= UART_C1_PE_MASK;     

    UART_D_REG(module) = 0x55;
    
    for(i=0; i< 0x400; i++); // delay to allow transmission to start
    
    /* Clear C2[TE] bit to queue an idle character for transmission */
    UART_C2_REG(module) &= ~UART_C2_TE_MASK; 
    
    /* Set C2[TE] to re-enable transmitter - sends an idle character */
    UART_C2_REG(module) |= UART_C2_TE_MASK;
    
    UART_D_REG(module) = 0x55;
    
    /* Delay to allow last test to complete */    
    for(i=0; i< 0x10000; i++); 
    

    
    
    /* All tests complete. Clear any bits that were set during the test */
    UART_C1_REG(module) &= ~UART_C1_M_MASK;    // clear M bit
    UART_C4_REG(module) &= ~UART_C4_M10_MASK;   // clear M10 bit    
    UART_C1_REG(module) &= ~UART_C1_PE_MASK;  // clear PE bit 
    
    return 0;    
}
/********************************************************************/
/* UART transmitter disable test.
 * The UART should continue to send the current character even if the
 * transmitter is disabled  during the transmission. A scope should
 * be used to watch the character transmission and make sure it is 
 * complete.
 *
 * Since the test is not self checking it will always return 0 (pass).
 */
int uart_tx_disable_test()
{
    UART_MemMapPtr module;
    int i;

    printf("\nStarting setup for UART transmitter disable test.\n");

    /* Determine which UART to use for testing. UART2 or UART3 could
     * be used. We'll use UART2 as long as it isn't used as the TERM_PORT.
     */
    if (TERM_PORT != UART2_BASE_PTR)
    {      
       module = UART2_BASE_PTR;   // set the module pointer
       
       printf("\nPlease connect a scope to PTE16/UART2_TX (J4 pin 2 on EDSNDC-256.\n");
       printf("Set scope for single edge trigger (rising).\n");
       printf("Press any key when ready.\n");
       (void)in_char();
   }      
    else
    {
       printf("\nNo more available UARTs\n");
       (void)in_char();
    }
    
    /* NOTE: This first test should fail to send a full character right now.
     * This is a defect on current silicon.
     */
    
#if 1
    
    printf("Testing disabling the transmitter during character transmission.\n");
    printf("\nReady to send a character and disable transmitter during send...\n");
    printf("Press any key when scope is ready.\n");
    (void)in_char();
    
    UART_D_REG(module) = 0x55;

    for(i=0; i< 0x300; i++); // delay to allow transmission to start
    
    /* Clear C2[TE] bit to disable the transmitter */
    UART_C2_REG(module) &= ~UART_C2_TE_MASK;   
    
#endif    
    
    
    /* Now send a character again, but use the uart_putchar function
     * instead of writing directly to the data register. The function
     * will read the S1 register to make sure that there is space in
     * the FIFO for a character before writing to the data register.
     * This read of S1 should allow the character to finish transmit
     * when the transmitter is disabled.
     */

    
    printf("Testing reading S1 before disabling the transmitter during character transmission.\n");
    printf("\nReady to send a character and disable transmitter during send...\n");
    printf("Press any key when scope is ready.\n");
    (void)in_char();

    /* Re-enable the transmitter */
    /* Put a breakpoint here. Previous character will finish transmitting 
     * when the transmitter is turned back on.
     */
    
    UART_C2_REG(module) |= UART_C2_TE_MASK;   
    
    uart_putchar (module, 0x55);

    for(i=0; i< 0x100; i++); // delay to allow transmission to start
    
    /* Clear C2[TE] bit to disable the transmitter */
    UART_C2_REG(module) &= ~UART_C2_TE_MASK; 
    
    for(i=0; i< 0x10000; i++); // delay to allow test to complete
 
    /* Test complete. Re-enable the transmitter before moving on to next test */
    UART_C2_REG(module) |= UART_C2_TE_MASK; 
        
    return 0;    
}   
/********************************************************************/

