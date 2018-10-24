/*
 * File:		uart_irda_tests.c
 * Purpose:		Tests the IRDA features of the UART.
 *
 */

#include "common.h"
#include "uart.h"
#include "uart_tests.h"

/********************************************************************/
/* UART IRDA mode transmit test.
 * This test looks at the transmit operation in IRDA mode. This bit
 * is the same across UART instantiations, so only one UART needs to be
 * tested. The test will transmit using all four values for the
 * UARTn_IR[TNP] field, then run the same tests with the transmit data
 * inversion enabled (UARTn_C3[TXINV] set). A scope will be used to 
 * verify that the transmit waveforms match the expected values.
 *
 * Since the test is not self checking it will always return 0 (pass).
 */
int uart_irda_tx_test()
{
    UART_MemMapPtr module;
    volatile char ch;
    
    /* Determine which UART to use for testing. UART2 or UART3 could
     * be used. We'll use UART2 as long as it isn't used as the TERM_PORT.
     */
    if (TERM_PORT != UART2_BASE_PTR)
    {      
       module = UART2_BASE_PTR;   // set the module pointer
       
       printf("\nStarting UART IRDA transmit test on UART2.\n");
       printf("\nPlease connect a scope to UART2_TX (J4 pin 2 on EDSNDC-256.\n");
       printf("Set scope for single edge trigger (rising or falling).\n");
       printf("Press any key when ready.\n");
       ch = in_char();
   }      
    else
    {
       printf("\nNo more available UARTs.\n");
       ch = in_char();
    }
 
    /* Configure the UART for IrDA mode */
    UART_IR_REG(module) |= UART_IR_IREN_MASK;
    
 
    /* Test non-inverted 3/16 pulse mode */
    printf("\n\nUART configured for non-inverted 3/16 pulses.\n");
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

    /* Test non-inverted 1/16 pulse mode */
    UART_IR_REG(module) = (UART_IR_IREN_MASK | UART_IR_TNP(1));
    
    printf("\n\nUART configured for non-inverted 1/16 pulses.\n");
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

    /* Test non-inverted 1/32 pulse mode */
    UART_IR_REG(module) = (UART_IR_IREN_MASK | UART_IR_TNP(2));
    
    printf("\n\nUART configured for non-inverted 1/32 pulses.\n");
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

    /* Test non-inverted 1/4 pulse mode */
    UART_IR_REG(module) = (UART_IR_IREN_MASK | UART_IR_TNP(3));
    
    printf("\n\nUART configured for non-inverted 1/4 pulses.\n");
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
    
    
    /* Now run the same tests in Tx inverted mode */ 
    UART_C3_REG(module) |= UART_C3_TXINV_MASK;

    /* Test inverted 3/16 pulse mode */
    UART_IR_REG(module) = (UART_IR_IREN_MASK | UART_IR_TNP(0));

    printf("\n\nUART configured for inverted 3/16 pulses.\n");
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

    /* Test inverted 1/16 pulse mode */
    UART_IR_REG(module) = (UART_IR_IREN_MASK | UART_IR_TNP(1));
    
    printf("\n\nUART configured for inverted 1/16 pulses.\n");
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

    /* Test inverted 1/32 pulse mode */
    UART_IR_REG(module) = (UART_IR_IREN_MASK | UART_IR_TNP(2));
    
    printf("\n\nUART configured for inverted 1/32 pulses.\n");
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

    /* Test inverted 1/4 pulse mode */
    UART_IR_REG(module) = (UART_IR_IREN_MASK | UART_IR_TNP(3));
    
    printf("\n\nUART configured for inverted 1/4 pulses.\n");
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
    

    /* All tests complete. Restore default values before moving on to next test */
    UART_IR_REG(module) = 0;
    UART_C3_REG(module) &= ~UART_C3_TXINV_MASK;

    return 0;
}
/********************************************************************/
/* UART IrDA receive tests.
 * Tests receive operation of the UART in IrDA mode. This includes 
 * checking operation with all settings of TNP. These functions 
 * are the same across UART instantiations, so only one UART needs to be 
 * tested. 
 * 
 * Receive tests will use one UART as the transmitter and other as the 
 * receiver. Using the same module to communicate is not an ideal test setup,
 * but since the transmit functions were previously verified using a scope
 * we know they send data as expected. So using the multiple instantiations
 * of the same module for the transmit and receive side of this test should
 * be ok.
 *
 * If all received values match the value transmitted, then test passes 
 * and function returns 0 otherwise returns the number of errors. 
 */
