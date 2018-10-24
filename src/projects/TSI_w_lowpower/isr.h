/******************************************************************************
* File:    isr.h
* Purpose: Define interrupt service routines referenced by the vector table.
* Note: Only "vectors.c" should include this header file.
******************************************************************************/

#ifndef __ISR_H
#define __ISR_H 1


/* Example */



#undef  VECTOR_053
#define VECTOR_053 TSI_isr


#undef  VECTOR_025
#define VECTOR_025 llwu_isr

extern void llwu_isr(void);
extern void TSI_isr(void);


#endif  //__ISR_H

/* End of "isr.h" */
