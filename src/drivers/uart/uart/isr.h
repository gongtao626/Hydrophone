/******************************************************************************
* File:    isr.h
* Purpose: Define interrupt service routines referenced by the vector table.
* Note: Only "vectors.c" should include this header file.
******************************************************************************/

#ifndef __ISR_H
#define __ISR_H 1

/* Define UART0 vectors */
#undef  VECTOR_031
#define VECTOR_031 uart0_lon_isr
#undef  VECTOR_032
#define VECTOR_032 uart0_status_isr
#undef  VECTOR_033
#define VECTOR_033  uart0_error_isr

/* Define UART1 vectors */
#undef  VECTOR_034
#define VECTOR_034 uart1_status_isr
#undef  VECTOR_035
#define VECTOR_035 uart1_error_isr

/* Define UART2 vectors */
#undef  VECTOR_036
#define VECTOR_036 uart2_status_isr
#undef  VECTOR_037
#define VECTOR_037 uart2_error_isr

/* Define LLWU vector */
#undef  VECTOR_025
#define VECTOR_025 llwu_isr

#warning "select the correct interrupt source"
#undef VECTOR_56
#define VECTOR_56 abort_isr

// ISRs are defined in "uart_isr.c".
extern void uart0_lon_isr(void);
extern void uart0_status_isr(void);
extern void uart0_error_isr(void);

extern void uart1_status_isr(void);
extern void uart1_error_isr(void);

extern void uart2_status_isr(void);
extern void uart2_error_isr(void);

extern void abort_isr(void);

// LLWU ISR defined for low power testing (isr in llwu.c)
extern void llwu_isr(void);

#endif  //__ISR_H

/* End of "isr.h" */
