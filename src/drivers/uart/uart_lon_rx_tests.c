/*
 * File:		uart_lon_rx_tests.c
 * Purpose:		LON mode basic receive tests.
 *
 */

#include "common.h"
#include "uart.h"
#include "uart_tests.h"

/* Globals for counting number of interrupts */
int num_lon_rx_int = 0;


/********************************************************************/
/* UART module LON mode basic receive test.
 *
 * Tests basic LON mode receive operation for a single UART. This test
 * must be run with a second processor board connected to supply the 
 * expected traffic. The uart_lon_basic_tx_test() function should be
 * running on the transmitting board that is connected to the receiver
 * on the board being tested. The LON data line also needs a 
 * pulldown resistor connected.
 *
 * The test will use both values for MSBF and then expect to receive
 * specific bytes. Anytime the expected byte is not received the code
 * will increment the error counter.
 */
int uart_lon_basic_rx_test(UART_MemMapPtr module)
{
    volatile char ch, data;
    int error = 0;  
            
    /* Disable the receiver on the UART that is being used for Rx to avoid
     * any issues with the beginning signal state while wires are being
     * connected.
     */
    UART_C2_REG(module) &= ~UART_C2_RE_MASK;

    printf("\nStarting UART LON mode basic receive test.\n");
    printf("\nPlease connect a second board to J2 pin 2 (PTD6/UART0_RX).\n");
    printf("Press any key when ready.\n");
    ch = in_char();
    
    /* Configure the UART for LON mode */
    UART_C6_REG(module) = UART_C6_EN709_MASK;
    
    UART_PRE_REG(module) = 0x4;
    
    UART_TPL_REG(module) = 0x01;    
    
    /* Re-enable the receiver for the test */
    UART_C2_REG(module) |= UART_C2_RE_MASK;
    
    
    /* Test receiving 0xAA - LSBF*/
    printf("Waiting for packet...\n");
    /* Wait for a receive packet to be detected */
    while( !(UART_S3_REG(module) & UART_S3_PRXF_MASK));
    
    printf("Packet received...\n");
                
    /* Get the received data */
    data = UART_D_REG(module);
    
    /* Test the received data against the expected value */
    if (data != 0xAA)
    {
      error++;      
      printf("\nERR! Incorrect data received. rx_data =  0x%02X\n", data);
    } 

    /* Test the receive packet length */
    if (UART_RPL_REG(module) != 0x1)
    {
      error++;      
      printf("ERR! Incorrect receive packet length. rx_pkt_len =  0x%02X\n", UART_RPL_REG(module));
    }        

    /* Clear the packet received flag */
    UART_S3_REG(module) = UART_S3_PRXF_MASK;
                
      
    /* Test receiving 0x55 - LSBF */        
    printf("Waiting for next packet...\n");
    /* Wait for a receive packet to be detected */
    while( !(UART_S3_REG(module) & UART_S3_PRXF_MASK));
    
    printf("Packet received...\n");
                
    /* Get the received data */
    data = UART_D_REG(module);
    
    /* Test the received data against the expected value */
    if (data != 0x55)
    {
      error++;      
      printf("\nERR! Incorrect data received. rx_data =  0x%02X\n", data);
    } 
    
    /* Test the receive packet length */
    if (UART_RPL_REG(module) != 0x1)
    {
      error++;      
      printf("ERR! Incorrect receive packet length. rx_pkt_len =  0x%02X\n", UART_RPL_REG(module));
    }        

    /* Clear the packet received flag */
    UART_S3_REG(module) = UART_S3_PRXF_MASK;
        
    
    /* Now test reception with MSBF first */    
    UART_S2_REG(module) |= UART_S2_MSBF_MASK;
    

    /* Test receiving 0xAA - LSBF*/
    printf("Waiting for packet...\n");
    /* Wait for a receive packet to be detected */
    while( !(UART_S3_REG(module) & UART_S3_PRXF_MASK));
    
    printf("Packet received...\n");

    /* Get the received data */
    data = UART_D_REG(module);
    
    /* Test the received data against the expected value */
    if (data != 0xAA)
    {
      error++;      
      printf("\nERR! Incorrect data received. rx_data =  0x%02X\n", data);
    } 

    /* Test the receive packet length */
    if (UART_RPL_REG(module) != 0x1)
    {
      error++;      
      printf("ERR! Incorrect receive packet length. rx_pkt_len =  0x%02X\n", UART_RPL_REG(module));
    }        
                
    /* Clear the packet received flag */
    UART_S3_REG(module) = UART_S3_PRXF_MASK;

    
    /* Test receiving 0x55 - LSBF */        
    printf("Waiting for next packet...\n");
    /* Wait for a receive packet to be detected */
    while( !(UART_S3_REG(module) & UART_S3_PRXF_MASK));
    
    printf("Packet received...\n");

    /* Get the received data */
    data = UART_D_REG(module);
    
    /* Test the received data against the expected value */
    if (data != 0x55)
    {
      error++;      
      printf("\nERR! Incorrect data received. rx_data =  0x%02X\n", data);
    } 

    /* Test the receive packet length */
    if (UART_RPL_REG(module) != 0x1)
    {
      error++;      
      printf("ERR! Incorrect receive packet length. rx_pkt_len =  0x%02X\n", UART_RPL_REG(module));
    }        
                
    /* Clear the packet received flag */
    UART_S3_REG(module) = UART_S3_PRXF_MASK;
    
    
    /* All tests complete. Return registers to default values */
    UART_S2_REG(module) &= ~UART_S2_MSBF_MASK;
    UART_C6_REG(module) = 0;   
    UART_PRE_REG(module) = 0;    
    UART_TPL_REG(module) = 0;    
        
    return 0;    
} 
/********************************************************************/
/* UART LON receive packet interrupt test.
 * Tests detection and interrupt generation for LON receive complete.
 *
 * This test must be run with a second processor board connected to 
 * supply the expected traffic. The uart_lon_basic_tx_test() function 
 * should be running on the transmitting board that is connected to the 
 * receiver on the board being tested. The LON data line also needs a 
 * pulldown resistor connected.
 *
 *
 * The global num_lon_rx_int counter is used to keep track of the number
 * of receive complete interrupts detected during this test. Every time there 
 * is a prxf interrupt detected by the interrupt handler the counter is 
 * incremented. In total this test should generate 4 different receive complete
 * interrupts. The number of errors returned by this test is 4 (the expected 
 * number of interrupts) - num_lon_rx_int (the actual number of LON receive
 * complete interrupts detected while the test is running).
 */
