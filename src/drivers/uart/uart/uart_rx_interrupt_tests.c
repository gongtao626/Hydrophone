/*
 * File:		uart_rx_interrupt_tests.c
 * Purpose:		UART receive interrupt tests. These tests cover
 *              all receive type interrupts (both status and error).
 *
 */

#include "common.h"
#include "uart.h"
#include "uart_tests.h"

/* Globals for counting number of interrupts */
int num_rdrf_int = 0;
int num_or_errors = 0;
int num_rxuf_errors = 0;
int num_parity_errors = 0;
int num_nf_errors = 0;
int num_framing_errors = 0;
int num_idle_int = 0;
int num_lin_breaks = 0;

/* Globals from sysinit.c */
extern int core_clk_khz;
extern int core_clk_mhz;
extern int periph_clk_khz;

/********************************************************************/
/* UART receive data register full interrupt test.
 * Tests detection of receive data status and interrupt generation. This
 * test will run once using a UART without a FIFO as the receiver, and
 * then run again with a UART that includes a FIFO used as the receiver.
 * 
 *
 * The global num_rdrf_int counter is used to keep track of the number
 * of receive register full interrupts. Every time there is a receive register
 * interrupt detected the interrupt handler increments the counter. In total
 * this test should generate 2 different receiver register full interrupts.
 * The number of errors returned by this test is 2 (the expected number of 
 * interrupts) - num_rdrf_int (the actual number of interrupts detected while 
 * the test is running).
 */
int uart_rdrf_interrupt_test()
{
    UART_MemMapPtr tx_module, rx_module;
    volatile char ch;
    uint32 i;    
    
    /* Make sure the number of receive register full interrupts is zero before starting */
    num_rdrf_int = 0;
    
    printf("\n\nStarting receive data register full interrupt test without a FIFO.\n");

    /* Determine which UART to use as the transmitter for testing. UART0 or UART1
     * could be used. We'll use UART0 as long as it isn't used as the TERM_PORT.
     */
    if (TERM_PORT != UART0_BASE_PTR)
    {      
       tx_module = UART0_BASE_PTR;   // set the module pointer
       
       printf("\nUsing UART0 on the daughter card as the transmitter.");
       printf("\nConnect one end of a blue wire to J4 pin 2 (UART0_TX)");
    }      
    else
    {      
       tx_module = UART1_BASE_PTR;   // set the module pointer
       
       printf("\nUsing UART1 on the daughter card as the transmitter.");
       printf("\nConnect one end of a blue wire to J4 pin 2 (UART1_TX)");
    }

    printf("\nPress any key when ready.\n");
    ch = in_char();
    
    /* Determine which UART to use as the receiver for testing. UART1 or UART2
     * could be used. We'll use UART1 as long as it isn't used as the TERM_PORT.
     */
    if (TERM_PORT != UART1_BASE_PTR)
    {      
       rx_module = UART1_BASE_PTR;   // set the module pointer
       
       /* Configure NVIC to enable UART status interrupts for the receiver */
       enable_irq(34-16);
       
       printf("\nUsing UART1 on the daughter card as the receiver.");
       printf("\nConnect other end of blue wire to J13 pin 2 (UART1_RX)");
    }      
    else
    {      
       rx_module = UART2_BASE_PTR;   // set the module pointer

       /* Configure NVIC to enable UART status interrupts for the receiver */
       enable_irq(36-16);

       printf("\nUsing UART2 on the daughter card as the receiver.");
       printf("\nConnect other end of blue wire J9 pin 14 (UART2_RX)");
    }

    printf("\nPress any key when ready.\n");
    ch = in_char();
    
    printf("Setup complete. Starting RDRF test...\n");
    printf("This test should generate 1 RDRF interrupt.\n");
    

    /* Enable the rx full interrupt in the receiver */
    UART_C2_REG(rx_module) |= UART_C2_RIE_MASK;    
        
    /* Transmit data to fill the receiver */
    UART_D_REG(tx_module) = 0xAA;   

    /* Delay to allow time for interrupt */
    for( i=0; i < 0x1000; i++);
    
    /* Disable the rx full interrupt in the receiver */
    UART_C2_REG(rx_module) &= ~UART_C2_RIE_MASK;    

    /* Disable the IRQ */
    if (rx_module == UART1_BASE_PTR)
        disable_irq(34-16);  //disable interrupts for UART1 in NVIC
    else
        disable_irq(36-16);  //disable interrupts for UART2 in NVIC
    
    printf("\nStarting receive data register full interrupt test with a FIFO.\n");

    /* Determine which UART to use as the receiver for testing with a FIFO. 
     * UART0 could only be used. We'll use UART0 as long as it isn't used
     * as the TERM_PORT.
     */
    if (TERM_PORT != UART0_BASE_PTR)
    {      
       rx_module = UART0_BASE_PTR;   // set the module pointer
       
       /* Configure NVIC to enable UART status interrupts for the receiver */
       enable_irq(32-16);
       tx_module = UART2_BASE_PTR;;
    }      
    else
    {      
       //rx_module = UART1_BASE_PTR;   // set the module pointer

       /* Configure NVIC to enable UART status interrupts for the receiver */
       //enable_irq(34-16);
      printf("UART0 cannot be used as the terminal port\n");
      return -1;
    }

    printf("\nPress any key when ready.\n");
    ch = in_char();
    
    printf("Setup complete. Starting RDRF test with FIFO enabled...\n");
    printf("This test should generate 1 RDRF interrupt.\n");
    

    /* Enable the receive FIFO */
    UART_PFIFO_REG(rx_module) |= UART_PFIFO_RXFE_MASK;

    /* Enable the rx full interrupt in the receiver */
    UART_C2_REG(rx_module) |= UART_C2_RIE_MASK;    
        
    /* Transmit data to fill the receiver */
    UART_D_REG(tx_module) = 0xAA;   

    /* Delay to allow time for interrupt */
    for( i=0; i < 0x1000; i++);
         
    /* Disable the rx full interrupt in the receiver */
    UART_C2_REG(rx_module) &= ~UART_C2_RIE_MASK;    

    /* Disable the IRQ */
    if (rx_module == UART0_BASE_PTR)
        disable_irq(32-16);  //disable interrupts for UART0 in NVIC
    else
        disable_irq(34-16);  //disable interrupts for UART1 in NVIC
    
    /* Disable the receive FIFO */
    UART_PFIFO_REG(rx_module) &= ~UART_PFIFO_RXFE_MASK;
    
         
    printf("\nNumber of receiver full interrupts detected during test = %d\n", num_rdrf_int);

    return (2 - num_rdrf_int);
}    
/********************************************************************/
/* UART parity error tests.
 * Tests detection of parity errors for even and odd parity modes different
 * settings for MSBF in 8-, 9-, and 10-bit modes. This function also tests
 * the parity error interrupt generation. 
 * 
 * Receive tests will use one UART as the transmitter and other as the 
 * receiver. Using the same module to communicate is not an ideal test setup,
 * but since the transmit functions were previously verified using a scope
 * we know they send data as expected. So using the multiple instantiations
 * of the same module for the transmit and receive side of this test should
 * be ok.
 *
 * The global num_parity_errors counter is used to keep track of the number
 * of parity errors detected during this test. Ever time there is a parity
 * error detected by the interrupt handler the counter is incremented. In total
 * this test should generate 24 different parity errors. The number of errors
 * returned by this test is 24 (the expected number of errors) - num_parity_errors
 * (the actual number of parity errors detected while the test is running).
 */
