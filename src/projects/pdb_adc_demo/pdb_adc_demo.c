/**
  \mainpage 
  \n Copyright (c) 2011 Freescale
  \n Freescale Confidential Proprietary 
  \brief 
  \author   	Freescale Semiconductor
  \author    
  \version      m.n
  \date         Sep 14, 2011
  
  This demo use the PDB to trigger the ADC, the PDB is adjusted to a frequency equal to 6.4KHZ which 
it is near to the minimum time of conversion of the two channels in sequence. 

So, the sequence error interrupt is enabled to monitor that the triggers are working properly.


Both ADC channels A and B are reading the voltage of the POT and they are showed on the terminal.


*/
/****************************************************************************************************/
/*                                                                                                  */
/* All software, source code, included documentation, and any implied know-how are property of      */
/* Freescale Semiconductor and therefore considered CONFIDENTIAL INFORMATION.                       */
/* This confidential information is disclosed FOR DEMONSTRATION PURPOSES ONLY.                      */
/*                                                                                                  */
/* All Confidential Information remains the property of Freescale Semiconductor and will not be     */
/* copied or reproduced without the express written permission of the Discloser, except for copies  */
/* that are absolutely necessary in order to fulfill the Purpose.                                   */
/*                                                                                                  */
/* Services performed by FREESCALE in this matter are performed AS IS and without any warranty.     */
/* CUSTOMER retains the final decision relative to the total design and functionality of the end    */
/* product.                                                                                         */
/* FREESCALE neither guarantees nor will be held liable by CUSTOMER for the success of this project.*/
/*                                                                                                  */
/* FREESCALE disclaims all warranties, express, implied or statutory including, but not limited to, */
/* implied warranty of merchantability or fitness for a particular purpose on any hardware,         */
/* software ore advise supplied to the project by FREESCALE, and or any product resulting from      */
/* FREESCALE services.                                                                              */
/* In no event shall FREESCALE be liable for incidental or consequential damages arising out of     */
/* this agreement. CUSTOMER agrees to hold FREESCALE harmless against any and all claims demands or */
/* actions by anyone on account of any damage,or injury, whether commercial, contractual, or        */
/* tortuous, rising directly or indirectly as a result of the advise or assistance supplied CUSTOMER*/ 
/* in connectionwith product, services or goods supplied under this Agreement.                      */
/*                                                                                                  */
/****************************************************************************************************/

/*****************************************************************************************************
* Include files
*****************************************************************************************************/
#include "common.h"
#include "adc16.h"
#include "pdb_adc_demo.h"

       
#define MAX_SAMPLES   128u


//globle variable
tADC_Config Master_Adc_Config;  
tADC_Cal_Blk CalibrationStore[1];

volatile uint16  wSamplesBuf[MAX_SAMPLES];
volatile uint16  wSamplesBufOffline[MAX_SAMPLES];

volatile uint16  wSamplesCounter;
volatile uint8   bSamplesReady;
volatile uint32  bSeqErr;

// Prototypes
void adc_init(void);
void pdb_init(void);   
void pdb_seq_error(uint16 channel,uint16 pretrigger);
void process_samples_offline(void);
void handle_pdb_seq_err(void);

// ASSUME: IPBus clock is 50MHz
int main (void)
{
  
  #ifdef KEIL
	start();
  #endif
  
  printf("Hello Kinetis!\r\n");
  printf("Welcome to PDB trigger ADC demo!\r\n");
  printf("Please make sure J15 for the POT is set, press any key to continue...!");
  in_char();
  printf("\r\n");
  
  // Initialize global variables
  wSamplesCounter = 0;
  bSamplesReady = 0;
  bSeqErr = 0;
  
  // Disable ADC and PDB interrupts
 disable_irq(INT_ADC0-16) ;   // not ready for this interrupt yet. Plug vector first.
 disable_irq(INT_PDB0-16) ;    // not ready for this interrupt yet. Plug vector first.

// Dynamic interrupt vector modification whilst those interruts are disabled
 __VECTOR_RAM[INT_ADC0] = (uint32)adc0_isr;  // plug isr into vector table in case not there already
 __VECTOR_RAM[INT_PDB0] = (uint32)pdb_isr;   // plug isr into vector table in case not there already
  
  /* Turn on the ADC0 clock as well as PDB clock
   */
  SIM_SCGC6 |= (SIM_SCGC6_ADC0_MASK | SIM_SCGC6_PDB_MASK);

 enable_irq(INT_ADC0-16) ;   // ready for this interrupt.
 enable_irq(INT_PDB0-16) ;    // ready for this interrupt.

// In case previous test did not end with interrupts enabled, enable used ones.
 EnableInterrupts ;

 

 adc_init();
 pdb_init();
 while(1){
  
   if(bSamplesReady)
   {
     // process samples
     bSamplesReady = 0;     
    // DisableInterrupt();
     process_samples_offline();
    // EnableInterrupt();
   }  
   if(bSeqErr)
   {
     handle_pdb_seq_err();
   } 
 };
}


