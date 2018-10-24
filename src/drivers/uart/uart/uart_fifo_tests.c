/*
 * File:		uart_fifo_tests.c
 * Purpose:     UART FIFO tests. All of these test use one UART
 *              as a transmitter sending to a second UART in order to test
 *              that the TxFIFOs and RxFIFOs are both working. A scope should
 *              also be used to double check correct operation.
 *
 */

#include "common.h"
#include "uart.h"
#include "uart_tests.h"

/* Globals from sysinit.c */
extern int core_clk_khz;
extern int core_clk_mhz;
extern int periph_clk_khz;

/********************************************************************/
/* UART FIFO flush test.
 * Tests TXFLUSH and RXFLUSH operation. The TXFLUSH test will load
 * bytes into the TxFIFO, then flush the FIFO. Once the FIFO is flushed
 * the TCFIFO value is checked to make sure it is zero. Then the FIFO
 * is reloaded with new values that are transmitted. 
 * 
 * The RXFLUSH test will receive bytes, then flush the FIFO. Once the FIFO
 * is flushed the RCFIFO value is check to make sure it is zero. Then a
 * new set of data values will be received. Data values are checked to make
 * sure they match the second data set and the first data set that was 
 * flushed is completely gone.
 *
 * If all receive data values match expected values and TCFIFO and RCFIFO
 * checks also match the expected values, then the test returns 0 (pass).
 * Otherwise the function returns the number of errors.
 */