int uart_parity_error_test()
{
    UART_MemMapPtr tx_module, rx_module;
    volatile char ch, ch1, ch2;
    uint32 i;    
    
    /* Make sure the number of parity errors counter is zero before starting */
    num_parity_errors = 0;
    
    /* Set values for ch1 and ch2 that will be used for all tests */
    ch1 = 0x2A; // 0x02A will be used for 10-bit tests; the 9th bit is hard coded to 0
    ch2 = 0x15; // 0x115 will be used for 10-bit tests; the 9th bit is hard coded to 1
        
    printf("\n\nStarting parity error tests.\n");

    /* Determine which UART to use as the transmitter for testing. UART2 or UART3
     * could be used. We'll use UART2 as long as it isn't used as the TERM_PORT.
     */
    if (TERM_PORT != UART0_BASE_PTR)
    {      
       tx_module = UART0_BASE_PTR;   // set the module pointer
       
       printf("\nUsing UART0 on the daughter card as the transmitter.");
       printf("\nConnect one end of a blue wire to J4 pin 2 (UART0_TX)");
    }      
    else
    {
       tx_module = UART1_BASE_PTR;   // set the module pointer
       
       printf("\nUsing UART1 on the daughter card as the transmitter.");
       printf("\nConnect one end of a blue wire to J4 pin 2 (UART1_TX)");
    }

    printf("\nPress any key when ready.\n");
    ch = in_char();
    #warning "check uart port being used"
    /* Determine which UART to use as the receiver for testing. UART4 or UART5
     * could be used. We'll use UART4 as long as it isn't used as the TERM_PORT.
     */
    if (TERM_PORT != UART1_BASE_PTR)
    {      
       rx_module = UART1_BASE_PTR;   // set the module pointer
       
       /* Configure NVIC to enable UART error and status interrupts for the receiver */
       enable_irq(34-16);
       enable_irq(35-16);
       
       printf("\nUsing UART1 on the daughter card as the receiver.");
       printf("\nConnect other end of blue wire to J13 pin 2 (UART1_RX)");
    }      
    else
    {      
       rx_module = UART2_BASE_PTR;   // set the module pointer

       /* Configure NVIC to enable UART error and status interrupts for the receiver */
       enable_irq(36-16);
       enable_irq(37-16);

       printf("\nUsing UART2 on the daughter card as the receiver.");
       printf("\nConnect other end of blue wire J9 pin 14 (UART2_RX)");
    }

    printf("\nPress any key when ready.\n");
    ch = in_char();
    
    printf("Setup complete. Starting parity error tests...\n");
    printf("This test should generate 24 parity errors.\n");
    
    /* Enable parity error interrupts for the receiver */
    UART_C3_REG(rx_module) |= UART_C3_PEIE_MASK;
    
    /* Enable parity on both transmitter and receiver */
    UART_C1_REG(tx_module) |= UART_C1_PE_MASK;    
    UART_C1_REG(rx_module) |= UART_C1_PE_MASK;    

  
    /* Test 8-bit communication with even parity enabled and lsb first */
    
    /* Configure the receiver for even parity */
    UART_C1_REG(rx_module) &= ~UART_C1_PT_MASK;
    
    /* In order to generate a parity error, configure the transmitter for odd parity */
    UART_C1_REG(tx_module) |= UART_C1_PT_MASK;
    
    uart_putchar (tx_module, ch1); 
    
    /* Delay to allow time for interrupt */
    for( i=0; i < 0x1000; i++);
        
    uart_putchar (tx_module, ch2); 

    /* Delay to allow time for interrupt */
    for( i=0; i < 0x1000; i++);
  

    /* Test 8-bit communication with odd parity enabled and lsb first */
    
    /* Configure the receiver for odd parity */
    UART_C1_REG(rx_module) |= UART_C1_PT_MASK;
    
    /* In order to generate a parity error, configure the transmitter for even parity */
    UART_C1_REG(tx_module) &= ~UART_C1_PT_MASK;
    
    uart_putchar (tx_module, ch1); 
    
    /* Delay to allow time for interrupt between characters. */
    for( i=0; i < 0x1000; i++);
        
    uart_putchar (tx_module, ch2); 
 
    /* Delay to allow time for interrupt */
    for( i=0; i < 0x1000; i++);


    
    /* Enable MSB first mode */
    UART_S2_REG(tx_module) |= UART_S2_MSBF_MASK;    
    UART_S2_REG(rx_module) |= UART_S2_MSBF_MASK;    
 
    /* Test 8-bit communication with even parity enabled and msb first */
    
    /* Configure the receiver for even parity */
    UART_C1_REG(rx_module) &= ~UART_C1_PT_MASK;
    
    /* In order to generate a parity error, configure the transmitter for odd parity */
    UART_C1_REG(tx_module) |= UART_C1_PT_MASK;
    
    uart_putchar (tx_module, ch1); 
    
    /* Delay to allow time for interrupt */
    for( i=0; i < 0x1000; i++);
        
    uart_putchar (tx_module, ch2); 

    /* Delay to allow time for interrupt */
    for( i=0; i < 0x1000; i++);
  

    /* Test 8-bit communication with odd parity enabled and msb first */
    
    /* Configure the receiver for odd parity */
    UART_C1_REG(rx_module) |= UART_C1_PT_MASK;
    
    /* In order to generate a parity error, configure the transmitter for even parity */
    UART_C1_REG(tx_module) &= ~UART_C1_PT_MASK;
    
    uart_putchar (tx_module, ch1); 
    
    /* Delay to allow time for interrupt between characters. */
    for( i=0; i < 0x1000; i++);
        
    uart_putchar (tx_module, ch2); 
 
    /* Delay to allow time for interrupt */
    for( i=0; i < 0x1000; i++);
    
    /* Disable MSB first mode before next test */
    UART_S2_REG(tx_module) &= ~UART_S2_MSBF_MASK;    
    UART_S2_REG(rx_module) &= ~UART_S2_MSBF_MASK;    
    

    
    /* Start of 9-bit mode tests - Enable 9-bit mode */
    UART_C1_REG(tx_module) |= UART_C1_M_MASK;
    UART_C1_REG(rx_module) |= UART_C1_M_MASK;
    
    /* Test 9-bit communication with odd parity enabled and lsb first */
    
    /* Configure the receiver for even parity */
    UART_C1_REG(rx_module) &= ~UART_C1_PT_MASK;
    
    /* In order to generate a parity error, configure the transmitter for odd parity */
    UART_C1_REG(tx_module) |= UART_C1_PT_MASK;
    
    uart_putchar (tx_module, ch1); 
    
    /* Delay to allow time for interrupt */
    for( i=0; i < 0x1000; i++);
        
    uart_putchar (tx_module, ch2); 

    /* Delay to allow time for interrupt */
    for( i=0; i < 0x1000; i++);
  

    /* Test 9-bit communication with odd parity enabled and lsb first */
    
    /* Configure the receiver for odd parity */
    UART_C1_REG(rx_module) |= UART_C1_PT_MASK;
    
    /* In order to generate a parity error, configure the transmitter for even parity */
    UART_C1_REG(tx_module) &= ~UART_C1_PT_MASK;
    
    uart_putchar (tx_module, ch1); 
    
    /* Delay to allow time for interrupt between characters. */
    for( i=0; i < 0x1000; i++);
        
    uart_putchar (tx_module, ch2); 
 
    /* Delay to allow time for interrupt */
    for( i=0; i < 0x1000; i++);


    
    /* Enable MSB first mode */
    UART_S2_REG(tx_module) |= UART_S2_MSBF_MASK;    
    UART_S2_REG(rx_module) |= UART_S2_MSBF_MASK;    
 
    /* Test 9-bit communication with even parity enabled and msb first */
    
    /* Configure the receiver for even parity */
    UART_C1_REG(rx_module) &= ~UART_C1_PT_MASK;
    
    /* In order to generate a parity error, configure the transmitter for odd parity */
    UART_C1_REG(tx_module) |= UART_C1_PT_MASK;
    
    uart_putchar (tx_module, ch1); 
    
    /* Delay to allow time for interrupt */
    for( i=0; i < 0x1000; i++);
        
    uart_putchar (tx_module, ch2); 

    /* Delay to allow time for interrupt */
    for( i=0; i < 0x1000; i++);
  

    /* Test 9-bit communication with odd parity enabled and msb first */
    
    /* Configure the receiver for odd parity */
    UART_C1_REG(rx_module) |= UART_C1_PT_MASK;
    
    /* In order to generate a parity error, configure the transmitter for even parity */
    UART_C1_REG(tx_module) &= ~UART_C1_PT_MASK;
    
    uart_putchar (tx_module, ch1); 
    
    /* Delay to allow time for interrupt between characters. */
    for( i=0; i < 0x1000; i++);
        
    uart_putchar (tx_module, ch2); 
 
    /* Delay to allow time for interrupt */
    for( i=0; i < 0x1000; i++);
    
    /* Disable MSB first mode before next test */
    UART_S2_REG(tx_module) &= ~UART_S2_MSBF_MASK;    
    UART_S2_REG(rx_module) &= ~UART_S2_MSBF_MASK;    



    /* Start of 10-bit mode tests - Enable 9-bit mode (M and PE bits are already set)*/
    UART_C4_REG(tx_module) |= UART_C4_M10_MASK;
    UART_C4_REG(rx_module) |= UART_C4_M10_MASK;
    
    /* Test 10-bit communication with odd parity enabled and lsb first */
    
    /* Configure the receiver for even parity */
    UART_C1_REG(rx_module) &= ~UART_C1_PT_MASK;
    
    /* In order to generate a parity error, configure the transmitter for odd parity */
    UART_C1_REG(tx_module) |= UART_C1_PT_MASK;
    
    /* Make sure T8 (the 9th bit) is cleared */
    UART_C3_REG(tx_module) &= ~UART_C3_T8_MASK;   
    uart_putchar (tx_module, ch1); 
    
    /* Delay to allow time for interrupt */
    for( i=0; i < 0x1000; i++);
        
    /* Make sure T8 (the 9th bit) is set */
    UART_C3_REG(tx_module) |= UART_C3_T8_MASK;   
    uart_putchar (tx_module, ch2); 

    /* Delay to allow time for interrupt */
    for( i=0; i < 0x1000; i++);
  

    /* Test 10-bit communication with odd parity enabled and lsb first */
    
    /* Configure the receiver for odd parity */
    UART_C1_REG(rx_module) |= UART_C1_PT_MASK;
    
    /* In order to generate a parity error, configure the transmitter for even parity */
    UART_C1_REG(tx_module) &= ~UART_C1_PT_MASK;
    
    /* Make sure T8 (the 9th bit) is cleared */
    UART_C3_REG(tx_module) &= ~UART_C3_T8_MASK;   
    uart_putchar (tx_module, ch1); 
    
    /* Delay to allow time for interrupt */
    for( i=0; i < 0x1000; i++);
        
    /* Make sure T8 (the 9th bit) is set */
    UART_C3_REG(tx_module) |= UART_C3_T8_MASK;   
    uart_putchar (tx_module, ch2); 

    /* Delay to allow time for interrupt */
    for( i=0; i < 0x1000; i++);


    
    /* Enable MSB first mode */
    UART_S2_REG(tx_module) |= UART_S2_MSBF_MASK;    
    UART_S2_REG(rx_module) |= UART_S2_MSBF_MASK;    
 
    /* Test 10-bit communication with even parity enabled and msb first */
    
    /* Configure the receiver for even parity */
    UART_C1_REG(rx_module) &= ~UART_C1_PT_MASK;
    
    /* In order to generate a parity error, configure the transmitter for odd parity */
    UART_C1_REG(tx_module) |= UART_C1_PT_MASK;
    
    /* Make sure T8 (the 9th bit) is cleared */
    UART_C3_REG(tx_module) &= ~UART_C3_T8_MASK;   
    uart_putchar (tx_module, ch1); 
    
    /* Delay to allow time for interrupt */
    for( i=0; i < 0x1000; i++);
        
    /* Make sure T8 (the 9th bit) is set */
    UART_C3_REG(tx_module) |= UART_C3_T8_MASK;   
    uart_putchar (tx_module, ch2); 

    /* Delay to allow time for interrupt */
    for( i=0; i < 0x1000; i++);
  

    /* Test 10-bit communication with odd parity enabled and msb first */
    
    /* Configure the receiver for odd parity */
    UART_C1_REG(rx_module) |= UART_C1_PT_MASK;
    
    /* In order to generate a parity error, configure the transmitter for even parity */
    UART_C1_REG(tx_module) &= ~UART_C1_PT_MASK;
    
    /* Make sure T8 (the 9th bit) is cleared */
    UART_C3_REG(tx_module) &= ~UART_C3_T8_MASK;   
    uart_putchar (tx_module, ch1); 
    
    /* Delay to allow time for interrupt */
    for( i=0; i < 0x1000; i++);
        
    /* Make sure T8 (the 9th bit) is set */
    UART_C3_REG(tx_module) |= UART_C3_T8_MASK;   
    uart_putchar (tx_module, ch2); 

    /* Delay to allow time for interrupt */
    for( i=0; i < 0x1000; i++);
    
    /* Disable MSB first mode before next test */
    UART_S2_REG(tx_module) &= ~UART_S2_MSBF_MASK;    
    UART_S2_REG(rx_module) &= ~UART_S2_MSBF_MASK;    
    
    
    printf("\nNumber of parity errors detected during test = %d\n", num_parity_errors);
    
    
    /* Testing is complete. Put everything back the way it was */
    /* Disable interrupts in the NVIC */
    if (rx_module == UART1_BASE_PTR)
    {      
       disable_irq(34-16);
       disable_irq(35-16);
    }      
    else
    {      
       /*disabling UART2*/
       disable_irq(36-16);
       disable_irq(37-16);
    }
    
    /* Disnable parity error interrupts for the receiver */
    UART_C3_REG(rx_module) &= ~UART_C3_PEIE_MASK;
    
    /* Clear M, M10, and PE to go back to default mode before starting next test */
    UART_C1_REG(tx_module) = 0;
    UART_C4_REG(tx_module) = 0;
    UART_C1_REG(rx_module) = 0;
    UART_C4_REG(rx_module) = 0;
    
    return (24 - num_parity_errors);
}
/********************************************************************/
/* UART receiver overrun error interrupt test.
 * Tests detection of receive FIFO overrun detection and interrupt generation. 
 * This test will run once using a UART without a FIFO as the receiver, and
 * then run again with a UART that includes a FIFO used as the receiver.
 * 
 *
 * The global num_or_int counter is used to keep track of the number
 * of receiver overrun interrupts. Every time there is a receiver overrun
 * interrupt detected the interrupt handler increments the counter. In total
 * this test should generate 2 different receiver overrun interrupts.
 * The number of errors returned by this test is 2 (the expected number of 
 * interrupts) - num_or_errors (the actual number of interrupts detected while 
 * the test is running).
 */
