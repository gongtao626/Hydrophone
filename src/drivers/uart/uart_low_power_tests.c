/*
 * File:		uart_low_power_tests.c
 * Purpose:		UART low power mode tests.
 *
 */

#include "common.h"
#include "uart.h"
#include "smc.h"
#include "uart_tests.h"
#include "dma_channels.h"

//function prototypes
void pee_to_blpi(void);
void pee_to_blpe(void);

/* Globals for counting number of interrupts */
int num_edge_int;

/* Globals from sysinit.c */
extern int mcg_clk_hz;
extern int core_clk_khz;
extern int core_clk_mhz;
extern int periph_clk_khz;

/********************************************************************/
/* UART wait mode operation test. This test will configure the DMA
 * to transmit data to the UART, and then put the processor into 
 * WAIT mode immediately before enabling the DMA transfer. A message
 * should be able to transmit even though the processor is in WAIT mode.
 *
 * The UART will also be configured to generate an interrupt on an RX
 * edge. This interrupt will be used to wake the core and resume normal
 * RUN mode operation.
 *
 * This test does not have any paramters to check, so it always returns 0
 * (pass). The user will need to check that the terminal displays as
 * expected.
 */
int uart_wait_mode_test()
{
    UART_MemMapPtr module;
    char string[40];
    uint16 strLen;
    int channel;
    uint32 i;

    printf("\nStarting UART wait mode operation test.\n");
        

    /* Initialize the string we will transmit in WAIT mode */
    strcpy(string, "\nUART transmitting while CPU is stopped.");
    strLen = 40;	
    
    /* Initialize DMA so that there will be a source to provide
     * data to transmit other than the CPU.
     */
    
    /* Enable the clocks to the DMA channel muxes */
    SIM_SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
    
    /* Enable the clock to the DMA module */
    SIM_SCGC7 |= SIM_SCGC7_DMA_MASK;
        
    /* We'll use the main TERM_PORT for this test. */
    module = TERM_PORT;
    
    /* Initialize the DMA channel mux, set a variable for the DMA channel
     * to use for this test, and enable status interrupts for the selected
     * UART.
     */
    if (TERM_PORT == UART0_BASE_PTR)
    {      
        DMAMUX0_CHCFG0 = ( 0
                        | DMAMUX_CHCFG_ENBL_MASK    // enable the DMA channel request
                        | DMAMUX_CHCFG_SOURCE(3));  // UART0-Tx is source 3
        channel = 0;
        enable_irq(32-16);
    }
    if (TERM_PORT == UART1_BASE_PTR)
    {
        DMAMUX0_CHCFG1 = ( 0
                        | DMAMUX_CHCFG_ENBL_MASK    // enable the DMA channel request
                        | DMAMUX_CHCFG_SOURCE(5));  // UART1-Tx is source 5
        channel = 1;
        enable_irq(34-16);
    }        
    if (TERM_PORT == UART2_BASE_PTR)
    {
        DMAMUX0_CHCFG2 = ( 0
                        | DMAMUX_CHCFG_ENBL_MASK    // enable the DMA channel request
                        | DMAMUX_CHCFG_SOURCE(7));  // UART2-Tx is source 7
        channel = 2;
        enable_irq(36-16);
    }
        
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
        

    /* Enable UART Rx edge interrupts - This will be the wakeup method from WAIT mode */
    UART_BDH_REG(module) |= UART_BDH_RXEDGIE_MASK;
    

    printf("\nAbout to enter WAIT mode.");
    printf("\nMessage \"UART transmitting while CPU is stopped.\" should display.");
    printf("\nAfter message displays press any key to wake the CPU.");
    printf("\nTest complete message should only be displayed after pressing a key to wake CPU.");

    /* Delay to allow printf to complete. */
    for( i=0; i < 0x1000; i++);

    /* Configure UART to generate Tx DMA requests */
    UART_C5_REG(module) |= UART_C5_TDMAS_MASK;
    UART_C2_REG(module) |= UART_C2_TIE_MASK;
        
    /* Enble DMA requests for the channel */
    DMA_SERQ = channel;
    
    /* Enter wait mode */
    enter_wait();
							
    /* If this printf executes before a key is pressed, then the CPU didn't
     * actually go into WAIT mode.
     */
    printf("\n\nWait mode test complete!\n");

    
    /* Make sure all UART interrupts are disabled before starting next test */
    disable_irq(32-16);
    disable_irq(33-16);
    disable_irq(34-16);
    disable_irq(35-16);
    disable_irq(36-16);
    disable_irq(37-16);

    /* Disable UART Rx edge interrupts */
    UART_BDH_REG(module) &= ~UART_BDH_RXEDGIE_MASK;


    /* Disable Tx DMA requests */
    UART_C2_REG(module) &= ~UART_C2_TIE_MASK;
    UART_C5_REG(module) &= ~UART_C5_TDMAS_MASK;
                              
    return 0;
}  
/********************************************************************/
/* UART wait mode disable test. This test will configure the UART
 * to be disabled in WAIT mode. Otherwise the setup is the same as the
 * WAIT mode operation test. This time the goal is to see that the UART
 * does not transmit data while in WAIT mode.
 *
 * The UART will also be configured to generate an interrupt on an RX
 * edge. This interrupt will be used to wake the core and resume normal
 * RUN mode operation.
 *
 * This test does not have any paramters to check, so it always returns 0
 * (pass). The user will need to check that the terminal displays as
 * expected.
 */
