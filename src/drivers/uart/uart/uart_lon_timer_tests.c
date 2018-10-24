/*
 * File:		uart_lon_timer_tests.c
 * Purpose:		LON mode timers tests. The tests
 *                      in this file cover the beta1,
 *                      wbase, and packet cycle timers
 *                      that are used in LON mode.
 *
 */

#include "common.h"
#include "uart.h"
#include "uart_tests.h"


/********************************************************************/
/* UART module beta1 timer test.
 *
 * Tests the LON mode beta1 timer. The timer is one of the determinates
 * for the gap between two packets being trasnmitted. This test will
 * go through several different values for the beta1 timer.
 * 
 * A scope should be used to verify that the gap between two packets
 * sent varies as expected based on the current beta1 timer value. The
 * space between packets should = 4 bit times (line code violation)  + 
 * beta1 value. Bit time = 8.8us at 115200.
 *
 * Since the test is not self checking it will always return 0 (pass).
 */
int uart_lon_beta1_test(UART_MemMapPtr module)
{
    volatile char ch;
    uint32 i, beta1;
            
    printf("\nStarting UART LON mode beta1 timer test.\n");
    printf("\nPlease connect a scope to J53 pin 1 (PTD7/UART0_TX).\n");
    printf("Set scope for single edge trigger (rising).\n");
    printf("Press any key when ready.\n");
    ch = in_char();
    
    /* Configure the UART for LON mode */
    UART_C6_REG(module) = UART_C6_EN709_MASK;   
    
    UART_TPL_REG(module) = 0x01;    
    
    UART_PRE_REG(module) = 0x4;
            
    /* Start with a beta1 length of 1, then multiply x2 */
    for(beta1=1; beta1 <= 0x80; beta1 = beta1 * 2)
    {  
        printf("Sending two 1 byte packets with beta 1 timer = %d\n", beta1);
        printf("Press any key when scope is ready.\n");
        ch = in_char();

        UART_B1T_REG
          
          (module) = beta1;
    
        /* Send first packet */    
        UART_C6_REG(module) |= UART_C6_TX709_MASK;
    
        uart_putchar (module, 0xAA); 
        
        /* Wait for transmit to complete */
        while( !(UART_S3_REG(module) & UART_S3_PTXF_MASK));

        /* Clear the PTXF flag */
        UART_S3_REG(module) = UART_S3_PTXF_MASK;
        
        /* Load a second character immediately, so that next packet is ready */
        uart_putchar (module, 0x55); 
        
        /* Wait for transmit to complete */
        while( !(UART_S3_REG(module) & UART_S3_PTXF_MASK));

        /* Disable transmit to prevent a second packet from attempting to go out */
        UART_C6_REG(module) &= ~UART_C6_TX709_MASK;

        /* Clear the PTXF flag */
        UART_S3_REG(module) = UART_S3_PTXF_MASK;
    }
    
    /* Run test one last time with max beta1 value */
    beta1 = 0xFF;
           
    printf("Sending two 1 byte packets with beta 1 timer = %d\n", beta1);
    printf("Press any key when scope is ready.\n");
    ch = in_char();

    UART_B1T_REG(module) = beta1;

    /* Send first packet */    
    UART_C6_REG(module) |= UART_C6_TX709_MASK;
    
    uart_putchar (module, 0xAA); 
        
    /* Wait for transmit to complete */
    while( !(UART_S3_REG(module) & UART_S3_PTXF_MASK));

    /* Clear the PTXF flag */
    UART_S3_REG(module) = UART_S3_PTXF_MASK;

    /* Load a second character immediately, so that next packet is ready */
    uart_putchar (module, 0x55); 
        
    /* Wait for transmit to complete */
    while( !(UART_S3_REG(module) & UART_S3_PTXF_MASK));

    /* Disable transmit to prevent a second packet from attempting to go out */
    UART_C6_REG(module) &= ~UART_C6_TX709_MASK;

    /* Clear the PTXF flag */
    UART_S3_REG(module) = UART_S3_PTXF_MASK;
    
    
    /* Delay before post-test clean-up */
    for( i=0; i < 0x10000; i++);

    /* All tests complete. Return registers to default values */
    UART_C6_REG(module) = 0;   
    UART_PRE_REG(module) = 0;    
    UART_TPL_REG(module) = 0;          

    return 0;
}
/********************************************************************/
/* UART module secondary delay timer test.
 *
 * Tests the LON mode secondary delay timer. The timer is one of 
 * the determinates for the gap between two packets being trasnmitted. 
 * This test will go through several different values for the 
 * secondary delay timer.
 * 
 * A scope should be used to verify that the gap between two packets
 * sent varies as expected based on the current SDTH and SDTL value. The
 * space between packets should = 4 bit times (line code violation)  + 
 * 1 bit time (beta1) + SDTH-L value. Bit time = 8.8us at 115200.
 * 
 * NOTE: The larger values might be very difficult to capture on a scope.
 *
 *
 * Since the test is not self checking it will always return 0 (pass).
 */