int uart_lon_rx_int_test(UART_MemMapPtr module)
{
    volatile char ch;
  
    /* Make sure the number of receive complete interrupts counter is zero before starting */
    num_lon_rx_int = 0;

    
    
    /***********MG******************/
    /* Configure PTA13 as a GPIO output driving high */
    PORTA_PCR13 = PORT_PCR_MUX(0x1); // GPIO is alt1 function
    GPIOA_PDDR |= 1<<13; // Configure as output
    GPIOA_PDOR |= 1<<13; // Drive pin high
    /*****************************/
    
    
    
    printf("\nStarting UART LON receive complete interrupt test.\n");
    printf("\nPlease connect a second board to J2 pin 2 (PTD6/UART0_RX).\n");
    printf("Press any key when ready.\n");
    ch = in_char();
    
    /* Enable the UART0 LON interrupt in the NVIC */
    enable_irq(15);
    
    /* Enable the LON Rx packet interrupt in the UART */
    UART_IE_REG(module) |= UART_IE_PRXIE_MASK;
    
    /* Configure the UART for LON mode */
    UART_C6_REG(module) = UART_C6_EN709_MASK;
        
    UART_PRE_REG(module) = 0x4;
    
    UART_TPL_REG(module) = 0x01;    
    
    UART_PCTL_REG(module) = 0x10;
    
    printf("This test should generate 4 receive complete interrupts.\n");
    printf("UART LON configuration complete. Send 4 packets now...\n\n");
    

    printf("Press any key when all 4 packets have been sent.\n");
    ch = in_char();
    
    /* All tests complete. Return registers to default values */
    UART_C6_REG(module) = 0;   
    UART_PRE_REG(module) = 0;    
    UART_TPL_REG(module) = 0;    
        
    /* Disable the IRQ */
    disable_irq(15);  
            
    printf("\nNumber of LON tx interrupts detected during test = %d\n", num_lon_rx_int);

    return (4 - num_lon_rx_int);        
}
/********************************************************************/   
/* UART module LON mode receive packet length test.
 *
 * Tests LON mode receive packet length for a single UART. The test 
 * will test a variety of transmit lengths including the minimum and
 * maximum allowable values. 
 *
 *
 * This test must be run with a second processor board connected to 
 * supply the expected traffic. The uart_lon_tx_len_test() function 
 * should be running on the transmitting board that is connected to the 
 * receiver on the board being tested. The LON data line also needs a 
 * pulldown resistor connected.
 *
 * The test will check the received packet length and all of the received
 * data values against the expected results. Any mismatches in the length
 * or data will increment the error counter that is returned.
 */
