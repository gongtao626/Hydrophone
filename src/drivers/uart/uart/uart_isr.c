/*
 * File:		uart_isr.c
 * Purpose:		interrupt handler routines for the Kinetis uart tests
 *
 */

#include "common.h"

/* Globals from uart_tx_interrupt_tests.c */
extern int num_txof_errors;
extern int num_tc_int;
extern int num_tdre_int;

/* Globals from uart_rx_interrupt_tests.c */
extern int num_rdrf_int;
extern int num_or_errors;
extern int num_rxuf_errors;
extern int num_parity_errors;
extern int num_nf_errors;
extern int num_framing_errors;
extern int num_idle_int;
extern int num_lin_breaks;

/* Globals from uart_low_power_tests.c */
extern int num_edge_int;

/* Globals from LON tests */
extern int num_lon_tx_int;
extern int num_lon_txf_int;
extern int num_lon_rx_int;
extern int num_lon_isd_int;
extern int num_lon_psf_int;

/********************************************************************/
/* Generic UART status interrupt handler.
 * This function is called by the specific UART interupt handler routines,
 * so it isn't technically an interrupt handler. The individual interrupt
 * handlers will call this function immediately, so this function contains
 * most of the code. This function will check all possible interrupt sources,
 * so if there are multiple interrupt flags they will all be handled.
 * 
 * NOTE: Since the handler doesn't have access to any transmit or recieve 
 * buffers from an application layer, the transmit and receive interrupts
 * are not really serviced by this routine (don't know what data the application
 * would want to transmit and don't know where the application wants receive
 * data to be stored).
 *
 * Parameters: 
 * channel      UART channel that triggered the interrupt
 * 
 */
void uart_status_handler(UART_MemMapPtr channel)
{
    uint8 status, temp;
    
    /* Since the flag clearing mechanism will clear any flags in S1
     * that are set, we have to save off the value of the status register
     * and then check against the saved value to be able to detect all of
     * the flags that were set (if you read the status register over and 
     * over again, then you'll only capture the first one that was set.
     */
    
    /* Read and save the S1 value */
    status = UART_S1_REG(channel);
    
    printf("\nS1= 0x%02x",status);
    
    
    /* Determine the source of the interrupt */

    /* Check to see if the transmit data register empty flag is set */
    if (status & UART_S1_TDRE_MASK)
    {
        printf("\nUART transmit data register empty.\n");
        num_tdre_int++;

        /* Disable the tx interrupt in the module */
        /* Without having data to load in the buffer, this is the only way
         * to prevent the interrupt from occurring continuously.
         */
        UART_C2_REG(channel) &= ~UART_C2_TIE_MASK;
    }
  
    /* Check to see if the transmit complete flag is set */
    if (status & UART_S1_TC_MASK)
    {
        printf("\nUART transmit complete.\n");
        num_tc_int++;

        /* Disable the tx complete interrupt in the module */
        /* Without having data to load in the buffer, this is the only way
         * to prevent the interrupt from occurring continuously.
         */
        UART_C2_REG(channel) &= ~UART_C2_TCIE_MASK;
    }
  
    /* Check to see if the rx full flag is set */
    if (status & UART_S1_RDRF_MASK)
    {
        printf("\nUART receive data register full.\n");
        num_rdrf_int++;

        /* Read data register to clear the flag */
        temp = UART_D_REG(channel);
        
        /* Print out the character received (useful for low power mode tests) */
        out_char(temp);
    }

    /* Check to see if the idle line flag is set */
    if (status & UART_S1_IDLE_MASK)
    {
        printf("\nUART idle line detected.\n");
        num_idle_int++;

        /* Read data register to clear the flag */
        temp = UART_D_REG(channel);
    }

    /* Check to see if the LIN break detect flag is set */
    if (UART_S2_REG(channel) & UART_S2_LBKDIF_MASK)
    {
        printf("\nUART LIN break detected.\n");
        num_lin_breaks++;

        /* Write 1 to flag to clear the flag */
        UART_S2_REG(channel) = UART_S2_LBKDIF_MASK;
    }
    
    /* Check to see if the RxD active edge flag is set */
    if (UART_S2_REG(channel) & UART_S2_RXEDGIF_MASK)
    {
        printf("\nUART RXD active edge detected.\n");
        num_edge_int++;

        /* Write 1 to flag to clear the flag */
        UART_S2_REG(channel) = UART_S2_RXEDGIF_MASK;
    }
}  
/********************************************************************/  
/* Generic UART error interrupt handler.
 * This function is called by the specific UART interupt handler routines,
 * so it isn't technically an interrupt handler. The individual interrupt
 * handlers will call this function immediately, so the real interrupt
 * handling is happening in this function. This function will check all 
 * possible interrupt sources, so if there are multiple error conditions
 * they will all be handled.
 *
 * Parameters: 
 * channel      UART channel that triggered the interrupt
 * 
 */