int uart_lon_sdt_test(UART_MemMapPtr module)
{
    volatile char ch;
    uint32 i, sdt;
            
    printf("\nStarting UART LON mode SDT timer test.\n");
    printf("\nPlease connect a scope to J53 pin 1 (PTD7/UART0_TX).\n");
    printf("Set scope for single edge trigger (rising).\n");
    printf("Press any key when ready.\n");
    ch = in_char();
    
    /* Configure the UART for LON mode */
    UART_C6_REG(module) = UART_C6_EN709_MASK;   
    
    UART_TPL_REG(module) = 0x01;    
    
    UART_PRE_REG(module) = 0x4;
                
    /* Set beta1 timer to 1 as 0 seems to max out delay */
    UART_B1T_REG(module) = 1;

    /* Start with a SDT count of 1, then multiply x2 */
    for(sdt=1; sdt <= 0x8000; sdt = sdt * 2)
    {  
        printf("Sending two 1 byte packets with sdt timer = %d\n", sdt);
        printf("Press any key when scope is ready.\n");
        ch = in_char();

        UART_SDTH_REG(module) = (sdt & 0xFF00) >> 8;
        UART_SDTL_REG(module) = (sdt & 0x00FF);
    
        /* Send first packet */    
        UART_C6_REG(module) |= UART_C6_TX709_MASK;
    
        uart_putchar (module, 0xAA);
        
//MG   /* Wait for transmit to complete */
        while( !(UART_S3_REG(module) & UART_S3_PTXF_MASK));

//MG   /* Clear the PTXF flag */
        UART_S3_REG(module) = UART_S3_PTXF_MASK;
        
        /* Load a second character immediately, so that next packet is ready */
        uart_putchar (module, 0x55);
        
//MG   /* Wait for transmit to complete */
        while( !(UART_S3_REG(module) & UART_S3_PTXF_MASK));

//MG   /* Disable transmit to prevent a second packet from attempting to go out */
        UART_C6_REG(module) &= ~UART_C6_TX709_MASK;

//MG   /* Clear the PTXF flag */
        UART_S3_REG(module) = UART_S3_PTXF_MASK;
    }
    
    /* Run test one last time with max SDT value */
    sdt = 0xFFFF;
           
    printf("Sending two 1 byte packets with sdt timer = %d\n", sdt);
    printf("Press any key when scope is ready.\n");
    ch = in_char();

    UART_SDTH_REG(module) = (sdt & 0xFF00) >> 8;
    UART_SDTL_REG(module) = (sdt & 0x00FF);

    /* Send first packet */    
    UART_C6_REG(module) |= UART_C6_TX709_MASK;
    
    uart_putchar (module, 0xAA); 
        
    /* Load a second character immediately, so that next packet is ready */
    uart_putchar (module, 0x55); 
    
//MG   /* Wait for transmit to complete */
        while( !(UART_S3_REG(module) & UART_S3_PTXF_MASK));

//MG   /* Disable transmit to prevent a second packet from attempting to go out */
        UART_C6_REG(module) &= ~UART_C6_TX709_MASK;

//MG   /* Clear the PTXF flag */
        UART_S3_REG(module) = UART_S3_PTXF_MASK;
    
    /* Delay to allow transmit to complete before post-test clean-up */
    for( i=0; i < 0x1000000; i++);

    /* All tests complete. Return registers to default values */
    UART_C6_REG(module) = 0;   
    UART_PRE_REG(module) = 0;    
    UART_TPL_REG(module) = 0;          

    return 0;
}
/********************************************************************/
/* UART module packet cycle timer test.
 *
 * Tests the LON mode packet cycle timer. The packet cycle timer
 * determines the maximum period of timer after the line code
 * violation during which the bus can remain idle without decrementing
 * the back log count. In order to test the timer period this test
 * will use a GPIO that is toggled low when sending out a packet and
 * then is toggled high in the LON interrupt handler when a PCT timeout
 * is detected.
 * 
 * A scope should be used to verify that the gap between the packet end
 * and the GPIO toggle high matches the expected based on the current 
 * PCTH and PCTL value. The space between the packet end should = 4 bit 
 * times (line code violation)  + PCTH-L value + delay associated with 
 * the interrupt handler and GPIO toggle. Bit time = 8.8us at 115200.
 * 
 * NOTE: The larger values might be very difficult to capture on a scope.
 * 
 * NOTE: All printfs should be commented out of the LON isr handler when
 * running this test. Otherwise the delay for the ISR code will dominate
 * the GPIO toggle timing, and you won't be able to see the impact of the 
 * different PCT counter values.
 *
 *
 * Since the test is not self checking it will always return 0 (pass).
 */