void process_samples_offline(void)
{
  int i;
  
  //printf("Output of samples in current sampling period:\n");
  //printf("=============================================\n");
  for(i=0;i<MAX_SAMPLES;i++) {
    printf("\rPOT= %6d",wSamplesBufOffline[i]);
    }  
  //printf("\n=============================================\n");
}

void handle_pdb_seq_err(void)
{
  printf("Error: PDB Sequence error!\r\n");
}


void adc_init(void)
{
   uint8_t cal_ok;
   
  /* 48MHz IPBus clock
   * ADC clock = 50M/2/8 = 3.125MHz
   * Total conversion time: 56N+4ADCK
   * Given sampling rate Fs = 6.4K, 156us/sample,  156*3= 468 ADCK
   * the maximum h/w average number = 8
   * use h/w average number = 4
   * Total conversion time: 56*4+4 = 228 ADC clocks,73us
   * There are 468-228 = 240 ADC clocks (ie. 77us) free for post processing
   */
 
  // Initialize ADC0
  // Do calibration first with 32 h/w averages
  Master_Adc_Config.CONFIG1  = ADLPC_NORMAL | ADC_CFG1_ADIV(ADIV_8) | ADLSMP_LONG | ADC_CFG1_MODE(MODE_16)
                              | ADC_CFG1_ADICLK(ADICLK_BUS_2);  
  Master_Adc_Config.CONFIG2  = MUXSEL_ADCA | ADACKEN_DISABLED | ADHSC_HISPEED | ADC_CFG2_ADLSTS(ADLSTS_20) ;
  Master_Adc_Config.COMPARE1 = 0x1234u ; 
  Master_Adc_Config.COMPARE2 = 0x5678u ;
  Master_Adc_Config.STATUS2  = ADTRG_SW | ACFE_DISABLED | ACFGT_GREATER | ACREN_ENABLED | DMAEN_DISABLED | ADC_SC2_REFSEL(REFSEL_EXT);
  Master_Adc_Config.STATUS3  = CAL_OFF | ADCO_SINGLE | AVGE_ENABLED | ADC_SC3_AVGS(AVGS_32);
  Master_Adc_Config.STATUS1A = AIEN_OFF | DIFF_SINGLE | ADC_SC1_ADCH(31);       
  Master_Adc_Config.STATUS1B = AIEN_OFF | DIFF_SINGLE | ADC_SC1_ADCH(31);  
  
  ADC_Config_Alt(ADC0_BASE_PTR, &Master_Adc_Config);  // config ADC
  cal_ok = ADC_Cal(ADC0_BASE_PTR);                    // do the calibration
  ADC_Read_Cal(ADC0_BASE_PTR,&CalibrationStore[0]);   // store the cal
  
  // Now do normal ADC configuration with 4 h/w averages and h/w trigger from PDB
  Master_Adc_Config.CONFIG1  = ADLPC_NORMAL | ADC_CFG1_ADIV(ADIV_8) | ADLSMP_LONG | ADC_CFG1_MODE(MODE_16)
                              | ADC_CFG1_ADICLK(ADICLK_BUS_2);  
  Master_Adc_Config.CONFIG2  = MUXSEL_ADCA | ADACKEN_ENABLED | ADHSC_HISPEED | ADC_CFG2_ADLSTS(ADLSTS_20) ;
  Master_Adc_Config.COMPARE1 = 0x1234u ; 
  Master_Adc_Config.COMPARE2 = 0x5678u ;
  Master_Adc_Config.STATUS2  = ADTRG_HW | ACFE_DISABLED | ACFGT_GREATER | ACREN_DISABLED | DMAEN_DISABLED | ADC_SC2_REFSEL(REFSEL_EXT);
  Master_Adc_Config.STATUS3  = CAL_OFF | ADCO_SINGLE | AVGE_ENABLED | ADC_SC3_AVGS(AVGS_4);
  //Master_Adc_Config.PGA      = PGAEN_DISABLED | PGACHP_NOCHOP | PGALP_NORMAL | ADC_PGA_PGAG(PGAG_64); // Not available for K2050MHz

  Master_Adc_Config.STATUS1A = AIEN_ON | DIFF_SINGLE | ADC_SC1_ADCH(POT); 
  
#define   USE_ADC_SINGLE_MODE  
#if defined(USE_ADC_SINGLE_MODE)
  // use single end for others
  Master_Adc_Config.STATUS1B = AIEN_ON | DIFF_SINGLE | ADC_SC1_ADCH(POT);    // POT on ADC0_DM3
#else  
  // use differential mode for channel 1 on tower boards
  Master_Adc_Config.STATUS1B = AIEN_ON | DIFF_DIFFERENTIAL | ADC_SC1_ADCH(3);  //ADC0_DP3-ADC0_DM3
#endif
  
  ADC_Config_Alt(ADC0_BASE_PTR, &Master_Adc_Config);  // config the ADC again to default conditions      

}



