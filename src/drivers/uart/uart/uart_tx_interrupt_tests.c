/*
 * File:		uart_tx_interrupt_tests.c
 * Purpose:		UART transmit interrupt tests. These tests cover
 *              all transmit type interrupts (both status and error).
 *
 */

#include "common.h"
#include "uart.h"
#include "uart_tests.h"

/* Globals for counting number of interrupts */
int num_tdre_int = 0;
int num_tc_int = 0;
int num_txof_errors = 0;


/********************************************************************/
/* UART transmit FIFO overflow interrupt tests.
 * Tests detection and interrupt generation for TX FIFO overflow errors.
 * Since this test is short and doesn't require connection of external
 * hardware, this test will be run on all of the UART modules including the
 * TERM_PORT (extra characters will be displayed on the terminal). So this
 * test will also serve to test the connection of each UART's error interrupt
 * request to the NVIC module. Once the regular tests are done, UART0 and UART1
 * are tested a second time with the TxFIFO enabled.
 * 
 * The global num_txof_errors counter is used to keep track of the number
 * of transmit overflow errors detected during this test. Every time there 
 * is a tx overflow error detected by the interrupt handler the counter is 
 * incremented. In total this test should generate 8 (Only 5 on P0) different transmit overflow
 * errors. The number of errors returned by this test is 8(Only 5 on P0) (the expected number
 * of errors) - num_txof_errors (the actual number of transmit overflow errors
 * detected while the test is running).
 */
int uart_txof_error_test()
{
    UART_MemMapPtr module;
    uint32 i;
    
    /* Make sure the number of transmit oveflow errors counter is zero before starting */
    num_txof_errors = 0;

    printf("\nStarting UART transmit overflow error test.\n");
    printf("This test should generate 4 transmit overflow errors.\n");
    
    /* Start test with UART0 */
    module = UART0_BASE_PTR;
    
    /* Enable the UART0 error interrupt in the NVIC */
    enable_irq(33-16);
 
    /* Run tx overflow test on all UARTs. */
     while( module )       
    {  
        /* Enable the tx overflow interrupts in the module */
        UART_CFIFO_REG(module) |= UART_CFIFO_TXOFE_MASK;
        
        /* Write bytes to the data register back-to-back to generate 
         * the transmitter overflow error.
         */
        UART_D_REG(module) = 0xAA;
        UART_D_REG(module) = 0xAA;
        
        /* Delay to allow time for interrupt */
        for( i=0; i < 0x100000; i++);
        
        /* Disable the tx overflow interrupts in the module */
        UART_CFIFO_REG(module) &= ~UART_CFIFO_TXOFE_MASK;
       
        /* Advance the mem map ptr to the next UART module to test */
        if (module == UART0_BASE_PTR)
        {    
            disable_irq(33-16);  //disable interrupts for UART0 in NVIC
            module = UART1_BASE_PTR;
            enable_irq(35-16);   //enable interrupts for UART1 in NVIC 
        }
        else if (module == UART1_BASE_PTR)
        {
            disable_irq(35-16);  //disable interrupts for UART1 in NVIC
            module = UART2_BASE_PTR;
            enable_irq(37-16);   //enable interrupts for UART2 in NVIC 
        }
        else if (module == UART2_BASE_PTR)
        {  
            disable_irq(37-16);  //disable interrupts for UART0 in NVIC
            /* All common UART registers have been checked, so drop out of the loop */                          
            module = 0;
        } 
    } 
        
    /* Now test UART0, but with the TxFIFO enabled */
    
    /* Re-test UART0 with the FIFO enabled */
    module = UART0_BASE_PTR;
    
    /* Enable the transmit FIFO */
    UART_PFIFO_REG(module) |= UART_PFIFO_TXFE_MASK;

    /* Enable the UART0 error interrupt in the NVIC */
    enable_irq(33-16);
 
    /* Enable the tx overflow interrupts in the module */
    UART_CFIFO_REG(module) |= UART_CFIFO_TXOFE_MASK;
        
    /* Write bytes to the data register back-to-back to generate 
     * the transmitter overflow error.
     */
    UART_D_REG(module) = 0xAA;
    UART_D_REG(module) = 0xAA;
    UART_D_REG(module) = 0xAA;
    UART_D_REG(module) = 0xAA;
    UART_D_REG(module) = 0xAA;
    UART_D_REG(module) = 0xAA;
    UART_D_REG(module) = 0xAA;
    UART_D_REG(module) = 0xAA;
    UART_D_REG(module) = 0xAA;
        
    /* Delay to allow time for interrupt */
    for( i=0; i < 0x1000; i++);
        
    /* Disable the tx overflow interrupts in the module */
    UART_CFIFO_REG(module) &= ~UART_CFIFO_TXOFE_MASK;

    /* Disable IRQ for UART 0 */
    disable_irq(33-16);  //disable interrupts for UART0 in NVIC

    /* Disable the transmit FIFO */
    UART_PFIFO_REG(module) &= ~UART_PFIFO_TXFE_MASK;

    
    printf("\nNumber of transmit overflow errors detected during test = %d\n", num_txof_errors);

    return (4 - num_txof_errors);
}      
/********************************************************************/
/* UART transmit complete interrupt test.
 * Tests detection and interrupt generation for transmit complete.
 * Since this test is short and doesn't require connection of external
 * hardware, this test will be run on all of the UART modules incl. the
 * TERM_PORT. So this test will also serve to test the connection of each UART's
 * status interrupt request to the NVIC module. Once the regular tests are done,
 * UART0 and UART1 are tested a second time with the TxFIFO enabled.
 * 
 * The global num_tc_int counter is used to keep track of the number
 * of transmit complete interrupts detected during this test. Every time there 
 * is a tc interrupt detected by the interrupt handler the counter is 
 * incremented. In total this test should generate 8(Only 5 on P0) different transmit complete
 * interrupts. The interrupt on the terminal port will interrupt itself, so it 
 * won't actually increment the counter, so the number of errors returned by this
 * test is 7(Only 4 on P0) (the expected number of interrupts) - num_tc_int (the actual number
 * of transmit complete interrupts detected while the test is running).
 */
