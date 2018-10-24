/******************************************************************************
* File:    ADC_PDB.h
* Purpose: header file for ADC_PDB.c
* Note: 
******************************************************************************/

#ifndef __ADC_PDB_H
#define __ADC_PDB_H 1



/*     #defines  */

#define PIN_LOW     GPIOC_PCOR = 0x01 << 7;               // clear
#define PIN_HIGH    GPIOC_PSOR = 0x01 << 7;               // set


void delay(void);
void Init_Gpio(void);


#endif  //__ADC_PDB_H

/* End of "ADC_PDB.h" */