void pdb_init(void)
{

 //use ptc7 8 to indicate program status
  PORTC_PCR7 = PORT_PCR_MUX(1) ;        // selec GPIO function
  GPIOC_PCOR = 0x01 << 7 ;              // initial out low 
  GPIOC_PDDR|= (1<<7);                  // set as output 
  
  PORTC_PCR8 = PORT_PCR_MUX(1) ;        // select GPIO function
  GPIOC_PCOR =  0x01 << 8 ;             // initial out low
  GPIOC_PDDR|= (1<<8);                  // set as output
 
  //-------------------------pdb setup---------------------------------
 //1.Set to software trigger input 
 //2.Enable pdb module
 //3.Prescale set to 0
 //4. LDMOD = 0: load immediately after setting LDOK
 //5. enable PDB sequence error interrupt
 //Only enable pdb module then you can write in all channel delay value
 
 PDB0_SC = 0x00000000;
 
 // NOTE: PDB must be enabled before writting to buffered registers
 // otherwise, it will be always previous value.
 //Enable PDB
 PDB0_SC |= PDB_SC_PDBEN_MASK;
 
 PDB0_SC |= PDB_SC_TRGSEL(0xF);
   
  //Enable continuous mode
 PDB0_SC |= PDB_SC_CONT_MASK;
 

 //Enable pre-trigger out to ADC0
 PDB0_CH0C1 = PDB_C1_TOS(3)|PDB_C1_EN(3);
 
 //Set CH0 DLY0/1
 PDB0_CH0DLY0 = 0;
 PDB0_CH0DLY1 = 3650;    // delay the conversion time = 73us
 
 
 //Set PDB_MOD to 156us corresponding to sampling frequency Fs=6.4KHz
 PDB0_MOD = 7800;
 
 //Set interrupt delay value 
 PDB0_IDLY = 7800;
 
 //Load delay value
 PDB0_SC |= PDB_SC_LDOK_MASK;
 
 //Enable interrupt 
 PDB0_SC |= PDB_SC_PDBIE_MASK;
 PDB0_SC |= PDB_SC_PDBEIE_MASK;

 //Set software trigger
 PDB0_SC |= PDB_SC_SWTRIG_MASK;
 }
 
void adc0_isr(void){
  uint16 adc_sample0;
  uint16 adc_sample1;  
  int16  i;
  
  if((ADC0_SC1A&ADC_SC1_COCO_MASK)!= 0){
     GPIOC_PTOR = 0x01 << 7 ; //toggle PTC7
     adc_sample0 = ADC0_RA;
  }
  if((ADC0_SC1B&ADC_SC1_COCO_MASK)!= 0){
     GPIOC_PTOR = 0x01 << 8 ; //toggle PTC8
    adc_sample1 = ADC0_RB;

   
    wSamplesBuf[wSamplesCounter++] = adc_sample1;   
    if(wSamplesCounter == MAX_SAMPLES) {

      // Copy samples to offline backup buf
      for(i=0;i<MAX_SAMPLES;i++) {
        wSamplesBufOffline[i]=wSamplesBuf[i];
      }
   
      bSamplesReady = 1;
      wSamplesCounter = 0;
    }        
  }
}


void pdb_isr(void) 
{
   // Check if pdb sequence error
   if(PDB0_CH0S & PDB_S_ERR(1))
   {
      pdb_seq_error(0,0); // pretrigger 0 sequence error
   }
   if(PDB0_CH0S & PDB_S_ERR(2))
   {
      pdb_seq_error(0,1); // pretrigger 1 sequence error
   }
  
   if(PDB0_SC & PDB_SC_PDBIF_MASK)
   {
      PDB0_SC &= ~PDB_SC_PDBIF_MASK;  // write 0 to PDBIF to clear which is different from EM256
   }
}

 
void pdb_seq_error(uint16 channel,uint16 pretrigger)
{
    bSeqErr++;
   if(channel==0)
    {
      PDB0_CH0S &= ~PDB_S_ERR(1<<pretrigger);//clear interrupt flag      
    }
}
/********************************************************************/
