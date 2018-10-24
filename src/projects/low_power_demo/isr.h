/******************************************************************************
* File:    isr.h
* Purpose: Define interrupt service routines referenced by the vector table.
* Note: Only "vectors.c" should include this header file.
******************************************************************************/

#ifndef __ISR_H
#define __ISR_H 1


extern void port_c_isr(void);
extern void llwu_isr(void);

#undef VECTOR_058
#define VECTOR_058 port_c_isr


#undef VECTOR_025
#define VECTOR_025 llwu_isr


#endif  //__ISR_H

/* End of "isr.h" */
