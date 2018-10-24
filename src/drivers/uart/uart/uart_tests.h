/*
 * File:		uart_tests.h
 * Purpose:		header file for Kinetis uart tests
 *
 */

#include "uart.h"

/* function prototypes */

/* uart register tests */
int uart_reg_rst(void);
int uart_reg_rw(uint8 reg_val);

/* uart basic test */
int uart_basic_test(void);

/* uart baud rate tests */
int uart_sbr_test(void);
int uart_brfa_test(void);

/* uart tx tests */
int uart_msbf_test(void);
int uart_9bit_tx_test(void);
int uart_10bit_tx_test(void);
int uart_parity_test(void);
int uart_9bit_parity_test(void);
int uart_txinv_test(void);
int uart_break_char_test(void);
int uart_idle_char_test(void);
int uart_tx_disable_test(void);

/* uart rx tests */
int uart_8bit_rx_test(void);
int uart_9bit_rx_test(void);
int uart_10bit_rx_test(void);
int uart_9_bit_maen_test(void);
int uart_8_bit_maen_test(void);
int uart_rx_wakeup_test(void);
int uart_idle_type_test(void);

/* uart tx interrupt tests */
int uart_txof_error_test(void);
int uart_tc_interrupt_test(void);
int uart_tdre_interrupt_test(void);

/* uart rx interrupt tests */
int uart_rdrf_interrupt_test(void);
int uart_parity_error_test(void);
int uart_or_error_test(void);
int uart_nf_error_test(void);
int uart_rxuf_error_test(void);
int uart_framing_error_test(void);
int uart_idle_interrupt_test(void);
int uart_lin_break_test(void);

/* uart dma tests */
int uart_tdre_dma_test(void);
int uart_rdrf_dma_test(void);

/* uart flow control tests */
int uart_flow_control_test(void);
int uart_txrts_test(void);

/* uart fifo tests */
int uart_fifo_size_test(void);
int uart_fifo_flush_test(void);
int uart_txempt_rxempt_test(void);
int uart_fifo_wm_test(void);
int uart_fifo_paritye_test(void);

/* uart loop mode tests */
int uart_loop_mode_test(void);
int uart_single_wire_test(void);

/* uart low power mode tests */
int uart_wait_mode_test(void);
int uart_wait_mode_disable_test(void);
int uart_stop_mode_test(void);
int uart_vlpr_mode_test(void);
int uart_vlpr_mode_test2(void);
int uart_vlpw_mode_test(void);
int uart_vlps_mode_test(void);
int uart_lls_mode_test(void);
int uart_vlls_mode_test(void);

/* uart pin mux tests */
int uart0_pin_mux_test(void);
int uart1_pin_mux_test(void);
int uart3_pin_mux_test(void);
int uart4_pin_mux_test(void);
int uart5_pin_mux_test(void);
int uart2_pin_mux_test(void); /* This test uses a different TERM_PORT */
int uart_function_test(UART_MemMapPtr, UART_MemMapPtr);

/* uart irda mode tests */
int uart_irda_tx_test(void);
int uart_irda_rx_test(void);
int uart_irda_rxinv_test(void);

/* uart LON mode tests */
int uart_lon_basic_tx_test(UART_MemMapPtr );
int uart_lon_preamble_tx_test(UART_MemMapPtr );
int uart_lon_tx_int_test(UART_MemMapPtr );
int uart_lon_col_test(UART_MemMapPtr );
int uart_lon_basic_rx_test(UART_MemMapPtr );
int uart_lon_rx_int_test(UART_MemMapPtr );
int uart_lon_tx_len_test(UART_MemMapPtr );
int uart_lon_rx_len_test(UART_MemMapPtr );
int uart_lon_preamble_rx_test(UART_MemMapPtr );
int uart_lon_isd_int_test(UART_MemMapPtr );
int uart_lon_psf_int_test(UART_MemMapPtr );
int uart_lon_pre_err_test(UART_MemMapPtr );
int uart_lon_beta1_test(UART_MemMapPtr );
int uart_lon_wbase_test(UART_MemMapPtr );
int uart_lon_sdt_test(UART_MemMapPtr );
int uart_lon_pct_test(UART_MemMapPtr );
int uart_lon_wbase_test(UART_MemMapPtr );
int uart_lon_txf_int_test(UART_MemMapPtr );
int uart_lon_ridt_test(UART_MemMapPtr );
int uart_lon_tidt_test_dut(UART_MemMapPtr );
int uart_lon_tidt_test_secondary(UART_MemMapPtr );



















