/*
 * File:		uart_lon_int_tests.c
 * Purpose:		LON mode interrupt tests. The LON
 *                      transmit and receive interrupts are
 *                      found in the uart_lon_tx_tests.c
 *                      and uart_lon_rx_tests.c files. This
 *                      file contains other LON interrupt tests.
 *
 */

#include "common.h"
#include "uart.h"
#include "uart_tests.h"

/* Globals for counting number of interrupts */
int num_lon_isd_int = 0;
int num_lon_psf_int = 0;
int num_lon_txf_int = 0;

/********************************************************************/
/* UART LON initial sync detect interrupt test.
 * Tests detection of initial sync and interrupt generation.
 *
 * This test must be run with a second processor board connected to 
 * supply the expected traffic. The uart_lon_basic_tx_test() function 
 * should be running on the transmitting board that is connected to the 
 * receiver on the board being tested. The LON data line also needs a 
 * pulldown resistor connected.
 *
 *
 * The global num_lon_isd_int counter is used to keep track of the number
 * of initial sync detect interrupts detected during this test. Every time there 
 * is an ISD interrupt detected by the interrupt handler the counter is 
 * incremented. In total this test should generate just one interrupt because
 * the initial sync flag only changes state once each time LON mode is enabled.
 * The number of errors returned by this test is 1 (the expected 
 * number of interrupts) - num_lon_isd_int (the actual number of LON ISD
 * interrupts detected while the test is running).
 */
int uart_lon_isd_int_test(UART_MemMapPtr module)
{
    volatile char ch;
  
    /* Make sure the number of ISD interrupts counter is zero before starting */
    num_lon_isd_int = 0;

    printf("\nStarting UART LON initial sync detect interrupt test.\n");
    printf("\nPlease connect a second board to J2 pin 2 (PTD6/UART0_RX).\n");
    printf("Press any key when ready.\n");
    ch = in_char();
    
    /* Enable the UART0 LON interrupt in the NVIC */
    enable_irq(15);
    
    /* Enable the LON initial sync detect interrupt in the UART */
    UART_IE_REG(module) |= UART_IE_ISDIE_MASK;
    
    /* Configure the UART for LON mode */
    UART_C6_REG(module) = UART_C6_EN709_MASK;
        
    UART_PRE_REG(module) = 0x4;
    
    UART_TPL_REG(module) = 0x01;    
    
    UART_PCTL_REG(module) = 0x10;
    
    printf("This test should generate 1 initial sync detect interrupt.\n");
    printf("UART LON configuration complete. Send 4 packets now...\n\n");
    

    printf("Press any key when all 4 packets have been sent.\n");
    ch = in_char();
    
    /* All tests complete. Return registers to default values */
    UART_C6_REG(module) = 0;   
    UART_PRE_REG(module) = 0;    
    UART_TPL_REG(module) = 0;    
    UART_IE_REG(module) = 0;
    
        
    /* Disable the IRQ */
    disable_irq(15);  
            
    printf("\nNumber of LON initial sync detect interrupts during test = %d\n", num_lon_isd_int);

    return (1 - num_lon_isd_int);        
}
/********************************************************************/
/* UART module LON preamble start interrupt test.
 *
 * Tests LON mode transmit preamble start interrupt for a single UART. 
 * The test will send 7 packets all with different preamble lengths
 * and make sure that the preamble start interrupt is generated for 
 * each
 *
 *
 * The global num_lon_psf_int counter is used to keep track of the number
 * of preamble start interrupts detected during this test. Every time a
 * preamble start interrupt is detected by the interrupt handler the counter is 
 * incremented. In total this test should generate 7 interrupts.
 * The number of errors returned by this test is 7 (the expected 
 * number of interrupts) - num_lon_psf_int (the actual number of LON PSF
 * interrupts detected while the test is running).
 */ 