int uart_wait_mode_disable_test()
{
    UART_MemMapPtr module;
    char string[40];
    uint16 strLen;
    int channel;
    uint32 i;

    printf("\nStarting UART wait mode disable test.\n");
        

    /* Initialize the string we will transmit in WAIT mode */
    strcpy(string, "\nUART transmitting while CPU is stopped.");
    strLen = 40;	
    
    /* Initialize DMA so that there will be a source to provide
     * data to transmit other than the CPU.
     */
    
    /* Enable the clocks to the DMA channel muxes */
    SIM_SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
    
    /* Enable the clock to the DMA module */
    SIM_SCGC7 |= SIM_SCGC7_DMA_MASK;    

        
    /* We'll use the main TERM_PORT for this test. */
    module = TERM_PORT;
    
    /* Initialize the DMA channel mux, set a variable for the DMA channel
     * to use for this test, and enable status interrupts for the selected
     * UART.
     */
    if (TERM_PORT == UART0_BASE_PTR)
    {      
        DMAMUX0_CHCFG0 = ( 0
                        | DMAMUX_CHCFG_ENBL_MASK    // enable the DMA channel request
                        | DMAMUX_CHCFG_SOURCE(3));  // UART0-Tx is source 3
        channel = 0;
        enable_irq(32-16);
    }
    if (TERM_PORT == UART1_BASE_PTR)
    {
        DMAMUX0_CHCFG1 = ( 0
                        | DMAMUX_CHCFG_ENBL_MASK    // enable the DMA channel request
                        | DMAMUX_CHCFG_SOURCE(5));  // UART1-Tx is source 5
        channel = 1;
        enable_irq(34-16);
    }        
    if (TERM_PORT == UART2_BASE_PTR)
    {
        DMAMUX0_CHCFG2 = ( 0
                        | DMAMUX_CHCFG_ENBL_MASK    // enable the DMA channel request
                        | DMAMUX_CHCFG_SOURCE(7));  // UART2-Tx is source 7
        channel = 2;
        enable_irq(36-16);
    }
        
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
        

    /* Enable UART Rx edge interrupts - This will be the wakeup method from WAIT mode */
    UART_BDH_REG(module) |= UART_BDH_RXEDGIE_MASK;
    

    printf("\nAbout to enter WAIT mode.");
    printf("\nPressing a key on the UART should NOT be able to wake up the CPU.");
    printf("\nThe ABORT button can be used to wake up the CPU.");
    
    printf("\nMessage \"UART transmitting while CPU is stopped.\" should NOT be displayed.");    
    printf("\nTest complete message should only be displayed after CPU wake up.");

    /* Delay to allow printf to complete. */
    for( i=0; i < 0x1000; i++);

    /* Configure UART to generate Tx DMA requests */
    UART_C5_REG(module) |= UART_C5_TDMAS_MASK;
    UART_C2_REG(module) |= UART_C2_TIE_MASK;

    /* Set the C1[UARTSWAI] bit so that the UART will be disabled in WAIT mode */
    UART_C1_REG(module) |= UART_C1_UARTSWAI_MASK;

    /* Enble DMA requests for the channel */
    DMA_SERQ = channel;
    
    /* Enter wait mode */
    enter_wait();
							
    /* Disable UART Tx DMA requests */
    UART_C5_REG(module) &= ~UART_C5_TDMAS_MASK;
    UART_C2_REG(module) &= ~UART_C2_TIE_MASK;

    /* Disable DMA requests for the channel */
    DMA_CERQ = channel;

    /* If this printf executes before a key is pressed, then the CPU didn't
     * actually go into WAIT mode.
     */
    printf("\n\nWait mode UART disabled test complete!\n");
    
    /* Make sure all UART interrupts are disabled before starting next test */
    disable_irq(32-16);
    disable_irq(33-16);
    disable_irq(34-16);
    disable_irq(35-16);
    disable_irq(36-16);
    disable_irq(37-16);

    /* Disable UART Rx edge interrupts */
    UART_BDH_REG(module) &= ~UART_BDH_RXEDGIE_MASK;


    /* Disable Tx DMA requests */
    UART_C2_REG(module) &= ~UART_C2_TIE_MASK;
    UART_C5_REG(module) &= ~UART_C5_TDMAS_MASK;
                              
    return 0;
}    
/********************************************************************/
/* UART stop mode test. This test will configure the UART for an Rx
 * edge interrupt. This interrupt will be used to wake the core and
 * resume normal RUN mode operation.
 *
 * This test does not have any paramters to check, so it always returns 0
 * (pass). The user will need to check that the terminal displays as
 * expected.
 */
