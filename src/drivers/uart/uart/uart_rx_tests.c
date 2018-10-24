/*
 * File:		uart_rx_tests.c
 * Purpose:		UART receive tests. All of these test use one UART
 *              as a transmitter sending to a second UART that is
 *              the reciever being tested.
 *
 */

#include "common.h"
#include "uart.h"
#include "uart_tests.h"

/********************************************************************/
/* UART receiver wake-up tests.
 * Tests operation of programmable receiver wakeup feature of the UART.
 * These functions are the same across UART instantiations, so only one 
 * UART needs to be tested. 
 * 
 * In order to make sure that data can be sent and received without
 * delays between characters, UART0 and UART1 will be used for the
 * testing with the FIFO enabled. 
 *
 * If only data values sent after the programmed wakeup (either an idle
 * line condition or an address-mark) are received, then the test passes
 * and function returns 0 otherwise returns the number of errors. 
 */
int uart_rx_wakeup_test()
{
    UART_MemMapPtr tx_module, rx_module;
    char ch;
    int i, j, error = 0;    
    
        
    printf("\n\nStarting setup for receiver wakeup tests.\n");

    /* Test always uses UART0 as the transmitter since it needs a 
     * UART with a FIFO. A variable is still being used so that this
     * code can be ported to other devices that might support FIFOs 
     * on more or other UARTs more easily.
     */
    tx_module = UART0_BASE_PTR;

    /* Test always uses UART0 as the receiver since it needs a 
     * UART with a FIFO. A variable is still being used so that this
     * code can be ported to other devices that might support FIFOs 
     * on more or other UARTs more easily.
     */
    rx_module = UART0_BASE_PTR;//UART1_BASE_PTR;
    
    /* Give directions on the connections that need to be made for the test */
    printf("\n\nMake the following connections on a DC:\n\n");
    printf("Connect (UART0_TX) to (UART0_RX)\n");
    
    printf("\nPress any key when ready.\n");
    ch = in_char();
        
    printf("Setup complete. Starting receiver wakeup tests...\n");

    /* Enable the FIFOs */
    UART_PFIFO_REG(tx_module) |= UART_PFIFO_TXFE_MASK;
    UART_PFIFO_REG(rx_module) |= UART_PFIFO_RXFE_MASK;
    
    
    /* Test idle-line wakeup condition. For this test we will send four
     * characters back-to-back, delay (send an idle), and send four
     * more characters. Only the last four characters should be received.
     */
    
    /* Make sure the receiver is configured for an idle line wakeup */
    UART_C1_REG(rx_module) &= ~UART_C1_WAKE_MASK;
    
    /* Temporarily disable the transmitter so that the TxFIFO can be 
     * filled without sending any of the data out yet.
     */
    UART_C2_REG(tx_module) &= ~UART_C2_TE_MASK;
    
    /* Queue five characters into the TxFIFO. The first character is needed
     * to ensure that the line is not IDLE when the receiver wakeup is enabled.
     */
    for( i=0; i < 5; i++)
    {      
        /* Write data into the FIFO */
        UART_D_REG(tx_module) = i;
    }
    
    /* Enable the transceiver to start sending characters */
    UART_C2_REG(tx_module) |= UART_C2_TE_MASK;
    
    /* Poll the RAF flag on the receiver to make sure first character has 
     * started sending before enabling the receiver wakeup.
     */
    while ( !(UART_S2_REG(rx_module) & UART_S2_RAF_MASK));
    
    /* Enable the receiver wakeup function */
    UART_C2_REG(rx_module) |= UART_C2_RWU_MASK;
    
    /* Wait for the TxFIFO to empty out */
    while( UART_TCFIFO_REG(tx_module) > 0);
    
    /* Delay a bit to make sure there is enough time to detect an idle before
     * sending more characters.
     */
    for (i=0; i < 0x2000; i++);
    
    /* Send three more characters. These should actually be received. */
    for( i=5; i < 8; i++)
    {      
        /* Write data into the FIFO */
        UART_D_REG(tx_module) = i;
    }
    
    /* Test that RWU is cleared indicating receiver woke up */
    if ( (UART_C2_REG(rx_module) & UART_C2_RWU_MASK))
    {
        error++;
        printf("\nERR! RWU detected set when it should be cleared.");
        printf("\nC2 = 0x%02X", UART_C2_REG(rx_module));
    }

    /* Wait for RxFIFO to have 3 bytes - last four sent */
    while( UART_RCFIFO_REG(rx_module) < 3);
    
    
    /* Make sure we got the right data */

    /* Test next four bytes. Should be 5, 6, 7, 8. 1, 2, 3, and 4 should have been ignored. */
    for( i=5; i < 8; i++)
    {      
        ch = UART_D_REG(rx_module);
    
        if ( ch != i)
        {
            error++;
            printf("\nERR! Incorrect data detected. Expected: 0x%02X Received: 0x%02X", i, ch);
        }     
    }
    

    /* Test address mark wakeup condition. For this test we will send four
     * characters with delay between all of them, then send an address + 3
     * more characters. Only the last four characters should be received.
     */
    
    /* Configure the receiver for address mark wakeup */
    UART_C1_REG(rx_module) |= UART_C1_WAKE_MASK;
    
    /* Enable the receiver wakeup function */
    UART_C2_REG(rx_module) |= UART_C2_RWU_MASK;

    /* Send four bytes w/o address mark - the should be ignored */
    for( i=1; i < 5; i++)
    {      
        /* Write data into the FIFO */
        UART_D_REG(tx_module) = i;
        
        /* Delay to allow character to transmit */
        for (j=0; j < 0x1000; j++);
    }
    
    /* Now send an address character (MSB set) to wake up the receiver */
    UART_D_REG(tx_module) = 0x80;
    
    /* Send three more characters. These should actually be received. */
    for( i=5; i < 8; i++)
    {      
        /* Write data into the FIFO */
        UART_D_REG(tx_module) = i;

        /* Delay to allow character to transmit */
        for (j=0; j < 0x1000; j++);
    }
    
    /* Test that RWU is cleared indicating receiver woke up */
    if ( (UART_C2_REG(rx_module) & UART_C2_RWU_MASK))
    {
        error++;
        printf("\nERR! RWU detected set when it should be cleared.");
        printf("\nC2 = 0x%02X", UART_C2_REG(rx_module));
    }

    /* Wait for RxFIFO to have 4 bytes - last four sent */
    while( UART_RCFIFO_REG(rx_module) < 4)
              printf("\nRCFIFO = 0x%02X", UART_RCFIFO_REG(rx_module));

    
    
    /* Make sure we got the right data */

    /* Test first byte - Should be 0x80 (the address sent) */
    ch = UART_D_REG(rx_module);
    
    if ( ch != 0x80)
    {
        error++;
        printf("\nERR! Incorrect data detected. Expected: 0x80 Received: 0x%02X", ch);
    }

    /* Test next three bytes. Should be 5, 6, 7. 1, 2, 3, and 4 should have been ignored. */
    for( i=5; i < 8; i++)
    {      
        ch = UART_D_REG(rx_module);
    
        if ( ch != i)
        {
            error++;
            printf("\nERR! Incorrect data detected. Expected: 0x%02X Received: 0x%02X", i, ch);
        }     
    }
    
    
    /* Test complete. Restore default values before moving on to next test. */

    /* Configure the receiver for idle line wakeup */
    UART_C1_REG(rx_module) &= ~UART_C1_WAKE_MASK;

    /* Disable the FIFOs */
    UART_PFIFO_REG(tx_module) &= ~UART_PFIFO_TXFE_MASK;
    UART_PFIFO_REG(rx_module) &= ~UART_PFIFO_RXFE_MASK;
    
    return error;    
}    

/********************************************************************/
