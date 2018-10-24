/******************************************************************************
* File:    isr.h
* Purpose: Define interrupt service routines referenced by the vector table.
* Note: Only "vectors.c" should include this header file.
******************************************************************************/

#ifndef __ISR_H
#define __ISR_H 1


#undef  VECTOR_039
#define VECTOR_039 cmp0_isr


// ISR(s) are defined in your project directory.
extern void cmp0_isr(void);

#endif  //__ISR_H

/* End of "isr.h" */