int uart_stop_mode_test()
{
    uint32 i;
    UART_MemMapPtr module;
    
    /* We'll use the main TERM_PORT for this test. */
    module = TERM_PORT;
        
    printf("\nStarting UART stop mode test.\n");
        
    /* Only one UART is used for this test, but it is easier to enable
     * interrupts for all of them.
     */
    enable_irq(32-16);
    enable_irq(33-16);
    enable_irq(34-16);
    enable_irq(35-16);
    enable_irq(36-16);
    enable_irq(37-16);


    /* Enable UART Rx edge interrupts - This will be the wakeup method from WAIT mode */
    UART_BDH_REG(module) |= UART_BDH_RXEDGIE_MASK;

    printf("\nAbout to enter STOP mode.");
    printf("\nPressing a key on the UART should be able to wake up the CPU.");
    printf("\nTest complete message should only be displayed after CPU wake up.");

    /* Delay to allow printf to complete. */
    for( i=0; i < 0x1000; i++);

    /* Set the PLLSTEN bit. This way the PLL output is gated but does still
     * stay on in STOP mode. We need to set this bit so that the UART will
     * be at the right baud rate immediately after exiting STOP. Otherwise
     * the PLL has to be re-enabled to get the right baud rate.
     */
    MCG_C5 |= MCG_C5_PLLSTEN_MASK;
    
    /* Enter stop mode */
    enter_stop();       
							
    /* If this printf executes before a key is pressed, then the CPU didn't
     * actually go into STOP mode.
     */
    printf("\n\nStop mode UART test complete!\n");



    /* Make sure all UART interrupts are disabled before starting next test */
    disable_irq(32-16);
    disable_irq(33-16);
    disable_irq(34-16);
    disable_irq(35-16);
    disable_irq(36-16);
    disable_irq(37-16);

    /* Disable UART Rx edge interrupts */
    UART_BDH_REG(module) &= ~UART_BDH_RXEDGIE_MASK;

    return 0;
}    
/********************************************************************/
/* UART VLPR mode test. This test will configure the processor for
 * very low power run mode. The UART will need to be re-initialized
 * after entry into the mode to calculate the new baud rate divider.
 *
 * After entering the mode transmit and receive functions will be checked
 * at the new system clock rate. 
 *
 * This test does not have any paramters to check, so it always returns 0
 * (pass). The user will need to check that the terminal displays as
 * expected.
 */

/* NOTE: Currently this test hangs when exiting VLPR mode. I think this
 * is due to known issues with VLPR mode on first silicon.
 */

