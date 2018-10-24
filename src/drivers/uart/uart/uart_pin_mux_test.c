/*
 * File:		uart_pin_mux_test.c
 * Purpose:		Tests UART functionality on all multiplexed pins.
 *
 */

#include "common.h"
#include "uart.h"
#include "uart_tests.h"

/********************************************************************/
/* UART0 pin multiplexing test.
 * This function will test all of the possible UART0 pins. 
 * 
 * NOTE: This test is meant to be run on the Firebird using a EDSNDC-256BGA
 * only and some pins are normally used for JTAG, so this test has to be run 
 * out of flash.
 *
 *
 * NOTE: I'm seeing problems sometimes when switching the pin functions
 * from pin to pin. If I test the pins individually they do work, but it
 * seems like there can be conflicts caused by moving the pins around. I
 * think this is an issue specific to my code and the way I'm testing 
 * the functions. I don't think customers would run into this since they
 * would normally configure one set of pins for a function and leave it.
 *
 * 
 * If receive data matches the transmitted data, then this indicates that
 * the CTS and TX pins are working correctly and test returns 0 (pass) o
 * therwise the test returns the number of errors.
 */
int uart0_pin_mux_test()
{
    UART_MemMapPtr tx_module, rx_module;
    volatile char ch;
    int error = 0;        
    
    printf("\n\nStarting UART0 pin mux tests.\n");
    
    /* Tx pin tests will all use UART1 as the receiver. A variable is still
     * being used so that this code can be ported more easily.
     */
    rx_module = UART1_BASE_PTR;
    
    
    /* Test all of the UART0 Tx pins - TX and CTS */
    tx_module = UART0_BASE_PTR;
        
#if 0
    /* Test the PTD7/UART0_TX and PTD5_UART0_CTS pins - pins already configured in main */    

    /* Give directions on the connections that need to be made for the test */
    printf("\n\nMake the following connections on a DC card:\n\n");
    printf("Connect (PTE1/UART1_RX)  to (PTD7/UART0_TX)\n");
    printf("Connect (PTC1/UART1_RTS) to (PTD5/UART0_CTS)\n");
    
    printf("\nPress any key when ready.\n");
    ch = in_char();
        
    error += uart_function_test(tx_module, rx_module);
#endif 
    
    /* Disable the UART Tx functions on the pins just tested */
    PORTD_PCR5 = PORT_PCR_MUX(0x0); 
    PORTD_PCR7 = PORT_PCR_MUX(0x0); 
   
#if 0
    /* Test the PTA2/UART0_TX and PTA0/UART0_CTS pins */    
    PORTA_PCR0 = PORT_PCR_MUX(0x2); 
    PORTA_PCR2 = PORT_PCR_MUX(0x2); 
    
    /* Give directions on the connections that need to be made for the test */
    printf("\n\nMake the following connections on on a DC card:\n\n");
    printf("Connect (PTE1/UART1_RX) to (PTA2/UART0_TX)\n");
    printf("Connect (PTC1/UART1_RTS) to (PTA0/UART0_CTS)\n");
    
    printf("\nPress any key when ready.\n");
    ch = in_char();
        
    error += uart_function_test(tx_module, rx_module);

    /* Put these pins back to their JTAG functions */
    PORTA_PCR0 = PORT_PCR_MUX(0x7); 
    PORTA_PCR2 = PORT_PCR_MUX(0x7); 
#endif    
    
#if 0
    /* Test the PTB17/UART0_TX and PTB3/UART0_CTS pins */    
    PORTB_PCR17 = PORT_PCR_MUX(0x3); 
    PORTB_PCR3 = PORT_PCR_MUX(0x3); 
    
    /* Give directions on the connections that need to be made for the test */
    printf("\n\nMake the following connections on a a DC card:\n\n");
    printf("Connect (PTE1/UART1_RX to (PTB17/UART0_TX)\n");
    printf("Connect (PTC1/UART1_RTS) to (PTB3/UART0_CTS)\n");
    
    printf("\nPress any key when ready.\n");
    ch = in_char();
        
    error += uart_function_test(tx_module, rx_module);

    /* Disable the UART Tx functions on the pins just tested */
    PORTB_PCR17 = PORT_PCR_MUX(0x0); 
    PORTB_PCR3 = PORT_PCR_MUX(0x0); 
#endif    

    /* Rx pin tests will all use UART1 as the transmitter. A variable is still
     * being used so that this code can be ported more easily.
     */
    tx_module = UART1_BASE_PTR;
       
    /* Test all of the UART0 Rx pins - RX and RTS */
    rx_module = UART0_BASE_PTR;
        

#if 0
    /* Test the PTD6/UART0_RX and PTD4/UART0_RTS pins - pins already configured in main */    

    /* Give directions on the connections that need to be made for the test */
    printf("\n\nMake the following connections on a a DC card:\n\n");
    printf("Connect (PTE0/UART1_TX) to  (PTD6/UART0_RX)\n");
    printf("Connect (PTC2/UART1_CTS) to (PTD4/UART0_RTS)\n");
    
    printf("\nPress any key when ready.\n");
    ch = in_char();
        
    error += uart_function_test(tx_module, rx_module);
#endif  
    
    /* Disable the UART Rx functions on the pins just tested */
    PORTD_PCR6 = PORT_PCR_MUX(0x0); 
    PORTD_PCR4 = PORT_PCR_MUX(0x0); 
    
#if 0
    /* Test the PTA1/UART0_RX and PTA3/UART0_RTS pins */    
    PORTA_PCR1 = PORT_PCR_MUX(0x2); 
    PORTA_PCR3 = PORT_PCR_MUX(0x2); 

    /* Give directions on the connections that need to be made for the test */
    printf("\n\nMake the following connections on a a DC card:\n\n");
    printf("Connect (PTE0/UART1_TX) to (PTA1/UART0_RX)\n");
    printf("Connect (PTC2/UART1_CTS) to (PTA3/UART0_RTS)\n");
    
    printf("\nPress any key when ready.\n");
    ch = in_char();
        
    error += uart_function_test(tx_module, rx_module);

    /* Put these pins back to their JTAG functions */
    PORTA_PCR1 = PORT_PCR_MUX(0x7); 
    PORTA_PCR3 = PORT_PCR_MUX(0x7); 
#endif    
 
#if 1
    /* Test the PTB16/UART0_RX and PTB2/UART0_RTS pins */    
    PORTB_PCR16 = PORT_PCR_MUX(0x3); 
    PORTB_PCR2 = PORT_PCR_MUX(0x3); 

    /* Give directions on the connections that need to be made for the test */
    printf("\n\nMake the following connections on a a DC card:\n\n");
    printf("Connect (PTE0/UART1_TX) to (PTB16/UART0_RX)\n");
    printf("Connect (PTC2/UART1_CTS) to (PTB2/UART0_RTS)\n");
    
    printf("\nPress any key when ready.\n");
    ch = in_char();
        
    error += uart_function_test(tx_module, rx_module);

    /* Disable the UART Rx functions on the pins just tested */
    PORTB_PCR16 = PORT_PCR_MUX(0x0); 
    PORTB_PCR2 = PORT_PCR_MUX(0x0); 
#endif    
    
    /* Test complete */
    printf("\n\nUART0 pin mux test complete.\n");
    
    /* Enable default pins for UART0 before moving on to next test */
    PORTD_PCR4 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
    PORTD_PCR5 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
    PORTD_PCR6 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
    PORTD_PCR7 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin

    
    return error;  
}
/********************************************************************/
/* UART1 pin multiplexing test.
 * This function will test all of the possible UART1 pins. 
 * 
 * VERY IMPORTANT INSTRUCTIONS BELOW!!!!!!
 * 
 * NOTE: This test is meant to be run on the Firebird using a EDSNDC-256BGA
 * only. In order to test UART1 the code has to be compiled to use a different
 * TERM_PORT. In order to run the test modify the firebird.h file to change the 
 * TERM_PORT to use UART2. Then connect J9 pin 15 to J4 pin 5 and J9 pin 12
 * to J4 pin 2. Connect the PC terminal to the UART2 port on the baseboard.
 *
 *
 * NOTE: I'm seeing problems sometimes when switching the pin functions
 * from pin to pin. If I test the pins individually they do work, but it
 * seems like there can be conflicts caused by moving the pins around. I
 * think this is an issue specific to my code and the way I'm testing 
 * the functions. I don't think customers would run into this since they
 * would normally configure one set of pins for a function and leave it.
 *
 * 
 * If receive data matches the transmitted data, then this indicates that
 * the CTS and TX pins are working correctly and test returns 0 (pass) o
 * therwise the test returns the number of errors.
 */