int uart_fifo_flush_test()
{
    UART_MemMapPtr tx_module, rx_module;
    char ch;
    int error = 0;        
    volatile uint32 i, temp;
    
    printf("\n\nStarting setup for UART FIFO flush tests.\n");
    
    /* Test always uses UART0 as the transmitter since it needs a 
     * UART with a FIFO. A variable is still being used so that this
     * code can be ported to other devices that might support FIFOs 
     * on more or other UARTs more easily.
     */
    tx_module = UART0_BASE_PTR;

    /* Test always uses UART1 as the receiver since it needs a 
     * UART with a FIFO. A variable is still being used so that this
     * code can be ported to other devices that might support FIFOs 
     * on more or other UARTs more easily.
     */
    rx_module = UART1_BASE_PTR;
    
    /* Give directions on the connections that need to be made for the test */
    printf("\n\nMake the following connections on a EDSNDC-256 CPU card:\n\n");
    printf("Connect J53 pin 1 (UART0_TX) to J7 pin 5 (UART1_RX)\n");
    
    printf("\nPress any key when ready.\n");
    ch = in_char();
        
    printf("Setup complete. Starting UART FIFO flush tests...\n");

    /* Enable the FIFOs */
    UART_PFIFO_REG(tx_module) |= UART_PFIFO_TXFE_MASK;
    UART_PFIFO_REG(rx_module) |= UART_PFIFO_RXFE_MASK;
    
    /* Temporarily disable the transmitter so that the TxFIFO can be 
     * filled without sending any of the data out yet.
     */
    UART_C2_REG(tx_module) &= ~UART_C2_TE_MASK;
    
    /* Loop to fill the entire TxFIFO */
    for( i=1; i < 9; i++)
    {      
        /* Write data into the FIFO */
        UART_D_REG(tx_module) = i;
    }
    
    /* Test to make sure TCFIFO is 8 (FIFO full) */
    if ( UART_TCFIFO_REG(tx_module) != 8)
    {
        error++;
        printf("\nERR! TCFIFO value is incorrect. Expected: 0x8 Read: 0x%02X", UART_TCFIFO_REG(tx_module));
    }
    
    /* Flush the TxFIFO */
    UART_CFIFO_REG(tx_module) |= UART_CFIFO_TXFLUSH_MASK;
    
    /* Test to make sure TCFIFO is 0 now (FIFO flushed) */
    if ( UART_TCFIFO_REG(tx_module) != 0)
    {
        error++;
        printf("\nERR! TCFIFO value is incorrect. Expected: 0x0 Read: 0x%02X", UART_TCFIFO_REG(tx_module));
    }
    
    /* Loop to fill the entire TxFIFO with a new data set */
    for( i=0x11; i < 0x19; i++)
    {      
        /* Write data into the FIFO */
        UART_D_REG(tx_module) = i;
    }

    
    /* Set the Rx watermark to 8 so that we can detect when the transmit is complete */
    UART_RWFIFO_REG(rx_module) = 8;
    
    /* Now re-enable the transmitter. This should cause all 8 bytes to transmit
     * back to back.
     */
    UART_C2_REG(tx_module) |= UART_C2_TE_MASK;
    
    /* Wait for RDRF flag to set indicating transmit is complete */
    while( !(UART_S1_REG(rx_module) & UART_S1_RDRF_MASK));
    
    
     /* Test to make sure RCFIFO is 8 (FIFO full) */
    if ( UART_RCFIFO_REG(rx_module) != 8)
    {
        error++;
        printf("\nERR! RCFIFO value is incorrect. Expected: 0x8 Read: 0x%02X", UART_RCFIFO_REG(rx_module));
    }
    
    /* Flush the RxFIFO */
    UART_CFIFO_REG(rx_module) |= UART_CFIFO_RXFLUSH_MASK;
    
    /* Test to make sure RCFIFO is 0 now (FIFO flushed) */
    if ( UART_RCFIFO_REG(rx_module) != 0)
    {
        error++;
        printf("\nERR! RCFIFO value is incorrect. Expected: 0x0 Read: 0x%02X", UART_RCFIFO_REG(rx_module));
    }
    

    /* Clear the RDRF flag - flushing the FIFO won't cause the flag to clear */
    temp = UART_S1_REG(rx_module);
    ch = UART_D_REG(rx_module);    
    
    /* Flush the FIFO again to reset the RxFIFO pointers */
    UART_CFIFO_REG(rx_module) |= UART_CFIFO_RXFLUSH_MASK;
    
    
    /* Temporarily disable the transmitter so that the TxFIFO can be 
     * filled without sending any of the data out yet.
     */
    UART_C2_REG(tx_module) &= ~UART_C2_TE_MASK;

    /* Loop to fill the entire TxFIFO with a new data set (third set) */
    for( i=0x21; i < 0x29; i++)
    {      
        /* Write data into the FIFO */
        UART_D_REG(tx_module) = i;
    }

    /* Re-enable the transmitter so data will be sent out. */
    UART_C2_REG(tx_module) |= UART_C2_TE_MASK;
    
    
    /* Wait for RDRF flag to set indicating transmit is complete */
    while( !(UART_S1_REG(rx_module) & UART_S1_RDRF_MASK));

    /* Loop to empty the entire RxFIFO and make sure the FIFO flush hasn't
     * had any permanent effect.
     */
    for( i=1; i < 9; i++)
    {          
        /* Read data from the FIFO */
        ch = UART_D_REG(rx_module);
        
        /* Test the data value to make sure it matches the expected value */
        if (ch != (i+0x20))
        {
            error++;
            printf("\nERR! Incorrect data value received. Expected: 0x%02X Read: 0x%02X", (i+0x20), ch);
        }                         
    
        /* Test RCFIFO value to make sure it decremented */
        if (UART_RCFIFO_REG(rx_module) != (8 - i))
        {
            error++;
            printf("\nERR! RCFIFO value is incorrect. Expected: 0x%02X Read: 0x%02X", (8-i), UART_RCFIFO_REG(rx_module));
        }
    }       
    
    /* Disable the FIFOs */
    UART_PFIFO_REG(tx_module) &= ~UART_PFIFO_TXFE_MASK;
    UART_PFIFO_REG(rx_module) &= ~UART_PFIFO_RXFE_MASK;
        
    /* Clear the Rx watermark */
    UART_RWFIFO_REG(rx_module) = 0;
    
    return error;
}
/********************************************************************/
/* UART FIFO size test.
 * Tests send and receive of 8 characters using the TxFIFO and RxFIFO.
 * This test will also check that FIFO counts are reported as expected
 * by the TCFIFO and RCFIFO registers
 *
 * If receive data matches the transmitted data, and all TCFIFO and RCFIFO
 * values match the current expected value then this indicates the FIFO
 * is working correctly and test returns 0 (pass) otherwise the test
 * returns the number of errors. A scope should also be used to monitor 
 * data flow and make sure that data is transmitting with the expected
 * timing.
 */
