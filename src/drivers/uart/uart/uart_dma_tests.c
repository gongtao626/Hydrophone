/*
 * File:		uart_dma_tests.c
 * Purpose:		Tests integration of the UART and eDMA.
 *
 */

#include "common.h"
#include "uart.h"
#include "uart_tests.h"
#include "dma_channels.h"

/********************************************************************/
/* UART module transmit DMA test.
 * Tests operation of the UART with the DMA used to provide transmit data to 
 * the UART. This test runs on all of the UART modules with the exception of 
 * the main TERM_PORT.  
 * This test will also retest UART0 and UART1 with the FIFO enabled.
 * 
 * NOTE: A second COM port on the PC is needed for this test. It should be 
 * configured for 8-N-1 at the TERMINAL_BAUD.
 * 
 * If user input is not correct (indicating incorrect transmit and/or incorrect
 * receive) or if the DMA channel generates an error then the test returns the 
 * number of errors. Otherwise returns 0 (pass).
 */
int uart_tdre_dma_test()
{
    UART_MemMapPtr module;
    char ch, channel;
    int error = 0;
    char string[20];
    uint16 strLen;
    

    /* Initialize test variables */
    /* Note: for the FIFO enabled tests to work correctly strLen must be
     * evenly divisible by 4.
     */
    strcpy(string, "\nUART and EDMA test.");
    strLen = 20;	
    
    
    /* Enable the clocks to the DMA channel muxes */
    SIM_SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
    
    /* Enable the clock to the DMA module */
    SIM_SCGC7 |= SIM_SCGC7_DMA_MASK;
    
    /* Initialize the DMA channel muxes for all of the DMA channels used
     * by this test.
     */
    DMAMUX0_CHCFG0 = ( 0
                        | DMAMUX_CHCFG_ENBL_MASK    // enable the DMA channel request
                        | DMAMUX_CHCFG_SOURCE(3));  // UART0-Tx is source 3
    
    DMAMUX0_CHCFG1 = ( 0
                        | DMAMUX_CHCFG_ENBL_MASK    // enable the DMA channel request
                        | DMAMUX_CHCFG_SOURCE(5));  // UART1-Tx is source 5

    DMAMUX0_CHCFG2 = ( 0
                        | DMAMUX_CHCFG_ENBL_MASK    // enable the DMA channel request
                        | DMAMUX_CHCFG_SOURCE(7));  // UART2-Tx is source 7

        
    /* Pointer for accessing a specific UART. Set to UART0 to start as long
     * as UART0 isn't the TERM_PORT. Otherwise start with UART1.
     */
    if (TERM_PORT != UART0_BASE_PTR)
      module = UART0_BASE_PTR;
    else
      module = UART1_BASE_PTR;   
    
    printf("\nStarting UART transmit DMA test.\n");
    
    printf("\nA second PC terminal is required for this test.");
    printf("\nPlease open a second 8-N-1 terminal at the same baud as the main terminal.");
    printf("\nThe second terminal should be connected to UART2 on the baseboard.");
    printf("\nTerminal 1 will give directions including connections to make on a EDSNDC-256 card.");
    printf("\nTerminal 2 will be used for communication with the UART module under test.");
    printf("\nPress any key when ready.\n");
    ch = in_char();
        
    /* Run DMA transmit test on all UARTs with no FIFOs */
     while( module )
    {  
        /* Give directions for connecting up the UART module currently under test */
        if (module == UART0_BASE_PTR)
        {  
            printf("\n\nTesting UART 0...");
            printf("\nConnect J2 pin 2 to J9 pin 15 (UART0_RX)");
            printf("\nConnect J53 pin 1 to J9 pin 12 (UART0_TX)");
            
            channel = 0;//DMA_UART0TX_CH; // init DMA channel to use for this UART
        }  
        if (module == UART1_BASE_PTR)
        {  
            printf("\n\nTesting UART 1...");
            printf("\nConnect J7 pin 5 to J9 pin 15 (UART1_RX)");
            printf("\nConnect J7 pin 2 to J9 pin 12 (UART1_TX)");

            channel = 1;//DMA_UART1TX_CH; // init DMA channel to use for this UART
        }  
        if (module == UART2_BASE_PTR)
        {  
            printf("\n\nTesting UART 2...");
            printf("\nConnect J4 pin 5 to J9 pin 15 (UART2_RX)");
            printf("\nConnect J4 pin 2 to J9 pin 12 (UART2_TX)");

            channel = 2;//DMA_UART2TX_CH; // init DMA channel to use for this UART
        }
        
        /* Wait for input on terminal 2 (UART under test) */
        printf("Type something at the alternate terminal\n");
        ch = uart_getchar(module);
        
        printf("\nPress any key on terminal 1 once connections are complete.\n");
        ch = in_char();
        
	/* Configure UART to generate Tx DMA requests */
	UART_C2_REG(module) |= UART_C2_TIE_MASK;
        UART_C5_REG(module) |= UART_C5_TDMAS_MASK;
        
	/* Configure DMA Channel TCD */
        DMA_SADDR_REG(DMA_BASE_PTR,channel) = (uint32) string; // DMA source is the string address
        
        DMA_ATTR_REG(DMA_BASE_PTR,channel) = ( 0
                                                 | DMA_ATTR_SSIZE(0)
                                                 | DMA_ATTR_DSIZE(0) );
                                                   
        DMA_SOFF_REG(DMA_BASE_PTR,channel) = 0x01; // increment source by 1 after each read
        
        DMA_NBYTES_MLNO_REG(DMA_BASE_PTR,channel) = 1; // move 1 byte per request
		
        DMA_SLAST_REG(DMA_BASE_PTR,channel) = 0x0; // no adjust after last transfer
								
        DMA_DADDR_REG(DMA_BASE_PTR,channel) = (uint32) &UART_D_REG(module); // destination is the UART data register
	
	DMA_CITER_ELINKNO_REG(DMA_BASE_PTR,channel) = ( 0        // execute the minor loop strLen times
                                                    | DMA_CITER_ELINKNO_CITER(strLen) );
	
        DMA_DOFF_REG(DMA_BASE_PTR,channel) = 0x0; // no increment for destination
	
        DMA_DLAST_SGA_REG(DMA_BASE_PTR,channel) = 0x0; // no adjust after last transfer

        DMA_BITER_ELINKNO_REG(DMA_BASE_PTR,channel) = ( 0        // execute the minor loop strLen times
                                                    | DMA_BITER_ELINKNO_BITER(strLen) );
                                                         
        DMA_CSR_REG(DMA_BASE_PTR,channel) = ( 0
                                            | DMA_CSR_DREQ_MASK );  // disable request when the transfer is complete
        
	/* Enble DMA requests for the channel */
	DMA_SERQ = channel;
							
	/* Wait for DMA to complete or to generate an error */
        while( (!(DMA_CSR_REG(DMA_BASE_PTR,channel) & DMA_CSR_DONE_MASK)) );
        
  	/* Check for errors */
	if (DMA_ES)
	{
		printf("ERROR!!! An error ocurred while processing.\n");
		printf("ES = 0x%08x\n",DMA_ES);
                error++;
	}
                   
        printf("\nThe characters \"UART and EDMA test.\" should be displayed on terminal 2.");
        printf("\n\nPress SPACE on terminal 2 if characters are correct.\n");
        
        /* Wait for input on terminal 2 (UART under test) */
        ch = uart_getchar(module);
        
        /* If the input character isn't SPACE (0x20), then that indicates an 
         * error in the transmit and/or the recieve.
         */
        if (ch != 0x20)
        {  
            printf("\nERR! Incorrect input character received. Error counter has been incremented.\n");
            error++;
        }  
        
	/* Disable Tx DMA requests */
	UART_C2_REG(module) &= ~UART_C2_TIE_MASK;
        UART_C5_REG(module) &= ~UART_C5_TDMAS_MASK;
                            
        /* Advance the mem map ptr to the next available UART module to test */
        if (module == UART0_BASE_PTR)
        {  
          if (TERM_PORT == UART1_BASE_PTR)
              module = UART2_BASE_PTR;
          else
              module = UART1_BASE_PTR;
        }
        else if (module == UART1_BASE_PTR)
        {
          if (TERM_PORT == UART2_BASE_PTR)
              /* All UARTs have been checked, so drop out of the loop */
              module = 0;
          else
              module = UART2_BASE_PTR;
        }
        else if (module == UART2_BASE_PTR)
        {  
           /* All UARTs have been checked, so drop out of the loop */
           module = 0;
        }
    }

    /* Run tests again with the FIFO enabled for UART0 and UART1 */
    
    /* Pointer for accessing a specific UART. Set to UART0 to start as long
     * as UART0 isn't the TERM_PORT. Otherwise start with UART1.
     */
    if (TERM_PORT != UART0_BASE_PTR)
      module = UART0_BASE_PTR;
    else
      module = UART1_BASE_PTR;
     
    return error;    
} 
/********************************************************************/
/* UART module receive DMA test.
 * Tests operation of the UART with the DMA used to remove receive data from
 * the UART. This test runs on all of the UART modules with the exception of 
 * the main TERM_PORT.  
 * This test will also retest UART0 and UART1 with the FIFO enabled.
 * 
 * NOTE: A second COM port on the PC is needed for this test. It should be 
 * configured for 8-N-1 at the TERMINAL_BAUD.
 * 
 * If user input is not correct (indicating incorrect transmit and/or incorrect
 * receive) or if the DMA channel generates an error then the test returns the 
 * number of errors. Otherwise returns 0 (pass).
 */
