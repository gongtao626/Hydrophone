/******************************************************************************
* File:    isr.h
* Purpose: Define interrupt service routines referenced by the vector table.
* Note: Only "vectors.c" should include this header file.
******************************************************************************/

#ifndef __ISR_H
#define __ISR_H 1


/* Example */
/*
#undef  VECTOR_101
#define VECTOR_101 lpt_isr


// ISR(s) are defined in your project directory.
extern void lpt_isr(void);
*/

#undef  VECTOR_032
#define VECTOR_032 UART0_isr

#undef VECTOR_034
#define VECTOR_034 UART1_isr

#undef  VECTOR_046
#define VECTOR_046 pit0_isr

#undef VECTOR_047
#define VECTOR_047 pit1_isr

#undef VECTOR_057
#define VECTOR_057 PTB_1_isr

extern void UART0_isr(void);
extern void UART1_isr(void);
extern void pit0_isr(void);
extern void pit1_isr(void);
extern void PTB_1_isr(void);

#endif  //__ISR_H

/* End of "isr.h" */
