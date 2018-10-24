/*
 * File:		uart.h
 * Purpose:     Provide common ColdFire UART routines for polled serial IO
 *
 * Notes:
 */

#ifndef __UART_H__
#define __UART_H__
/********************************************************************/

void uart_init (UART_MemMapPtr, int, int);
char uart_getchar (UART_MemMapPtr);
void uart_putchar (UART_MemMapPtr, char);
int uart_getchar_present (UART_MemMapPtr);
void hw_uart_sendN(UART_MemMapPtr, uint16, uint8*);
void hw_uart1_422_init(void);
void hw_uart1_422_change (int baud_to_change);
void hw_uart0_232_re_int_open(void);
void hw_uart0_232_re_int_close(void);
void hw_uart1_422_re_int_open(void);
void hw_uart1_422_re_int_close(void);
/********************************************************************/

#endif /* __UART_H__ */
