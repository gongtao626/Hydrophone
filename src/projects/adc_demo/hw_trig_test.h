/******************************************************************************
* File:    hw_trig_test.h
* Purpose: header file for hw_trig_test.c
* Note: 
******************************************************************************/

#ifndef __HW_TRIG_TEST_H
#define __HW_TRIG_TEST_H 1


#define POT           21
#define ADC0_CHANA    POT                                   // set to desired ADC0 channel trigger A    
#define ADC0_CHANB    POT                                  // set to desired ADC0 channel trigger B    


#define ADC0_DLYA     0x4000                                // ADC0 trigger A delay 
#define ADC0_DLYB     0x8000                                // ADC0 trigger B delay 


// PIN signal on PTC7
#define PIN_TOGGLE   GPIOC_PTOR = 0x01 << 7;              // xor - toggle

// PIN1 signal on PTC8
#define PIN1_LOW     GPIOC_PCOR = 0x01 << 8;               // clear
#define PIN1_HIGH    GPIOC_PSOR = 0x01 << 8;               // set

// PIN2 signal on PTC9
#define PIN2_LOW     GPIOC_PCOR = 0x01 << 9;               // clear 
#define PIN2_HIGH    GPIOC_PSOR = 0x01 << 9;               // set

 

#define ADC0A_DONE   0x01       
#define ADC0B_DONE   0x02       

/* Addresses for VECTOR_TABLE and VECTOR_RAM come from the linker file */  
extern uint32 __VECTOR_RAM[];

/* function prototypes */

uint8_t Hw_Trig_Test(void);

extern void adc0_isr(void);  //  files have local isrs for ADC 
extern void pdb_isr(void);
static void Init_Gpio2(void);

#endif  //__HW_TRIG_TEST_H

/* End of "hw_trig_test.h" */