int uart_tc_interrupt_test()
{
    UART_MemMapPtr module;
    uint32 i;
    
    /* Make sure the number of transmit complete interrupts counter is zero before starting */
    num_tc_int = 0;

    printf("\nStarting UART transmit complete interrupt test.\n");
    printf("This test should generate 3 transmit complete interrupts if UART0 is not used. 2 interrupts if UART0 is used.\n");
        
    
    /* Start test with UART0 */
    module = UART0_BASE_PTR;
    
    /* Enable the UART0 status interrupt in the NVIC */
    enable_irq(32-16);
 
    /* Run tx complete test on all UARTs. */
     while( module )       
    {  
        /* Write  a byte to the data register */
        UART_D_REG(module) = 0xAA;
        
        /* Enable the tx complete interrupt in the module */
        UART_C2_REG(module) |= UART_C2_TCIE_MASK;
        
        /* Delay to allow time for interrupt */
        for( i=0; i < 0x1000; i++);
        
       
        /* Advance the mem map ptr to the next UART module to test */
        if (module == UART0_BASE_PTR)
        {    
            disable_irq(32-16);  //disable interrupts for UART0 in NVIC
            module = UART1_BASE_PTR;
            enable_irq(34-16);   //enable interrupts for UART1 in NVIC 
        }
        else if (module == UART1_BASE_PTR)
        {
            disable_irq(34-16);  //disable interrupts for UART1 in NVIC
            module = UART2_BASE_PTR;
            enable_irq(36-16);   //enable interrupts for UART2 in NVIC 
        }
        else if (module == UART2_BASE_PTR)
        {  
            disable_irq(36-16);  //disable interrupts for UART2 in NVIC
            /* All common UART registers have been checked, so drop out of the loop */                          
            module = 0;
        }
    } 
        
    /* Now test UART0 again, but with the TxFIFO enabled */
    
    /* Re-test UART0 with the FIFO enabled */
    module = UART0_BASE_PTR;
    
    /* Enable the transmit FIFO */
    UART_PFIFO_REG(module) |= UART_PFIFO_TXFE_MASK;

    /* Enable the UART0 status interrupt in the NVIC */
    enable_irq(32-16);
         
    /* Write bytes to the data register */
    UART_D_REG(module) = 0xAA;
    UART_D_REG(module) = 0xAA;
    UART_D_REG(module) = 0xAA;
    UART_D_REG(module) = 0xAA;
    UART_D_REG(module) = 0xAA;
    UART_D_REG(module) = 0xAA;
    UART_D_REG(module) = 0xAA;
    UART_D_REG(module) = 0xAA;
    UART_D_REG(module) = 0xAA;
    
    /* Enable the tx complete interrupt in the module */
    UART_C2_REG(module) |= UART_C2_TCIE_MASK;    
        
    /* Delay to allow time for interrupt */
    for( i=0; i < 0x10000; i++);
         
    /* Disable IRQ for UART 0 */
    disable_irq(32-16);  //disable interrupts for UART0 in NVIC

    /* Disable the transmit FIFO */
    UART_PFIFO_REG(module) &= ~UART_PFIFO_TXFE_MASK;

    
    printf("\nNumber of transmit complete interrupts detected during test = %d\n", num_tc_int);
if(TERM_PORT==UART0_BASE_PTR)
    return (2 - num_tc_int);
else
    return (3 - num_tc_int);
}      
/********************************************************************/
/* UART transmit data register empty interrupt test.
 * Tests detection and interrupt generation for transmitter empty.
 * This test will run on just two UARTs--one with a FIFO and one without. 
 * Since the UART status interrupt connection has already been verified and
 * the operation of the TDRE flag is the same across most UARTs this should
 * provide adequate coverage.
 * 
 * The global num_tdre_int counter is used to keep track of the number
 * of transmitter empty interrupts detected during this test. Every time there 
 * is a tdre interrupt detected by the interrupt handler the counter is 
 * incremented. In total this test should generate 2 different transmit complete
 * interrupts. The number of errors returned by this test is 2 (the expected 
 * number of interrupts) - num_tdre_int (the actual number of transmitter 
 * empty interrupts detected while the test is running).
 */