int uart1_pin_mux_test()
{
    UART_MemMapPtr tx_module, rx_module;
    volatile char ch;
    int error = 0;        
    
    printf("\n\nStarting UART1 pin mux tests.\n");
    
    /* Tx pin tests will all use UART0 as the receiver. A variable is still
     * being used so that this code can be ported more easily.
     */
    rx_module = UART0_BASE_PTR;
    
    
    /* Test all of the UART1 Tx pins - TX and CTS */
    tx_module = UART1_BASE_PTR;
        
#if 0
    /* Test the PTE0/UART1_TX - pins already configured in main */    
    PORTE_PCR0 = PORT_PCR_MUX(0x3); 

    /* Give directions on the connections that need to be made for the test */
    printf("\n\nMake the following connections on a DC card:\n\n");
    printf("Connect (PTD6/UART0_RX) to (PTE0/UART1_TX)\n");
    printf("Connect (PTD4/UART0_RTS) to (PTC2/UART1_CTS)\n");
    
    printf("\nPress any key when ready.\n");
    ch = in_char();
        
    error += uart_function_test(tx_module, rx_module);
#endif
    
    /* Disable the UART Tx functions on the pins just tested */
    PORTE_PCR0 = PORT_PCR_MUX(0x0); 

#if 0
    /* Test the PTC4/UART1_TX and PTC2/UART1_CTS pins */    
    PORTC_PCR4 = PORT_PCR_MUX(0x3); 
    PORTC_PCR2 = PORT_PCR_MUX(0x3); 

    /* Give directions on the connections that need to be made for the test */
    printf("\n\nMake the following connections on a DC card:\n\n");
    printf("Connect (PTD6/UART0_RX) to (PTC4/UART1_TX)\n");
    printf("Connect (PTD4/UART0_RTS) to (PTC2/UART1_CTS)\n");
        
    printf("\nPress any key when ready.\n");
    ch = in_char();
        
    error += uart_function_test(tx_module, rx_module);
#endif 
    
    /* Disable the UART Tx functions on the pins just tested */
    PORTC_PCR4 = PORT_PCR_MUX(0x0); 
    PORTC_PCR2 = PORT_PCR_MUX(0x0); 


    /* Rx pin tests will all use UART0 as the transmitter. A variable is still
     * being used so that this code can be ported more easily.
     */
    tx_module = UART0_BASE_PTR;
    
    
    /* Test all of the UART1 Rx pins - RX and RTS */
    rx_module = UART1_BASE_PTR;
        

#if 0
    /* Test the PTE1/UART1_RX - pins already configured in main */    
    PORTE_PCR1 = PORT_PCR_MUX(0x3);  

    /* Give directions on the connections that need to be made for the test */
    printf("\n\nMake the following connections on a DC:\n\n");
    printf("Connect (PTD7/UART0_TX) to (PTE1/UART1_RX)\n");
    printf("Connect (PTD5/UART0_CTS) to (PTC1/UART1_RTS)\n");
    
    printf("\nPress any key when ready.\n");
    ch = in_char();
        
    error += uart_function_test(tx_module, rx_module);
#endif
    
    /* Disable the UART Rx functions on the pins just tested */
    PORTE_PCR1 = PORT_PCR_MUX(0x0);      
    
#if 1    
    /* Test the PTC3/UART1_RX and PTC1/UART1_RTS pins - pins already configured in main */    
    PORTC_PCR1 = PORT_PCR_MUX(0x3); 
    PORTC_PCR3 = PORT_PCR_MUX(0x3); 

    /* Give directions on the connections that need to be made for the test */
    printf("\n\nMake the following connections on a DC:\n\n");
    printf("Connect (PTD7/UART0_TX) to (PTC3/UART1_RX)\n");
    printf("Connect (PTD5/UART0_CTS) to (PTC1/UART1_RTS)\n");
    
    printf("\nPress any key when ready.\n");
    ch = in_char();
        
    error += uart_function_test(tx_module, rx_module);
#endif  
    
    /* Disable the UART Rx functions on the pins just tested */
    PORTC_PCR3 = PORT_PCR_MUX(0x0); 
    PORTC_PCR1 = PORT_PCR_MUX(0x0);   
    
    /* Test complete */
    printf("\n\nUART1 pin mux test complete.\n");
    
    /* Enable default pins for UART1 before moving on to next test */
    PORTE_PCR0 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
    PORTE_PCR1 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
    
    return error;  
}