int uart_or_error_test()
{
    UART_MemMapPtr tx_module, rx_module;
    volatile char ch;
    uint32 i;    
    
    /* Make sure the number of receiver overrun interrupts is zero before starting */
    num_or_errors = 0;
    
    printf("\n\nStarting receiver overrun interrupt test without a FIFO.\n");

    /* Determine which UART to use as the transmitter for testing. UART2 or UART3
     * could be used. We'll use UART2 as long as it isn't used as the TERM_PORT.
     */
    if (TERM_PORT != UART0_BASE_PTR)
    {      
       tx_module = UART0_BASE_PTR;   // set the module pointer
       
       printf("\nUsing UART0 on the daughter card as the transmitter.");
       printf("\nConnect one end of a blue wire to J4 pin 2 (UART0_TX)");
    }      
    else
    {      
       tx_module = UART1_BASE_PTR;   // set the module pointer

       printf("\nUsing UART1 on the daughter card as the transmitter.");
       printf("\nConnect one end of a blue wire to J43 pin 2 (UART1_TX)");
    }

    printf("\nPress any key when ready.\n");
    ch = in_char();
    
    /* Determine which UART to use as the receiver for testing. UART4 or UART5
     * could be used. We'll use UART4 as long as it isn't used as the TERM_PORT.
     */
    if (TERM_PORT != UART1_BASE_PTR)
    {      
       rx_module = UART1_BASE_PTR;   // set the module pointer
       
       /* Configure NVIC to enable UART status interrupts for the receiver */
       enable_irq(35-16);
       
       printf("\nUsing UART1 on the daughter card as the receiver.");
       printf("\nConnect other end of blue wire to J13 pin 2 (UART1_RX)");
    }      
    else
    {      
       rx_module = UART2_BASE_PTR;   // set the module pointer

       /* Configure NVIC to enable UART status interrupts for the receiver */
       enable_irq(37-16);

       printf("\nUsing UART2 on the daughter card as the receiver.");
       printf("\nConnect other end of blue wire J9 pin 14 (UART2_RX)");
    }

    printf("\nPress any key when ready.\n");
    ch = in_char();
    
    printf("Setup complete. Starting OR test...\n");
    printf("This test should generate 1 OR interrupt.\n");
    

    /* Enable the receiver overrun interrupt */
    UART_C3_REG(rx_module) |= UART_C3_ORIE_MASK;    

    /* Transmit data to overrun the receiver */
    uart_putchar(tx_module,0x1);   
    uart_putchar(tx_module,0x2);   

    /* Delay to allow time for interrupt */
    for( i=0; i < 0x200000; i++);

    /* Disable the receiver overrun interrupt in the receiver */
    UART_C3_REG(rx_module) &= ~UART_C3_ORIE_MASK;    

    /* Disable the IRQ */
    if (rx_module == UART1_BASE_PTR)
        disable_irq(34-16);  //disable interrupts for UART1 in NVIC
    else
        disable_irq(36-16);  //disable interrupts for UART2 in NVIC
    
    
    printf("\nStarting receiver overrun interrupt test with a FIFO.\n");

    /* Determine which UART to use as the receiver for testing with a FIFO. 
     * UART0 or UART1 could be used. We'll use UART0 as long as it isn't used
     * as the TERM_PORT.
     */
    if (TERM_PORT != UART0_BASE_PTR)
    {      
       rx_module = UART0_BASE_PTR;   // set the module pointer
       tx_module = UART2_BASE_PTR;
       
       /* Configure NVIC to enable UART status interrupts for the receiver */
       enable_irq(33-16);
       
       printf("\nUsing UART0 on the daughter card as the receiver.");
       printf("\nRemove blue wire from previous receiver.");
       printf("\nConnect the blue wire to J2 pin 2 (UART0_RX)");
    }      
    else
    {      
       //rx_module = UART1_BASE_PTR;   // set the module pointer

       /* Configure NVIC to enable UART status interrupts for the receiver */
       //enable_irq(34-16);

       //printf("\nUsing UART0 on the daughter card as the receiver.");
       //printf("\nRemove blue wire from previous receiver.");
       //printf("\nConnect the blue wire to J7 pin 5 (UART1_RX)");
      printf("UART0 cannot be the TERM PORT\n");
      return -1;
    }

    printf("\nPress any key when ready.\n");
    ch = in_char();
    
    printf("Setup complete. Starting OR test with FIFO enabled...\n");
    printf("This test should generate 1 OR interrupt.\n");
    

    /* Enable the receive FIFO */
    UART_PFIFO_REG(rx_module) |= UART_PFIFO_RXFE_MASK;

    /* Enable the receiver overrun interrupt */
    UART_C3_REG(rx_module) |= UART_C3_ORIE_MASK;    
    
    /* Transmit data to overrun the receiver */
    uart_putchar(tx_module,0x1);   
    uart_putchar(tx_module,0x2);   
    uart_putchar(tx_module,0x3);   
    uart_putchar(tx_module,0x4);   
    uart_putchar(tx_module,0x5);   
    uart_putchar(tx_module,0x6);   
    uart_putchar(tx_module,0x7);   
    uart_putchar(tx_module,0x8);   
    uart_putchar(tx_module,0x9);   
    uart_putchar(tx_module,0x9);   
    uart_putchar(tx_module,0x9);   

    /* Delay to allow time for interrupt */
    for( i=0; i < 0x8000; i++);
 
    /* Disable the receiver overrun interrupt in the receiver */
    UART_C3_REG(rx_module) &= ~UART_C3_ORIE_MASK;    

    /* Disable the receive FIFO */
    UART_PFIFO_REG(rx_module) &= ~UART_PFIFO_RXFE_MASK;
#warning "unknown assignation!!"
    /* Disable the IRQ */
    if (rx_module == UART1_BASE_PTR)
        disable_irq(35-16);  //disable interrupts for UART1 in NVIC
    else
        disable_irq(37-16);  //disable interrupts for UART2 in NVIC
    
         
    printf("\nNumber of receiver overrun interrupts detected during test = %d\n", num_or_errors);

    return (2 - num_or_errors);
}  
/********************************************************************/
/* UART receiver underflow error interrupt test.
 * Tests detection of receive FIFO underflow detection and interrupt generation. 
 * This test will run once using a UART without a FIFO as the receiver, and
 * then run again with a UART that includes a FIFO used as the receiver.
 * 
 *
 * The global num_rxuf_errors counter is used to keep track of the number
 * of receiver underrun interrupts. Every time there is a receiver underflow
 * interrupt detected the interrupt handler increments the counter. In total
 * this test should generate 2 different receiver underflow interrupts.
 * The number of errors returned by this test is 2 (the expected number of 
 * interrupts) - num_rxuf_errors (the actual number of interrupts detected while 
 * the test is running).
 */