int uart_vlpr_mode_test()
{
    char ch;
    int i;
            
    printf("\nGetting ready for entry into VLPR for UART VLPR mode test.\n");
    printf("\nOnce VLPR mode is entered, the terminal will echo characters.");
    printf("\nPress the SPACE key to exit VLPR.\n\n");
    
    printf("\nDue to the low speed during VLPR the baud rate will be changed.");
    printf("\nSwitch terminal to 19200 baud now and press any key when ready.\n");
    
    /* Wait for user input */
    ch = in_char();

    /* Enter BLPI mode */
    pee_to_blpi();
    
    MCG_C6 &= ~MCG_C6_CME_MASK;
    
    printf("\nEntering VLPR mode...");
    
    /* Delay to allow printf to complete. */
    for( i=0; i < 0x1000; i++);
    
    /* Enter VLPR mode */
    enter_vlpr(1);
							
    if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 4)
        printf("  in VLPR Mode !\n ");
    else if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 1)
        printf("  in Run Mode  !\n ");
  
    /* Echo characters until the input character is a SPACE. */
    
    ch = in_char();

    while (ch != 0x20)
    {
        out_char(ch);
        ch = in_char();
    }
    
    printf("\nExiting VLPR mode now.");
    printf("\nSwitch terminal back to default baud and press any key when ready.\n");

    /* Delay to allow printf to complete. */
    for( i=0; i < 0x1000; i++);
    
    if ((TERM_PORT == UART0_BASE_PTR) | (TERM_PORT == UART1_BASE_PTR))
        uart_init (TERM_PORT, core_clk_khz, TERMINAL_BAUD);
    else
        uart_init (TERM_PORT, periph_clk_khz, TERMINAL_BAUD);

    /* Wait for user input */
    ch = in_char();
           
    /* Return to regular RUN mode */
    exit_vlpr();
            
    printf("\n\nVLPR mode UART test complete!\n");

    return 0;
}    
/********************************************************************/
/* UART VLPR mode test2. This test will configure the processor for
 * very low power run mode. The UART will need to be re-initialized
 * after entry into the mode to calculate the new baud rate divider.
 *
 * For this test, the mode controller will be configured to wake-up
 * on an interrupt event, so instead of manually exiting VLPR the 
 * interrupt will be used to go back to regular RUN mode. 
 *
 * This test does not have any parameters to check, so it always returns 0
 * (pass). The user will need to check that the terminal displays as
 * expected.
 */

/* NOTE: VLPR is not guaranteed to work right now. This test must execute
 * from RAM to work. Running it in flash will not work on current Si.
 */

int uart_vlpr_mode_test2()
{
    UART_MemMapPtr module;
    volatile char ch;
    int i;
            
    /* We'll use the main TERM_PORT for this test. */
    module = TERM_PORT;

    printf("\nGetting ready for entry into VLPR for UART VLPR mode test2.\n");
    printf("\nOnce VLPR mode is entered, the terminal will display a message.");
    
    printf("\nDue to the low speed during VLPR the baud rate will be changed.");
    printf("\nSwitch terminal to 19200 baud now and press any key when ready.\n");
    
    /* Wait for user input */
    ch = in_char();
    
    MCG_C6 &= ~MCG_C6_CME_MASK;
    
    /* Enter BLPI mode */
    pee_to_blpi();
    
    printf("\nEntering VLPR mode...");

    /* Delay to allow printf to complete. */
    for( i=0; i < 0x1000; i++);    
        
    /* Only one UART is used for this test, but it is easier to enable
     * interrupts for all of them.
     */
    enable_irq(32-16);
    enable_irq(33-16);
    enable_irq(34-16);
    enable_irq(35-16);
    enable_irq(36-16);
    enable_irq(37-16);

    /* Make sure the Rx edge interrupt count is 0 */
    num_edge_int = 0;

    /* Enter VLPR mode */
    enter_vlpr(1);
							
    /* Display message to indicate we are now in VLPR mode */
    if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 4)
        printf("  in VLPR Mode !\n ");
    else if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 1)
        printf("  in Run Mode  !\n ");
    printf("\nPress any key to exit VLPR mode.");
    
    /* Enable UART Rx edge interrupts - This will be the wakeup method from WAIT mode */
    UART_BDH_REG(module) |= UART_BDH_RXEDGIE_MASK;
        
    /* Wait for Rx edge interrupt */
    while((num_edge_int)< 2);
    
    /* Should be back in normal RUN mode now */    
    printf("\nProcessor should be back in RUN mode now.");
    if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 4)
        printf("  in VLPR Mode !\n ");
    else if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 1)
        printf("  in Run Mode  !\n ");
    
    printf("\n\nVLPR mode UART test2 complete!\n");

    /* Make sure all UART interrupts are disabled before starting next test */
    disable_irq(32-16);
    disable_irq(33-16);
    disable_irq(34-16);
    disable_irq(35-16);
    disable_irq(36-16);
    disable_irq(37-16);

    /* Disable UART Rx edge interrupts */
    UART_BDH_REG(module) &= ~UART_BDH_RXEDGIE_MASK;

    return 0;
}  
/********************************************************************/
/* UART VLPW mode operation test. This test will configure the DMA
 * to transmit data to the UART, then put the processor into VLPR mode
 * and reconfigure the UART for a new baudrate. Once VLPR is running
 * the code will put the processor into VLPW mode immediately after 
 * enabling the DMA transfer. A message should be able to transmit even
 * though the processor is in VLPW mode.
 *
 * The UART will also be configured to generate an interrupt on an RX
 * edge. This interrupt will be used to wake the core and resume VLPR
 * operation.
 *
 * NOTE: Right now exit from VLPR mode back to RUN is not working. This
 * test will need some modification to put the processor back in full
 * RUN mode once that is working correctly on Si.
 *
 * This test does not have any paramters to check, so it always returns 0
 * (pass). The user will need to check that the terminal displays as
 * expected.
 */
