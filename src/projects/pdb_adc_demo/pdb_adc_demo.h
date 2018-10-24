/******************************************************************************
* File:    pdb_adc_demo.h
* Purpose: header file for pdb_adc_demo.c
* Note: 
******************************************************************************/

#ifndef __HW_TRIG_TEST_H
#define __HW_TRIG_TEST_H 1

#define POT           21
#define ADC0_CHANA    26   //temp sensor                   // set to desired ADC0 channel trigger A    
#define ADC0_CHANB    POT                                  // set to desired ADC0 channel trigger B    


// PIN signal on  PTC7
#define PIN_TOGGLE   GPIOC_PTOR = 0x01 << 7;              // xor - toggle

// PIN1 signal on PTC8
#define PIN1_LOW     GPIOC_PCOR = 0x01 << 8;               // clear
#define PIN1_HIGH    GPIOC_PSOR = 0x01 << 8;               // set

// PIN2 signal on PTC9
#define PIN2_LOW     GPIOC_PCOR = 0x01 << 9;               // clear 
#define PIN2_HIGH    GPIOC_PSOR = 0x01 << 9;               // set   

/* Addresses for VECTOR_TABLE and VECTOR_RAM come from the linker file */  
extern uint32 __VECTOR_RAM[];

/* function prototypes */

extern void adc0_isr(void);  //  files have local isrs for ADC 
extern void pdb_isr(void);

#endif  //__HW_TRIG_TEST_H

/* End of "hw_trig_test.h" */
