/*
 * File:		uart_flow_control_tests.c
 * Purpose:     UART flow control tests. All of these test use one UART
 *              as a transmitter sending to a second UART in order to test
 *              that the flow control is working. A scope should also be
 *              used to double check correct operation.
 *
 */

#include "common.h"
#include "uart.h"
#include "uart_tests.h"

/********************************************************************/
/* UART flow control test.
 * Tests operation of the TxCTS and RxRTS flow control signals. One UART
 * will be configured as a transmitter that will only send data when the
 * TxCTS input is asserted. A second UART will be configured as the receiver
 * that asserts RxCTS when it can accept another charcter. 
 *
 * The test will run once with FIFOs disabled and then run a second time
 * with FIFOs enabled. Since two UARTs with FIFOs are needed to run this
 * test correctly it is hard coded to use UART0 as the transmitter and UART1
 * as the receiver. 
 * 
 * If receive data matches the transmitted data, then this indicates flow
 * control prevented data loss and test returns 0 (pass) otherwise the test
 * returns the number of errors. A scope should also be used to monitor 
 * data flow and make sure that CTS/RTS are throttling data as expected.
 */
int uart_flow_control_test()
{
    UART_MemMapPtr tx_module, rx_module;
    volatile char ch;
    int error = 0;        
    uint32 i;
    
    printf("\n\nStarting setup for UART flow control tests.\n");
    
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
        
    printf("Setup complete. Starting UART flow control tests...\n");

    
    /* Enable TxCTS signal on the transmiter */
    UART_MODEM_REG(tx_module) |= UART_MODEM_TXCTSE_MASK; 
    
    /* Enable RxRTS signal on the receiver */
    UART_MODEM_REG(rx_module) |= UART_MODEM_RXRTSE_MASK;
    

    /* Test flow control operation with FIFO disabled */
    printf("\nTesting flow control operation with FIFO disabled.");
    
    /* Attempt to transmit two bytes */
    uart_putchar (tx_module, 0x1); 
    
    /* Need delay for this test to work right now. Otherwise the transmitter
     * will start sending the second byte before the receiver has a chance
     * to negate RTS. I'm talking to design about changing the timing for
     * the signals so that no delay is needed.
     */
    for(i=0; i < 0x1000; i++);
    
    
    uart_putchar (tx_module, 0x2);    
    
    /* Since we haven't read from the receiver, it should only have space
     * to accept one of the characters. TDRE should be set to indicate that
     * the transmitter has not send the second character. And RDRF should
     * be set on the receiver set to indicate that one character has been 
     * received.
     */
    
    /* Make sure TDRE is cleared in the transmitter - wasn't able to send second char */
    if (UART_S1_REG(tx_module) & UART_S1_TDRE_MASK)
    {
        error++;
        printf("\nERR! TDRE not set when expected.");
        printf("\nS1 = 0x%02X",UART_S1_REG(tx_module));
    }
        printf("\nS1 = 0x%02X",UART_S1_REG(tx_module));
    
    /* Read one character from the receiver. This should trigger transmission
     * of the second character.
     */
    if ( uart_getchar(rx_module) != 0x1)
    {
        error++;
        printf("\nERR! Incorrect character received.");
    }
        
    if ( uart_getchar(rx_module) != 0x2)
    {
        error++;
        printf("\nERR! Incorrect character received.");
    }
    

    /* Test flow control operation with FIFO enabled */
    printf("\nTesting flow control operation with FIFO enabled.");
    
    /* Enable the TxFIFO on the transmitter */
    UART_PFIFO_REG(tx_module) |= UART_PFIFO_TXFE_MASK;
    
    /* Enable the RxFIFO on the receiver */
    UART_PFIFO_REG(rx_module) |= UART_PFIFO_RXFE_MASK;
    
    /* Set the Tx watermark */
    UART_TWFIFO_REG(tx_module) = 0x4;

    /* Set the Rx watermark */
    UART_RWFIFO_REG(rx_module) = 0x2;
    
    /* Attempt to transmit four bytes */
    uart_putchar (tx_module, 0x1); 
    uart_putchar (tx_module, 0x2);    

    /* Need delay for this test to work right now. Otherwise the transmitter
     * will start sending the second byte before the receiver has a chance
     * to negate RTS. I'm talking to design about changing the timing for
     * the signals so that no delay is needed.
     */
//    for(i=0; i < 0x1000; i++);

    uart_putchar (tx_module, 0x3); 
    uart_putchar (tx_module, 0x4);    

    
    /* Since we haven't read from the receiver, it should deassert RTS after
     * the first two characters are transmitted. 
     */
    
    printf("\nRXCOUNT = 0x%02X",UART_RCFIFO_REG(rx_module));
        
    if ( uart_getchar(rx_module) != 0x1)
    {
        error++;
        printf("\nERR! Incorrect character received.");
    }

    printf("\nRXCOUNT = 0x%02X",UART_RCFIFO_REG(rx_module));
        
    if ( uart_getchar(rx_module) != 0x2)
    {
        error++;
        printf("\nERR! Incorrect character received.");
    }

    printf("\nRXCOUNT = 0x%02X",UART_RCFIFO_REG(rx_module));
    
    if ( uart_getchar(rx_module) != 0x3)
    {
        error++;
        printf("\nERR! Incorrect character received.");
    }

    printf("\nRXCOUNT = 0x%02X",UART_RCFIFO_REG(rx_module));

    if ( UART_D_REG(rx_module) != 0x4)
    {
        error++;
        printf("\nERR! Incorrect character received.");
    }
    
    
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
/* UART TxRTS test.
 * Tests operation of the TxRTS signal. One of the UARTs with a FIFO
 * implemented will be used for testing so that operation both without
 * and with the FIFO enabled is tested. The TxRTS can be active high or 
 * low, so both polarities will be tested.
 *
 * A scope should be used to verify that the signals transmitted
 * match the expected timing.
 *
 * Since the test is not self checking it will always return 0 (pass).* 
 */
int uart_txrts_test()
{
    UART_MemMapPtr module;
    volatile char ch;       
    
    uint32 i;
    
    printf("\n\nStarting setup for TxRTS flow control tests.\n");

    /* Determine which UART to use for testing. UART0 or UART1 could be used. 
     * We'll use UART0 as long as it isn't used as the TERM_PORT.
     */
    if (TERM_PORT != UART0_BASE_PTR)
    {      
       module = UART0_BASE_PTR;   // set the module pointer
       printf("\nUsing UART0 on the daughter card as the transmitter.");
       printf("\nConnect one scope channel to UART0_TX (J53 pin 1 on EDSNDC-256).");
       printf("\nConnect second scope channel to UART0_RTS (J2 pin 8)");
       printf("\nSet scope for single edge trigger (rising or falling) on the TX channel.\n");
    }      
    else
    {      
       module = UART1_BASE_PTR;   // set the module pointer
       printf("\nUsing UART1 on the daughter card as the transmitter.");
       printf("\nConnect one scope channel to UART1_TX (J7 pin 2 on EDSNDC-256).");
       printf("\nConnect second scope channel to UART1_RTS (J7 pin 11)");
       printf("\nSet scope for single edge trigger (rising or falling) on the TX channel.\n");
    }

    printf("\nPress any key when ready.\n");
    ch = in_char();
        

    /* Enable TxRTS mode - active low is default */
    UART_MODEM_REG(module) |= UART_MODEM_TXRTSE_MASK;
    
    
    printf("\nTesting TxRTS enabled in active low mode - no FIFO.\n");
    
    /* Test TxRTS active low - No FIFO */
    printf("Press any key when scope is ready.\n");
    ch = in_char();

    printf("Sending two characters with FIFO disabled...\n");
    uart_putchar (module, 0xAA); 
    uart_putchar (module, 0xAA); 
    
    
    printf("\nTesting TxRTS active high mode - no FIFO.\n");

    /* Test TxRTS while sending one character at a time - No FIFO */
    printf("Press any key when scope is ready.\n");
    ch = in_char();

    /* Switch to active high polarity */
    UART_MODEM_REG(module) |= UART_MODEM_TXRTSPOL_MASK;

    printf("Sending two characters with FIFO disabled...\n");
    uart_putchar (module, 0xAA); 
    uart_putchar (module, 0xAA); 
    
    
    /* Delay to allow FIFO disabled tests to complete */
    for (i=0; i < 0x10000; i++);
    


    /* Run both tests with the FIFO enabled */
    UART_PFIFO_REG(module) |= UART_PFIFO_TXFE_MASK;
    
    UART_TWFIFO_REG(module) = 0x4;
    

    printf("\nTesting TxRTS active low mode - FIFO.\n");

    /* Test TxRTS active low - FIFO */
    printf("Press any key when scope is ready.\n");
    ch = in_char();

    /* Switch to active low polarity */
    UART_MODEM_REG(module) &= ~UART_MODEM_TXRTSPOL_MASK;

    printf("Sending six characters with FIFO enabled...\n");
    uart_putchar (module, 0xAA); 
    uart_putchar (module, 0xAA); 
    uart_putchar (module, 0xAA); 
    uart_putchar (module, 0xAA); 
    uart_putchar (module, 0xAA); 
    uart_putchar (module, 0xAA); 
    
    
    printf("\nTesting TxRTS active high mode - FIFO.\n");

    /* Test TxRTS while sending one character at a time - No FIFO */
    printf("Press any key when scope is ready.\n");
    ch = in_char();

    /* Switch to active high polarity */
    UART_MODEM_REG(module) |= UART_MODEM_TXRTSPOL_MASK;

    printf("Sending six characters with FIFO enabled...\n");
    uart_putchar (module, 0xAA); 
    uart_putchar (module, 0xAA); 
    uart_putchar (module, 0xAA); 
    uart_putchar (module, 0xAA); 
    uart_putchar (module, 0xAA); 
    uart_putchar (module, 0xAA); 
    

    /* Delay to allow FIFO enabled tests to complete */
    for (i=0; i < 0x10000; i++);
    
    /* Disable the FIFO */
    UART_PFIFO_REG(module) &= ~UART_PFIFO_TXFE_MASK;  
    
    /* Clear the Tx watermark */
    UART_TWFIFO_REG(module) = 0x0;
    
    /* Disable TxRTS mode */
    UART_MODEM_REG(module) &= ~UART_MODEM_TXRTSE_MASK;
    
    /* Clear the TXRTSPOL bit */
    UART_MODEM_REG(module) &= ~UART_MODEM_TXRTSPOL_MASK;
    
    
    return 0;    
}    
/********************************************************************/
