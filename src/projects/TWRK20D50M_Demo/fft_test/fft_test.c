#include "common.h"
//#include "fft_sample.h"
#include "arm_math.h" 
#include "adc_test.h"
#include "fft_test.h"
#include "TWR_K20D50M_DEF.h"


char filter_1(uint16 input, filter1_t   *fil);
void tone_filter_init(void);
void tone_clap_detection(void);



 filter1_t  tone_filter;



/************************************************Real 128 FFT Variables *********************************************************************/


/* ------------------------------------------------------------------- 
* External Input and Output buffer Declarations for FFT Bin Example 
* ------------------------------------------------------------------- */ 
extern int16 adc_buffer2[];


q15_t rfft128testOutput[TEST_LENGTH_SAMPLES_RFFT_128*2]; 
q15_t rfft128Magnitud[TEST_LENGTH_SAMPLES_RFFT_128];
q15_t mag_mean;

/* ------------------------------------------------------------------ 
* Global variables for FFT Bin Example 
* ------------------------------------------------------------------- */ 
uint32_t rfft128Size = TEST_LENGTH_SAMPLES_RFFT_128; 
uint32_t irfft128Flag = 0; 
uint32_t r128doBitReverse = 1; 

extern q15_t ADC_test_signal[]; //señal de prueba
void tone_filer(void);
/*################################################Choose the FFT Algorithm######################################################################*/
#define REAL_FFT_128
//#undef REAL_FFT_128
//#define REAL_FFT_512
//#undef REAL_FFT_512
//#define COMPLEX_FFT_256
//#undef COMPLEX_FFT_256    

/* Definitions */
   #define NUMBER_OF_SAMPLES    128


/*  Variables */
//q15_t adc_buffer[NUMBER_OF_SAMPLES*2];          //ADC Samples buffer size 512 to suport all algorithm
uint16 adc_sample_counter = 0;
uint32  ti_msec;
uint8 rfftFlag = 1;
int32 clap_energy; //instant energey
int32 clap_peak_energy;  //this is decreased by timer


char peak_index=0;
FreqMag_t  peak_table[32];

q15_t adc_buffer2[130];
extern int16 max_value;


int adc_gain  =     9;  //330 for 10 bits 
int adc_offset = 1215;  //330 for 10 bits 

 
uint16 median_energy;
uint16 energy_scaled;
char clap_detected;
char clap_counter;
char tone_detected;
char tone_counter;

#define ADC_IN_LIMT 1500
#define CLAP_PULSE_TIME    7
#define CLAP_SILENCE_TIME 35

#define TONE_SILENCE_TIME 40


uint32 clap_detect_threshold  =10000;
uint32 clap_silence_threshold = 100;
uint32 clap_silence_band      = 1000;
uint32 tone_silence_band      = 1000;


//runs every 7.8msec

void clap_filter2(uint32 clap_filter)
{   static uint8 clap_trigger;
    static uint8 time_clap_detection;
    static uint8 time_clap_disabled;
    
    if ((abs(clap_energy - median_energy)> clap_detect_threshold)  && !clap_trigger) 
    {
      clap_trigger=1;
      time_clap_detection=CLAP_PULSE_TIME;  //3 cycles  1/62 seg
      time_clap_disabled =CLAP_SILENCE_TIME; // time clap disabled
     }

   if (time_clap_detection)
   {
     time_clap_detection--;
     if  (abs(clap_energy-median_energy)< clap_silence_threshold) clap_trigger=2;
     time_clap_disabled =CLAP_SILENCE_TIME; // time clap disabled
   }
   if (time_clap_disabled)
     { 
     time_clap_disabled--;
     if  (abs(clap_energy-median_energy)> clap_silence_band) clap_trigger=3;
     }
   if (time_clap_disabled==0 && clap_trigger)
    {
      if (clap_trigger == 2) 
     {
       clap_detected=1;
       clap_counter++;
     }
    clap_trigger=0;
    } 

}

void tone_filer(void)
{
 static uint8 tone_trigger;
 static char aux_tone_fil;
 static uint8 time_tone_disabled;    
 if (input_fall(tone_filter.filter_output,&aux_tone_fil))
  {
    tone_trigger=1;
    time_tone_disabled =TONE_SILENCE_TIME;
  }
  if (time_tone_disabled)
     { 
     time_tone_disabled--;
     if  (abs(clap_energy-median_energy)> tone_silence_band) tone_trigger=3; //noise
     }
   if (time_tone_disabled==0 && tone_trigger)
    {
      if (tone_trigger == 1) 
     {
       tone_detected=1;
       tone_counter++;
     }
     else tone_filter.filter_output=0; //it is was noise
       
    tone_trigger=0;
    }     
 
 

}