void uart_error_handler(UART_MemMapPtr channel)
{  
    volatile uint8 status, temp;
  
    /* Since the flag clearing mechanism will clear any flags in S1
     * that are set, we have to save off the value of the status register
     * and then check against the saved value to be able to detect all of
     * the flags that were set (if you read the status register over and 
     * over again, then you'll only capture the first one that was set.
     */
    
    /* Read and save the S1 value */
    status = UART_S1_REG(channel);
  
    /* Check to see if a receiver overrun has been detected */
    if (status & UART_S1_OR_MASK)
    {
        printf("\nUART receiver overrun detected.\n");
        num_or_errors++;
        
        /* Read data register to clear the flag */
        temp = UART_D_REG(channel);
    }
    
    /* Check to see if the noise flag is set */
    if (status & UART_S1_NF_MASK)
    {
        printf("\nUART noise error detected.\n");
        num_nf_errors++;
        
        /* Read data register to clear the flag */
        temp = UART_D_REG(channel);
    }
    
    /* Check to see if a framing error was detected */
    if (status & UART_S1_FE_MASK)
    {
        printf("\nUART framing error detected.\n");
        num_framing_errors++;
     
        /* Read data register to clear the flag */
        temp = UART_D_REG(channel);
    }
    
    /* Check to see if a parity error was detected */
    if (status & UART_S1_PF_MASK)
    {
        printf("\nUART parity error detected.\n");
        num_parity_errors++;

        /* Read data register to clear the flag */
        temp = UART_D_REG(channel);
    }

    /* Check to see if a transmit buffer overflow was detected */
    if (UART_SFIFO_REG(channel) & UART_SFIFO_TXOF_MASK)
    {
        printf("\nUART transmit buffer overflow detected.\n");
        num_txof_errors++;

        /* Write 1 to flag to clear the flag */
        UART_SFIFO_REG(channel) = UART_SFIFO_TXOF_MASK;
    }

    /* Check to see if a receiver underflow was detected */
    if (UART_SFIFO_REG(channel) & UART_SFIFO_RXUF_MASK)
    {
        printf("\nUART receiver buffer underflow detected.\n");
        num_rxuf_errors++;

        /* Write 1 to flag to clear the flag */
        UART_SFIFO_REG(channel) = UART_SFIFO_RXUF_MASK;
     }
}    
/********************************************************************/  
/* UART0 status interrupt handler.
 * Display message stating handler has been entered, then jumps to generic
 * UART status interrupt handler.
 */
void uart0_status_isr(void)
{
   printf("\n****UART0 Status Interrupt Handler*****\n");
   uart_status_handler(UART0_BASE_PTR);
   return;
}
/********************************************************************/  
/* UART0 error interrupt handler.
 * Display message stating handler has been entered, then jumps to generic
 * UART error interrupt handler.
 */
void uart0_error_isr(void)
{
   printf("\n****UART0 Error Interrupt Handler*****\n");
   uart_error_handler(UART0_BASE_PTR);
   return;
}
/********************************************************************/  
/* UART1 status interrupt handler.
 * Display message stating handler has been entered, then jumps to generic
 * UART status interrupt handler.
 */