int uart_vlpw_mode_test()
{
    UART_MemMapPtr module;
    char string[40];
    uint16 strLen;
    int channel;
    volatile char ch;
    int i;

    printf("\nStarting UART VLPW mode operation test.\n");
        

    /* Initialize the string we will transmit in WAIT mode */
    strcpy(string, "\nUART transmitting while CPU is stopped.");
    strLen = 40;	
    
    /* Initialize DMA so that there will be a source to provide
     * data to transmit other than the CPU.
     */
    
    
    /* Enable the clocks to the DMA channel muxes */
    SIM_SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
    
    /* Enable the clock to the DMA module */
    SIM_SCGC7 |= SIM_SCGC7_DMA_MASK;    

        
    /* We'll use the main TERM_PORT for this test. */
    module = TERM_PORT;
    
    /* Initialize the DMA channel mux, set a variable for the DMA channel
     * to use for this test, and enable status interrupts for the selected
     * UART.
     */
    /* Initialize the DMA channel mux, set a variable for the DMA channel
     * to use for this test, and enable status interrupts for the selected
     * UART.
     */
    if (TERM_PORT == UART0_BASE_PTR)
    {      
        DMAMUX0_CHCFG0 = ( 0
                        | DMAMUX_CHCFG_ENBL_MASK    // enable the DMA channel request
                        | DMAMUX_CHCFG_SOURCE(3));  // UART0-Tx is source 3
        channel = 0;
        enable_irq(32-16);
    }
    if (TERM_PORT == UART1_BASE_PTR)
    {
        DMAMUX0_CHCFG1 = ( 0
                        | DMAMUX_CHCFG_ENBL_MASK    // enable the DMA channel request
                        | DMAMUX_CHCFG_SOURCE(5));  // UART1-Tx is source 5
        channel = 1;
        enable_irq(34-16);
    }        
    if (TERM_PORT == UART2_BASE_PTR)
    {
        DMAMUX0_CHCFG2 = ( 0
                        | DMAMUX_CHCFG_ENBL_MASK    // enable the DMA channel request
                        | DMAMUX_CHCFG_SOURCE(7));  // UART2-Tx is source 7
        channel = 2;
        enable_irq(36-16);
    }
        
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
        

    printf("\nGetting ready for entry into VLPR for UART VLPW mode test.\n");
    
    printf("\nDue to the low speed during VLPR the baud rate will be changed.");
    printf("\nSwitch terminal to 19200 baud now and press any key when ready.\n");
    
    /* Wait for user input */
    ch = in_char();
    
    /* Enter BLPI mode */
    pee_to_blpe();
    
    MCG_C6 &= ~MCG_C6_CME_MASK;
    
    printf("\nEntering VLPR mode...");

    /* Delay to allow printf to complete. */
    for( i=0; i < 0x1000; i++);

    /* Enter VLPR mode */
    enter_vlpr(1/*FSL: 0*/);
							
    if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 4)
        printf("  in VLPR Mode !\n ");
    else if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 1)
        printf("  in Run Mode  !\n ");

    printf("\nAbout to enter VLPW mode.");
    printf("\nMessage \"UART transmitting while CPU is stopped.\" should display.");
    printf("\nAfter message displays press any key to wake the CPU.");
    printf("\nTest complete message should only be displayed after pressing a key to wake CPU.");

    /* Delay to allow printf to complete. */
    for( i=0; i < 0x1000; i++);

    /* Configure UART to generate Tx DMA requests */
    UART_C5_REG(module) |= UART_C5_TDMAS_MASK;
    UART_C2_REG(module) |= UART_C2_TIE_MASK;
        
    /* Enble DMA requests for the channel */
    DMA_SERQ = channel;
    
    /* Write 1 to make sure the RXEDGIF is cleared before enabling interrupts */
    UART_S2_REG(module) = UART_S2_RXEDGIF_MASK;
    
    /* Enable UART Rx edge interrupts - This will be the wakeup method from WAIT mode */
    UART_BDH_REG(module) |= UART_BDH_RXEDGIE_MASK;

    /* Enter VLPW mode */
    enter_wait();
							
    if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 4)
        printf("  in VLPR Mode !\n ");
    else if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 1)
        printf("  in Run Mode  !\n ");

    /* If this printf executes before a key is pressed, then the CPU didn't
     * actually go into WAIT mode.
     */
    printf("\n\nVLPW mode test complete!\n");

    /* Make sure all UART interrupts are disabled before starting next test */
    disable_irq(32-16);
    disable_irq(33-16);
    disable_irq(34-16);
    disable_irq(35-16);
    disable_irq(36-16);
    disable_irq(37-16);

    /* Disable UART Rx edge interrupts */
    UART_BDH_REG(module) &= ~UART_BDH_RXEDGIE_MASK;


    /* Disable Tx DMA requests */
    UART_C2_REG(module) &= ~UART_C2_TIE_MASK;
    UART_C5_REG(module) &= ~UART_C5_TDMAS_MASK;
                              
    return 0;
}  
/********************************************************************/
/* UART VLPS mode test. This test will configure the UART for an Rx
 * edge interrupt. Then the processor will be put into VLPR mode and from
 * there enter VLPS mode. An interrupt with LPWUI cleared will be used
 * to return the processor to VLPR mode.
 *
 *
 * IMPORTANT NOTE: In order to prevent clock issues when exiting LLS.
 * This test should be run with NO_PLL_INIT defined!!!!!
 *
 *
 * This test does not have any paramters to check, so it always returns 0
 * (pass). The user will need to check that the terminal displays as
 * expected.
 */