int uart_fifo_size_test()
{
    UART_MemMapPtr tx_module, rx_module;
    char ch;
    int error = 0;        
    uint32 i;
    
    printf("\n\nStarting setup for UART FIFO size tests.\n");
    
    /* Test always uses UART0 as the transmitter since it needs a 
     * UART with a FIFO. A variable is still being used so that this
     * code can be ported to other devices that might support FIFOs 
     * on more or other UARTs more easily.
     */
    tx_module = UART0_BASE_PTR;

    /* Test always uses UART1 as the receiver since it needs a 
     * UART with a FIFO. A variable is still being used so that this
     * code can be ported to other devices that might support FIFOs 
     * on more or other UARTs more easily.
     */
    rx_module = UART1_BASE_PTR;
    
    /* Give directions on the connections that need to be made for the test */
    printf("\n\nMake the following connections on a EDSNDC-256 CPU card:\n\n");
    printf("Connect J53 pin 1 (UART0_TX) to J7 pin 5 (UART1_RX)\n");
    
    printf("\nPress any key when ready.\n");
    ch = in_char();
        
    printf("Setup complete. Starting UART FIFO size tests...\n");

    /* Enable the FIFOs */
    UART_PFIFO_REG(tx_module) |= UART_PFIFO_TXFE_MASK;
    UART_PFIFO_REG(rx_module) |= UART_PFIFO_RXFE_MASK;
    
    /* Temporarily disable the transmitter so that the TxFIFO can be 
     * filled without sending any of the data out yet.
     */
    UART_C2_REG(tx_module) &= ~UART_C2_TE_MASK;
    
    /* Loop to fill the entire TxFIFO and test TCFIFO values */
    for( i=1; i < 9; i++)
    {          
        /* Write data into the FIFO */
        UART_D_REG(tx_module) = i;
        
//        printf("\nTCFIFO = 0x%02X", UART_TCFIFO_REG(tx_module));
    
        /* Test TCFIFO value to make sure it incremented */
        if (UART_TCFIFO_REG(tx_module) != i)
        {
            error++;
            printf("\nERR! TCFIFO value is incorrect. Expected: 0x%02X Read: 0x%02X", i, UART_TCFIFO_REG(tx_module));
        }
    }   
    
    
    /* Set the Rx watermark to 8 so that we can detect when the transmit is complete */
    UART_RWFIFO_REG(rx_module) = 8;
    
    /* Now re-enable the transmitter. This should cause all 8 bytes to transmit
     * back to back.
     */
    UART_C2_REG(tx_module) |= UART_C2_TE_MASK;
    
    /* Wait for RDRF flag to set indicating transmit is complete */
    while( !(UART_S1_REG(rx_module) & UART_S1_RDRF_MASK));

    /* Loop to empty the entire RxFIFO and test data and RCFIFO values */
    for( i=1; i < 9; i++)
    {          
        /* Read data from the FIFO */
        ch = UART_D_REG(rx_module);
        
//        printf("\nRCFIFO = 0x%02X", UART_RCFIFO_REG(rx_module));

        /* Test the data value to make sure it matches the expected value */
        if (ch != i)
        {
            error++;
            printf("\nERR! Incorrect data value received. Expected: 0x%02X Read: 0x%02X", i, ch);
        }                         
    
        /* Test RCFIFO value to make sure it decremented */
        if (UART_RCFIFO_REG(rx_module) != (8 - i))
        {
            error++;
            printf("\nERR! RCFIFO value is incorrect. Expected: 0x%02X Read: 0x%02X", (8-i), UART_RCFIFO_REG(rx_module));
        }
    }   
    
    
    /* Disable the FIFOs */
    UART_PFIFO_REG(tx_module) &= ~UART_PFIFO_TXFE_MASK;
    UART_PFIFO_REG(rx_module) &= ~UART_PFIFO_RXFE_MASK;
        
    /* Clear the Rx watermark */
    UART_RWFIFO_REG(rx_module) = 0;
    
    return error;
}
/********************************************************************/
/* UART TxFIFO empty and RxFIFO empty tests.
 * Tests operation of the TXEMPT and RXEMPT status bits. These flags 
 * should be set by default when the tests starts (since the FIFOs
 * should start off empty). So the first test makes sure that they
 * start off set.
 * 
 * The next step is to test that the bits set as the FIFOs go
 * empty. For this one UART will be configured as a transmitter that will
 * only send data when the TxCTS input is asserted. A second UART will be 
 * configured as the receiver that asserts RxCTS when it can accept another
 * charcter. Using flow control will allow the test to hold the FIFOs in a
 * partially filled state make sure the flags are cleared, and then test
 * that they clear once the FIFOs empty.
 *
 * If all checks of the RXEMPT and TXEMPT flags match the expected values,
 * then the test returns 0 (pass). Otherwise function returns the number
 * of errors.
 */