void uart1_status_isr(void)
{
   printf("\n****UART1 Status Interrupt Handler*****\n");
   uart_status_handler(UART1_BASE_PTR);
   return;
}
/********************************************************************/  
/* UART1 error interrupt handler.
 * Display message stating handler has been entered, then jumps to generic
 * UART error interrupt handler.
 */
void uart1_error_isr(void)
{
   printf("\n****UART1 Error Interrupt Handler*****\n");
   uart_error_handler(UART1_BASE_PTR);
   return;
}
/********************************************************************/  
/* UART2 status interrupt handler.
 * Display message stating handler has been entered, then jumps to generic
 * UART status interrupt handler.
 */
void uart2_status_isr(void)
{
   printf("\n****UART2 Status Interrupt Handler*****\n");
   uart_status_handler(UART2_BASE_PTR);
   return;
}
/********************************************************************/  
/* UART2 error interrupt handler.
 * Display message stating handler has been entered, then jumps to generic
 * UART error interrupt handler.
 */
void uart2_error_isr(void)
{
   printf("\n****UART2 Error Interrupt Handler*****\n");
   uart_error_handler(UART2_BASE_PTR);
   return;
}
/********************************************************************/
/* UART0 IS0-7816 error interrupt handler.
 * When using 7816 mode this interrupt handler should be used instead
 * of the regular uart0_error_isr.
 */
void uart0_7816_error_isr(void)
{
     printf("\n****UART0 ISO-7816 Error Interrupt Handler*****\n");

    /* Check to see if a 7816 transmit threshold exceeded error detected */
    if (UART0_IS7816 & UART_IS7816_TXT_MASK)
    {
        printf("\nUART ISO-7816 transmit threshold exceeded limit.\n");

        /* Write 1 to flag to clear the flag */
        UART0_IS7816 = UART_IS7816_TXT_MASK;
    }

    /* Check to see if a 7816 receive threshold exceeded error detected */
    if (UART0_IS7816 & UART_IS7816_RXT_MASK)
    {
        printf("\nUART ISO-7816 receive threshold exceeded limit.\n");

        /* Write 1 to flag to clear the flag */
        UART0_IS7816 = UART_IS7816_RXT_MASK;
    }

    /* Check to see if a 7816 wait timer interrupt was detected */
    if (UART0_IS7816 & UART_IS7816_WT_MASK)
    {
        printf("\nUART ISO-7816 wait timer interrupt.\n");

        /* Write 1 to flag to clear the flag */
        UART0_IS7816 = UART_IS7816_WT_MASK;
    }

    /* Check to see if a 7816 character wait timer interrupt was detected */
    if (UART0_IS7816 & UART_IS7816_CWT_MASK)
    {
        printf("\nUART ISO-7816 character wait timer interrupt.\n");

        /* Write 1 to flag to clear the flag */
        UART0_IS7816 = UART_IS7816_CWT_MASK;
    }
    
    /* Check to see if a 7816 block wait timer interrupt was detected */
    if (UART0_IS7816 & UART_IS7816_BWT_MASK)
    {
        printf("\nUART ISO-7816 block wait timer interrupt.\n");

        /* Write 1 to flag to clear the flag */
        UART0_IS7816 = UART_IS7816_BWT_MASK;
    }

    /* Check to see if a 7816 character guard timer interrupt was detected */
    if (UART0_IS7816 & UART_IS7816_GTV_MASK)
    {
        printf("\nUART ISO-7816 guard timer interrupt.\n");

        /* Write 1 to flag to clear the flag */
        UART0_IS7816 = UART_IS7816_GTV_MASK;
    }   
   return;
}
/********************************************************************/
/* UART0 IS0-7816 status interrupt handler.
 * When using 7816 mode this interrupt handler should be used instead
 * of the regular uart0_status_isr.
 */