int uart_vlps_mode_test()
{
    UART_MemMapPtr module;
    int i;
    
    /* We'll use the main TERM_PORT for this test. */
    module = TERM_PORT;
        
    printf("\nStarting UART VLPS mode test.\n");
        
    /* Only one UART is used for this test, but it is easier to enable
     * interrupts for all of them.
     */
    enable_irq(32-16);
    enable_irq(33-16);
    enable_irq(34-16);
    enable_irq(35-16);
    enable_irq(36-16);
    enable_irq(37-16);


    /* Enable UART Rx edge interrupts - This will be the wakeup method from WAIT mode */
    UART_BDH_REG(module) |= UART_BDH_RXEDGIE_MASK;

    printf("\nAbout to enter VLPS mode.");
    printf("\nPressing a key on the UART should be able to wake up the CPU.");
    printf("\nTest complete message should only be displayed after CPU wake up.");

 
    printf("\nEntering VLPS mode...");

    /* Delay to allow printf to complete. */
    for( i=0; i < 0x10000; i++);
     
    /* Enter stop mode */
    enter_vlps(1);
							
    /* If this printf executes before a key is pressed, then the CPU didn't
     * actually go into STOP mode.
     */
    printf("\n\nStop mode UART test complete!\n");



    /* Make sure all UART interrupts are disabled before starting next test */
    disable_irq(32-16);
    disable_irq(33-16);
    disable_irq(34-16);
    disable_irq(35-16);
    disable_irq(36-16);
    disable_irq(37-16);

    /* Disable UART Rx edge interrupts */
    UART_BDH_REG(module) &= ~UART_BDH_RXEDGIE_MASK;

    return 0;
}    
/********************************************************************/
/* UART LLS mode test. This test will configure the LLWU module to 
 * wake up on an edge for a UART pin, then put the processor into LLS
 * mode. Once the part is in LLS mode a received character (edge on
 * the UART pin should be able to wake the core and resume normal RUN
 * mode operation. The UART registers should retain state in LLS mode
 * so no reconfiguration of the UART should be needed after wakeup.
 *
 *
 * IMPORTANT NOTE: In order to prevent clock issues when exiting LLS.
 * This test should be run with NO_PLL_INIT defined!!!!!
 *
 *
 *
 * NOTE: Since only a limited number of pins can be used for an LLWU
 * wakeup, this test is hard coded to use a specific pin. 
 *
 *
 * This test does not have any paramters to check, so it always returns 0
 * (pass). The user will need to check that the terminal displays as
 * expected.
 */
