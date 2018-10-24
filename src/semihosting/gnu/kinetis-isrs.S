/* ISRs for kinetis
 *
 * Version: Sourcery G++ 2010.09-66
 * Support: https://support.codesourcery.com/GNUToolchain/
 *
 * Copyright (c) 2007, 2008, 2009, 2010 CodeSourcery, Inc.
 *
 * The authors hereby grant permission to use, copy, modify, distribute,
 * and license this software and its documentation for any purpose, provided
 * that existing copyright notices are retained in all copies and that this
 * notice is included verbatim in any distributions.  No written agreement,
 * license, or royalty fee is required for any of the authorized uses.
 * Modifications to this software may be copyrighted by their authors
 * and need not follow the licensing terms described here, provided that
 * the new terms are clearly indicated on the first page of each file where
 * they apply.
 */
	.thumb

#if defined (L_kinetis_isr_interrupt)
	.globl	__cs3_isr_interrupt
	.type	__cs3_isr_interrupt, %function
__cs3_isr_interrupt:
	b .
	.size	__cs3_isr_interrupt, . - __cs3_isr_interrupt

	.weak	__cs3_isr_nmi
	.globl	__cs3_isr_nmi
	.set	__cs3_isr_nmi, __cs3_isr_interrupt
	.weak	__cs3_isr_hard_fault
	.globl	__cs3_isr_hard_fault
	.set	__cs3_isr_hard_fault, __cs3_isr_interrupt
	.weak	__cs3_isr_mpu_fault
	.globl	__cs3_isr_mpu_fault
	.set	__cs3_isr_mpu_fault, __cs3_isr_interrupt
	.weak	__cs3_isr_bus_fault
	.globl	__cs3_isr_bus_fault
	.set	__cs3_isr_bus_fault, __cs3_isr_interrupt
	.weak	__cs3_isr_usage_fault
	.globl	__cs3_isr_usage_fault
	.set	__cs3_isr_usage_fault, __cs3_isr_interrupt
	.weak	__cs3_isr_reserved_7
	.globl	__cs3_isr_reserved_7
	.set	__cs3_isr_reserved_7, __cs3_isr_interrupt
	.weak	__cs3_isr_reserved_8
	.globl	__cs3_isr_reserved_8
	.set	__cs3_isr_reserved_8, __cs3_isr_interrupt
	.weak	__cs3_isr_reserved_9
	.globl	__cs3_isr_reserved_9
	.set	__cs3_isr_reserved_9, __cs3_isr_interrupt
	.weak	__cs3_isr_reserved_10
	.globl	__cs3_isr_reserved_10
	.set	__cs3_isr_reserved_10, __cs3_isr_interrupt
	.weak	__cs3_isr_svcall
	.globl	__cs3_isr_svcall
	.set	__cs3_isr_svcall, __cs3_isr_interrupt
	.weak	__cs3_isr_debug
	.globl	__cs3_isr_debug
	.set	__cs3_isr_debug, __cs3_isr_interrupt
	.weak	__cs3_isr_reserved_13
	.globl	__cs3_isr_reserved_13
	.set	__cs3_isr_reserved_13, __cs3_isr_interrupt
	.weak	__cs3_isr_pendsv
	.globl	__cs3_isr_pendsv
	.set	__cs3_isr_pendsv, __cs3_isr_interrupt
	.weak	__cs3_isr_systick
	.globl	__cs3_isr_systick
	.set	__cs3_isr_systick, __cs3_isr_interrupt
	.weak	__cs3_isr_dma0
	.globl	__cs3_isr_dma0
	.set	__cs3_isr_dma0, __cs3_isr_interrupt
	.weak	__cs3_isr_dma1
	.globl	__cs3_isr_dma1
	.set	__cs3_isr_dma1, __cs3_isr_interrupt
	.weak	__cs3_isr_dma2
	.globl	__cs3_isr_dma2
	.set	__cs3_isr_dma2, __cs3_isr_interrupt
	.weak	__cs3_isr_dma3
	.globl	__cs3_isr_dma3
	.set	__cs3_isr_dma3, __cs3_isr_interrupt
	.weak	__cs3_isr_dma4
	.globl	__cs3_isr_dma4
	.set	__cs3_isr_dma4, __cs3_isr_interrupt
	.weak	__cs3_isr_dma5
	.globl	__cs3_isr_dma5
	.set	__cs3_isr_dma5, __cs3_isr_interrupt
	.weak	__cs3_isr_dma6
	.globl	__cs3_isr_dma6
	.set	__cs3_isr_dma6, __cs3_isr_interrupt
	.weak	__cs3_isr_dma7
	.globl	__cs3_isr_dma7
	.set	__cs3_isr_dma7, __cs3_isr_interrupt
	.weak	__cs3_isr_dma8
	.globl	__cs3_isr_dma8
	.set	__cs3_isr_dma8, __cs3_isr_interrupt
	.weak	__cs3_isr_dma9
	.globl	__cs3_isr_dma9
	.set	__cs3_isr_dma9, __cs3_isr_interrupt
	.weak	__cs3_isr_dma10
	.globl	__cs3_isr_dma10
	.set	__cs3_isr_dma10, __cs3_isr_interrupt
	.weak	__cs3_isr_dma11
	.globl	__cs3_isr_dma11
	.set	__cs3_isr_dma11, __cs3_isr_interrupt
	.weak	__cs3_isr_dma12
	.globl	__cs3_isr_dma12
	.set	__cs3_isr_dma12, __cs3_isr_interrupt
	.weak	__cs3_isr_dma13
	.globl	__cs3_isr_dma13
	.set	__cs3_isr_dma13, __cs3_isr_interrupt
	.weak	__cs3_isr_dma14
	.globl	__cs3_isr_dma14
	.set	__cs3_isr_dma14, __cs3_isr_interrupt
	.weak	__cs3_isr_dma15
	.globl	__cs3_isr_dma15
	.set	__cs3_isr_dma15, __cs3_isr_interrupt
	.weak	__cs3_isr_dma_error
	.globl	__cs3_isr_dma_error
	.set	__cs3_isr_dma_error, __cs3_isr_interrupt
	.weak	__cs3_isr_mcm
	.globl	__cs3_isr_mcm
	.set	__cs3_isr_mcm, __cs3_isr_interrupt
	.weak	__cs3_isr_flash_command_complete
	.globl	__cs3_isr_flash_command_complete
	.set	__cs3_isr_flash_command_complete, __cs3_isr_interrupt
	.weak	__cs3_isr_flash_read_collision
	.globl	__cs3_isr_flash_read_collision
	.set	__cs3_isr_flash_read_collision, __cs3_isr_interrupt
	.weak	__cs3_isr_mode_controller
	.globl	__cs3_isr_mode_controller
	.set	__cs3_isr_mode_controller, __cs3_isr_interrupt
	.weak	__cs3_isr_llwu
	.globl	__cs3_isr_llwu
	.set	__cs3_isr_llwu, __cs3_isr_interrupt
	.weak	__cs3_isr_wdog
	.globl	__cs3_isr_wdog
	.set	__cs3_isr_wdog, __cs3_isr_interrupt
	.weak	__cs3_isr_rngb
	.globl	__cs3_isr_rngb
	.set	__cs3_isr_rngb, __cs3_isr_interrupt
	.weak	__cs3_isr_i2c0
	.globl	__cs3_isr_i2c0
	.set	__cs3_isr_i2c0, __cs3_isr_interrupt
	.weak	__cs3_isr_i2c1
	.globl	__cs3_isr_i2c1
	.set	__cs3_isr_i2c1, __cs3_isr_interrupt
	.weak	__cs3_isr_spi0
	.globl	__cs3_isr_spi0
	.set	__cs3_isr_spi0, __cs3_isr_interrupt
	.weak	__cs3_isr_spi1
	.globl	__cs3_isr_spi1
	.set	__cs3_isr_spi1, __cs3_isr_interrupt
	.weak	__cs3_isr_spi2
	.globl	__cs3_isr_spi2
	.set	__cs3_isr_spi2, __cs3_isr_interrupt
	.weak	__cs3_isr_can0_ored_message_buffer
	.globl	__cs3_isr_can0_ored_message_buffer
	.set	__cs3_isr_can0_ored_message_buffer, __cs3_isr_interrupt
	.weak	__cs3_isr_can0_buf_off
	.globl	__cs3_isr_can0_buf_off
	.set	__cs3_isr_can0_buf_off, __cs3_isr_interrupt
	.weak	__cs3_isr_can0_error
	.globl	__cs3_isr_can0_error
	.set	__cs3_isr_can0_error, __cs3_isr_interrupt
	.weak	__cs3_isr_can0_transmit_warning
	.globl	__cs3_isr_can0_transmit_warning
	.set	__cs3_isr_can0_transmit_warning, __cs3_isr_interrupt
	.weak	__cs3_isr_can0_receive_warning
	.globl	__cs3_isr_can0_receive_warning
	.set	__cs3_isr_can0_receive_warning, __cs3_isr_interrupt
	.weak	__cs3_isr_can0_wake_up
	.globl	__cs3_isr_can0_wake_up
	.set	__cs3_isr_can0_wake_up, __cs3_isr_interrupt
	.weak	__cs3_isr_can0_imeu
	.globl	__cs3_isr_can0_imeu
	.set	__cs3_isr_can0_imeu, __cs3_isr_interrupt
	.weak	__cs3_isr_can0_lost_receive
	.globl	__cs3_isr_can0_lost_receive
	.set	__cs3_isr_can0_lost_receive, __cs3_isr_interrupt
	.weak	__cs3_isr_can1_ored_message_buffer
	.globl	__cs3_isr_can1_ored_message_buffer
	.set	__cs3_isr_can1_ored_message_buffer, __cs3_isr_interrupt
	.weak	__cs3_isr_can1_buf_off
	.globl	__cs3_isr_can1_buf_off
	.set	__cs3_isr_can1_buf_off, __cs3_isr_interrupt
	.weak	__cs3_isr_can1_error
	.globl	__cs3_isr_can1_error
	.set	__cs3_isr_can1_error, __cs3_isr_interrupt
	.weak	__cs3_isr_can1_transmit_warning
	.globl	__cs3_isr_can1_transmit_warning
	.set	__cs3_isr_can1_transmit_warning, __cs3_isr_interrupt
	.weak	__cs3_isr_can1_receive_warning
	.globl	__cs3_isr_can1_receive_warning
	.set	__cs3_isr_can1_receive_warning, __cs3_isr_interrupt
	.weak	__cs3_isr_can1_wake_up
	.globl	__cs3_isr_can1_wake_up
	.set	__cs3_isr_can1_wake_up, __cs3_isr_interrupt
	.weak	__cs3_isr_can1_imeu
	.globl	__cs3_isr_can1_imeu
	.set	__cs3_isr_can1_imeu, __cs3_isr_interrupt
	.weak	__cs3_isr_can1_lost_receive
	.globl	__cs3_isr_can1_lost_receive
	.set	__cs3_isr_can1_lost_receive, __cs3_isr_interrupt
	.weak	__cs3_isr_uart0_status_sources
	.globl	__cs3_isr_uart0_status_sources
	.set	__cs3_isr_uart0_status_sources, __cs3_isr_interrupt
	.weak	__cs3_isr_uart0_error_sources
	.globl	__cs3_isr_uart0_error_sources
	.set	__cs3_isr_uart0_error_sources, __cs3_isr_interrupt
	.weak	__cs3_isr_uart1_status_sources
	.globl	__cs3_isr_uart1_status_sources
	.set	__cs3_isr_uart1_status_sources, __cs3_isr_interrupt
	.weak	__cs3_isr_uart1_error_sources
	.globl	__cs3_isr_uart1_error_sources
	.set	__cs3_isr_uart1_error_sources, __cs3_isr_interrupt
	.weak	__cs3_isr_uart2_status_sources
	.globl	__cs3_isr_uart2_status_sources
	.set	__cs3_isr_uart2_status_sources, __cs3_isr_interrupt
	.weak	__cs3_isr_uart2_error_sources
	.globl	__cs3_isr_uart2_error_sources
	.set	__cs3_isr_uart2_error_sources, __cs3_isr_interrupt
	.weak	__cs3_isr_uart3_status_sources
	.globl	__cs3_isr_uart3_status_sources
	.set	__cs3_isr_uart3_status_sources, __cs3_isr_interrupt
	.weak	__cs3_isr_uart3_error_sources
	.globl	__cs3_isr_uart3_error_sources
	.set	__cs3_isr_uart3_error_sources, __cs3_isr_interrupt
	.weak	__cs3_isr_uart4_status_sources
	.globl	__cs3_isr_uart4_status_sources
	.set	__cs3_isr_uart4_status_sources, __cs3_isr_interrupt
	.weak	__cs3_isr_uart4_error_sources
	.globl	__cs3_isr_uart4_error_sources
	.set	__cs3_isr_uart4_error_sources, __cs3_isr_interrupt
	.weak	__cs3_isr_uart5_status_sources
	.globl	__cs3_isr_uart5_status_sources
	.set	__cs3_isr_uart5_status_sources, __cs3_isr_interrupt
	.weak	__cs3_isr_uart5_error_sources
	.globl	__cs3_isr_uart5_error_sources
	.set	__cs3_isr_uart5_error_sources, __cs3_isr_interrupt
	.weak	__cs3_isr_adc0
	.globl	__cs3_isr_adc0
	.set	__cs3_isr_adc0, __cs3_isr_interrupt
	.weak	__cs3_isr_adc1
	.globl	__cs3_isr_adc1
	.set	__cs3_isr_adc1, __cs3_isr_interrupt
	.weak	__cs3_isr_cmp0
	.globl	__cs3_isr_cmp0
	.set	__cs3_isr_cmp0, __cs3_isr_interrupt
	.weak	__cs3_isr_cmp1
	.globl	__cs3_isr_cmp1
	.set	__cs3_isr_cmp1, __cs3_isr_interrupt
	.weak	__cs3_isr_cmp2
	.globl	__cs3_isr_cmp2
	.set	__cs3_isr_cmp2, __cs3_isr_interrupt
	.weak	__cs3_isr_ftm0
	.globl	__cs3_isr_ftm0
	.set	__cs3_isr_ftm0, __cs3_isr_interrupt
	.weak	__cs3_isr_ftm1
	.globl	__cs3_isr_ftm1
	.set	__cs3_isr_ftm1, __cs3_isr_interrupt
	.weak	__cs3_isr_ftm2
	.globl	__cs3_isr_ftm2
	.set	__cs3_isr_ftm2, __cs3_isr_interrupt
	.weak	__cs3_isr_cmt
	.globl	__cs3_isr_cmt
	.set	__cs3_isr_cmt, __cs3_isr_interrupt
	.weak	__cs3_isr_rtc
	.globl	__cs3_isr_rtc
	.set	__cs3_isr_rtc, __cs3_isr_interrupt
	.weak	__cs3_isr_reserved_83
	.globl	__cs3_isr_reserved_83
	.set	__cs3_isr_reserved_83, __cs3_isr_interrupt
	.weak	__cs3_isr_pit0
	.globl	__cs3_isr_pit0
	.set	__cs3_isr_pit0, __cs3_isr_interrupt
	.weak	__cs3_isr_pit1
	.globl	__cs3_isr_pit1
	.set	__cs3_isr_pit1, __cs3_isr_interrupt
	.weak	__cs3_isr_pit2
	.globl	__cs3_isr_pit2
	.set	__cs3_isr_pit2, __cs3_isr_interrupt
	.weak	__cs3_isr_pit3
	.globl	__cs3_isr_pit3
	.set	__cs3_isr_pit3, __cs3_isr_interrupt
	.weak	__cs3_isr_pdb
	.globl	__cs3_isr_pdb
	.set	__cs3_isr_pdb, __cs3_isr_interrupt
	.weak	__cs3_isr_usb_otg
	.globl	__cs3_isr_usb_otg
	.set	__cs3_isr_usb_otg, __cs3_isr_interrupt
	.weak	__cs3_isr_usb_charger_detect
	.globl	__cs3_isr_usb_charger_detect
	.set	__cs3_isr_usb_charger_detect, __cs3_isr_interrupt
	.weak	__cs3_isr_ethernet_mac_timer
	.globl	__cs3_isr_ethernet_mac_timer
	.set	__cs3_isr_ethernet_mac_timer, __cs3_isr_interrupt
	.weak	__cs3_isr_ethernet_mac_transmit
	.globl	__cs3_isr_ethernet_mac_transmit
	.set	__cs3_isr_ethernet_mac_transmit, __cs3_isr_interrupt
	.weak	__cs3_isr_ethernet_mac_receive
	.globl	__cs3_isr_ethernet_mac_receive
	.set	__cs3_isr_ethernet_mac_receive, __cs3_isr_interrupt
	.weak	__cs3_isr_ethernet_mac_error
	.globl	__cs3_isr_ethernet_mac_error
	.set	__cs3_isr_ethernet_mac_error, __cs3_isr_interrupt
	.weak	__cs3_isr_i2s
	.globl	__cs3_isr_i2s
	.set	__cs3_isr_i2s, __cs3_isr_interrupt
	.weak	__cs3_isr_sdhc
	.globl	__cs3_isr_sdhc
	.set	__cs3_isr_sdhc, __cs3_isr_interrupt
	.weak	__cs3_isr_dac0
	.globl	__cs3_isr_dac0
	.set	__cs3_isr_dac0, __cs3_isr_interrupt
	.weak	__cs3_isr_dac1
	.globl	__cs3_isr_dac1
	.set	__cs3_isr_dac1, __cs3_isr_interrupt
	.weak	__cs3_isr_tsi
	.globl	__cs3_isr_tsi
	.set	__cs3_isr_tsi, __cs3_isr_interrupt
	.weak	__cs3_isr_mcg
	.globl	__cs3_isr_mcg
	.set	__cs3_isr_mcg, __cs3_isr_interrupt
	.weak	__cs3_isr_low_power_timer
	.globl	__cs3_isr_low_power_timer
	.set	__cs3_isr_low_power_timer, __cs3_isr_interrupt
	.weak	__cs3_isr_reserved_102
	.globl	__cs3_isr_reserved_102
	.set	__cs3_isr_reserved_102, __cs3_isr_interrupt
	.weak	__cs3_isr_port_control_module_a
	.globl	__cs3_isr_port_control_module_a
	.set	__cs3_isr_port_control_module_a, __cs3_isr_interrupt
	.weak	__cs3_isr_port_control_module_b
	.globl	__cs3_isr_port_control_module_b
	.set	__cs3_isr_port_control_module_b, __cs3_isr_interrupt
	.weak	__cs3_isr_port_control_module_c
	.globl	__cs3_isr_port_control_module_c
	.set	__cs3_isr_port_control_module_c, __cs3_isr_interrupt
	.weak	__cs3_isr_port_control_module_d
	.globl	__cs3_isr_port_control_module_d
	.set	__cs3_isr_port_control_module_d, __cs3_isr_interrupt
	.weak	__cs3_isr_port_control_module_e
	.globl	__cs3_isr_port_control_module_e
	.set	__cs3_isr_port_control_module_e, __cs3_isr_interrupt
#endif /* interrupt */