int uart_rdrf_dma_test()
{
    UART_MemMapPtr module;
    char ch, channel;
    int error = 0;
    char string[25];
    uint16 strLen;
    

    /* Initialize test variables */
    /* Note: for the FIFO enabled tests to work correctly strLen must be
     * evenly divisible by 4.
     */
    strLen = 20;	
    
    /* Enable the clocks to the DMA channel muxes */
    SIM_SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
    
    /* Enable the clock to the DMA module */
    SIM_SCGC7 |= SIM_SCGC7_DMA_MASK;
    
    /* Initialize the DMA channel mux for all of the DMA channels used
     * by this test.
     */
    DMAMUX0_CHCFG0 = ( 0
                        | DMAMUX_CHCFG_ENBL_MASK    // enable the DMA channel request
                        | DMAMUX_CHCFG_SOURCE(2));  // UART0-Rx is source 2
    
    DMAMUX0_CHCFG1 = ( 0
                        | DMAMUX_CHCFG_ENBL_MASK    // enable the DMA channel request
                        | DMAMUX_CHCFG_SOURCE(4));  // UART1-Rx is source 4

    DMAMUX0_CHCFG2 = ( 0
                        | DMAMUX_CHCFG_ENBL_MASK    // enable the DMA channel request
                        | DMAMUX_CHCFG_SOURCE(6));  // UART2-Rx is source 6

        
    /* Pointer for accessing a specific UART. Set to UART0 to start as long
     * as UART0 isn't the TERM_PORT. Otherwise start with UART1.
     */
    if (TERM_PORT != UART0_BASE_PTR)
      module = UART0_BASE_PTR;
    else
      module = UART1_BASE_PTR;   
    
    printf("\nStarting UART receive DMA test.\n");
    
    printf("\nA second PC terminal is required for this test.");
    printf("\nPlease open a second 8-N-1 terminal at the same baud as the main terminal.");
    printf("\nThe second terminal should be connected to UART2 on the baseboard.");
    printf("\nTerminal 1 will give directions including connections to make on a EDSNDC-256 card.");
    printf("\nTerminal 2 will be used for communication with the UART module under test.");
    printf("\nPress any key when ready.\n");
    ch = in_char();
        
    /* Run DMA transmit test on all UARTs with no FIFOs */
    while( module )
    {  
        /* Give directions for connecting up the UART module currently under test */
        if (module == UART0_BASE_PTR)
        {  
            printf("\n\nTesting UART 0...");
            printf("\nConnect J2 pin 2 to J9 pin 15 (UART0_RX)");
            printf("\nConnect J53 pin 1 to J9 pin 12 (UART0_TX)");
            
            channel = 0;//DMA_UART0RX_CH; // init DMA channel to use for this UART
            
            printf("\n\nPress a character in alternate terminal\n\n");
            uart_putchar(UART0_BASE_PTR, '*');
            ch = uart_getchar(UART0_BASE_PTR);
            uart_putchar(UART0_BASE_PTR, ch);
        }  
        if (module == UART1_BASE_PTR)
        {  
            printf("\n\nTesting UART 1...");
            printf("\nConnect J7 pin 5 to J9 pin 15 (UART1_RX)");
            printf("\nConnect J7 pin 2 to J9 pin 12 (UART1_TX)");

            channel = 1;//DMA_UART1RX_CH; // init DMA channel to use for this UART

            printf("\n\nPress a character in alternate terminal\n\n");
            uart_putchar(UART1_BASE_PTR, '*');
            ch = uart_getchar(UART1_BASE_PTR);
            uart_putchar(UART1_BASE_PTR, ch);
            
        }  
        if (module == UART2_BASE_PTR)
        {  
            printf("\n\nTesting UART 2...");
            printf("\nConnect J4 pin 5 to J9 pin 15 (UART2_RX)");
            printf("\nConnect J4 pin 2 to J9 pin 12 (UART2_TX)");

            channel = 2;//DMA_UART2RX_CH; // init DMA channel to use for this UART
            
            printf("\n\nPress a character in alternate terminal\n\n");
            uart_putchar(UART2_BASE_PTR, '*');
            ch = uart_getchar(UART2_BASE_PTR);
            uart_putchar(UART2_BASE_PTR, ch);
        }
        
        printf("\nPress any key on terminal 1 once connections are complete.\n");
        ch = in_char();
        
	/* Configure UART to generate Rx  DMA requests */
	UART_C2_REG(module) |= UART_C2_RIE_MASK;
        UART_C5_REG(module) |= UART_C5_RDMAS_MASK;
   
	/* Configure DMA Channel TCD */
        DMA_SADDR_REG(DMA_BASE_PTR,channel) = (uint32) &UART_D_REG(module); // DMA source is the UART data register
        
        DMA_ATTR_REG(DMA_BASE_PTR,channel) = ( 0
                                                 | DMA_ATTR_SSIZE(0)
                                                 | DMA_ATTR_DSIZE(0) );
                                                   
        DMA_SOFF_REG(DMA_BASE_PTR,channel) = 0x0; // don't increment source address
        
        DMA_NBYTES_MLNO_REG(DMA_BASE_PTR,channel) = 1; // move 1 byte per request
		
        DMA_SLAST_REG(DMA_BASE_PTR,channel) = 0x0; // no adjust after last transfer
								
        DMA_DADDR_REG(DMA_BASE_PTR,channel) = (uint32) string; // destination is the string variable
	
	DMA_CITER_ELINKNO_REG(DMA_BASE_PTR,channel) = ( 0        // execute the minor loop strLen times
                                                    | DMA_CITER_ELINKNO_CITER(strLen) );
	
        DMA_DOFF_REG(DMA_BASE_PTR,channel) = 0x01; // increment dest by 1 after each transfer
	
        DMA_DLAST_SGA_REG(DMA_BASE_PTR,channel) = 0x0; // no adjust after last transfer

        DMA_BITER_ELINKNO_REG(DMA_BASE_PTR,channel) = ( 0        // execute the minor loop strLen times
                                                    | DMA_BITER_ELINKNO_BITER(strLen) );
                                                         
        DMA_CSR_REG(DMA_BASE_PTR,channel) = ( 0
                                            | DMA_CSR_DREQ_MASK );  // disable request when the transfer is complete
        
	/* Enble DMA requests for the channel */
	DMA_SERQ = channel;
							
	printf("\n\nThe test will collect 20 input characters from terminal 2. \n");
	printf("Once the 20 characters are input the UART will retransmit the"); 
        printf("\ncharacters on this terminal.\n");

	/* Wait for DMA to complete or to generate an error */
        while( !(DMA_CSR_REG(DMA_BASE_PTR,channel) & DMA_CSR_DONE_MASK))
        {;}
	
	/* Check for errors */
	if (DMA_ES)
	{
		printf("ERROR!!! An error ocurred while processing.\n");
		printf("ES = 0x%08x\n",DMA_ES);
                error++;
	}
        
        /* Set the last byte of the string to a NULL character so it will print correctly */
        string[20] = '\0';      
    
        printf("\n string collected = ");
        printf(string);
        printf("\n\nPress SPACE if this is the correct string.\n");
        
        /* Wait for input on terminal 1 */
        ch = in_char();
        
        /* If the input character isn't SPACE (0x20), then that indicates an 
         * error in the transmit and/or the recieve.
         */
        if (ch != 0x20)
        {  
            printf("\nERR! Incorrect input character received. Error counter has been incremented.\n");
            error++;
        }  
        
	/* Disable Rx DMA requests */
	UART_C2_REG(module) &= ~UART_C2_RIE_MASK;
        UART_C5_REG(module) &= ~UART_C5_RDMAS_MASK;
                            
        /* Advance the mem map ptr to the next available UART module to test */
        if (module == UART0_BASE_PTR)
        {  
          if (TERM_PORT == UART1_BASE_PTR)
              module = UART2_BASE_PTR;
          else
              module = UART1_BASE_PTR;
        }
        else if (module == UART1_BASE_PTR)
        {
          if (TERM_PORT == UART2_BASE_PTR)
              /* All UARTs have been checked, so drop out of the loop */
              module = 0;
          else
              module = UART2_BASE_PTR;
        }
        else if (module == UART2_BASE_PTR)
        {  
          /* All UARTs have been checked, so drop out of the loop */
          module = 0;
        }
    }
     
    return error;    
} 
/********************************************************************/
   