/********************************************************************/
/* UART2 pin multiplexing test.
 * This function will test all of the possible UART2 pins. 
 * 
 *
 *
 * NOTE: I'm seeing problems sometimes when switching the pin functions
 * from pin to pin. If I test the pins individually they do work, but it
 * seems like there can be conflicts caused by moving the pins around. I
 * think this is an issue specific to my code and the way I'm testing 
 * the functions. I don't think customers would run into this since they
 * would normally configure one set of pins for a function and leave it.
 *
 * 
 * If receive data matches the transmitted data, then this indicates that
 * the CTS and TX pins are working correctly and test returns 0 (pass) o
 * therwise the test returns the number of errors.
 */
int uart2_pin_mux_test()
{
    UART_MemMapPtr tx_module, rx_module;
    volatile char ch;
    int error = 0;        
    
    printf("\n\nStarting UART2 pin mux tests.\n");
    
     
    /* Tx pin tests will all use UART1 as the receiver. A variable is still
     * being used so that this code can be ported more easily.
     */
    rx_module = UART0_BASE_PTR;
    
    
    /* Test all of the UART0 Tx pins - TX and CTS */
    tx_module = UART2_BASE_PTR;
   
#if 1
    /* Test the PTD3/UART2_TX and PTD1/UART2_CTS pins */    
    PORTD_PCR1 = PORT_PCR_MUX(0x3); 
    PORTD_PCR3 = PORT_PCR_MUX(0x3); 
    
    /* Give directions on the connections that need to be made for the test */
    printf("\n\nMake the following connections on a DC:\n\n");
    printf("Connect (PTD6/UART1_RX) to (PTD3/UART2_TX)\n");
    printf("Connect (PTD4/UART1_RTS) to (PTD1/UART2_CTS)\n");
    
    printf("\nPress any key when ready.\n");
    ch = in_char();
        
    error += uart_function_test(tx_module, rx_module);

    /* Disable the UART Tx functions on the pins just tested */
    PORTD_PCR3 = PORT_PCR_MUX(0x0); 
    PORTD_PCR1 = PORT_PCR_MUX(0x0); 
#endif    

    /* Rx pin tests will all use UART0 as the transmitter. A variable is still
     * being used so that this code can be ported more easily.
     */
    tx_module = UART0_BASE_PTR;
    
    
    /* Test all of the UART1 Rx pins - RX and RTS */
    rx_module = UART2_BASE_PTR;
    
#if 1
    /* Test the PTD2/UART2_RX and PTD0/UART2_RTS pins */    
    PORTD_PCR2 = PORT_PCR_MUX(0x3); 
    PORTD_PCR0 = PORT_PCR_MUX(0x3); 

    /* Give directions on the connections that need to be made for the test */
    printf("\n\nMake the following connections on a DC:\n\n");
    printf("Connect (PTD7/UART1_TX) to (PTD2/UART2_RX)\n");
    printf("Connect (PTD5/UART1_CTS) to (PTD0/UART2_RTS)\n");
    
    printf("\nPress any key when ready.\n");
    ch = in_char();
        
    error += uart_function_test(tx_module, rx_module);

    /* Disable the UART Rx functions on the pins just tested */
    PORTD_PCR2 = PORT_PCR_MUX(0x0); 
    PORTD_PCR0 = PORT_PCR_MUX(0x0); 
#endif    

    printf("\n\nUART2 pin mux test complete.\n");
    return error;  
}
/********************************************************************/
/* UART function test.
 * This function is a generic UART test with flow control enabled.
 * It is meant to be used to test that Tx and/or Rx pins are working
 * correctly. 
 * 
 * NOTE: Pins must be configured and hardware connections needed for
 * the test should be done before calling this function.
 * 
 * If receive data matches the transmitted data, then this indicates that
 * the CTS and TX pins are working correctly and test returns 0 (pass) 
 * Otherwise the test returns the number of errors.
 */