int uart_lon_rx_len_test(UART_MemMapPtr module)
{
    char ch;
    int error = 0;    
    uint32 rx_len, i;
    
    /* Disable the receiver on the UART that is being used for Rx to avoid
     * any issues with the beginning signal state while wires are being
     * connected.
     */
    UART_C2_REG(module) &= ~UART_C2_RE_MASK;    
            
    printf("\nStarting UART LON mode receive packet length test.\n");
    printf("\nPlease connect a second board to J2 pin 2 (PTD6/UART0_RX).\n");
    printf("Press any key when ready.\n");
    ch = in_char();
    
    /* Configure the UART for LON mode */
    UART_C6_REG(module) = UART_C6_EN709_MASK;
    
    /* Set the preamble length */
    UART_PRE_REG(module) = 0x7;
    
    /* In order to avoid possible overflows, enable the RxFIFO */
    UART_PFIFO_REG(module) |= UART_PFIFO_RXFE_MASK;
    
    /* Set the watermark to 1 */
    UART_RWFIFO_REG(module) = 1;
    
    /* Re-enable the receiver for the test */
    UART_C2_REG(module) |= UART_C2_RE_MASK;
    
   
    /* Start with a packet length of 1, then multiply packet length x2 */
    for(rx_len=1; rx_len <= 0x80; rx_len = rx_len * 2)
    {  
        printf("\nWaiting for %d byte packet...\n", rx_len);
        
        /* Check bytes as they are recieved */
        for(i=0; i < rx_len; i++)
        {          
          ch = uart_getchar(module);
          
          /* Check the data, expected value is the byte number */
          if (ch != i)
          {
            error++;      
            printf("\nERR! Incorrect data received. rx_data: 0x%02X expected: 0x%02X\n", ch,i);
          } 
        }
        
        /* Wait for a receive packet to be detected */
        while( !(UART_S3_REG(module) & UART_S3_PRXF_MASK));
 
        /* Wait a bit before testing RPL */
        for( i=0; i < 0x100; i++);
//        while(!(UART_RPL_REG(module)));
    
        /* All expected bytes have been received now test the recieve packet length */
        if (UART_RPL_REG(module) != rx_len)
        {
          error++;      
          printf("ERR! Incorrect receive packet length. RPL: 0x%02X expected: 0x%02X\n", UART_RPL_REG(module), rx_len);
        }  
        
        
         /* Clear the packet received flag */
        UART_S3_REG(module) = UART_S3_PRXF_MASK;
    }        
        
    
    /* Test max packet length (255) */
    rx_len = 0xFF;
    
    printf("\nWaiting for %d byte packet...\n", rx_len);
        
    /* Check bytes as they are recieved */
    for(i=0; i < rx_len; i++)
    {          
      ch = uart_getchar(module);
          
      /* Check the data, expected value is the byte number */
      if (ch != i)
      {
        error++;      
        printf("\nERR! Incorrect data received. rx_data: 0x%02X expected: 0x%02X\n", ch,i);
      } 
    }
        
    /* Wait for a receive packet to be detected */
    while( !(UART_S3_REG(module) & UART_S3_PRXF_MASK));
 
    /* Wait a bit before testing RPL */
//    for( i=0; i < 0x100; i++);        
    while(!(UART_RPL_REG(module)));
    
    /* All expected bytes have been received now test the recieve packet length */
    if (UART_RPL_REG(module) != rx_len)
    {
      error++;      
      printf("ERR! Incorrect receive packet length. RPL: 0x%02X expected: 0x%02X\n", UART_RPL_REG(module), rx_len);
    }  
                
    /* Clear the packet received flag */
    UART_S3_REG(module) = UART_S3_PRXF_MASK;

    
    /* All tests complete. Return registers to default values */
    UART_C6_REG(module) = 0;   
    UART_PRE_REG(module) = 0;    
    UART_TPL_REG(module) = 0;    
        
    return error;    
} 
/********************************************************************/
/* UART module LON preamble receive test.
 *
 * Tests LON mode receive preamble lengths for a single UART. The test 
 * will test a variety of preamble lengths including the minimum and
 * maximum allowable values. 
 *
 *
 * This test must be run with a second processor board connected to 
 * supply the expected traffic. The uart_lon_preanble_tx_test() function 
 * should be running on the transmitting board that is connected to the 
 * receiver on the board being tested. The LON data line also needs a 
 * pulldown resistor connected.
 *
 * The test will check the received preamble length and data values received. 
 * Any mismatches will increment the error counter that is returned.
 */
