/*
 * File:		uart_tests_main.c
 * Purpose:		Main process for calling all UART tests.
 *
 */

#include "common.h"
#include "uart.h"
#include "mcg.h"
#include "uart_tests.h"

/* Globals from sysinit.c */
extern int core_clk_khz;
extern int core_clk_mhz;
extern int periph_clk_khz;

/********************************************************************/
int main (void)
{
	char ch;
        int test_failed;
        
  	printf("\nRunning the UART test project\n\n");
        
        /* Make sure the clocks are enabled to all UARTs */
       	SIM_SCGC4 |= (  SIM_SCGC4_UART0_MASK
                      | SIM_SCGC4_UART1_MASK
                      | SIM_SCGC4_UART2_MASK);
        
        /*uart0*/
        /* Disable the UART0_TXD function on PTD6 */
        //PORTD_PCR6 = PORT_PCR_MUX(0x0); // UART is alt3 function for this pin
        /* Disable the UART0_RXD function on PTD7 */
        //PORTD_PCR7 = PORT_PCR_MUX(0x0); // UART is alt3 function for this pin
        /*uart1*/
        /* Disable the UART1_TXD function on PTE0 (1) */
        //PORTE_PCR0 = PORT_PCR_MUX(0x0); // UART is alt3 function for this pin
        /* Disable the UART1_RXD function on PTE1 (2) */
        //PORTE_PCR1 = PORT_PCR_MUX(0x0); // UART is alt3 function for this pin 
        /*uart2*/
        /* Disable the UART2_TXD function on PTD3 */
        //PORTD_PCR3 = PORT_PCR_MUX(0x0); // UART is alt3 function for this pin
        /* Disable the UART2_RXD function on PTD2 */
        //PORTD_PCR2 = PORT_PCR_MUX(0x0); // UART is alt3 function for this pin
        
/*UART0 pins*/       
        /* Enable a full set of UART signals for each of the UART modules */
        #if 0 
        /* Enable UART0 functions on PTA */
        PORTA_PCR0 = PORT_PCR_MUX(0x2); // UART is alt2 function for this pin: CTS_COL: shared with JTAG in cJTAG
        PORTA_PCR1 = PORT_PCR_MUX(0x2); // UART is alt2 function for this pin: RX: shared with JTAG in SWD/cJTAG
        PORTA_PCR2 = PORT_PCR_MUX(0x2); // UART is alt2 function for this pin: TX: shared with JTAG in SWD/cJTAG
        PORTA_PCR3 = PORT_PCR_MUX(0x2); // UART is alt2 function for this pin: RTS: shared with JTAG in cJTAG
        #elif 0
        /* Enable UART0 functions on PTB */
        PORTB_PCR2 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin: RTS
        PORTB_PCR3 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin: CTS_COL
        PORTB_PCR16= PORT_PCR_MUX(0x3); // UART is alt3 function for this pin: RX ---> used for LON!
        PORTB_PCR17= PORT_PCR_MUX(0x3); // UART is alt3 function for this pin: TX ---> used for LON!
        #elif 1        
        /* Enable UART0 functions on PTD */
        //PORTD_PCR4 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin: RTS
        //PORTD_PCR5 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin: CTS_COL
        PORTD_PCR6 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin: RX
        PORTD_PCR7 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin: TX
        #else
        #error "Select a valid UART set of pins"
        #endif  
        
/*UART1 pins*/
        /* Enable the UART1 functions on PTC */
        //****PORTC_PCR1 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin: RTS
        //****PORTC_PCR2 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin: CTS
        #if 0   /*first set for xmit and rx*/
        PORTC_PCR3 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin: RX
        PORTC_PCR4 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin: TX
        #else        
        PORTE_PCR0 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin: TX
        PORTE_PCR1 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin: RX
        #endif
        
/*UART2 pins*/
        /* Enable the UART2 functions on PTD */
        //****PORTD_PCR0 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin: RTS
        //****PORTD_PCR1 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin: CTS
        PORTD_PCR2 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin: RX
  	PORTD_PCR3 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin: TX
        
        
        
        /* Enable the ABORT button IRQ, this is needed for some of the low
         * power mode tests. 
         *
        * NOTE: J35: 5-6, J16 set, J14: 2-3, J19: 2-3
         */
        /* Configure the PTA4 pin for its GPIO function */
        //PORTA_PCR4 = PORT_PCR_MUX(0x1); // GPIO is alt1 function for this pin
    
        /* Configure the PTA4 pin for rising edge interrupts */
        //PORTA_PCR4 |= PORT_PCR_IRQC(0x9); 
    
        /* Enable the associated IRQ in the NVIC */
        //enable_irq(40);


 	/* initialize number of failed tests counter to zero */
	test_failed = 0;
#if 0
        uart_init (UART0_BASE_PTR, core_clk_khz,TERMINAL_BAUD/*78000*/);
        uart_init (UART1_BASE_PTR, core_clk_khz,TERMINAL_BAUD); 
        uart_init (UART2_BASE_PTR, periph_clk_khz,TERMINAL_BAUD); 
        
        /* Run UART tests */
        for(;;)
        {
         uart_putchar(UART2_BASE_PTR,'x'); 
        }
        //uart_putchar(UART2_BASE_PTR, 'x');
        //ch = uart_getchar(UART2_BASE_PTR);
        //uart_putchar(UART2_BASE_PTR, ch);
#else  

        /* UART register tests */
        test_failed += uart_reg_rst();
        
        printf("1st Test completed\n\r");
         
        /* Call register read/write test with different register values to test.
         * The UART module doesn't have a software reset bit to allow for easily
         * resetting the UART after this test. Since this test changes register
         * values from the defaults and would impact subsequent tests, the 
         * sci_reg_rw test should only be run on its own. These lines should be
         * commented out when running other tests. 
         */
          //test_failed += uart_reg_rw(0x00); //MG PASS
          //test_failed += uart_reg_rw(0xFF); //MG PASS
          //test_failed += uart_reg_rw(0x5A); //MG PASS

        /* This is the start of the functional tests, so stop and initialize
          * all of the UARTs to a default state.
          */
        uart_init (UART0_BASE_PTR, core_clk_khz,TERMINAL_BAUD);
        uart_init (UART1_BASE_PTR, core_clk_khz,TERMINAL_BAUD); 
        uart_init (UART2_BASE_PTR, periph_clk_khz,TERMINAL_BAUD);        
#endif
        
        /* UART basic test */
        //test_failed += uart_basic_test();//OK
 
        /* UART baud rate tests */
        //test_failed += uart_sbr_test();  
 
        /* UART transmit tests - Use scope to verify */
//        test_failed += uart_msbf_test();
//        test_failed += uart_txinv_test();
//        test_failed += uart_9bit_tx_test();   
//        test_failed += uart_10bit_tx_test();             
//        test_failed += uart_parity_test();     
//        test_failed += uart_9bit_parity_test();  
//        test_failed += uart_break_char_test();
//        test_failed += uart_idle_char_test();
//        test_failed += uart_tx_disable_test();
        
        /* UART LON tests */
//        test_failed += uart_lon_basic_tx_test(UART0_BASE_PTR);//OK
//        test_failed += uart_lon_preamble_tx_test(UART0_BASE_PTR);//OK
//        test_failed += uart_lon_preamble_rx_test(UART0_BASE_PTR);//OK
//        test_failed += uart_lon_tx_int_test(UART0_BASE_PTR);//OK
//        test_failed += uart_lon_col_test(UART0_BASE_PTR);//OK MG                 
//        test_failed += uart_lon_basic_rx_test(UART0_BASE_PTR);//OK MG
//        test_failed += uart_lon_rx_int_test(UART0_BASE_PTR);//OK MG
//        test_failed += uart_lon_tx_len_test(UART0_BASE_PTR);//OK
//        test_failed += uart_lon_rx_len_test(UART0_BASE_PTR);//OK
//        test_failed += uart_lon_isd_int_test(UART0_BASE_PTR);//OK MG
//        test_failed += uart_lon_psf_int_test(UART0_BASE_PTR);//OK
//        test_failed += uart_lon_pre_err_test(UART0_BASE_PTR);//OK MG
//        test_failed += uart_lon_beta1_test(UART0_BASE_PTR);//OK MG
//        test_failed += uart_lon_sdt_test(UART0_BASE_PTR);//OK MG
//        test_failed += uart_lon_pct_test(UART0_BASE_PTR);//OK MG
//        test_failed += uart_lon_wbase_test(UART0_BASE_PTR);//OK MG
//        test_failed += uart_lon_txf_int_test(UART0_BASE_PTR);//OK
//        test_failed += uart_lon_ridt_test(UART0_BASE_PTR);// OK MG

        
        /* This test needs UART0 and UART1, so code needs to be compiled to 
         * use a different TERM_PORT when running on the Firebird since UART1
         * is the default TERM_PORT */
//        test_failed += uart_rx_wakeup_test();   // MG Test need to be modified because P0 has only 1 uart with 8 FIFOs.. 

        /* UART transmit interrupt tests */
        //test_failed += uart_txof_error_test();     //OK with extra underflow errors
        //test_failed += uart_tc_interrupt_test();   //OK
        //test_failed += uart_tdre_interrupt_test(); //OK
       
        /* UART receive interrupt tests */
        //test_failed += uart_rdrf_interrupt_test();   //OK 
        //test_failed += uart_parity_error_test();     //OK
        //test_failed += uart_or_error_test();         //OK
        //test_failed += uart_rxuf_error_test();       //OK
//        test_failed += uart_nf_error_test();     // Do not uncomment this test - it isn't working yet
        //test_failed += uart_framing_error_test();    //OK
        //test_failed += uart_idle_interrupt_test();   //OK     
        //test_failed += uart_lin_break_test();        //Pending 12-bit BREAK
        
        /* UART DMA tests */
        //test_failed += uart_tdre_dma_test();        //OK
        //test_failed += uart_rdrf_dma_test();        //OK
       
        /* UART flow control tests */
        //test_failed += uart_flow_control_test();    //Not run: Kone project solved this one
//      //test_failed += uart_txrts_test();           //Not run: Kone project solved this one

        /* UART loop mode tests */
        //test_failed += uart_loop_mode_test();          //OK
       
        /* UART low power mode tests */
        /* These tests should all be run individually and need to 
         * execute from the flash. Make sure to power cycle the 
         * processor after removing the debugger to ensure the part
         * actually enters the correct low power mode.
         */
//        test_failed += uart_wait_mode_test();             //OK
//        test_failed += uart_wait_mode_disable_test();     //OK
//        test_failed += uart_stop_mode_test();             //OK
//        test_failed += uart_vlpr_mode_test();             //OK
//        test_failed += uart_vlpr_mode_test2();            //OK
        test_failed += uart_vlpw_mode_test();               //OK
//        test_failed += uart_vlps_mode_test();             //OK
//        test_failed += uart_lls_mode_test();              //Ok
//        test_failed += uart_vlls_mode_test();             //OK
        
        /* UART pin multiplexing tests */
          //test_failed += uart0_pin_mux_test();    // MG PASS
          //test_failed += uart1_pin_mux_test();    // MG PASS 
        
          /* This test doesn't use the default TERM_PORT, so it must always
         * be run on it's own. See function for directions.
         */
          //test_failed += uart2_pin_mux_test();    // MG PASS
        
          /* UART IRDA mode tests */
        //test_failed += uart_irda_tx_test();       //OK
        //test_failed += uart_irda_rx_test();       //OK
        
        
	if (test_failed)
	{
		printf("\n\nTEST FAILED!!!\n");
		printf("\n\n%d test(s) failed.\n", test_failed);
	}
	else
		printf("\n\nAll tests PASS!\n\n");

	while(1)
	{
		ch = in_char();
		out_char(ch);
	} 
}
/********************************************************************/