int uart_rxuf_error_test()
{
    UART_MemMapPtr tx_module, rx_module;
    volatile char ch;
    uint32 i;    
    
    /* Make sure the number of receiver underflow interrupts is zero before starting */
    num_rxuf_errors = 0;
    
    printf("\n\nStarting receiver underflow interrupt test without a FIFO.\n");

    /* Determine which UART to use as the transmitter for testing. UART2 or UART3
     * could be used. We'll use UART2 as long as it isn't used as the TERM_PORT.
     */
    if (TERM_PORT != UART0_BASE_PTR)
    {      
       tx_module = UART0_BASE_PTR;   // set the module pointer
       
       printf("\nUsing UART0 on the daughter card as the transmitter.");
       printf("\nConnect one end of a blue wire to J4 pin 2 (UART0_TX)");
    }      
    else
    {      
       tx_module = UART1_BASE_PTR;   // set the module pointer

       printf("\nUsing UART1 on the daughter card as the transmitter.");
       printf("\nConnect one end of a blue wire to J43 pin 2 (UART1_TX)");
    }

    printf("\nPress any key when ready.\n");
    ch = in_char();
    
    /* Determine which UART to use as the receiver for testing. UART4 or UART5
     * could be used. We'll use UART4 as long as it isn't used as the TERM_PORT.
     */
    if (TERM_PORT != UART1_BASE_PTR)
    {      
       rx_module = UART1_BASE_PTR;   // set the module pointer
       
       /* Configure NVIC to enable UART status interrupts for the receiver */
       enable_irq(35-16);
       
       printf("\nUsing UART4 on the daughter card as the receiver.");
       printf("\nConnect other end of blue wire to J13 pin 2 (UART4_RX)");
    }      
    else
    {      
       rx_module = UART2_BASE_PTR;   // set the module pointer

       /* Configure NVIC to enable UART status interrupts for the receiver */
       enable_irq(37-16);

       printf("\nUsing UART2 on the daughter card as the receiver.");
       printf("\nConnect other end of blue wire J9 pin 14 (UART2_RX)");
    }

    printf("\nPress any key when ready.\n");
    ch = in_char();
    
    printf("Setup complete. Starting RXUF test...\n");
    printf("This test should generate 1 RXUF interrupt.\n");
    

    /* Enable the receiver underflow interrupt */
    UART_CFIFO_REG(rx_module) |= UART_CFIFO_RXUFE_MASK;    

    /* Transmit data to the receiver */
    uart_putchar(tx_module,0x1);  
    
    /* Wait for data to be received */
    while (UART_RCFIFO_REG(rx_module) < 1);//--> no FIFO in UART2
    
    /* Now read from the receiver too many times to cause an underflow */
    ch = UART_D_REG(rx_module);
    //ch = UART_D_REG(rx_module);

    /* Delay to allow time for interrupt */
    for( i=0; i < 0x4000; i++);

    /* Disable the receiver underflow interrupt in the receiver */
    UART_CFIFO_REG(rx_module) &= ~UART_CFIFO_RXUFE_MASK;    

    /* Disable the IRQ */
    if (rx_module == UART1_BASE_PTR)
        disable_irq(35-16);  //disable interrupts for UART1 in NVIC
    else
        disable_irq(37-16);  //disable interrupts for UART2 in NVIC
    
    
    printf("\nStarting receiver underflow interrupt test with a FIFO.\n");

    /* Determine which UART to use as the receiver for testing with a FIFO. 
     * UART0 or UART1 could be used. We'll use UART0 as long as it isn't used
     * as the TERM_PORT.
     */
    if (TERM_PORT != UART0_BASE_PTR)
    {      
       rx_module = UART0_BASE_PTR;   // set the module pointer
       tx_module = UART2_BASE_PTR;   // set the module pointer
       
       /* Configure NVIC to enable UART status interrupts for the receiver */
       enable_irq(33-16);
       
       printf("\nUsing UART0 on the daughter card as the receiver.");
       printf("\nRemove blue wire from previous receiver.");
       printf("\nConnect the blue wire to J2 pin 2 (UART0_RX)");
    }      
    else
    {      
       //rx_module = UART1_BASE_PTR;   // set the module pointer

       /* Configure NVIC to enable UART status interrupts for the receiver */
       //enable_irq(48);

       //printf("\nUsing UART0 on the daughter card as the receiver.");
       //printf("\nRemove blue wire from previous receiver.");
       //printf("\nConnect the blue wire to J7 pin 5 (UART1_RX)");
       printf("UART0 cannot be used as a TERMINAL PORT\n");
       return -1;
    }

    printf("\nPress any key when ready.\n");
    ch = in_char();
    
    printf("Setup complete. Starting RXUF test with FIFO enabled...\n");
    printf("This test should generate 1 RXUF interrupt.\n");
    

    /* Enable the receive FIFO */
    UART_PFIFO_REG(rx_module) |= UART_PFIFO_RXFE_MASK;

    /* Enable the receiver underflow interrupt */
    UART_CFIFO_REG(rx_module) |= UART_CFIFO_RXUFE_MASK;    
    
    /* Transmit data to the receiver */
    uart_putchar(tx_module,0x1);   
    uart_putchar(tx_module,0x2);   
    uart_putchar(tx_module,0x3);   
    uart_putchar(tx_module,0x4);   
    //uart_putchar(tx_module,0x5);   
    //uart_putchar(tx_module,0x6);   
    //uart_putchar(tx_module,0x7);   
    //uart_putchar(tx_module,0x8);   
 
    /* Now read from the receiver too many times to cause an underflow */
    ch = UART_D_REG(rx_module);
    //ch = UART_D_REG(rx_module);
    //ch = UART_D_REG(rx_module);
    //ch = UART_D_REG(rx_module);
    //ch = UART_D_REG(rx_module);
    //ch = UART_D_REG(rx_module);
    //ch = UART_D_REG(rx_module);
    //ch = UART_D_REG(rx_module);
    //ch = UART_D_REG(rx_module);

    /* Delay to allow time for interrupt */
    for( i=0; i < 0x100000; i++);
         
    /* Disable the receiver overrun interrupt in the receiver */
    UART_CFIFO_REG(rx_module) &= ~UART_CFIFO_RXUFE_MASK;    

    /* Disable the receive FIFO */
    UART_PFIFO_REG(rx_module) &= ~UART_PFIFO_RXFE_MASK;

    /* Disable the IRQ */
    if (rx_module == UART1_BASE_PTR)
        disable_irq(35-16);  //disable interrupts for UART4 in NVIC
    else
        disable_irq(37-16);  //disable interrupts for UART5 in NVIC
    
         
    printf("\nNumber of receiver underrun interrupts detected during test = %d\n", num_rxuf_errors);

    return (2 - num_rxuf_errors);
} 
/********************************************************************/
/* UART receiver noise error interrupt test.
 * Tests detection of receive noise detection and interrupt generation. 
 * Since this operation is the same on all UARTs only one UART will be 
 * tested. 
 *
 * NOTE: THIS TEST IS NOT COMPLETE!!!!!!!!!!!!!!!!!!!!!
 * I have not figured out a means of generating controllable "noise" to the
 * transmitter to trigger the noise flag. I tried configuring the transmitting
 * UART for a slightly different baud rate, and also configuring two UARTs
 * to send data and shorting them together, but neither of those worked.
 * 
 *
 * The global num_nf_errors counter is used to keep track of the number
 * of receiver noise flag interrupts. Every time there is a receiver noise error
 * interrupt detected the interrupt handler increments the counter. In total
 * this test should generate 1 receiver noise flag interrupt.
 * The number of errors returned by this test is 1 (the expected number of 
 * interrupts) - num_nf_errors (the actual number of interrupts detected while 
 * the test is running).
 */