void uart0_7816_status_isr(void)
{
    printf("\n****UART0 ISO-7816 Status Interrupt Handler*****\n");
     
    /* Check to see if the 7816 initial character detect flag is set */
    if (UART0_IS7816 & UART_IS7816_INITD_MASK)
    {
        printf("\nUART ISO-7816 initial character detected.\n");

        /* Write 1 to flag to clear the flag */
        UART0_IS7816 = UART_IS7816_INITD_MASK;
    }
   return;
}
/********************************************************************/  
/* Generic UART LON interrupt handler.
 * This function will check all possible interrupt sources, so if 
 * there are multiple error conditions they will all be handled.
 */
void uart0_lon_isr(void)
{  

//****    printf("\n****UART0 LON Interrupt Handler*****\n");
     
    /* Check to see if the Wbase expired flag is set */
    if (UART0_S3 & UART_S3_WBEF_MASK)
    {
        /* Toggle GPIO used for Wbase timer test high */
        GPIOA_PDOR |= 0x2000; // Drive pin high

//        printf("\nUART LON Wbase expired flag detected.\n");

        /* Write 1 to flag to clear the flag */
        UART0_S3 = UART_S3_WBEF_MASK;
    }
    
    /* Check to see if the Initial sync detect flag is set */
    if (UART0_S3 & UART_S3_ISD_MASK)
    {
//****        printf("\nUART LON initial sync detected.\n");
        num_lon_isd_int++;

        /* The ISD flag cannot be cleared, so disable the interrupt
         * in order to avoid getting continuous interrupts from this
         * point on.
         */
        UART0_IE &= ~UART_IE_ISDIE_MASK;
    }
    
    /* Check to see if the packet received flag is set */
    if (UART0_S3 & UART_S3_PRXF_MASK)
    {        
//****        printf("\nUART LON packet received flag detected.\n");
        num_lon_rx_int++;

        /* Write 1 to flag to clear the flag */
        UART0_S3 = UART_S3_PRXF_MASK;
        GPIOA_PTOR |= (1<<13); // Drive pin high//MG
    }

    /* Check to see if the packet transmitted flag is set */
    if (UART0_S3 & UART_S3_PTXF_MASK)
    {
        /* Disable transmit to prevent a second packet from attempting to go out */
        UART0_C6 &= ~UART_C6_TX709_MASK;
    
//****        printf("\nUART LON packet transmitted flag detected.\n");
        num_lon_tx_int++;

        /* Write 1 to flag to clear the flag */
        UART0_S3 = UART_S3_PTXF_MASK;
    }
  
    /* Check to see if the packet cycle timer expired flag is set */
    if (UART0_S3 & UART_S3_PCTEF_MASK)
    {
        /* Toggle GPIO used for PCT timer test high */
        GPIOD_PDOR |= 2; // Drive pin high
      
//        printf("\nUART LON packet cycle timer expired flag detected.\n");

        /* Write 1 to flag to clear the flag */
        UART0_S3 = UART_S3_PCTEF_MASK;
    }
  
    /* Check to see if the preamble start flag is set */
    if (UART0_S3 & UART_S3_PSF_MASK)
    {
//****        printf("\nUART LON preamble transmit flag detected.\n");
        num_lon_psf_int++;

        /* Write 1 to flag to clear the flag */
        UART0_S3 = UART_S3_PSF_MASK;
    }
    
    /* Check to see if the transmit fail flag is set */
    if (UART0_S3 & UART_S3_TXFF_MASK)
    {
//****        printf("\nUART LON transmit failed flag detected.\n");
        num_lon_txf_int++;

        /* Write 1 to flag to clear the flag */
        UART0_S3 = UART_S3_TXFF_MASK;
    }
    
}
/******************************************************************************/
/* Generic interrupt handler for the PTA4 GPIO interrupt connected to an 
 * abort switch. 
 * NOTE: For true abort operation this handler should be modified
 * to jump to the main process instead of executing a return.
 */
void abort_isr(void)
{
   /* Write 1 to the PTA4 interrupt flag bit to clear the interrupt */
   PORTA_PCR4 = PORT_PCR_ISF_MASK;    
  
   printf("\n****Abort button interrupt****\n\n");
   return;
}
/******************************************************************************/   