int uart_lon_preamble_rx_test(UART_MemMapPtr module)
{
    volatile char ch, data;
    int error = 0;
    uint32 pre_len;
            
    /* Disable the receiver on the UART that is being used for Rx to avoid
     * any issues with the beginning signal state while wires are being
     * connected.
     */
    UART_C2_REG(module) &= ~UART_C2_RE_MASK;    
            
    printf("\nStarting UART LON mode preamble receive test.\n");
    printf("\nPlease connect a second board to J2 pin 2 (PTD6/UART0_RX).\n");
    printf("Press any key when ready.\n");
    ch = in_char();
    
    /* Configure the UART for LON mode */
    UART_C6_REG(module) = UART_C6_EN709_MASK;   
    
    UART_TPL_REG(module) = 0x01;    
    
    /* Re-enable the receiver for the test */
    UART_C2_REG(module) |= UART_C2_RE_MASK;
    
    /* Test different preamble lengths starting with 4 multiplying x 2) */
    for( pre_len = 4; pre_len <= 128; pre_len = pre_len * 2)
    {
      printf("\nWaiting for %d bit preamble...\n",pre_len);
      
      /* Wait for a receive packet to be detected */
      while( !(UART_S3_REG(module) & UART_S3_PRXF_MASK));
      
      printf("Packet received...\n");
  
      /* Test the receive preamble length */
      if (UART_RPREL_REG(module) != pre_len)
      {
        error++;      
        printf("ERR! Incorrect receive preamble length. RPREL =  0x%02X\n", UART_RPREL_REG(module));
      }        
                  
      /* Get the received data */
      data = UART_D_REG(module);
      
      /* Test the received data against the expected value */
      if (data != 0xAA)
      {
        error++;      
        printf("\nERR! Incorrect data received. rx_data =  0x%02X\n", data);
      } 
  
      /* Clear the packet received flag */
      UART_S3_REG(module) = UART_S3_PRXF_MASK;
    }
    
    /* Test max preamble length (255) */
    pre_len = 0xFF;

    printf("\nWaiting for 255 bit preamble...\n");
      
    /* Wait for a receive packet to be detected */
    while( !(UART_S3_REG(module) & UART_S3_PRXF_MASK));
      
    printf("Packet received...\n");
  
    /* Test the receive preamble length */
    if (UART_RPREL_REG(module) != pre_len)
    {
      error++;      
      printf("ERR! Incorrect receive preamble length. RPREL =  0x%02X\n", UART_RPREL_REG(module));
    }        
                  
    /* Get the received data */
    data = UART_D_REG(module);
      
    /* Test the received data against the expected value */
    if (data != 0xAA)
    {
      error++;      
      printf("\nERR! Incorrect data received. rx_data =  0x%02X\n", data);
    } 
  
    /* Clear the packet received flag */
    UART_S3_REG(module) = UART_S3_PRXF_MASK;
      
    /* All tests complete. Return registers to default values */
    UART_C6_REG(module) = 0;   
    UART_PRE_REG(module) = 0;    
    UART_TPL_REG(module) = 0;     
    
    return error;
}
/********************************************************************/
/* UART module LON preamble error test.
 *
 * Tests LON mode preamble error test. The test will purposely be setup
 * to expect preamble lengths that don't match what is transmitted and
 * make sure that errors are generated. 
 *
 *
 * This test must be run with a second processor board connected to 
 * supply the expected traffic. The uart_lon_preanble_tx_test() function 
 * should be running on the transmitting board that is connected to the 
 * receiver on the board being tested. The LON data line also needs a 
 * pulldown resistor connected.
 *
 * The test will check the preamble error flag, received preamble length,
 * and data values received. Any mismatches will increment the error 
 * counter that is returned.
 */