int uart_lls_mode_test()
{
    int i;
    volatile char ch;
        
    printf("\nStarting UART LLS mode test.\n");
    
    /* Enable the PTE1/UART1_RX pin as an LLWU wakeup source (falling edge) */
    LLWU_PE1 = LLWU_PE1_WUPE0(0x2);
    
     /* Clear the flag that got us into the interrupt - only WUF0 is enabled */
    LLWU_F1 = LLWU_F1_WUF0_MASK;
    
    
    /* Enable the LLWU interrupt source */
    enable_irq(25-16);                 
    
    /* Only one UART is used for this test, but it is easier to enable
     * interrupts for all of them.
     */
    enable_irq(32-16);
    enable_irq(33-16);
    enable_irq(34-16);
    enable_irq(35-16);
    enable_irq(36-16);
    enable_irq(37-16);
    
    printf("\nAbout to enter LLS mode.");
    printf("\nPressing a key on the UART should be able to wake up the CPU.");
    printf("\nTest complete message should only be displayed after CPU wake up.");
    
    printf("\n\nPress any key to enter LLS mode.");
    ch = in_char();

    printf("\nEntering LLS mode...");
    
    /* Delay to allow printf to complete. */
    for( i=0; i < 0x10000; i++);
    
    
    /* Enable UART Rx data interrupts  */
    UART_C2_REG(TERM_PORT) |= UART_C2_RIE_MASK;    
 
    
    /* Enter LLS mode */
    enter_lls();
    							    
    /* If this printf executes before a key is pressed, then the CPU didn't
     * actually go into LLS mode.
     */
    printf("\n\nLLS mode UART test complete!\n");
    
    
    /* Make sure all UART interrupts are disabled before starting next test */
    disable_irq(32-16);
    disable_irq(33-16);
    disable_irq(34-16);
    disable_irq(35-16);
    disable_irq(36-16);
    disable_irq(37-16);

    /* Disable UART Rx data interrupts */
     UART_C2_REG(TERM_PORT) &= ~UART_C2_RIE_MASK;    



    /* Disable LLWU interrupt before starting next test */
    disable_irq(25-16);                 

    return 0;
}    
/********************************************************************/
/* UART VLLS mode test. This test will configure the LLWU module to 
 * wake up on an edge for a UART pin, then put the processor into VLLS1
 * mode. Once the part is in VLLS1 mode a received character (edge on
 * the UART pin should be able to wake the core. The processor will go 
 * through a reset after the wakeup from VLLS, so the test complete 
 * message should never actually be reached. 
 *
 *
 * NOTE: Since only a limited number of pins can be used for an LLWU
 * wakeup, this test is hard coded to use a specific pin. 
 *
 *
 * This test does not have any paramters to check, so it always returns 0
 * (pass). The user will need to check that the terminal displays as
 * expected.
 */
int uart_vlls_mode_test()
{
    int i;
    volatile char ch;
        
    printf("\nStarting UART VLLS mode test.\n");
    
    /* Enable the PTE1/UART1_RX pin as an LLWU wakeup source (falling edge) */
    LLWU_PE1 = LLWU_PE1_WUPE0(0x2);
    
     /* Clear the flag that got us into the interrupt - only WUF0 is enabled */
    LLWU_F1 = LLWU_F1_WUF0_MASK;

    /* Enable the LLWU interrupt source */
    enable_irq(25-16);                 
    
    printf("\nAbout to enter VLLS3 mode.");
    printf("\nPressing a key on the UART should be able to wake up the CPU.");
    printf("\nTest complete message should only be displayed after CPU wake up.");
    
    printf("\n\nPress any key to enter VLLS3 mode.");
    ch = in_char();

    printf("\nEntering VLLS3 mode...");
    
    /* Delay to allow printf to complete. */
    for( i=0; i < 0x10000; i++);

    enable_lpwui();
    
    /* Enter VLLS3 mode */
    enter_vlls3();
    
    							    
    /* If this printf executes, then the CPU didn't
     * actually go into LLS mode.
     */
    printf("\n\nLLS mode UART test complete!\n");


    /* Disable LLWU interrupt before starting next test */
    disable_irq(25-16);                 

    return 0;
}  
/********************************************************************/
/* MCG PEE to BLPI. 
 * Routine to move the MCG from PEE mode to BLPI. This clock mode 
 * change is required before moving into any of the VLPx modes. Since
 * the clock setup is somewhat board specific, the entire sequence is
 * not covered by a function in the MCG driver. 
 */