int uart_nf_error_test()
{
    UART_MemMapPtr tx_module, rx_module;
    char ch;
    uint32 i;    
    
    /* Make sure the number of receiver noise flag interrupts is zero before starting */
    num_nf_errors = 0;
    
    printf("\n\nStarting receiver noise flag interrupt test.\n");

    /* Determine which UART to use as the transmitter for testing. UART2 or UART3
     * could be used. We'll use UART2 as long as it isn't used as the TERM_PORT.
     */
    if (TERM_PORT != UART0_BASE_PTR)
    {      
       tx_module = UART0_BASE_PTR;   // set the module pointer
       
       printf("\nUsing UART0 on the daughter card as the transmitter.");
       printf("\nConnect one end of a blue wire to J4 pin 2 (UART0_TX)");
    }      
    else
    {      
       tx_module = UART1_BASE_PTR;   // set the module pointer

       printf("\nUsing UART1 on the daughter card as the transmitter.");
       printf("\nConnect one end of a blue wire to J43 pin 2 (UART1_TX)");
    }

    printf("\nPress any key when ready.\n");
    ch = in_char();
    
    /* Determine which UART to use as the receiver for testing. UART4 or UART5
     * could be used. We'll use UART4 as long as it isn't used as the TERM_PORT.
     */
    if (TERM_PORT != UART1_BASE_PTR)
    {      
       rx_module = UART1_BASE_PTR;   // set the module pointer
       
       /* Configure NVIC to enable UART status interrupts for the receiver */
       enable_irq(47);
       
       printf("\nUsing UART1 on the daughter card as the receiver.");
       printf("\nConnect other end of blue wire to J13 pin 2 (UART4_RX)");
    }      
    else
    {      
       rx_module = UART2_BASE_PTR;   // set the module pointer

       /* Configure NVIC to enable UART status interrupts for the receiver */
       enable_irq(49);

       printf("\nUsing UART2 on the daughter card as the receiver.");
       printf("\nConnect other end of blue wire J9 pin 14 (UART5_RX)");
    }

    printf("\nPress any key when ready.\n");
    ch = in_char();
    
    printf("Setup complete. Starting NF test...\n");
    printf("This test should generate 1 NF interrupt.\n");
    

    /* Enable the noise flag interrupt */
    UART_C3_REG(rx_module) |= UART_C3_NEIE_MASK;  
    
    /* Configure the transmit UART for a different baud rate */
//    uart_init (tx_module, core_clk_khz, 19200);                  
    
    /* Transmit data to the receiver */
    uart_putchar(tx_module,0xA0);   
    uart_putchar(UART0_BASE_PTR,0x0A);   

    uart_putchar(tx_module,0x55);   
    uart_putchar(UART0_BASE_PTR,0xAA);   
    
    /* Delay to allow time for interrupt */
    for( i=0; i < 0x1000; i++);
    
    ch = uart_getchar(rx_module);
    printf("\nchar = %02X",ch);
    
    
    
    

    /* Disable the receiver noise flag interrupt in the receiver */
    UART_C3_REG(rx_module) &= ~UART_C3_NEIE_MASK;  
    
    /* Disable the IRQ */
    if (rx_module == UART1_BASE_PTR)
        disable_irq(48);  //disable interrupts for UART4 in NVIC
    else
        disable_irq(50);  //disable interrupts for UART5 in NVIC
    
    
    
         
    printf("\nNumber of receiver noise flag interrupts detected during test = %d\n", num_nf_errors);

    return (1 - num_nf_errors);
} 
/********************************************************************/
/* UART receiver framing error interrupt test.
 * Tests detection of receive framing errors and interrupt generation. 
 * Since this operation is the same on all UARTs only one UART will be 
 * tested. 
 *
 * The global num_framing_errors counter is used to keep track of the number
 * of receiver framing error interrupts. Every time there is a receiver framing
 * error interrupt detected the interrupt handler increments the counter. In total
 * this test should generate 1 receiver framing error interrupt.
 * The number of errors returned by this test is 1 (the expected number of 
 * interrupts) - num_framing_errors (the actual number of interrupts detected while 
 * the test is running).
 */