int uart_tdre_interrupt_test()
{
    UART_MemMapPtr module;
    uint32 i;
    
    /* Make sure the number of transmitter empty interrupts counter is zero before starting */
    num_tdre_int = 0;

    printf("\nStarting UART transmitter empty interrupt test.\n");
    printf("This test should generate 2 transmitter empty interrupts.\n");
    
    
    /* Determine which UART  with a FIFO to use for testing. UART0 or UART1 could
     * be used. We'll use UART0 as long as it isn't used as the TERM_PORT.
     */
    if (TERM_PORT != UART0_BASE_PTR)
    {      
       module = UART0_BASE_PTR;   // set the module pointer

       /* Enable the UART0 status interrupt in the NVIC */
       enable_irq(32-16);
    }
    else      
    {      
       //module = UART1_BASE_PTR;   // set the module pointer

       /* Enable the UART1 status interrupt in the NVIC */
       //enable_irq(34-16);
      printf("UART0 cannot be the TERM port.\n");
      return -1;
    }
    
    /* Enable the transmit FIFO */
    UART_PFIFO_REG(module) |= UART_PFIFO_TXFE_MASK;

    /* Write bytes to the data register */
    UART_D_REG(module) = 0xAA;
    UART_D_REG(module) = 0xAA;
    UART_D_REG(module) = 0xAA;
    UART_D_REG(module) = 0xAA;
    UART_D_REG(module) = 0xAA;
    UART_D_REG(module) = 0xAA;
    UART_D_REG(module) = 0xAA;
    UART_D_REG(module) = 0xAA;
    UART_D_REG(module) = 0xAA;
    
    /* Enable the tx empty interrupt in the module */
    UART_C2_REG(module) |= UART_C2_TIE_MASK;    
        
    /* Delay to allow time for interrupt */
    for( i=0; i < 0x10000; i++);
         
    /* Disable the transmit FIFO */
    UART_PFIFO_REG(module) &= ~UART_PFIFO_TXFE_MASK;
  
    /* Disable the IRQ */
    if (module == UART0_BASE_PTR)
        disable_irq(32-16);  //disable interrupts for UART0 in NVIC
    else
        disable_irq(34-16);  //disable interrupts for UART1 in NVIC


    /* Determine which UART  w/o a FIFO to use for testing. UART1 or UART2 could
     * be used. We'll use UART2 as long as it isn't used as the TERM_PORT.
     */
    if (TERM_PORT != UART1_BASE_PTR)
    {      
       module = UART1_BASE_PTR;   // set the module pointer

       /* Enable the UART1 status interrupt in the NVIC */
       enable_irq(34-16);
    }
    else      
    {      
       module = UART2_BASE_PTR;   // set the module pointer

       /* Enable the UART2 status interrupt in the NVIC */
       enable_irq(36-16);
    }
    
    /* Write bytes to the data register */
    UART_D_REG(module) = 0xAA;
    
    /* Enable the tx empty interrupt in the module */
    UART_C2_REG(module) |= UART_C2_TIE_MASK;    
        
    /* Delay to allow time for interrupt */
    for( i=0; i < 0x1000; i++);
         
    /* Disable the IRQ */
    if (module == UART1_BASE_PTR)
        disable_irq(34-16);  //disable interrupts for UART1 in NVIC
    else
        disable_irq(36-16);  //disable interrupts for UART2 in NVIC
    
    
    printf("\nNumber of transmitter empty interrupts detected during test = %d\n", num_tdre_int);

    return (2 - num_tdre_int);
}      
/********************************************************************/