int uart_lon_psf_int_test(UART_MemMapPtr module)
{
    volatile char ch;
    uint32 i, j, pre_len;
            
    /* Make sure the number of PSF interrupts counter is zero before starting */
    num_lon_psf_int = 0;

    printf("\nStarting UART LON mode preamble start interrupt test.\n");
    
    /* Configure the UART for LON mode */
    UART_C6_REG(module) = UART_C6_EN709_MASK;   
    
    UART_TPL_REG(module) = 0x01;    
    
    /* Set beta1 timer to 1 as 0 seems to max out delay */
    UART_B1T_REG(module) = 1;

    /* Enable the UART0 LON interrupt in the NVIC */
    enable_irq(15);

    /* Enable the preamble start interrupt */
    UART_IE_REG(module) |= UART_IE_PSIE_MASK;
    
    
    /* Test different preamble lengths starting with 4 multiplying x 2) */
    for( pre_len = 4, i = 1; pre_len <= 128; pre_len = pre_len * 2, i++)
    {
        printf("\nSending %d bit preamble...\n",pre_len);
        printf("Press any key when scope is ready.\n");
        ch = in_char();

        /* Test 4-bit preamble. The minimum legal value */
        UART_PRE_REG(module) = pre_len;

        UART_C6_REG(module) |= UART_C6_TX709_MASK;
    
        uart_putchar (module, 0xAA); 
    
        /* Delay to wait for interrupt */
        for(j=0; j < 0x10000; j++);
        
        /* Compare the number of interrupts detected to the loop counter (i) */
        if (num_lon_psf_int != i)
          printf("\nERR!! Incorrect number of interrupts. Expected %d Received %d\n",i, num_lon_psf_int);
    }        


    /* Test 255-bit preamble. The maximum value */
    UART_PRE_REG(module) = 0xFF;
        
    printf("\nSending 255 bit preamble...\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();

    
    UART_C6_REG(module) |= UART_C6_TX709_MASK;
    
    uart_putchar (module, 0xAA); 
    
    /* Delay to wait for interrupt */
    for(j=0; j < 0x10000; j++);
    /* Compare the number of interrupts detected to the loop counter (i) */
    if (num_lon_psf_int != i)
      printf("\nERR!! Incorrect number of interrupts. Expected %d Received %d\n",num_lon_psf_int, i);

    
    /* All tests complete. Return registers to default values */
    UART_C6_REG(module) = 0;   
    UART_PRE_REG(module) = 0;    
    UART_TPL_REG(module) = 0;          
    UART_IE_REG(module) = 0;

    printf("\nNumber of LON preamble start interrupts during test = %d\n", num_lon_psf_int);

    return (7 - num_lon_psf_int);        
}
/********************************************************************/
/* UART LON transmit fail interrupt test.
 * Tests detection and interrupt generation for LON transmit failure.
 * The test will send data packets without disabling the transmitter
 * once the packet is sent. This will cause the UART to attempt to send
 * a second packet where there is no data. This should trigger the TX
 * fail flag and cause the interrupt.
 *
 * The global num_lon_txf_int counter is used to keep track of the number
 * of transmit fail interrupts detected during this test. Every time there 
 * is a ptf interrupt detected by the interrupt handler the counter is 
 * incremented. In total this test should generate 4 different transmit fail
 * interrupts. The number of errors returned by this test is 4 (the expected 
 * number of interrupts) - num_lon_txf_int (the actual number of LON transmit
 * fail interrupts detected while the test is running).
 */
int uart_lon_txf_int_test(UART_MemMapPtr module)
{
    uint32 i;
  
    /* Make sure the number of transmit fail interrupts counter is zero before starting */
    num_lon_txf_int = 0;

    printf("\nStarting UART LON transmit fail interrupt test.\n");
    printf("This test should generate 4 transmit fail interrupts.\n");
    
    /* Enable the UART0 LON interrupt in the NVIC */
    enable_irq(15);
    
    /* Enable the LON Tx fail interrupt in the UART */
    UART_IE_REG(module) |= UART_IE_TXFIE_MASK;
    
    /* Configure the UART for LON mode */
    UART_C6_REG(module) = UART_C6_EN709_MASK;
    
    UART_PRE_REG(module) = 0x4;
    
    UART_TPL_REG(module) = 0x01;    
    
    /* Set beta1 timer to 1 as 0 seems to max out delay */
    UART_B1T_REG(module) = 1;
        
    /* Send 0xAA MSBF */
    UART_C6_REG(module) |= UART_C6_TX709_MASK;
    
    uart_putchar (module, 0xAA); 

    /* Delay to allow time for interrupt */
    for( i=0; i < 0x2000000; i++);
    
    /* Send 0x55 MSBF */    
    UART_C6_REG(module) |= UART_C6_TX709_MASK;
    
    uart_putchar (module, 0x55); 

    /* Delay to allow time for interrupt */
    for( i=0; i < 0x2000000; i++);


    /* Now send the same data but with MSBF bit set */
    /* Configure the UART to send data MSBF first */    
    UART_S2_REG(module) |= UART_S2_MSBF_MASK;

    /* Send 0xAA LSBF */
    UART_C6_REG(module) |= UART_C6_TX709_MASK;
    
    uart_putchar (module, 0xAA); 
    
    /* Delay to allow time for interrupt */
    for( i=0; i < 0x2000000; i++);
        
    /* Send 0x55 LSBF */
    UART_C6_REG(module) |= UART_C6_TX709_MASK;
    
    uart_putchar (module, 0x55); 

    /* Delay to allow time for interrupt */
    for( i=0; i < 0x2000000; i++);
    
    /* All tests complete. Return registers to default values */
    UART_C6_REG(module) = 0;   
    UART_PRE_REG(module) = 0;    
    UART_TPL_REG(module) = 0;    
        
    /* Disable the IRQ */
    disable_irq(15);  
            
    printf("\nNumber of LON tx fail interrupts detected during test = %d\n", num_lon_txf_int);

    return (4 - num_lon_txf_int);        
}
/********************************************************************/