void real128_fft(void)
{
  arm_status status; 
  arm_rfft_instance_q15 S;
  arm_cfft_radix4_instance_q15 S_CFFT;  
  int i,offset;
  q15_t q15maximum;
  char harmonic;
  q15_t adc_in;
  int32 i32sum;

  

  /*copy buffer as q_15 and removes offset  and clear buffer flag*/
  offset = 128 * (buffer & 0x01);
  for (i=0;i<128;i++)
    {
      adc_in = adc_buffer[i + offset];
      if (adc_in>ADC_IN_LIMT) adc_in = ADC_IN_LIMT;
      
     adc_buffer2[i] = (q15_t)((adc_in-adc_offset)<<adc_gain); // 6 con generado adc a 10 bits
     //adc_buffer2[i] = (q15_t)ADC_test_signal[i];        
    }
       
    status = ARM_MATH_SUCCESS; 
    rfft128Size = 128;    
    /* Initialize the CFFT/CIFFT module */  
    status = arm_rfft_init_q15(&S, &S_CFFT, rfft128Size, irfft128Flag, r128doBitReverse);                                                           
                                                         
    /* Process the data through the CFFT/CIFFT module */ 
    arm_rfft_q15(&S, &adc_buffer2[0], &rfft128testOutput[0] );                           
    
    /* Process the data through the Complex Magnitude Module for  
    calculating the magnitude at each bin */ 
    rfft128Size = TEST_LENGTH_SAMPLES_RFFT_128;
    arm_cmplx_mag_q15(&rfft128testOutput[0], &rfft128Magnitud[0],  
                    rfft128Size);  
    
 //   arm_mean_q15 (&rfft128Magnitud[1], 127, &mag_mean); 
  
   
    // search for maximum peak
   q15maximum = 0;
   harmonic = 0;
   
   for (i = 4; i<40;i++)
   {
     if (rfft128Magnitud[i]>q15maximum) 
      {
        q15maximum  = rfft128Magnitud[i];
        harmonic = i;  
     }
   }
   
    //store peak of this block
    peak_table[peak_index].magnitud=q15maximum;
    peak_table[peak_index].harmonic=harmonic;
    if (++peak_index > 31) peak_index = 0; 
   

/*
  filter tone
  
   params:
      min_magnitud
      filter_count
      high_level
      low_level
      maximum_count
   output
   */
   (void)filter_1(q15maximum,&tone_filter);  //check for a tone
   tone_filer();  //check for a silence after tone

    // look for maximum peak and coresponding harmonic
   i32sum = 0;
   
   for (i=2;i<20;i++)
   {
     i32sum += rfft128Magnitud[i];
   } 
  // (void)filter_1(q15maximum,&clap_filter);
    clap_energy = i32sum;
   
   if (clap_energy > clap_peak_energy) clap_peak_energy=clap_energy;
   clap_filter2(clap_energy);

}








/*

   if input>min
  this function is calles every 1/62msec
 */
char filter_1(uint16 input, filter1_t   *fil)
 {
  if (input > fil->min_magnitude)
    { 
      fil->filter_count += fil->attack_gain;
      if ( fil->filter_count >  fil->max_count) fil->filter_count = fil->max_count;
      if ( fil->filter_count >  fil->high_level) fil->filter_output = 1; 
    }
  else
   {
     fil->filter_count -= fil->decay_gain;
     if ( fil->filter_count < 0) fil->filter_count = 0;
     if ( fil->filter_count <  fil->low_level) fil->filter_output = 0; 
   }
  return fil->filter_output;  
  
  
  
 }


 void tone_filter_init(void)
   {
     filter1_t *fil;
     fil = &tone_filter;
     fil->min_magnitude = 3000; //2500
     fil->filter_output = 0;
     fil->filter_count  = 0;
     fil->max_count = 32;
     fil->high_level = 18;
     fil->low_level  = 1;
     fil->decay_gain = 1;
     fil->attack_gain= 3;
   }


  
/*   */
void  display_fft_info(void)
{
    int i;
    printf(" \n\r mag    mean=%i max adc = %i \n\r ",mag_mean,(int)max_value);
    for (i=1;i<12;i++)
    {   
      printf("\n\r [%03i] Mag=%05i",i,rfft128Magnitud[i]);
     // printf("\n\r  Harmonic[%03i] Mag=%05i", peak_table[i].harmonic,peak_table[i].magnitud);
    }
    printf(" \n\r ");

}        



             

q15_t ADC_test_signal[128]=
{
        0,
     439,
     862,
     1251,
     1592,
     1874,
     2089,
     2230,
     2297,
     2294,
     2226,
     2104,
     1940,
     1748,
     1544,
     1342,
     1159,
     1005,
     893,
     829,
     818,
     861,
     954,
     1092,
     1265,
     1462,
     1670,
     1876,
     2067,
     2229,
     2353,
     2431,
     2458,
     2431,
     2353,
     2229,
     2067,
     1876,
     1670,
     1462,
     1265,
     1092,
     954,
     861,
     818,
     829,
     893,
     1005,
     1159,
     1342,
     1544,
     1748,
     1940,
     2104,
     2226,
     2294,
     2297,
     2230,
     2089,
     1874,
     1592,
     1251,
     862,
     439,
     0,
     -439,
     -862,
     -1251,
     -1592,
     -1874,
     -2089,
     -2230,
     -2297,
     -2294,
     -2226,
     -2104,
     -1940,
     -1748,
     -1544,
     -1342,
     -1159,
     -1005,
     -893,
     -829,
     -818,
     -861,
     -954,
     -1092,
     -1265,
     -1462,
     -1670,
     -1876,
     -2067,
     -2229,
     -2353,
     -2431,
     -2458,
     -2431,
     -2353,
     -2229,
     -2067,
     -1876,
     -1670,
     -1462,
     -1265,
     -1092,
     -954,
     -861,
     -818,
     -829,
     -893,
     -1005,
     -1159,
     -1342,
     -1544,
     -1748,
     -1940,
     -2104,
     -2226,
     -2294,
     -2297,
     -2230,
     -2089,
     -1874,
     -1592,
     -1251,
     -862,
     -439,



  
};
