/******************************************************************************
* File:    isr.h
* Purpose: Define interrupt service routines referenced by the vector table.
* Note: Only "vectors.c" should include this header file.
******************************************************************************/

#ifndef __ISR_H
#define __ISR_H 1



#undef  VECTOR_038   // as it was previously defined in vectors.h
#undef  VECTOR_050   // as it was previously defined in vectors.h

                                     // address     vector irq    perihperal
#define VECTOR_038      adc0_isr     // 0x0000_0098 38     22     ADC0
#define VECTOR_050      pdb_isr      // 0x0000_00C8 50     34     PDB

// ISR(s) are defined in your project directory.
extern void adc0_isr(void);                                // ISR for ADC0
extern void pdb_isr(void);                                 // ISR for PDB

#endif  //__ISR_H

/* End of "isr.h" */
