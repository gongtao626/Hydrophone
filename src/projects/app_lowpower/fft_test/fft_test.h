/* fft */
#include "arm_math.h"


#define TEST_LENGTH_SAMPLES_CFFT  512
#define TEST_LENGTH_SAMPLES_RFFT_128  128
#define TEST_LENGTH_SAMPLES_RFFT_512  512

/* ------------------------------------------------------------------- 
* External Input and Output buffer Declarations for FFT Bin Example 
* ------------------------------------------------------------------- */ 

/************************************************Complex FFT Variables *********************************************************************/
extern q15_t complex_testInput_q15[TEST_LENGTH_SAMPLES_CFFT]; 
extern q15_t cfftMagnitud[TEST_LENGTH_SAMPLES_CFFT/2];

/************************************************Real 128 FFT Variables *********************************************************************/
extern q15_t real128_testInput_q15[TEST_LENGTH_SAMPLES_RFFT_128]; 
extern q15_t rfft128testOutput[TEST_LENGTH_SAMPLES_RFFT_128*2]; 
extern q15_t rfft128Magnitud[TEST_LENGTH_SAMPLES_RFFT_128];


/************************************************Real 512FFT Variables *********************************************************************/
extern q15_t real512_testInput_q15[TEST_LENGTH_SAMPLES_RFFT_512]; 
extern q15_t rfft512testOutput[TEST_LENGTH_SAMPLES_RFFT_512*2]; 
extern q15_t rfft512Magnitud[TEST_LENGTH_SAMPLES_RFFT_512];



/***********************************************Function Prototype**************************************************************************/
void complex_fft(void);
void real128_fft(void);
void real512_fft(void);
void  display_fft_info(void);

extern q15_t rfft128testOutput[TEST_LENGTH_SAMPLES_RFFT_128*2]; 
extern q15_t rfft128Magnitud[TEST_LENGTH_SAMPLES_RFFT_128];
extern q15_t adc_buffer2[];


typedef struct FILTER1_T
 {
  char filter_output;
  int16 min_magnitude;
  signed char filter_count;
  char max_count;
  char high_level;
  char low_level;
  char attack_gain;
  char decay_gain;
 }filter1_t;

// tone_filter.filter_output
// clap_filter.filter_output

extern filter1_t  tone_filter;
extern filter1_t  clap_filter;

extern int32 clap_energy; //instant energey
extern int32 clap_peak_energy;  //this is decreased by timer
extern  char clap_detected;
extern char clap_counter;
extern  char tone_detected;
extern char tone_counter;

void     tone_filter_init(void);
extern   uint16 median_energy;
extern   uint16 energy_scaled;
extern struct tipo_mediana arr_medianasClap[];
 


typedef  struct _FreqMag
{
   q15_t  magnitud;
   char   harmonic;
}FreqMag_t;

extern FreqMag_t  peak_table[];