int uart_txempt_rxempt_test()
{
    UART_MemMapPtr tx_module, rx_module;
    volatile char ch;
    int error = 0;        
    uint32 i; 
    
    printf("\n\nStarting setup for UART FIFO TxEMPT and RxEMPT tests.\n");
    
    /* Test always uses UART0 as the transmitter since it needs a 
     * UART with a FIFO to function correctly. A variable is still
     * being used so that this code can be ported to other devices
     * that might support FIFOs on more or other UARTs more easily.
     */
    tx_module = UART0_BASE_PTR;

    /* Test always uses UART1 as the receiver since it needs a 
     * UART with a FIFO to function correctly. A variable is still
     * being used so that this code can be ported to other devices
     * that might support FIFOs on more or other UARTs more easily.
     */
    rx_module = UART1_BASE_PTR;
    
    /* Give directions on the connections that need to be made for the test */
    printf("\n\nMake the following connections on a EDSNDC-256 CPU card:\n\n");
    printf("Connect J53 pin 1 (UART0_TX) to J7 pin 5 (UART1_RX)\n");
    printf("Connect J2 pin 5 (UART0_CTS) to J7 pin 11 (UART1_RTS)\n");
    
    printf("\nPress any key when ready.\n");
    ch = in_char();
        
    printf("Setup complete. Starting UART TxEMPT and RxEMPT tests...\n");
    
    
    /* Test that both bits are set right now */
        
    /* Test that the TxEMPT bit is set in the transmitter */
    if ( !(UART_SFIFO_REG(tx_module) & UART_SFIFO_TXEMPT_MASK))
    {
        error++;
        printf("\nERR! TXEMPT is cleared when it should be set.");
    }        

    /* Test that the RxEMPT bit is set in the receiver */
    if ( !(UART_SFIFO_REG(rx_module) & UART_SFIFO_RXEMPT_MASK))
    {
        error++;
        printf("\nERR! RXEMPT is cleared when it should be set.");
    }        


    /* Enable flow control and send enough data to fill the receiver to 
     * the watermark. Then load up additional bits that will be held
     * the the TxFIFO while /CTS is negated. Then we can test that
     * both TXEMPT and RXEMPT are cleared.
     */

    /* Enable TxCTS signal on the transmiter */
    UART_MODEM_REG(tx_module) |= UART_MODEM_TXCTSE_MASK; 
    
    /* Enable RxRTS signal on the receiver */
    UART_MODEM_REG(rx_module) |= UART_MODEM_RXRTSE_MASK;
      
    /* Enable the TxFIFO on the transmitter */
    UART_PFIFO_REG(tx_module) |= UART_PFIFO_TXFE_MASK;
    
    /* Enable the RxFIFO on the receiver */
    UART_PFIFO_REG(rx_module) |= UART_PFIFO_RXFE_MASK;
    
    /* Set the Tx watermark */
    UART_RWFIFO_REG(rx_module) = 0x4;

    /* Set the Rx watermark */
    UART_RWFIFO_REG(rx_module) = 0x2;
    
    /* Send two bytes to fill the receiver  */
    UART_D_REG(tx_module) = 0x1;
    UART_D_REG(tx_module) = 0x2;

    /* Need delay for this test to work right now. Otherwise the transmitter
     * will start sending the second byte before the receiver has a chance
     * to negate RTS. I'm talking to design about changing the timing for
     * the signals so that no delay is needed.
     */
    for(i=0; i < 0x1000; i++);
    
    /* Now queue two more bytes to send. These should be held in the
     * TxFIFO since the receiver is already at the watermark.
     */

    UART_D_REG(tx_module) = 0x3;
    UART_D_REG(tx_module) = 0x4;


    /* Test that both bits are cleared now */
        
    /* Test that the TxEMPT bit is cleared in the transmitter */
    if ((UART_SFIFO_REG(tx_module) & UART_SFIFO_TXEMPT_MASK))
    {
        error++;
        printf("\nERR! TXEMPT is set when it should be cleared.");
    }        

    /* Test that the RxEMPT bit is cleared in the receiver */
    if ((UART_SFIFO_REG(rx_module) & UART_SFIFO_RXEMPT_MASK))
    {
        error++;
        printf("\nERR! RXEMPT is set when it should be cleared.");
    }        
 
    
    /* Read two bytes from the receiver. This should allow the transmitter
     * to empty out.
     */

    ch = UART_D_REG(rx_module); 
    ch = UART_D_REG(rx_module); 
    
    /* Delay to allow transmit to complete */
    for(i=0; i < 0x1000; i++);    

    /* Test that the TxEMPT bit is set in the transmitter */
    if ( !(UART_SFIFO_REG(tx_module) & UART_SFIFO_TXEMPT_MASK))
    {
        error++;
        printf("\nERR! TXEMPT is cleared when it should be set.");
    }        
    
    
    /* Read the last two bytes from the receiver. This should empty
     * out the reciever.
     */
    
    ch = UART_D_REG(rx_module); 
    ch = UART_D_REG(rx_module); 
    

    /* Test that the RxEMPT bit is set in the receiver */
    if ( !(UART_SFIFO_REG(rx_module) & UART_SFIFO_RXEMPT_MASK))
    {
        error++;
        printf("\nERR! RXEMPT is cleared when it should be set.");
    }        
    
     
    /* All tests complete. Put the UARTs back into default state */   
    
    
    /* Disable the TxFIFO on the transmitter */
    UART_PFIFO_REG(tx_module) &= ~UART_PFIFO_TXFE_MASK;
    
    /* Disable the RxFIFO on the receiver */
    UART_PFIFO_REG(rx_module) &= ~UART_PFIFO_RXFE_MASK;
    
    /* Clear the Tx watermark */
    UART_TWFIFO_REG(tx_module) = 0x0;

    /* Clear the Rx watermark */
    UART_RWFIFO_REG(rx_module) = 0x0;    
    
    
    /* Disable TxCTS signal on the transmiter */
    UART_MODEM_REG(tx_module) &= ~UART_MODEM_TXCTSE_MASK; 
    
    /* Disable RxRTS signal on the receiver */
    UART_MODEM_REG(rx_module) &= ~UART_MODEM_RXRTSE_MASK;
    
    return error;
}
/********************************************************************/
/* UART FIFO watermark test.
 * Tests each of the watermark values for both the TxFIFO and RxFIFO.
 * This test will also check that FIFO counts are reported as expected
 * by the TCFIFO and RCFIFO registers
 *
 * If TDRE and RDRF flags match expected values and all TCFIFO and RCFIFO
 * values match the current expected value then this indicates the FIFO
 * is working correctly and test returns 0 (pass) otherwise the test
 * returns the number of errors. 
 */