int uart_function_test(UART_MemMapPtr tx_module, UART_MemMapPtr rx_module)
{
    char ch;
    int error = 0;        
    uint32 i;    
    
    /* Enable TxCTS signal on the transmiter */
    UART_MODEM_REG(tx_module) |= UART_MODEM_TXCTSE_MASK; 
    
    /* Enable RxRTS signal on the receiver */
    UART_MODEM_REG(rx_module) |= UART_MODEM_RXRTSE_MASK;
    
    /* Delay to allow RTS pin time to activate before sending data */
    for(i=0; i < 0x10000; i++);
    
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
    
    /* Read one character from the receiver. This should trigger transmission
     * of the second character.
     */
    ch = uart_getchar(rx_module);
    if ( ch != 0x1)
    {
        error++;
        printf("\nERR! Incorrect character received.");
    }

    ch = uart_getchar(rx_module);    
    if ( ch != 0x2)
    {
        error++;
        printf("\nERR! Incorrect character received.");
    }
    

   
    
    /* Disable TxCTS signal on the transmiter */
    UART_MODEM_REG(tx_module) &= ~UART_MODEM_TXCTSE_MASK; 
    
    /* Disable RxRTS signal on the receiver */
    UART_MODEM_REG(rx_module) &= ~UART_MODEM_RXRTSE_MASK;
    
    return error;
}
/********************************************************************/