int uart_lon_pre_err_test(UART_MemMapPtr module)
{
    volatile char ch, data;
    int error = 0;
    /* pre_len will be used to track the actual preamble length the
     * transmitter is expected to send.
     */
    uint32 pre_len; 
    /* pre_test will be used to store an incorrect preamble length (one
     * that doesn't match the actual preamble length the
     * transmitter is expected to send).
     */
    volatile uint32 pre_test;
            
    /* Disable the receiver on the UART that is being used for Rx to avoid
     * any issues with the beginning signal state while wires are being
     * connected.
     */
    UART_C2_REG(module) &= ~UART_C2_RE_MASK;    
            
    printf("\nStarting UART LON mode preamble error test.\n");
    printf("\nPlease connect a second board to J2 pin 2 (PTD6/UART0_RX).\n");
    printf("Press any key when ready.\n");
    ch = in_char();
    
    /* Configure the UART for LON mode */
    UART_C6_REG(module) = UART_C6_EN709_MASK;   
    
    UART_TPL_REG(module) = 0x01;    
    
    /* Re-enable the receiver for the test */
    UART_C2_REG(module) |= UART_C2_RE_MASK;
    

    /* Test pre_len = 0x4; test_len = 0x5 */   
    pre_len = 4;
    pre_test = 5;
    printf("\nWaiting for %d bit preamble...\n",pre_len);
      
    /* Wait for a receive packet to be detected */
    while( !(UART_S3_REG(module) & UART_S3_PRXF_MASK));
      
    printf("Packet received...\n");
    
    /* Make sure the preamble error flag set */
    if (!(UART_S3_REG(module) & UART_S3_PEF_MASK))
    {
      error++;      
      printf("ERR! PEF flag not detected set.\n");
    }        
    
    /* Clear the preamble error flag */
    UART_S3_REG(module) = UART_S3_PEF_MASK;
  
    /* Test the receive preamble length */
    if (UART_RPREL_REG(module) != pre_len)
    {
      error++;      
      printf("ERR! Incorrect receive preamble length. RPREL =  0x%02X\n", UART_RPREL_REG(module));
    }        
                  
    /* Get the received data */
    data = UART_D_REG(module);
      
    /* Test the received data against the expected value */
    if (data != 0xAA)
    {
      error++;      
      printf("\nERR! Incorrect data received. rx_data =  0x%02X\n", data);
    } 
  
    /* Clear the packet received flag */
    UART_S3_REG(module) = UART_S3_PRXF_MASK;

    /* Test pre_len = 0x8; test_len = 0x7 */   
    pre_len = 0x8;
    pre_test = 0x7;
    printf("\nWaiting for %d bit preamble...\n",pre_len);
      
    /* Wait for a receive packet to be detected */
    while( !(UART_S3_REG(module) & UART_S3_PRXF_MASK));
      
    printf("Packet received...\n");
    
    /* Make sure the preamble error flag set */
    if (!(UART_S3_REG(module) & UART_S3_PEF_MASK))
    {
      error++;      
      printf("ERR! PEF flag not detected set.\n");
    }        
    
    /* Clear the preamble error flag */
    UART_S3_REG(module) = UART_S3_PEF_MASK;
  
    /* Test the receive preamble length */
    if (UART_RPREL_REG(module) != pre_len)
    {
      error++;      
      printf("ERR! Incorrect receive preamble length. RPREL =  0x%02X\n", UART_RPREL_REG(module));
    }        
                  
    /* Get the received data */
    data = UART_D_REG(module);
      
    /* Test the received data against the expected value */
    if (data != 0xAA)
    {
      error++;      
      printf("\nERR! Incorrect data received. rx_data =  0x%02X\n", data);
    } 
  
    /* Clear the packet received flag */
    UART_S3_REG(module) = UART_S3_PRXF_MASK;
      
    /* Clear the false packet received flag */
    UART_S3_REG(module) = UART_S3_PRXF_MASK;
    


    /* Test pre_len = 0x10; test_len = 0x0F */   
    pre_len = 0x10;
    pre_test = 0x0F;
    printf("\nWaiting for %d bit preamble...\n",pre_len);
      
    /* Wait for a receive packet to be detected */
    while( !(UART_S3_REG(module) & UART_S3_PRXF_MASK));
      
    printf("Packet received...\n");
    
    /* Make sure the preamble error flag set */
    if (!(UART_S3_REG(module) & UART_S3_PEF_MASK))
    {
      error++;      
      printf("ERR! PEF flag not detected set.\n");
    }        
    
    /* Clear the preamble error flag */
    UART_S3_REG(module) = UART_S3_PEF_MASK;
  
    /* Test the receive preamble length */
    if (UART_RPREL_REG(module) != pre_len)
    {
      error++;      
      printf("ERR! Incorrect receive preamble length. RPREL =  0x%02X\n", UART_RPREL_REG(module));
    }        
                  
    /* Get the received data */
    data = UART_D_REG(module);
      
    /* Test the received data against the expected value */
    if (data != 0xAA)
    {
      error++;      
      printf("\nERR! Incorrect data received. rx_data =  0x%02X\n", data);
    } 
  
    /* Clear the packet received flag */
    UART_S3_REG(module) = UART_S3_PRXF_MASK;
      
    /* Clear the false packet received flag */
    UART_S3_REG(module) = UART_S3_PRXF_MASK;


    /* Test pre_len = 0x20; test_len = 0x21 */   
    pre_len = 0x20;
    pre_test = 0x21;
    printf("\nWaiting for %d bit preamble...\n",pre_len);
      
    /* Wait for a receive packet to be detected */
    while( !(UART_S3_REG(module) & UART_S3_PRXF_MASK));
      
    printf("Packet received...\n");
    
    /* Make sure the preamble error flag set */
    if (!(UART_S3_REG(module) & UART_S3_PEF_MASK))
    {
      error++;      
      printf("ERR! PEF flag not detected set.\n");
    }        
    
    /* Clear the preamble error flag */
    UART_S3_REG(module) = UART_S3_PEF_MASK;
  
    /* Test the receive preamble length */
    if (UART_RPREL_REG(module) != pre_len)
    {
      error++;      
      printf("ERR! Incorrect receive preamble length. RPREL =  0x%02X\n", UART_RPREL_REG(module));
    }        
                  
    /* Get the received data */
    data = UART_D_REG(module);
      
    /* Test the received data against the expected value */
    if (data != 0xAA)
    {
      error++;      
      printf("\nERR! Incorrect data received. rx_data =  0x%02X\n", data);
    } 
  
    /* Clear the packet received flag */
    UART_S3_REG(module) = UART_S3_PRXF_MASK;
      
    /* Clear the false packet received flag */
    UART_S3_REG(module) = UART_S3_PRXF_MASK;


    /* Test pre_len = 0x40; test_len = 0x3F */   
    pre_len = 0x40;
    pre_test = 0x3F;
    printf("\nWaiting for %d bit preamble...\n",pre_len);
      
    /* Wait for a receive packet to be detected */
    while( !(UART_S3_REG(module) & UART_S3_PRXF_MASK));
      
    printf("Packet received...\n");
    
    /* Make sure the preamble error flag set */
    if (!(UART_S3_REG(module) & UART_S3_PEF_MASK))
    {
      error++;      
      printf("ERR! PEF flag not detected set.\n");
    }        
    
    /* Clear the preamble error flag */
    UART_S3_REG(module) = UART_S3_PEF_MASK;
  
    /* Test the receive preamble length */
    if (UART_RPREL_REG(module) != pre_len)
    {
      error++;      
      printf("ERR! Incorrect receive preamble length. RPREL =  0x%02X\n", UART_RPREL_REG(module));
    }        
                  
    /* Get the received data */
    data = UART_D_REG(module);
      
    /* Test the received data against the expected value */
    if (data != 0xAA)
    {
      error++;      
      printf("\nERR! Incorrect data received. rx_data =  0x%02X\n", data);
    } 
  
    /* Clear the packet received flag */
    UART_S3_REG(module) = UART_S3_PRXF_MASK;
      
    /* Clear the false packet received flag */
    UART_S3_REG(module) = UART_S3_PRXF_MASK;
    

    /* Test pre_len = 0x80; test_len = 0x81 */   
    pre_len = 0x80;
    pre_test = 0x81;
    printf("\nWaiting for %d bit preamble...\n",pre_len);
      
    /* Wait for a receive packet to be detected */
    while( !(UART_S3_REG(module) & UART_S3_PRXF_MASK));
      
    printf("Packet received...\n");
    
    /* Make sure the preamble error flag set */
    if (!(UART_S3_REG(module) & UART_S3_PEF_MASK))
    {
      error++;      
      printf("ERR! PEF flag not detected set.\n");
    }        
    
    /* Clear the preamble error flag */
    UART_S3_REG(module) = UART_S3_PEF_MASK;
  
    /* Test the receive preamble length */
    if (UART_RPREL_REG(module) != pre_len)
    {
      error++;      
      printf("ERR! Incorrect receive preamble length. RPREL =  0x%02X\n", UART_RPREL_REG(module));
    }        
                  
    /* Get the received data */
    data = UART_D_REG(module);
      
    /* Test the received data against the expected value */
    if (data != 0xAA)
    {
      error++;      
      printf("\nERR! Incorrect data received. rx_data =  0x%02X\n", data);
    } 
  
    /* Clear the packet received flag */
    UART_S3_REG(module) = UART_S3_PRXF_MASK;
      
    
    /* Test pre_len = 0xFF; test_len = 0xFE */   
    pre_len = 0xFF;
    pre_test = 0xFE;
    printf("\nWaiting for %d bit preamble...\n",pre_len);
      
    /* Wait for a receive packet to be detected */
    while( !(UART_S3_REG(module) & UART_S3_PRXF_MASK));
      
    printf("Packet received...\n");
    
    /* Make sure the preamble error flag set */
    if (!(UART_S3_REG(module) & UART_S3_PEF_MASK))
    {
      error++;      
      printf("ERR! PEF flag not detected set.\n");
    }        
    
    /* Clear the preamble error flag */
    UART_S3_REG(module) = UART_S3_PEF_MASK;
  
    /* Test the receive preamble length */
    if (UART_RPREL_REG(module) != pre_len)
    {
      error++;      
      printf("ERR! Incorrect receive preamble length. RPREL =  0x%02X\n", UART_RPREL_REG(module));
    }        
                  
    /* Get the received data */
    data = UART_D_REG(module);
      
    /* Test the received data against the expected value */
    if (data != 0xAA)
    {
      error++;      
      printf("\nERR! Incorrect data received. rx_data =  0x%02X\n", data);
    } 
  
    /* Clear the packet received flag */
    UART_S3_REG(module) = UART_S3_PRXF_MASK;
          

    /* All tests complete. Return registers to default values */
    UART_C6_REG(module) = 0;   
    UART_PRE_REG(module) = 0;    
    UART_TPL_REG(module) = 0;     
    
    return error;
}
/********************************************************************/