int uart_irda_rx_test()
{
    UART_MemMapPtr tx_module, rx_module;
    volatile char ch, ch1, ch2, temp;
    int error = 0;    
    
    /* Set values for ch1 and ch2 that will be used for all tests */
    ch1 = 0x2A; 
    ch2 = 0x55; 
        
    printf("\n\nStarting setup for IrDA Rx tests.\n");

    /* Determine which UART to use as the transmitter for testing. UART2 or UART3
     * could be used. We'll use UART2 as long as it isn't used as the TERM_PORT.
     */
    if (TERM_PORT != UART2_BASE_PTR)
    {      
       tx_module = UART2_BASE_PTR;   // set the module pointer
       
       printf("\nUsing UART2 on the daughter card as the transmitter.");
       printf("\nConnect one end of a blue wire to J4 pin 2 (UART2_TX)");
    }      
    else
    {      
       printf("\nNo more available UARTs\n");
       (void)in_char();
    }

    printf("\nPress any key when ready.\n");
    ch = in_char();
    
    /* Determine which UART to use as the receiver for testing. UART4 or UART5
     * could be used. We'll use UART4 as long as it isn't used as the TERM_PORT.
     */
    if (TERM_PORT != UART0_BASE_PTR)
    {      
       rx_module = UART0_BASE_PTR;   // set the module pointer
       
       printf("\nUsing UART0 on the daughter card as the receiver.");
#warning "fix here"
       printf("\nConnect other end of blue wire to J13 pin 2 (UART4_RX)");
    }      
    else
    {      
       printf("\nNo more available UARTs\n");
       (void)in_char();
    }

    printf("\nPress any key when ready.\n");
    ch = in_char();
    
    printf("Setup complete. Starting IrDA Rx tests...\n");


    /* Test Rx in non-inverted 3/16 mode, Tx in non-inverted 3/16 mode */
    UART_IR_REG(tx_module) = UART_IR_IREN_MASK;
    UART_IR_REG(rx_module) = UART_IR_IREN_MASK;      
    
    uart_putchar (tx_module, ch1); 
    temp = uart_getchar (rx_module); 
    if (temp != ch1)
    {
        error++;
        printf("ERR! Incorrect value received. Rx = 0x%02X  Tx = 0x%02X\n", temp, ch1);
    }        
        
    uart_putchar (tx_module, ch2); 
    temp = uart_getchar (rx_module); 
    if (temp != ch2)
    {
        error++;
        printf("ERR! Incorrect value received. Rx = 0x%02X  Tx = 0x%02X\n", temp, ch2);
    }        
    
    /* Test Rx in non-inverted 3/16 mode, Tx in non-inverted 1/4 mode */
    UART_IR_REG(tx_module) = ( UART_IR_IREN_MASK | UART_IR_TNP(3) );
    
    uart_putchar (tx_module, ch1); 
    temp = uart_getchar (rx_module); 
    if (temp != ch1)
    {
        error++;
        printf("ERR! Incorrect value received. Rx = 0x%02X  Tx = 0x%02X\n", temp, ch1);
    }        
        
    uart_putchar (tx_module, ch2); 
    temp = uart_getchar (rx_module); 
    if (temp != ch2)
    {
        error++;
        printf("ERR! Incorrect value received. Rx = 0x%02X  Tx = 0x%02X\n", temp, ch2);
    }    



    /* Test Rx in non-inverted 1/16 mode, Tx in non-inverted 1/16 mode */
    UART_IR_REG(tx_module) = ( UART_IR_IREN_MASK | UART_IR_TNP(1));
    UART_IR_REG(rx_module) = ( UART_IR_IREN_MASK | UART_IR_TNP(1));      
    
    uart_putchar (tx_module, ch1); 
    temp = uart_getchar (rx_module); 
    if (temp != ch1)
    {
        error++;
        printf("ERR! Incorrect value received. Rx = 0x%02X  Tx = 0x%02X\n", temp, ch1);
    }        
        
    uart_putchar (tx_module, ch2); 
    temp = uart_getchar (rx_module); 
    if (temp != ch2)
    {
        error++;
        printf("ERR! Incorrect value received. Rx = 0x%02X  Tx = 0x%02X\n", temp, ch2);
    }        
    
    /* Test Rx in non-inverted 1/16 mode, Tx in non-inverted 3/16 mode */
    UART_IR_REG(tx_module) = ( UART_IR_IREN_MASK | UART_IR_TNP(0));
    
    uart_putchar (tx_module, ch1); 
    temp = uart_getchar (rx_module); 
    if (temp != ch1)
    {
        error++;
        printf("ERR! Incorrect value received. Rx = 0x%02X  Tx = 0x%02X\n", temp, ch1);
    }        
        
    uart_putchar (tx_module, ch2); 
    temp = uart_getchar (rx_module); 
    if (temp != ch2)
    {
        error++;
        printf("ERR! Incorrect value received. Rx = 0x%02X  Tx = 0x%02X\n", temp, ch2);
    }        
    
    /* Test Rx in non-inverted 1/16 mode, Tx in non-inverted 1/4 mode */
    UART_IR_REG(tx_module) = ( UART_IR_IREN_MASK | UART_IR_TNP(3));
    
    uart_putchar (tx_module, ch1); 
    temp = uart_getchar (rx_module); 
    if (temp != ch1)
    {
        error++;
        printf("ERR! Incorrect value received. Rx = 0x%02X  Tx = 0x%02X\n", temp, ch1);
    }        
        
    uart_putchar (tx_module, ch2); 
    temp = uart_getchar (rx_module); 
    if (temp != ch2)
    {
        error++;
        printf("ERR! Incorrect value received. Rx = 0x%02X  Tx = 0x%02X\n", temp, ch2);
    }        


    /* Test Rx in non-inverted 1/32 mode, Tx in non-inverted 1/32 mode */
    UART_IR_REG(tx_module) = ( UART_IR_IREN_MASK | UART_IR_TNP(2));
    UART_IR_REG(rx_module) = ( UART_IR_IREN_MASK | UART_IR_TNP(2));      
    
    uart_putchar (tx_module, ch1); 
    temp = uart_getchar (rx_module); 
    if (temp != ch1)
    {
        error++;
        printf("ERR! Incorrect value received. Rx = 0x%02X  Tx = 0x%02X\n", temp, ch1);
    }        
        
    uart_putchar (tx_module, ch2); 
    temp = uart_getchar (rx_module); 
    if (temp != ch2)
    {
        error++;
        printf("ERR! Incorrect value received. Rx = 0x%02X  Tx = 0x%02X\n", temp, ch2);
    }        

    /* Test Rx in non-inverted 1/32 mode, Tx in non-inverted 3/16 mode */
    UART_IR_REG(tx_module) = ( UART_IR_IREN_MASK | UART_IR_TNP(0));
    
    uart_putchar (tx_module, ch1); 
    temp = uart_getchar (rx_module); 
    if (temp != ch1)
    {
        error++;
        printf("ERR! Incorrect value received. Rx = 0x%02X  Tx = 0x%02X\n", temp, ch1);
    }        
        
    uart_putchar (tx_module, ch2); 
    temp = uart_getchar (rx_module); 
    if (temp != ch2)
    {
        error++;
        printf("ERR! Incorrect value received. Rx = 0x%02X  Tx = 0x%02X\n", temp, ch2);
    }     
    
    /* Test Rx in non-inverted 1/32 mode, Tx in non-inverted 1/16 mode */
    UART_IR_REG(tx_module) = ( UART_IR_IREN_MASK | UART_IR_TNP(1));
    
    uart_putchar (tx_module, ch1); 
    temp = uart_getchar (rx_module); 
    if (temp != ch1)
    {
        error++;
        printf("ERR! Incorrect value received. Rx = 0x%02X  Tx = 0x%02X\n", temp, ch1);
    }        
        
    uart_putchar (tx_module, ch2); 
    temp = uart_getchar (rx_module); 
    if (temp != ch2)
    {
        error++;
        printf("ERR! Incorrect value received. Rx = 0x%02X  Tx = 0x%02X\n", temp, ch2);
    }     
    
    /* Test Rx in non-inverted 1/32 mode, Tx in non-inverted 1/4 mode */
    UART_IR_REG(tx_module) = ( UART_IR_IREN_MASK | UART_IR_TNP(3));
    
    uart_putchar (tx_module, ch1); 
    temp = uart_getchar (rx_module); 
    if (temp != ch1)
    {
        error++;
        printf("ERR! Incorrect value received. Rx = 0x%02X  Tx = 0x%02X\n", temp, ch1);
    }        
        
    uart_putchar (tx_module, ch2); 
    temp = uart_getchar (rx_module); 
    if (temp != ch2)
    {
        error++;
        printf("ERR! Incorrect value received. Rx = 0x%02X  Tx = 0x%02X\n", temp, ch2);
    }       
    

    /* Test Rx in non-inverted 1/4 mode, Tx in non-inverted 1/4 mode */
    UART_IR_REG(tx_module) = ( UART_IR_IREN_MASK | UART_IR_TNP(3));
    UART_IR_REG(rx_module) = ( UART_IR_IREN_MASK | UART_IR_TNP(3));      
    
    uart_putchar (tx_module, ch1); 
    temp = uart_getchar (rx_module); 
    if (temp != ch1)
    {
        error++;
        printf("ERR! Incorrect value received. Rx = 0x%02X  Tx = 0x%02X\n", temp, ch1);
    }        
        
    uart_putchar (tx_module, ch2); 
    temp = uart_getchar (rx_module); 
    if (temp != ch2)
    {
        error++;
        printf("ERR! Incorrect value received. Rx = 0x%02X  Tx = 0x%02X\n", temp, ch2);
    }        
    

    /* All tests complete. Restore default values before moving on to next test */
    UART_IR_REG(tx_module) = 0;
    UART_IR_REG(rx_module) = 0;

    return error;
}

/********************************************************************/
   