void pee_to_blpi(void)
{
    uint32 fast_irc_freq = 4000000;
  
            /*Enable all operation modes because this is a write once register*/  
        SMC_PMPROT =  SMC_PMPROT_AVLLS_MASK |
                      SMC_PMPROT_ALLS_MASK  |    
                      SMC_PMPROT_AVLP_MASK;
    
    /* Move the MCG from PEE to PBE mode to prepare for entry into VLPR mode */    
    mcg_clk_hz = pee_pbe(CLK0_FREQ_HZ);
    
    /* Check for errors returned by pee_pbe() */
    if (mcg_clk_hz == 0x08)
        while(1);
    
    /* Move the MCG from PBE to FBE */
    mcg_clk_hz = pbe_fbe(CLK0_FREQ_HZ);
    
    /* Check for errors returned by pbe_fbe() */
    if (mcg_clk_hz == 0x04)
        while(1);
    
    /* Move from FBE to FBI mode */
    mcg_clk_hz = fbe_fbi(fast_irc_freq,FAST_IRC);
    
    /* Check for errors returned by fbe_fbi() */
    if (mcg_clk_hz == 0x04)
        while(1);
    
    /* Move from FBI to BLPI mode */
    mcg_clk_hz = fbi_blpi(fast_irc_freq,FAST_IRC);
    
    /* Check for errors returned by fbe_fbi() */
    if (mcg_clk_hz == 0x03)
        while(1);
    
    /* Reconfigure the clock dividers for the new frequency */
    /*FSL: getting 2MHz from MCG*/
    SIM_CLKDIV1 = ( 0
                        | SIM_CLKDIV1_OUTDIV1(0)
                        | SIM_CLKDIV1_OUTDIV2(1)
                        //| SIM_CLKDIV1_OUTDIV3(1)
                        | SIM_CLKDIV1_OUTDIV4(3) );

    /* Calculate a new core_clk_khz value */
    core_clk_khz = mcg_clk_hz/1000;

    /* Re-initialize the UART for the new frequency */
        if ((TERM_PORT == UART0_BASE_PTR) | (TERM_PORT == UART1_BASE_PTR))
            uart_init (TERM_PORT, core_clk_khz, 19200);
        else
  	    uart_init (TERM_PORT, periph_clk_khz, 19200);
    
    printf("\n In blpi mode now ready for entry into VLPx  \n\n"); 
}     
/********************************************************************/
/* MCG PEE to BLPI. 
 * Routine to move the MCG from PEE mode to BLPI. This clock mode 
 * change is required before moving into any of the VLPx modes. Since
 * the clock setup is somewhat board specific, the entire sequence is
 * not covered by a function in the MCG driver. 
 */
void pee_to_blpe(void)
{
    uint32 fast_irc_freq = 4000000;
    
    mcg_clk_hz = pee_pbe(CLK0_FREQ_HZ);
    mcg_clk_hz = pbe_blpe(CLK0_FREQ_HZ);
    /* Reduce Flash clock to 1MHz- Currently commented out due to errata KINETIS50MHZ_1N86B-4473*/
    //SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(1)|SIM_CLKDIV1_OUTDIV2(1)|SIM_CLKDIV1_OUTDIV4(0x7);
    /*Reduce Flash clock to 500KHz*/
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(1)|SIM_CLKDIV1_OUTDIV2(1)|SIM_CLKDIV1_OUTDIV4(0xF);

    /* Calculate a new core_clk_khz value */
    core_clk_khz = mcg_clk_hz/1000;

    /* Re-initialize the UART for the new frequency */
    if ((TERM_PORT == UART0_BASE_PTR) | (TERM_PORT == UART1_BASE_PTR))
        uart_init (TERM_PORT, core_clk_khz, 19200);
    else
  	uart_init (TERM_PORT, periph_clk_khz, 19200);
    
    printf("\n In blpi mode now ready for entry into VLPx  \n\n"); 
}     
/********************************************************************/
