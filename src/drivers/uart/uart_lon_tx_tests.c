/*
 * File:		uart_lon_tx_tests.c
 * Purpose:		LON mode basic transmit tests.
 *
 */

#include "common.h"
#include "uart.h"
#include "uart_tests.h"

/* Globals for counting number of interrupts */
int num_lon_tx_int = 0;


/********************************************************************/
/* UART module LON mode basic transmit test.
 *
 * Tests basic LON mode transmit operation for a single UART. The test 
 * will use both values for MSBF and then transmit specific
 * bytes. A scope should be used to verify that the values transmitted
 * match the expected value.
 *
 * Since the test is not self checking it will always return 0 (pass).
 */
int uart_lon_basic_tx_test(UART_MemMapPtr module)
{
    volatile char ch;
    uint32 i;
            
    printf("\nStarting UART LON mode basic transmit test.\n");
    printf("\nPlease connect a scope to J53 pin 1 (PTD7/UART0_TX).\n");
    printf("Set scope for single edge trigger (rising or falling).\n");
    printf("Press any key when ready.\n");
    ch = in_char();
    
    /* Configure the UART for LON mode */
    UART_C6_REG(module) = UART_C6_EN709_MASK;
    
    UART_PRE_REG(module) = 0x4;
    
    UART_TPL_REG(module) = 0x01;    
    
    /* Set beta1 timer to 1 as 0 seems to max out delay */
    UART_B1T_REG(module) = 1;


    printf("Sending 0xAA...\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();

    UART_C6_REG(module) |= UART_C6_TX709_MASK;
    
    uart_putchar (module, 0xAA); 
    
    /* Wait for transmit to complete */
    while( !(UART_S3_REG(module) & UART_S3_PTXF_MASK));

    /* Disable transmit to prevent a second packet from attempting to go out */
    UART_C6_REG(module) &= ~UART_C6_TX709_MASK;
    
    /* Clear the PTXF flag */
    UART_S3_REG(module) = UART_S3_PTXF_MASK;


    printf("Sending 0x55...\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();

    UART_C6_REG(module) |= UART_C6_TX709_MASK;
    
    uart_putchar (module, 0x55); 

    /* Wait for transmit to complete */
    while( !(UART_S3_REG(module) & UART_S3_PTXF_MASK));

    /* Disable transmit to prevent a second packet from attempting to go out */
    UART_C6_REG(module) &= ~UART_C6_TX709_MASK;
    
    /* Clear the PTXF flag */
    UART_S3_REG(module) = UART_S3_PTXF_MASK;    
    

    /* Now send the same data but with MSBF bit set */
    
    printf("\n\nUART configured for msb first.\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();

    /* Configure the UART to send data MSBF first */    
    UART_S2_REG(module) |= UART_S2_MSBF_MASK;

    printf("Sending 0xAA...\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();

    UART_C6_REG(module) |= UART_C6_TX709_MASK;
    
    uart_putchar (module, 0xAA); 
    
    /* Wait for transmit to complete */
    while( !(UART_S3_REG(module) & UART_S3_PTXF_MASK));

    /* Disable transmit to prevent a second packet from attempting to go out */
    UART_C6_REG(module) &= ~UART_C6_TX709_MASK;

    /* Clear the PTXF flag */
    UART_S3_REG(module) = UART_S3_PTXF_MASK;

    
    printf("Sending 0x55...\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();

    UART_C6_REG(module) |= UART_C6_TX709_MASK;
    
    uart_putchar (module, 0x55); 

    /* Wait for transmit to complete */
    while( !(UART_S3_REG(module) & UART_S3_PTXF_MASK));

    /* Disable transmit to prevent a second packet from attempting to go out */
    UART_C6_REG(module) &= ~UART_C6_TX709_MASK;

    /* Clear the PTXF flag */
    UART_S3_REG(module) = UART_S3_PTXF_MASK;

    
    /* Delay a bit before restoring register defaults */
    //for( i=0; i < 0x1000; i++);
    
    printf("Press any key to finish the test and return registers to default values.\n");
    ch = in_char();
    
    /* All tests complete. Return registers to default values */
    UART_S2_REG(module) &= ~UART_S2_MSBF_MASK;
    UART_C6_REG(module) = 0;   
    UART_PRE_REG(module) = 0;    
    UART_TPL_REG(module) = 0;    
        
    return 0;    
} 
/********************************************************************/
/* UART module LON preamble transmit test.
 *
 * Tests LON mode transmit preamble lengths for a single UART. A 
 * scope should be used to verify that the values transmitted
 * match the expected value.
 *
 * Since the test is not self checking it will always return 0 (pass).
 */
int uart_lon_preamble_tx_test(UART_MemMapPtr module)
{
    volatile char ch;
            
    printf("\nStarting UART LON mode preamble transmit test.\n");
    printf("\nPlease connect a scope to J53 pin 1 (PTD7/UART0_TX).\n");
    printf("Set scope for single edge trigger (rising).\n");
    printf("Press any key when ready.\n");
    ch = in_char();
    
    /* Configure the UART for LON mode */
    UART_C6_REG(module) = UART_C6_EN709_MASK;   
    
    UART_TPL_REG(module) = 0x01;    

    /* Set beta1 timer to 1 as 0 seems to max out delay */
    UART_B1T_REG(module) = 1;
    
    
    printf("Sending 4 bit preamble...\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();

    /* Test 4-bit preamble. The minimum legal value */
    UART_PRE_REG(module) = 0x4;

    UART_C6_REG(module) |= UART_C6_TX709_MASK;
    
    uart_putchar (module, 0xAA); 
    
    /* Wait for transmit to complete */
    while( !(UART_S3_REG(module) & UART_S3_PTXF_MASK));

    /* Disable transmit to prevent a second packet from attempting to go out */
    UART_C6_REG(module) &= ~UART_C6_TX709_MASK;
    
    /* Clear the PTXF flag */
    UART_S3_REG(module) = UART_S3_PTXF_MASK;
    

    printf("Sending 8 bit preamble...\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();

    /* Test 8-bit preamble. */
    UART_PRE_REG(module) = 0x8;

    UART_C6_REG(module) |= UART_C6_TX709_MASK;
    
    uart_putchar (module, 0xAA); 
    
    /* Wait for transmit to complete */
    while( !(UART_S3_REG(module) & UART_S3_PTXF_MASK));

    /* Disable transmit to prevent a second packet from attempting to go out */
    UART_C6_REG(module) &= ~UART_C6_TX709_MASK;
    
    /* Clear the PTXF flag */
    UART_S3_REG(module) = UART_S3_PTXF_MASK;
    
    
    printf("Sending 16 bit preamble...\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();

    /* Test 16-bit preamble. */
    UART_PRE_REG(module) = 0x10;
    
    UART_C6_REG(module) |= UART_C6_TX709_MASK;
    
    uart_putchar (module, 0xAA); 
    
    /* Wait for transmit to complete */
    while( !(UART_S3_REG(module) & UART_S3_PTXF_MASK));

    /* Disable transmit to prevent a second packet from attempting to go out */
    UART_C6_REG(module) &= ~UART_C6_TX709_MASK;
    
    /* Clear the PTXF flag */
    UART_S3_REG(module) = UART_S3_PTXF_MASK;
    
    
    printf("Sending 32 bit preamble...\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();

    /* Test 32-bit preamble. */
    UART_PRE_REG(module) = 0x20;
    
    UART_C6_REG(module) |= UART_C6_TX709_MASK;
    
    uart_putchar (module, 0xAA); 
    
    /* Wait for transmit to complete */
    while( !(UART_S3_REG(module) & UART_S3_PTXF_MASK));

    /* Disable transmit to prevent a second packet from attempting to go out */
    UART_C6_REG(module) &= ~UART_C6_TX709_MASK;
    
    /* Clear the PTXF flag */
    UART_S3_REG(module) = UART_S3_PTXF_MASK;
    
    
    printf("Sending 64 bit preamble...\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();

    /* Test 64-bit preamble. */
    UART_PRE_REG(module) = 0x40;
    
    UART_C6_REG(module) |= UART_C6_TX709_MASK;
    
    uart_putchar (module, 0xAA); 
    
    /* Wait for transmit to complete */
    while( !(UART_S3_REG(module) & UART_S3_PTXF_MASK));

    /* Disable transmit to prevent a second packet from attempting to go out */
    UART_C6_REG(module) &= ~UART_C6_TX709_MASK;
    
    /* Clear the PTXF flag */
    UART_S3_REG(module) = UART_S3_PTXF_MASK;
    
    
    printf("Sending 128 bit preamble...\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();

    /* Test 128-bit preamble. */
    UART_PRE_REG(module) = 0x80;
    
    UART_C6_REG(module) |= UART_C6_TX709_MASK;
    
    uart_putchar (module, 0xAA); 
    
    /* Wait for transmit to complete */
    while( !(UART_S3_REG(module) & UART_S3_PTXF_MASK));

    /* Disable transmit to prevent a second packet from attempting to go out */
    UART_C6_REG(module) &= ~UART_C6_TX709_MASK;
    
    /* Clear the PTXF flag */
    UART_S3_REG(module) = UART_S3_PTXF_MASK;
    
    
    printf("Sending 255 bit preamble...\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();

    /* Test 255-bit preamble. The maximum value */
    UART_PRE_REG(module) = 0xFF;
    
    UART_C6_REG(module) |= UART_C6_TX709_MASK;
    
    uart_putchar (module, 0xAA); 
    
    /* Wait for transmit to complete */
    while( !(UART_S3_REG(module) & UART_S3_PTXF_MASK));

    /* Disable transmit to prevent a second packet from attempting to go out */
    UART_C6_REG(module) &= ~UART_C6_TX709_MASK;
    
    /* Clear the PTXF flag */
    UART_S3_REG(module) = UART_S3_PTXF_MASK;
    

    /* All tests complete. Return registers to default values */
    UART_C6_REG(module) = 0;   
    UART_PRE_REG(module) = 0;    
    UART_TPL_REG(module) = 0;          

    return 0;
}
/********************************************************************/
/* UART LON transmit packet interrupt test.
 * Tests detection and interrupt generation for LON transmit complete.
 *
 * The global num_lon_tx_int counter is used to keep track of the number
 * of transmit complete interrupts detected during this test. Every time there 
 * is a ptxf interrupt detected by the interrupt handler the counter is 
 * incremented. In total this test should generate 4 different transmit complete
 * interrupts. The number of errors returned by this test is 4 (the expected 
 * number of interrupts) - num_lon_tx_int (the actual number of LON transmit
 * complete interrupts detected while the test is running).
 */
int uart_lon_tx_int_test(UART_MemMapPtr module)
{
    uint32 i;
  
    /* Make sure the number of transmit complete interrupts counter is zero before starting */
    num_lon_tx_int = 0;

    printf("\nStarting UART LON transmit complete interrupt test.\n");
    printf("This test should generate 4 transmit complete interrupts.\n");
    
    /* Enable the UART0 LON interrupt in the NVIC */
    enable_irq(15);
    
    /* Enable the LON Tx packet interrupt in the UART */
    UART_IE_REG(module) |= UART_IE_PTXIE_MASK;
    
    /* Configure the UART for LON mode */
    UART_C6_REG(module) = UART_C6_EN709_MASK;
    
    /* Try enabling the TxFIFO */
    UART_PFIFO_REG(module) |= UART_PFIFO_TXFE_MASK;
    
    
    UART_PRE_REG(module) = 0x4;
    
    UART_TPL_REG(module) = 0x04;    
    
    UART_PCTL_REG(module) = 0x10;
    
    /* Send 0xAA MSBF */
    UART_C6_REG(module) |= UART_C6_TX709_MASK;
    
    UART_D_REG(module) = 0xAA; 
    UART_D_REG(module) = 0xAA; 
    UART_D_REG(module) = 0xAA; 
    UART_D_REG(module) = 0xAA; 

    /* Delay to allow time for interrupt */
    for( i=0; i < 0x10000; i++);
    
    /* Send 0x55 MSBF */    
    UART_C6_REG(module) |= UART_C6_TX709_MASK;
    
    uart_putchar (module, 0x55); 
    uart_putchar (module, 0x55); 
    uart_putchar (module, 0x55); 
    uart_putchar (module, 0x55); 

    /* Delay to allow time for interrupt */
    for( i=0; i < 0x10000; i++);


    /* Now send the same data but with MSBF bit set */
    /* Configure the UART to send data MSBF first */    
    UART_S2_REG(module) |= UART_S2_MSBF_MASK;

    /* Send 0xAA LSBF */
    UART_C6_REG(module) |= UART_C6_TX709_MASK;
    
    uart_putchar (module, 0xAA); 
    uart_putchar (module, 0x55); 
    uart_putchar (module, 0x55); 
    uart_putchar (module, 0x55); 
    
    /* Delay to allow time for interrupt */
    for( i=0; i < 0x10000; i++);
        
    /* Send 0x55 LSBF */
    UART_C6_REG(module) |= UART_C6_TX709_MASK;
    
    uart_putchar (module, 0x55); 
    uart_putchar (module, 0x55); 
    uart_putchar (module, 0x55); 
    uart_putchar (module, 0x55); 

    /* Delay to allow time for interrupt */
    for( i=0; i < 0x10000; i++);
    
    /* All tests complete. Return registers to default values */
    UART_C6_REG(module) = 0;   
    UART_PRE_REG(module) = 0;    
    UART_TPL_REG(module) = 0;    
        
    /* Disable the IRQ */
    disable_irq(15);  
            
    printf("\nNumber of LON tx interrupts detected during test = %d\n", num_lon_tx_int);

    return (4 - num_lon_tx_int);        
}
/********************************************************************/
/* UART module LON mode transmit packet length test.
 *
 * Tests LON mode transmit packet length for a single UART. The test 
 * will test a variety of transmit lengths including the minimum and
 * maximum allowable values. A scope should be used to verify the 
 * packet lengths transmitted match the expected length.
 *
 * Since the test is not self checking it will always return 0 (pass).
 */
int uart_lon_tx_len_test(UART_MemMapPtr module)
{
    volatile char ch;
    uint32 tx_len, i;
            
    printf("\nStarting UART LON mode transmit packet length test.\n");
    printf("\nPlease connect a scope to J53 pin 1 (PTD7/UART0_TX).\n");
    printf("Set scope for single edge trigger (rising or falling).\n");
    printf("Press any key when ready.\n");
    ch = in_char();
    
    /* Configure the UART for LON mode */
    UART_C6_REG(module) = UART_C6_EN709_MASK;
    
    /* Set the preamble length */
    UART_PRE_REG(module) = 0x7;
    
    /* Set beta1 timer to 1 as 0 seems to max out delay */
    UART_B1T_REG(module) = 1;

    /* In order to avoid possible underflows, enable the TxFIFO */
    UART_PFIFO_REG(module) |= UART_PFIFO_TXFE_MASK;
    
    /* Set the watermark to 4 */
    UART_TWFIFO_REG(module) = 4;
    
    /* Start with a packet length of 1, then multiply packet length x2 */
    for(tx_len=1; tx_len <= 0x80; tx_len = tx_len * 2)
    {  
    
        printf("\nReady to send %d byte packet...\n", tx_len);
        printf("Press any key when scope is ready.\n");
        ch = in_char();

        /* Set transmit packet length */
        UART_TPL_REG(module) = tx_len;    
    
        UART_C6_REG(module) |= UART_C6_TX709_MASK;

        /* Transmit bytes up to the packet length being tested */
        for(i=0; i < tx_len; i++)
          uart_putchar(module, i);
        
        /* Wait for transmit to complete */
        while( !(UART_S3_REG(module) & UART_S3_PTXF_MASK));

        /* Disable transmit to prevent a second packet from attempting to go out */
        UART_C6_REG(module) &= ~UART_C6_TX709_MASK;
    
        /* Clear the PTXF flag */
        UART_S3_REG(module) = UART_S3_PTXF_MASK;
    }        
        
    
    /* Test max packet length (255) */
    tx_len = 0xFF;
    
    printf("\nReady to send %d byte packet...\n", tx_len);
    printf("Press any key when scope is ready.\n");
    ch = in_char();

    /* Set transmit packet length */
    UART_TPL_REG(module) = tx_len;    
    
    UART_C6_REG(module) |= UART_C6_TX709_MASK;

    /* Transmit bytes up to the packet length being tested */
    for(i=0; i < tx_len; i++)
      uart_putchar(module, i);
                
    /* Wait for transmit to complete */
    while( !(UART_S3_REG(module) & UART_S3_PTXF_MASK));

    /* Disable transmit to prevent a second packet from attempting to go out */
    UART_C6_REG(module) &= ~UART_C6_TX709_MASK;
    
    /* Clear the PTXF flag */
    UART_S3_REG(module) = UART_S3_PTXF_MASK;
    

    /* All tests complete. Return registers to default values */
    UART_S2_REG(module) &= ~UART_S2_MSBF_MASK;
    UART_C6_REG(module) = 0;   
    UART_PRE_REG(module) = 0;    
    UART_TPL_REG(module) = 0;    
        
    return 0;    
} 
/********************************************************************/   
