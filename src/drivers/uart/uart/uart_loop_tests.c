/*
 * File:		uart_loop_tests.c
 * Purpose:		UART loop mode tests (single wire mode
 *                      and loopback mode tests.
 *
 */

#include "common.h"
#include "uart.h"
#include "uart_tests.h"

/********************************************************************/
/* UART loop mode test.
 * Tests operation of the UART module in loopback mode. This function
 * is the same across UART instantiations, so only one UART needs to be
 * tested. 
 *
 * If all data values are received as expected, then the test passes 
 * and function returns 0 otherwise returns the number of errors. 
 */
int uart_loop_mode_test()
{
    UART_MemMapPtr module;
    char tx_ch, rx_ch;
    int error = 0;
    
    tx_ch = 0x55;  //initialize data value that will be used for the test
        
    /* Determine which UART to use for testing. UART2 or UART1 could be used. 
     * We'll use UART2 as long as it isn't used as the TERM_PORT.
     */

    if (TERM_PORT != UART2_BASE_PTR)
       module = UART2_BASE_PTR;   // set the module pointer     
    else
       module = UART1_BASE_PTR;   // set the module pointer

    printf("\nStarting UART loopback mode test.\n");
    
    /* Configure the UART for loopback mode */
    UART_C1_REG(module) |= UART_C1_LOOPS_MASK;
    
    /* Send data */
    uart_putchar(module, tx_ch);
    
    /* Check for received data */
    rx_ch = uart_getchar(module);
    
    /* Make sure the sent and received data are the same */
    if( rx_ch != tx_ch)
    {
        error++;
        printf("\nERR! Incorrect data received. Expected: 0x%02X Recieved: 0x%02X", tx_ch, rx_ch);
    }
    
    /* Disable loopback mode */
    UART_C1_REG(module) &= ~UART_C1_LOOPS_MASK;

    return error;    
} 

/********************************************************************/
   