int uart_framing_error_test()
{
    UART_MemMapPtr tx_module, rx_module;
    volatile char ch;
    uint32 i;    
    
    /* Make sure the number of framing error interrupts is zero before starting */
    num_framing_errors = 0;
    
    printf("\n\nStarting framing error interrupt test.\n");

    /* Determine which UART to use as the transmitter for testing. UART2 or UART3
     * could be used. We'll use UART2 as long as it isn't used as the TERM_PORT.
     */
    if (TERM_PORT != UART0_BASE_PTR)
    {      
       tx_module = UART0_BASE_PTR;   // set the module pointer
       
       printf("\nUsing UART0 on the daughter card as the transmitter.");
       printf("\nConnect one end of a blue wire to J4 pin 2 (UART2_TX)");
    }      
    else
    {      
       tx_module = UART1_BASE_PTR;   // set the module pointer

       printf("\nUsing UART1 on the daughter card as the transmitter.");
       printf("\nConnect one end of a blue wire to J43 pin 2 (UART3_TX)");
    }

    printf("\nPress any key when ready.\n");
    ch = in_char();
    
    /* Determine which UART to use as the receiver for testing. UART4 or UART5
     * could be used. We'll use UART4 as long as it isn't used as the TERM_PORT.
     */
    if (TERM_PORT != UART1_BASE_PTR)
    {      
       rx_module = UART1_BASE_PTR;   // set the module pointer
       
       /* Configure NVIC to enable UART status interrupts for the receiver */
       enable_irq(35-16);
       
       printf("\nUsing UART4 on the daughter card as the receiver.");
       printf("\nConnect other end of blue wire to J13 pin 2 (UART4_RX)");
    }      
    else
    {      
       rx_module = UART2_BASE_PTR;   // set the module pointer

       /* Configure NVIC to enable UART status interrupts for the receiver */
       enable_irq(37-16);

       printf("\nUsing UART5 on the daughter card as the receiver.");
       printf("\nConnect other end of blue wire J9 pin 14 (UART5_RX)");
    }

    printf("\nPress any key when ready.\n");
    ch = in_char();
    
    printf("Setup complete. Starting framing error test...\n");
    printf("This test should generate 1 framing error interrupt.\n");
    

    /* Enable the framing error interrupt */
    UART_C3_REG(rx_module) |= UART_C3_FEIE_MASK;  
    
    /* Configure the transmit UART for 9-bit mode */
    UART_C1_REG(tx_module) |= UART_C1_M_MASK;  
        
    /* Make sure T8 is cleared so that we send 0 when the receiver expects 
     * a stop bit.
     */
    UART_C3_REG(rx_module) &= ~UART_C3_T8_MASK;  
        
    /* Transmit data to the receiver */
    uart_putchar(tx_module,0xA0);   
    
    /* Delay to allow time for interrupt */
    for( i=0; i < 0x1000; i++);        

    /* Disable the framing error interrupt */
    UART_C3_REG(rx_module) &= ~UART_C3_FEIE_MASK;  
    
    /* Disable the IRQ */
    if (rx_module == UART1_BASE_PTR)
        disable_irq(35-16);  //disable interrupts for UART4 in NVIC
    else
        disable_irq(37-16);  //disable interrupts for UART5 in NVIC
    
    /* Take the tx UART out of 9-bit mode */
    UART_C1_REG(tx_module) &= ~UART_C1_M_MASK;  
    
    
         
    printf("\nNumber of framing error interrupts detected during test = %d\n", num_framing_errors);

    return (1 - num_framing_errors);
} 
/********************************************************************/
/* UART receive idle interrupt test.
 * Tests detection of received idle char and interrupt generation. This
 * test will cover reception of all the supported IDLE character sizes.
 * 
 * I've tried to write this test so that I think it is catching an IDLE
 * character sent between two data characters. There isn't really a way
 * to know for sure that it is catching the IDLE I want, and not detecting
 * that the line has gone IDLE after the second character. There might be
 * some way to improve this test in the future to ensure it is really
 * testing what we want.
 * 
 *
 * The global num_idle_int counter is used to keep track of the number
 * of idle character received interrupts. Every time there is an idle rx
 * interrupt detected the interrupt handler increments the counter. In total
 * this test should generate 4 different idle rx interrupts.
 * The number of errors returned by this test is 4 (the expected number of 
 * interrupts) - num_idle_int (the actual number of interrupts detected while 
 * the test is running).
 */