int uart_lon_pct_test(UART_MemMapPtr module)
{
    volatile char ch;
    uint32 i, pct;
            
    printf("\nStarting UART LON mode PCT timer test.\n");
    printf("\nPlease connect a scope CH1 to J53 pin 1 (PTD7/UART0_TX).\n");
    printf("\nPlease connect a scope CH2 to J2 pin 14 (PTD2/GPIO).\n");
    printf("Set scope for single edge trigger on CH1.\n");
    printf("Press any key when ready.\n");
    ch = in_char();
    

    /* Configure PTD2 as a GPIO output driving low */
    PORTD_PCR1 = PORT_PCR_MUX(0x1); // GPIO is alt1 function
    
    GPIOD_PDDR |= 2; // Configure as output
    
    GPIOD_PDOR &= ~2; // Drive pin low


    /* Configure the UART for LON mode */
    UART_C6_REG(module) = UART_C6_EN709_MASK;   
    
    UART_TPL_REG(module) = 0x01;    
    
    UART_PRE_REG(module) = 0x4;
    
    /* Enable the UART0 LON interrupt in the NVIC */
    enable_irq(15);

    /* Enable the PCT expired interrupt */
    UART_IE_REG(module) |= UART_IE_PCTEIE_MASK;
    
                
    /* Set beta1 timer to 1 as 0 seems to max out delay */
    UART_B1T_REG(module) = 1;

    /* Start with a PCT count of 1, then multiply x2 */
    for(pct=1; pct <= 0x8000; pct = pct * 2)
    {  
        printf("Sending packet with pct timer = %d\n", pct);
        printf("Press any key when scope is ready.\n");
        ch = in_char();

        /* Drive GPIO low before starting test */
        GPIOD_PDOR &= ~2; // Drive pin low

        UART_PCTH_REG(module) = (pct & 0xFF00) >> 8;
        UART_PCTL_REG(module) = (pct & 0x00FF);
    
        /* Enable transmitter */    
        UART_C6_REG(module) |= UART_C6_TX709_MASK;
    
        /* Send data */
        uart_putchar (module, 0xAA); 
        
        /* Wait for transmit to complete */
        while( !(UART_S3_REG(module) & UART_S3_PTXF_MASK));

        /* Disable transmit to prevent a second packet from attempting to go out */
        UART_C6_REG(module) &= ~UART_C6_TX709_MASK;
    
        /* Clear the PTXF flag */
        UART_S3_REG(module) = UART_S3_PTXF_MASK;
    }
    
    /* Run test one last time with max PCT value */
    printf("Sending packet with pct timer = 0xFFFF\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();
    
    /* Drive GPIO low before starting test */
    GPIOD_PDOR &= ~2; // Drive pin low

    pct = 0xFFFF;
    
    UART_PCTH_REG(module) = (pct & 0xFF00) >> 8;
    UART_PCTL_REG(module) = (pct & 0x00FF);
    
    /* Enable transmitter */    
    UART_C6_REG(module) |= UART_C6_TX709_MASK;
    
    /* Send data */
    uart_putchar (module, 0xAA); 
        
    /* Wait for transmit to complete */
    while( !(UART_S3_REG(module) & UART_S3_PTXF_MASK));

    /* Disable transmit to prevent a second packet from attempting to go out */
    UART_C6_REG(module) &= ~UART_C6_TX709_MASK;
    
    /* Clear the PTXF flag */
    UART_S3_REG(module) = UART_S3_PTXF_MASK;
    
    
    /* Delay to allow test to complete before post-test clean-up */
    for( i=0; i < 0x10000; i++);

    /* All tests complete. Return registers to default values */
    UART_C6_REG(module) = 0;   
    UART_PRE_REG(module) = 0;    
    UART_TPL_REG(module) = 0;          

    return 0;
}
/********************************************************************/
/* UART module Wbase timer test.
 *
 * Tests the LON mode Wbase timer. The Wbase timer determines the
 * randomizing window after the beta1 delay. If Wbase expires without
 * channel activity, then software should decrement the back log count. 
 * In order to test the timer period this test will use a GPIO that is
 * toggled low when sending out a packet and then is toggled high in 
 * the LON interrupt handler when a Wbase timeout is detected.
 * 
 * A scope should be used to verify that the gap between the packet end
 * and the GPIO toggle high matches the expected based on the current 
 * Wbase value. The space between the packet end should = 4 bit 
 * times (line code violation)  + 1 bit time (beta1) + Wbase value + 
 * delay associated with the interrupt handler and GPIO toggle. 
 * Bit time = 8.8us at 115200.
 * 
 * 
 * NOTE: All printfs should be commented out of the LON isr handler when
 * running this test. Otherwise the delay for the ISR code will dominate
 * the GPIO toggle timing, and you won't be able to see the impact of the 
 * different Wbase counter values.
 *
 *
 * Since the test is not self checking it will always return 0 (pass).
 */
int uart_lon_wbase_test(UART_MemMapPtr module)
{
    volatile char ch;
    uint32 i, wbase;
            
    printf("\nStarting UART LON mode Wbase timer test.\n");
    printf("\nPlease connect a scope CH1 to J53 pin 1 (PTD7/UART0_TX).\n");
    printf("\nPlease connect a scope CH2 to J2 pin 11 (PTD3/GPIO).\n");
    printf("Set scope for single edge trigger on CH1.\n");
    printf("Press any key when ready.\n");
    ch = in_char();
    

    /* Configure PTA13 as a GPIO output driving low */
    PORTA_PCR13 = PORT_PCR_MUX(0x1); // GPIO is alt1 function
    
    GPIOA_PDDR |= 1<<13; // Configure as output
    
    GPIOA_PDOR &= ~1<<13; // Drive pin low


    /* Configure the UART for LON mode */
    UART_C6_REG(module) = UART_C6_EN709_MASK;   
    
    UART_TPL_REG(module) = 0x01;    
    
    UART_PRE_REG(module) = 0x4;
        
    /* Enable the UART0 LON interrupt in the NVIC */
    enable_irq(15);

    /* Enable the Wbase expired interrupt */
    UART_IE_REG(module) |= UART_IE_WBEIE_MASK;
                   
    /* Set beta1 timer to 1 as 0 seems to max out delay */
    UART_B1T_REG(module) = 1;
    
    /* SDT must be set to something in order for the Wbase timer to work */
    UART_SDTL_REG(module) = 1;
    

    /* Start with a Wbase count of 1, then multiply x2 */
    for(wbase=1; wbase <= 0x80; wbase = wbase * 2)
    {  
        printf("Sending packet with wbase timer = %d\n", wbase);
        printf("Press any key when scope is ready.\n");
        ch = in_char();

        /* Drive GPIO low before starting test */
        GPIOA_PDOR &= ~(1<<13); // Drive pin low
        
        UART_WB_REG(module) = wbase;

        /* Enable transmitter */    
        UART_C6_REG(module) |= UART_C6_TX709_MASK;
    
        /* Send data */
        uart_putchar (module, 0xAA); 
        
        /* Wait for transmit to complete */
        while( !(UART_S3_REG(module) & UART_S3_PTXF_MASK));

        /* Disable transmit to prevent a second packet from attempting to go out */
        UART_C6_REG(module) &= ~UART_C6_TX709_MASK;
    
        /* Clear the PTXF flag */
        UART_S3_REG(module) = UART_S3_PTXF_MASK;
    }
    
    /* Run test one last time with max Wbase value */
    printf("Sending packet with wbase timer = 0xFF\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();
    
    /* Drive GPIO low before starting test */
    GPIOA_PDOR &= ~(1<<13); // Drive pin low

    wbase = 0xFF;
    
    UART_WB_REG(module) = wbase;
    
    /* Enable transmitter */    
    UART_C6_REG(module) |= UART_C6_TX709_MASK;
    
    /* Send data */
    uart_putchar (module, 0xAA); 
        
    /* Wait for transmit to complete */
    while( !(UART_S3_REG(module) & UART_S3_PTXF_MASK));

    /* Disable transmit to prevent a second packet from attempting to go out */
    UART_C6_REG(module) &= ~UART_C6_TX709_MASK;
    
    /* Clear the PTXF flag */
    UART_S3_REG(module) = UART_S3_PTXF_MASK;
    
    
    /* Delay to allow test to complete before post-test clean-up */
    for( i=0; i < 0x1000000; i++);

    /* All tests complete. Return registers to default values */
    UART_C6_REG(module) = 0;   
    UART_PRE_REG(module) = 0;    
    UART_TPL_REG(module) = 0;          

    return 0;
}
/********************************************************************/
/* UART module LON mode receive indeterminate timer test.
 *
 * Tests LON mode receive indeterminate timer operation for a single 
 * UART. This test must be run with a second processor board connected
 * to supply the expected traffic. The uart_lon_beta1_test() function should be
 * running on the transmitting board that is connected to the receiver
 * on the board being tested. The LON data line also needs a 
 * pulldown resistor connected.
 *
 * The LON beta1 test used to generate traffic sends two packets back to
 * back with different delays between the two packets. This test will use
 * a single value for the RIDT, then based on how the RIDT value compares 
 * to the exptected beta1 delay the test checks to see if one or both 
 * of the packets were received. Anytime the expected bytes are not 
 * received or an unexpected byte is receive the code will increment 
 * the error counter.
 */
int uart_lon_ridt_test(UART_MemMapPtr module)
{
    volatile char ch, data;
    int error = 0;    
    uint32 beta1, i;
    uint8 ridt = 0x0;    
            
    /* Disable the receiver on the UART that is being used for Rx to avoid
     * any issues with the beginning signal state while wires are being
     * connected.
     */
    UART_C2_REG(module) &= ~UART_C2_RE_MASK;

    printf("\nStarting UART LON mode RIDT test.\n");
    printf("\nPlease connect a second board to J2 pin 2 (PTD6/UART0_RX).\n");
    printf("Press any key when ready.\n");
    ch = in_char();
    
    /* Configure the UART for LON mode */
    UART_C6_REG(module) = UART_C6_EN709_MASK;
    
    UART_PRE_REG(module) = 0x4;
    
    UART_TPL_REG(module) = 0x01;    
    
    /* Re-enable the receiver for the test */
    UART_C2_REG(module) |= UART_C2_RE_MASK;
    
    /* In order to avoid possible overflows, enable the RxFIFO */
    UART_PFIFO_REG(module) |= UART_PFIFO_RXFE_MASK;

    /* Set the RIDT value to test */
    UART_RIDT_REG(module) = ridt;
    
    
    
     /* Test different beta1 lengths starting with 1 multiplying x 2) */
    for( beta1 = 1; beta1 <= 128; beta1 = beta1 * 2)
    {
      printf("\nWaiting for packets with %d bit beta1...\n",beta1);     
        
      /* Wait for a receive packet to be detected */
      while( !(UART_S3_REG(module) & UART_S3_PRXF_MASK));
      
      printf("Packet received...\n");
      
      /* Use a delay loop to wait some more. Because the code could
       * be receiving two packets we want to make sure both have had
       * a chance to arrive before testing the packets.
       */
      for( i=0; i< 0x1000000; i++);
      
      /* Check to see how many bytes were received. If beta1 < ridt, then
       * only one byte should have been accepted, the second byte should be 
       * ignored.
       */
      if ( ridt < beta1 ) // should receive 2 bytes in this case
      {    
          /* Test the number of bytes in the receive FIFO */
          if (UART_RCFIFO_REG(module) != 2)
          {
              error++;      
              printf("ERR! Incorrect number of bytes received. Expected 2. Received %d\n", UART_RCFIFO_REG(module));
          }  
          
          /* Get the first byte of data */
          data = UART_D_REG(module);
      
          /* Test the received data against the expected value */
          if (data != 0xAA)
          {
            error++;      
            printf("\nERR! Incorrect data received. Expected 0xAA. Received 0x%02X\n", data);
          } 
          
          /* Get the second byte of data */
          data = UART_D_REG(module);
      
          /* Test the received data against the expected value */
          if (data != 0x55)
          {
            error++;      
            printf("\nERR! Incorrect data received. Expected 0x55. Received 0x%02X\n", data);
          }  
      }
      else //should only have received 1 byte in this case
      {    
          /* Test the number of bytes in the receive FIFO */
          if (UART_RCFIFO_REG(module) != 1)
          {
              error++;      
              printf("ERR! Incorrect number of bytes received. Expected 1. Received %d\n", UART_RCFIFO_REG(module));
          }  
          
          /* Get the first byte of data */
          data = UART_D_REG(module);
      
          /* Test the received data against the expected value */
          if (data != 0xAA)
          {
            error++;      
            printf("\nERR! Incorrect data received. Expected 0xAA. Received 0x%02X\n", data);
          } 
      }
        
      /* Clear the packet received flag */
      UART_S3_REG(module) = UART_S3_PRXF_MASK;
    }
    
    /* Test max beta1 length (255) */
    beta1 = 0xFF;
    printf("\nWaiting for packets with %d bit beta1...\n",beta1);     
        
    /* Wait for a receive packet to be detected */     
    while( !(UART_S3_REG(module) & UART_S3_PRXF_MASK));
      
    printf("Packet received...\n");
      
    /* Use a delay loop to wait some more. Because the code could
     * be receiving two packets we want to make sure both have had
     * a chance to arrive before testing the packets.
     */
    for( i=0; i< 0x1000000; i++);
      
    /* Check to see how many bytes were received. If beta1 < ridt, then
     * only one byte should have been accepted, the second byte should be 
     * ignored.
     */
    if ( ridt < beta1 ) // should receive 2 bytes in this case
    {    
      /* Test the number of bytes in the receive FIFO */
      if (UART_RCFIFO_REG(module) != 2)
      {
        error++;      
        printf("ERR! Incorrect number of bytes received. Expected 2. Received %d\n", UART_RCFIFO_REG(module));
      }  
          
      /* Get the first byte of data */
      data = UART_D_REG(module);
      
      /* Test the received data against the expected value */
      if (data != 0xAA)
      {
        error++;      
        printf("\nERR! Incorrect data received. Expected 0xAA. Received 0x%02X\n", data);
      } 
          
      /* Get the second byte of data */
      data = UART_D_REG(module);
      
      /* Test the received data against the expected value */
      if (data != 0x55)
      {
        error++;      
        printf("\nERR! Incorrect data received. Expected 0x55. Received 0x%02X\n", data);
      }  
    }
    else //should only have received 1 byte in this case
    {    
      /* Test the number of bytes in the receive FIFO */
      if (UART_RCFIFO_REG(module) != 1)
      {
        error++;      
        printf("ERR! Incorrect number of bytes received. Expected 1. Received %d\n", UART_RCFIFO_REG(module));
      }  
          
      /* Get the first byte of data */
      data = UART_D_REG(module);
      
      /* Test the received data against the expected value */
      if (data != 0xAA)
      {
        error++;      
        printf("\nERR! Incorrect data received. Expected 0xAA. Received 0x%02X\n", data);
      } 
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