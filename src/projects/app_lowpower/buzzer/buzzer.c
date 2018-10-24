/****************************************************************************************************/
/**
Copyright (c) 2011 Freescale Semiconductor
Freescale Confidential Proprietary
\file      buzzer.c
\brief     Buzzer is based on a FTM and one channel,  also it uses and PIT for timer control  
\author     
\author     
\version    1.0
\date       Sep 26, 2011
*/

#include "common.h"
#include "buzzer.h"
#include "fft_test.h"
#define SWEEP_START_PER  3000
#define SWEEP_END_PER     500


uint32 ti_delay;
uint32 ti_accel_sampling;
uint16 ti_tone;
uint16 ti_pause;
uint16 ti_beep;
uint16 ti_fall;
uint16 ti_tone_wait;
uint8 base_time125m;
uint16 ti_test_leds;
uint16 ti_test_adc;



uint16 ti_max_value;
int16 max_value;


uint8  tone_enable; 
char echo_flag;
uint16 ti_echo;

const uint16 MONOCHROMATIC_SCALE[];


/**   buzzer_ftm_init
 * \brief    Initialize Periodic interrupt timer,
 * \brief    PIT1 is used for tone/buzzer time control
 * \author   b01252
 * \param    none
 * \return   none
 */  
void buzzer_ftm_init(void)
{
   SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
   SIM_SCGC6|=(SIM_SCGC6_FTM0_MASK);   
   PORTC_PCR4 = (0|PORT_PCR_MUX(4)); /* FTM0_CH0 enable on PTC3 */
   FTM0_MOD  = 10000;  /* 0x0063 / 25MHz = 4uS PWM period */
   FTM0_C3SC = 0x28;   /* No Interrupts; High True pulses on Edge Aligned PWM */
   FTM0_C3V  =  5000;  /* 90% pulse width */
   FTM0_SC   = 0x08 | FTM_SC_PS(1);     /* Edge Aligned PWM running from BUSCLK / 1 */
   
   buzzer_off();
}


/**   set_out_module
 * \brief    updates FTM module and dutycycle
 * \author   b01252
 * \param    uint16 new_module
 * \return   none
 * \todo
 * \warning
 */ 
void set_out_module(uint16 new_module)
{
         FTM0_MOD = new_module;  //starts with E
         FTM0_C3V = new_module/2;
}


/**   PIT_init
 * \brief    Initialize Periodic interrupt timer,
 * \brief    PIT1 is used for tone/buzzer time control
 * \author   b01252
 * \param    none
 * \return   none
 */  
void PIT_init(void)
{
    SIM_SCGC6 |= SIM_SCGC6_PIT_MASK; // enable PIT module
    enable_irq(INT_PIT1 - 16);
       
    PIT_MCR = 0x00;  // MDIS = 0  enables timer
    PIT_TCTRL1 = 0x00; // disable PIT0
    PIT_LDVAL1 = 72000; // 
    PIT_TCTRL1 = PIT_TCTRL_TIE_MASK; // enable PIT0 and interrupt
    PIT_TFLG1 = 0x01; // clear flag
    PIT_TCTRL1 |= PIT_TCTRL_TEN_MASK;
   
}





void pit1_isrv(void)
{  
     static char count_mseg=125;
    int32 i32temp;
    PIT_TFLG1 = 0x01; // clear flag
    PIT_TCTRL1; // dummy read to ensure the interrupt, 
                // must be executed here, or interrupt will never happen.
   
    
    if (count_mseg)count_mseg--;
    else
    {
     count_mseg=125;
     base_time125m++; 
    }
    if (ti_delay) ti_delay--;   
    if (ti_accel_sampling) ti_accel_sampling--;
    if (ti_tone_wait) ti_tone_wait--;
    if (ti_test_leds) ti_test_leds--;
    if (ti_test_adc) ti_test_adc--;
    
    if (ti_echo) ti_echo--;
    /*tone service */
    if (ti_tone)  ti_tone--;
    else
    {
      if (tone_enable) buzzer_off();
     tone_enable=0;
    }
    
    if (ti_fall) ti_fall--;
   /* 
    if (ti_pause) ti_pause--;
    else
    {
 
    }
    */
    
    if (ti_max_value) ti_max_value--;
    else
    {
      ti_max_value = 300;
     //reduce clap enrgy in 1/8
      if (clap_peak_energy>0) 
      {
        i32temp =  clap_peak_energy - (clap_peak_energy<<3);
        if (i32temp<0)i32temp=0;
        clap_peak_energy =i32temp;
      }
    }
}





/*PWM edge aligned high true pulses*/
void  buzzer_on(void)
{
  FTM0_C3SC = FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;   //0x28;   /* No Interrupts; High True pulses on Edge Aligned PWM */
}


/*clear ouptut on compare */
void buzzer_off(void)
{

  FTM0_C3SC = FTM_CnSC_MSA_MASK | FTM_CnSC_ELSB_MASK;   //0x28;   /* No Interrupts; High True pulses on Edge Aligned PWM */
}


//to this scale ADD NOTE_C as initial note
void tone(TONES tone,uint16 time,uint16 pause)
 {
   set_out_module(MONOCHROMATIC_SCALE[tone]);
   ti_tone  = time  ;
   ti_pause = time + pause;
   tone_enable=1;
   buzzer_on();

 }



void beep(uint8 tone,uint16 time)
{
   set_out_module(MONOCHROMATIC_SCALE[tone]);
   ti_beep = time;
   buzzer_on();
}


const uint16 C_MAJ_SCALE[20]=
{
    47778, // C off 0
    42565, // D off 1
    37921, // E off 2
    35793, // F off 3
    31888, // G off 4
    28409, // A off 5
    25309, // B off 6
    23889, // C off 7
    21282, // D off 8
    18960, // E off 9
    17896, // F off 10
    15944, // G off 11
    14204, // A off 12
    12654, // B off 13
    11944, // C off 14
    10641, // D off 15
    9480, // E off 16
    8948, // F off 17
    7972, // G off 18
    7102, // A off 19
};

const uint16 MONOCHROMATIC_SCALE[39]=
{
  63776, //G4 off = 0
    60196, //G4_S off = 1
    56818, //A4 off = 2
    53629, //A4_S off = 3
    50619, //B4 off = 4
    47778, //C5 off = 5
    45096, //C5_S off = 6
    42565, //D5 off = 7
    40176, //D5_S off = 8
    37921, //E5 off = 9
    35793, //F5 off = 10
    33784, //F5_S off = 11
    31888, //G5 off = 12
    30098, //G5_S off = 13
    28409, //A5 off = 14
    26814, //A5_S off = 15
    25309, //B5 off = 16
    23889, //C6 off = 17
    22548, //C6_S off = 18
    21282, //D6 off = 19
    20088, //D6_S off = 20
    18960, //E6 off = 21
    17896, //F6 off = 22
    16892, //F6_S off = 23
    15944, //G6 off = 24
    15049, //G6_S off = 25
    14204, //A6 off = 26
    13407, //A6_S off = 27
    12654, //B6 off = 28
    11944, //C7 off = 29
    11274, //C7_S off = 30
    10641, //D7 off = 31
    10044, //D7_S off = 32
    9480, //E7 off = 33
    8948, //F7 off = 34
    8446, //F7_S off = 35
    7972, //G7 off = 36
    7524, //G7_S off = 37
    7102 //A7 off = 38


};