int uart_fifo_wm_test()
{
    UART_MemMapPtr tx_module, rx_module;
    volatile char ch;
    int error = 0;        
    uint32 i,j;
    volatile uint8 temp;
    
    printf("\n\nStarting setup for UART FIFO watermark tests.\n");
    
    /* Test always uses UART0 as the transmitter since it needs a 
     * UART with a FIFO. A variable is still being used so that this
     * code can be ported to other devices that might support FIFOs 
     * on more or other UARTs more easily.
     */
    tx_module = UART0_BASE_PTR;

    /* Test always uses UART1 as the receiver since it needs a 
     * UART with a FIFO. A variable is still being used so that this
     * code can be ported to other devices that might support FIFOs 
     * on more or other UARTs more easily.
     */
    rx_module = UART1_BASE_PTR;
    
    /* Give directions on the connections that need to be made for the test */
    printf("\n\nMake the following connections on a EDSNDC-256 CPU card:\n\n");
    printf("Connect J53 pin 1 (UART0_TX) to J7 pin 5 (UART1_RX)\n");
    
    printf("\nPress any key when ready.\n");
    ch = in_char();
        
    printf("Setup complete. Starting UART FIFO watermark tests...\n");
        
    /* Enable the FIFOs */
    UART_PFIFO_REG(tx_module) |= UART_PFIFO_TXFE_MASK;
    UART_PFIFO_REG(rx_module) |= UART_PFIFO_RXFE_MASK;
    
    
    /* Test all possible values for the watermarks */
    for( i=1; i < 8; i++)
    {     
        /* Set the Tx watermark */
        UART_TWFIFO_REG(tx_module) = i;
      
        /* Test that TDRE is set to start - space in TxFIFO */
        if (!(UART_S1_REG(tx_module) & UART_S1_TDRE_MASK))
        {
           error++;
           printf("\nERR! TDRE is cleared when it should be set.");
           printf("\nS1 = 0x%02X",UART_S1_REG(tx_module));
        }        
      
        /* Temporarily disable the transmitter so that the TxFIFO can be 
         * filled without sending any of the data out yet.
         */
        UART_C2_REG(tx_module) &= ~UART_C2_TE_MASK;
    
        /* Loop to fill the TxFIFO to the watermark level */
        for( j=1; j <= (i+1) ; j++)
        {          
            /* If this is the last byte to write, read S1 before the write.
             * This way the TDRE flag will update.
             */
            if (j == i+1)
              temp = UART_S1_REG(tx_module);  

            /* Write data into the FIFO */
            UART_D_REG(tx_module) = j;
        
//            printf("\nTCFIFO = 0x%02X", UART_TCFIFO_REG(tx_module));
    
            /* Test TCFIFO value to make sure it incremented */
            if (UART_TCFIFO_REG(tx_module) != j)
            {
                error++;
                printf("\nERR! TCFIFO value is incorrect. Expected: 0x%02X Read: 0x%02X", j, UART_TCFIFO_REG(tx_module));
            }
        }   
 
        /* Test that TDRE is cleared now  - no space in FIFO */
         if ((UART_S1_REG(tx_module) & UART_S1_TDRE_MASK))
        {
           error++;
           printf("\nERR! TDRE is set when it should be cleared.");
           printf("\nS1 = 0x%02X",UART_S1_REG(tx_module));
           printf("\nTCFIFO = 0x%02X", UART_TCFIFO_REG(tx_module));
           printf("\nTWFIFO = 0x%02X", UART_TWFIFO_REG(tx_module));
        }                               
    
        /* Set the Rx watermark */
        UART_RWFIFO_REG(rx_module) = i;
        
        /* Test that RDRF is cleared - no data in FIFO */
         if ((UART_S1_REG(rx_module) & UART_S1_RDRF_MASK))
        {
           error++;
           printf("\nERR! RDRF is set when it should be cleared.");
           printf("\nS1 = 0x%02X",UART_S1_REG(rx_module));
        }        
                    
        /* Now re-enable the transmitter. This should cause all bytes to transmit
         * back to back.
         */
        UART_C2_REG(tx_module) |= UART_C2_TE_MASK;
    
        /* Wait for RDRF flag to set indicating transmit is complete */
        while( !(UART_S1_REG(rx_module) & UART_S1_RDRF_MASK));
        
        /* Now wait for RCFIFO flag to equal number of bytes sent. Because
         * the TDRE flag trips when the number of bytes is greater than the
         * the watermark, we have to send watermark + 1 bytes. On the other
         * hand the RDRE flag trips when number of bytes equals the watermark.
         * So there should be one more bit to receive after RDRF sets.
         */
        while( UART_RCFIFO_REG(rx_module) != (i+1));

//        printf("\n\n\nS1 = 0x%02X",UART_S1_REG(rx_module));
//        printf("\nRCFIFO = 0x%02X",UART_RCFIFO_REG(rx_module));

        
        /* Loop to empty the entire RxFIFO and test data and RCFIFO values */
        for( j=1; j <= (i+1); j++)
        {          
            /* If this is the last byte to read, read S1 before reading the data.
             * This way the RDRF flag will update.
             */
            if (j == (i+1))
               temp = UART_S1_REG(rx_module);  

            /* Read data from the FIFO */
            ch = UART_D_REG(rx_module);
        
//            printf("\nRCFIFO = 0x%02X", UART_RCFIFO_REG(rx_module));
    
            /* Test RCFIFO value to make sure it decremented */
            if (UART_RCFIFO_REG(rx_module) != (i + 1 - j))
            {
                error++;
                printf("\nERR! RCFIFO value is incorrect. Expected: 0x%02X Read: 0x%02X", (i + 1 - j), UART_RCFIFO_REG(rx_module));
            }
         }  
        
        /* Test that RDRF is cleared again - no data in FIFO */
         if ((UART_S1_REG(rx_module) & UART_S1_RDRF_MASK))
        {
           error++;
           printf("\nERR! RDRF is set when it should be cleared.");
           printf("\nS1 = 0x%02X",UART_S1_REG(rx_module));
           printf("\nRCFIFO = 0x%02X", UART_RCFIFO_REG(rx_module));
           printf("\nRWFIFO = 0x%02X", UART_RWFIFO_REG(rx_module));
        }        
    }   
        
    
    /* Disable the FIFOs */
    UART_PFIFO_REG(tx_module) &= ~UART_PFIFO_TXFE_MASK;
    UART_PFIFO_REG(rx_module) &= ~UART_PFIFO_RXFE_MASK;
        
    /* Clear the Rx watermark */
    UART_RWFIFO_REG(rx_module) = 0;

    /* Clear the Tx watermark */
    UART_TWFIFO_REG(tx_module) = 0;
      
    return error;
}
/********************************************************************/
/* UART FIFO parity error test.
 * Tests storage of the parity error flag in the extended data register. 
 * 
 * This test will use one UART as the transmitter and other as the 
 * receiver. The transmitter will be setup to send a mix of data with and
 * without parity errors. The receiver will check the ED[PARITYE] bit
 * every time it reads data and will make sure that the bit is set when
 * expected.
 * 
 * Since parity error detection was already tested for all modes, this test
 * will just use basic 8-bit mode, lsb first, even parity. The main goal
 * is to test that the PARITYE flag corresponds only to the bytes that have
 * a parity error, not to test parity error dection a second time.
 *
 * If all tests of the PARITYE bit return the expected value, then the
 * test returns 0 (pass) otherwise the test returns the number of errors. 
 */