int uart_idle_interrupt_test()
{
    UART_MemMapPtr tx_module, rx_module;
    volatile char ch;
    uint32 i;    
    
    /* Make sure the number of receive register full interrupts is zero before starting */
    num_idle_int = 0;
    
    printf("\n\nStarting receive idle character interrupt test.\n");

    /* Determine which UART to use as the transmitter for testing. UART2 or UART3
     * could be used. We'll use UART2 as long as it isn't used as the TERM_PORT.
     */
    if (TERM_PORT != UART0_BASE_PTR)
    {      
       tx_module = UART0_BASE_PTR;   // set the module pointer
       
       printf("\nUsing UART0 on the daughter card as the transmitter.");
       printf("\nConnect one end of a blue wire to J4 pin 2 (UART2_TX)");
    }      
    else
    {      
       tx_module = UART1_BASE_PTR;   // set the module pointer

       printf("\nUsing UART1 on the daughter card as the transmitter.");
       printf("\nConnect one end of a blue wire to J43 pin 2 (UART3_TX)");
    }

    printf("\nPress any key when ready.\n");
    ch = in_char();
    
    /* Determine which UART to use as the receiver for testing. UART4 or UART5
     * could be used. We'll use UART4 as long as it isn't used as the TERM_PORT.
     */
    if (TERM_PORT != UART1_BASE_PTR)
    {      
       rx_module = UART1_BASE_PTR;   // set the module pointer
       
       /* Configure NVIC to enable UART status interrupts for the receiver */
       enable_irq(34-16);
       
       printf("\nUsing UART1 on the daughter card as the receiver.");
       printf("\nConnect other end of blue wire to J13 pin 2 (UART4_RX)");
    }      
    else
    {      
       rx_module = UART2_BASE_PTR;   // set the module pointer

       /* Configure NVIC to enable UART status interrupts for the receiver */
       enable_irq(36-16);

       printf("\nUsing UART2 on the daughter card as the receiver.");
       printf("\nConnect other end of blue wire J9 pin 14 (UART5_RX)");
    }

    printf("\nPress any key when ready.\n");
    ch = in_char();
    
    printf("Setup complete. Starting IDLE interrupt test...\n");
    printf("This test should generate 4 IDLE interrupts.\n");
    

    /* M = 0, M10 = X, PE = X  gives you a 10-bit idle char
     * These are all default register settings, so no init is needed to 
     * setup for the first idle character.
     */
         
    UART_D_REG(tx_module) = 0x55;
        
    /* Enable the IDLE interrupt on the receiver */
    UART_C2_REG(rx_module) |= UART_C2_ILIE_MASK;

    for(i=0; i< 0x100; i++); // delay to allow transmission to start

    /* Clear C2[TE] bit to queue an idle character for transmission */
    UART_C2_REG(tx_module) &= ~UART_C2_TE_MASK; 
    
    /* Set C2[TE] to re-enable transmitter - sends an idle character */
    UART_C2_REG(tx_module) |= UART_C2_TE_MASK;
        
    UART_D_REG(tx_module) = 0x55;

    /* Delay to allow time for interrupt */
    for( i=0; i < 0x2000; i++);            

    /* Disable the IDLE interrupt on the receiver. We are trying to 
     * catch the idle between the two characters instead of the idle
     * after the second character.
     */
    UART_C2_REG(rx_module) &= ~UART_C2_ILIE_MASK;

    
    
    /* M = 1, M10 = 0, PE = X  gives you a 11-bit idle char */
    UART_C1_REG(tx_module) |= UART_C1_M_MASK;    
    UART_C1_REG(rx_module) |= UART_C1_M_MASK;    

    UART_D_REG(tx_module) = 0x55;
        
    /* Enable the IDLE interrupt on the receiver */
    UART_C2_REG(rx_module) |= UART_C2_ILIE_MASK;

    for(i=0; i< 0x100; i++); // delay to allow transmission to start

    /* Clear C2[TE] bit to queue an idle character for transmission */
    UART_C2_REG(tx_module) &= ~UART_C2_TE_MASK; 
    
    /* Set C2[TE] to re-enable transmitter - sends an idle character */
    UART_C2_REG(tx_module) |= UART_C2_TE_MASK;
        
    UART_D_REG(tx_module) = 0x55;

    /* Delay to allow time for interrupt */
    for( i=0; i < 0x2000; i++);            

    /* Disable the IDLE interrupt on the receiver. We are trying to 
     * catch the idle between the two characters instead of the idle
     * after the second character.
     */
    UART_C2_REG(rx_module) &= ~UART_C2_ILIE_MASK;


    
    /* M = 1, M10 = 1, PE = 0  gives you a 11-bit idle char */
    UART_C4_REG(tx_module) |= UART_C4_M10_MASK;     
    UART_C4_REG(rx_module) |= UART_C4_M10_MASK;     

    UART_D_REG(tx_module) = 0x55;
        
    /* Enable the IDLE interrupt on the receiver */
    UART_C2_REG(rx_module) |= UART_C2_ILIE_MASK;

    for(i=0; i< 0x100; i++); // delay to allow transmission to start

    /* Clear C2[TE] bit to queue an idle character for transmission */
    UART_C2_REG(tx_module) &= ~UART_C2_TE_MASK; 
    
    /* Set C2[TE] to re-enable transmitter - sends an idle character */
    UART_C2_REG(tx_module) |= UART_C2_TE_MASK;
        
    UART_D_REG(tx_module) = 0x55;

    /* Delay to allow time for interrupt */
    for( i=0; i < 0x2000; i++);            

    /* Disable the IDLE interrupt on the receiver. We are trying to 
     * catch the idle between the two characters instead of the idle
     * after the second character.
     */
    UART_C2_REG(rx_module) &= ~UART_C2_ILIE_MASK;

        
    /* M = 1, M10 = 1, PE = 1 gives you a 12-bit idle char */
    UART_C1_REG(tx_module) |= UART_C1_PE_MASK;     
    UART_C1_REG(rx_module) |= UART_C1_PE_MASK;     

    UART_D_REG(tx_module) = 0x55;
        
    /* Enable the IDLE interrupt on the receiver */
    UART_C2_REG(rx_module) |= UART_C2_ILIE_MASK;

    for(i=0; i< 0x100; i++); // delay to allow transmission to start

    /* Clear C2[TE] bit to queue an idle character for transmission */
    UART_C2_REG(tx_module) &= ~UART_C2_TE_MASK; 
    
    /* Set C2[TE] to re-enable transmitter - sends an idle character */
    UART_C2_REG(tx_module) |= UART_C2_TE_MASK;
        
    UART_D_REG(tx_module) = 0x55;

    /* Delay to allow time for interrupt */
    for( i=0; i < 0x2000; i++);            

    /* Disable the IDLE interrupt on the receiver. We are trying to 
     * catch the idle between the two characters instead of the idle
     * after the second character.
     */
    UART_C2_REG(rx_module) &= ~UART_C2_ILIE_MASK;
    

    
    
    /* All tests complete. Clear any bits that were set during the test */
    UART_C1_REG(tx_module) &= ~UART_C1_M_MASK;    // clear M bit
    UART_C4_REG(tx_module) &= ~UART_C4_M10_MASK;   // clear M10 bit    
    UART_C1_REG(tx_module) &= ~UART_C1_PE_MASK;  // clear PE bit 
    
    UART_C1_REG(rx_module) &= ~UART_C1_M_MASK;    // clear M bit
    UART_C4_REG(rx_module) &= ~UART_C4_M10_MASK;   // clear M10 bit    
    UART_C1_REG(rx_module) &= ~UART_C1_PE_MASK;  // clear PE bit 

    /* Disable the IRQ */
    if (rx_module == UART1_BASE_PTR)
        disable_irq(34-16);  //disable interrupts for UART4 in NVIC
    else
        disable_irq(36-16);  //disable interrupts for UART5 in NVIC


    printf("\nNumber of idle interrupts detected during test = %d\n", num_idle_int);

    return (4 - num_idle_int);
} 
/********************************************************************/
/* UART LIN break detect interrupt test.
 * Tests detection of the LIN break condition and interrupt generation. 
 * Since this operation is the same on all UARTs only one UART will be 
 * tested. 
 *
 * The global num_lin_breaks counter is used to keep track of the number
 * of LIN break detect interrupts. Every time there is a LIN break detect
 * interrupt detected the interrupt handler increments the counter. In total
 * this test should generate 2 LIN break interrupts.
 * The number of errors returned by this test is 2 (the expected number of 
 * interrupts) - num_lin_breaks (the actual number of interrupts detected while 
 * the test is running).
 */
int uart_lin_break_test()
{
    UART_MemMapPtr tx_module, rx_module;
    volatile char ch;
    uint32 i;    
    
    /* Make sure the number of LIN break interrupts is zero before starting */
    num_lin_breaks = 0;
    
    printf("\n\nStarting LIN break detect interrupt test.\n");

    /* Determine which UART to use as the transmitter for testing. UART2 or UART3
     * could be used. We'll use UART2 as long as it isn't used as the TERM_PORT.
     */
    if (TERM_PORT != UART0_BASE_PTR)
    {      
       tx_module = UART0_BASE_PTR;   // set the module pointer
       
       printf("\nUsing UART0 on the daughter card as the transmitter.");
       printf("\nConnect one end of a blue wire to J4 pin 2 (UART2_TX)");
    }      
    else
    {      
       tx_module = UART1_BASE_PTR;   // set the module pointer

       printf("\nUsing UART1 on the daughter card as the transmitter.");
       printf("\nConnect one end of a blue wire to J43 pin 2 (UART3_TX)");
    }

    printf("\nPress any key when ready.\n");
    ch = in_char();
    
    /* Determine which UART to use as the receiver for testing. UART4 or UART5
     * could be used. We'll use UART4 as long as it isn't used as the TERM_PORT.
     */
    if (TERM_PORT != UART1_BASE_PTR)
    {      
       rx_module = UART1_BASE_PTR;   // set the module pointer
       
       /* Configure NVIC to enable UART status interrupts for the receiver */
       enable_irq(34-16);
       
       printf("\nUsing UART4 on the daughter card as the receiver.");
       printf("\nConnect other end of blue wire to J13 pin 2 (UART4_RX)");
    }      
    else
    {      
       rx_module = UART2_BASE_PTR;   // set the module pointer

       /* Configure NVIC to enable UART status interrupts for the receiver */
       enable_irq(36-16);

       printf("\nUsing UART2 on the daughter card as the receiver.");
       printf("\nConnect other end of blue wire J9 pin 14 (UART5_RX)");
    }

    printf("\nPress any key when ready.\n");
    ch = in_char();
    
    printf("Setup complete. Starting LIN break detect test...\n");
    printf("This test should generate 2 LIN break detect interrupts.\n");
    

    /* Enable LIN break detection mode */
    UART_S2_REG(rx_module) |= UART_S2_LBKDE_MASK;
    
    /* Enable the LIN break detect interrupt */
    UART_BDH_REG(rx_module) |= UART_BDH_LBKDIE_MASK;
    

    /* Test sending a 10-bit break character. This should NOT generate an interrupt. */

    /* Set C2[SBK] bit to queue a break character for transmission */
    UART_C2_REG(tx_module) |= UART_C2_SBK_MASK; 
    
    /* Clear C2[SBK] so that only one break character is sent */
    UART_C2_REG(tx_module) &= ~UART_C2_SBK_MASK;
    
    /* Delay to allow time for interrupt */
    for( i=0; i < 0x1000; i++);    
    

    /* Test sending an 11-bit break character. This should generate an interrupt. */
    
    /* BRK13 = 0, M = 1, M10 = 0, PE = X  gives you a 11-bit break char */
    UART_C1_REG(tx_module) |= UART_C1_M_MASK;   

    /* Set C2[SBK] bit to queue a break character for transmission */
    UART_C2_REG(tx_module) |= UART_C2_SBK_MASK; 
    
    /* Clear C2[SBK] so that only one break character is sent */
    UART_C2_REG(tx_module) &= ~UART_C2_SBK_MASK;

    /* Delay to allow time for interrupt */
    for( i=0; i < 0x1000; i++);    
    
 
    
    /* Configure the receiver for a 12-bit LIN break */
    UART_C1_REG(rx_module) |= UART_C1_M_MASK;
    
    
    /* Test sending an 11-bit break character. This should generate NOT an interrupt. */
    
    /* BRK13 = 0, M = 1, M10 = 0, PE = X  gives you a 11-bit break char */
    UART_C1_REG(tx_module) |= UART_C1_M_MASK;   

    /* Set C2[SBK] bit to queue a break character for transmission */
    UART_C2_REG(tx_module) |= UART_C2_SBK_MASK; 
    
    /* Clear C2[SBK] so that only one break character is sent */
    UART_C2_REG(tx_module) &= ~UART_C2_SBK_MASK;

    /* Delay to allow time for interrupt */
    for( i=0; i < 0x1000; i++);    


    /* Test sending a 12-bit break character. This should generate an interrupt. */

    /* BRK13 = 0, M = 1, M10 = 1, PE = 1  gives you a 12-bit break char */
    UART_C4_REG(tx_module) |= UART_C4_M10_MASK;   
    UART_C1_REG(tx_module) |= UART_C1_PE_MASK;   

    /* Set C2[SBK] bit to queue a break character for transmission */
    UART_C2_REG(tx_module) |= UART_C2_SBK_MASK; 
    
    /* Clear C2[SBK] so that only one break character is sent */
    UART_C2_REG(tx_module) &= ~UART_C2_SBK_MASK;
    
    /* Delay to allow time for interrupt */
    for( i=0; i < 0x100000; i++);    
    
    
    /* All tests complete. Return to default UART settings */
    UART_C1_REG(rx_module) &= ~UART_C1_M_MASK;
    UART_S2_REG(rx_module) &= ~UART_S2_LBKDE_MASK;
    UART_BDH_REG(rx_module) &= ~UART_BDH_LBKDIE_MASK;
    
    UART_C1_REG(tx_module) &= ~UART_C1_M_MASK;   
    UART_C4_REG(tx_module) &= ~UART_C4_M10_MASK;   
    UART_C1_REG(tx_module) &= ~UART_C1_PE_MASK;       
    
         
    printf("\nNumber of LIN breaks detected during test = %d\n", num_lin_breaks);

    return (2 - num_lin_breaks);
} 
/********************************************************************/