int uart_fifo_paritye_test()
{
    UART_MemMapPtr tx_module, rx_module;
    volatile  char ch;
    volatile uint8 temp;
    int i, expected_paritye, error = 0;
     
    printf("\n\nStarting setup for UART FIFO PARITYE tests.\n");
    
    /* Test always uses UART0 as the transmitter since it needs a 
     * UART with a FIFO. A variable is still being used so that this
     * code can be ported to other devices that might support FIFOs 
     * on more or other UARTs more easily.
     */
    tx_module = UART0_BASE_PTR;

    /* Test always uses UART1 as the receiver since it needs a 
     * UART with a FIFO. A variable is still being used so that this
     * code can be ported to other devices that might support FIFOs 
     * on more or other UARTs more easily.
     */
    rx_module = UART1_BASE_PTR;
    
    /* Give directions on the connections that need to be made for the test */
    printf("\n\nMake the following connections on a EDSNDC-256 CPU card:\n\n");
    printf("Connect J53 pin 1 (UART0_TX) to J7 pin 5 (UART1_RX)\n");
    
    printf("\nPress any key when ready.\n");
    ch = in_char();
        
    printf("Setup complete. Starting UART FIFO PARITYE tests...\n");
        
    /* Enable the FIFOs */
    UART_PFIFO_REG(tx_module) |= UART_PFIFO_TXFE_MASK;
    UART_PFIFO_REG(rx_module) |= UART_PFIFO_RXFE_MASK;
    
    /* Set the Rx watermark to 8 */
    UART_RWFIFO_REG(rx_module) = 8;
    
    /* Enable parity on both UARTs */
    UART_C1_REG(tx_module) |= UART_C1_PE_MASK;
    UART_C1_REG(rx_module) |= UART_C1_PE_MASK;
    
    /* Send 8 bytes of data alternating between even and odd parity. Since
     * the receiver is configured for even parity only, every other byte
     * should have PARITYE set.
     */
    for( i=0; i < 8; i++)
    {      
        /* Read S1 so that TC flag will reset after data write */
        temp = UART_S1_REG(tx_module);
      
        /* Send a data byte */
        UART_D_REG(tx_module) = 0x55;
        
        /* Wait for transmit to complete */
        while( !(UART_S1_REG(tx_module) & UART_S1_TC_MASK));
        
        /* Switch the parity */
        if( UART_C1_REG(tx_module) & UART_C1_PT_MASK)
           UART_C1_REG(tx_module) &= ~UART_C1_PT_MASK;
        else 
           UART_C1_REG(tx_module) |= UART_C1_PT_MASK; 
    }
    

    /* Wait for RDRF flag to set indicating transmit is complete */
    while( !(UART_S1_REG(rx_module) & UART_S1_RDRF_MASK));

    
    /* Initialize expected_paritye flag to 0 - no error expect on 1st byte */
    expected_paritye = 0;
    
    /* Read data from the FIFO testing PARITYE for each byte */
    for( i=0; i < 8; i++)
    {
//        printf("\nED = 0x%02X", UART_ED_REG(rx_module));

        /* Test PARITYE for the byte */
        if((UART_ED_REG(rx_module) & UART_ED_PARITYE_MASK) != expected_paritye)
        {
            error++;
            printf("\nERR! ED value is incorrect. Expected: 0x%02X Read: 0x%02X",expected_paritye, UART_ED_REG(rx_module));
        }
        
        /* Read the current byte from the FIFO */
        ch = UART_D_REG(rx_module);
        
        /* Toggle expected_paritye for the next loop */
        if( expected_paritye == UART_ED_PARITYE_MASK)
            expected_paritye = 0;
        else
            expected_paritye = UART_ED_PARITYE_MASK;
    }                  
    
    
    /* Disable the FIFOs */
    UART_PFIFO_REG(tx_module) &= ~UART_PFIFO_TXFE_MASK;
    UART_PFIFO_REG(rx_module) &= ~UART_PFIFO_RXFE_MASK;
    
    /* Clear C1 in both UARTs to go back to default mode */
    UART_C1_REG(tx_module) = 0;
    UART_C1_REG(rx_module) = 0;
   
    
    return error;  
}
/********************************************************************/
