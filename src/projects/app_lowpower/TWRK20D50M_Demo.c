/**
  \mainpage 
  \n Copyright (c) 2011 Freescale
  \n Freescale 
  \brief        Shows different features of the TWR-K20D50M
  \brief        press buttons, touch electrode  toggle LEDs
  \brief        tilt board to create tones (I2C, accelerometer)
  \todo         clap() and board answer with a tone (ADC + microphone)
  \author   	Freescale Semiconductor
  \author    
  \version      1.1
  \date         January 10,2012

  
  Put here all the information needed of the Project. Basic configuration as well as information on
  the project definition will be very useful 
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
#include "TWR_K20D50M_DEF.h"
#include "buzzer.h"
#include "rtc.h"
#include "hal_i2c.h"
#include "hal_dev_mma8451.h"
#include "adc_test.h"
#include "angle_cal.h"
#include "fft_test.h"
#include "irda_test.h"
#include "cmt_test.h"
#include "median.h"
//#include "freemaster.h"
#include "task_mgr.h"
#include "UART.h"
#include "protocol.h"
#include "tsi.h"

uint16 bufferCount;

void tsi_test_service(void);
void tsi_test_init(void);


/*****************************************************************************************************
* Declaration of module wide FUNCTIONs - NOT for use in other modules
*****************************************************************************************************/
void  switch_test(void);
void  accel_read(void);
void  accel_init(void);
void  vfn_test_accel(void);
void vfn_null(void);

/*****************************************************************************************************
* Definition of module wide MACROs / #DEFINE-CONSTANTs - NOT for use in other modules
*****************************************************************************************************/

/*****************************************************************************************************
* Declaration of module wide TYPEs - NOT for use in other modules
*****************************************************************************************************/

/*****************************************************************************************************
* Definition of module wide VARIABLEs - NOT for use in other modules
*****************************************************************************************************/
signed short accel_x, accel_y, accel_z;

signed short resultx, resulty, resultz;


/*****************************************************************************************************
* Definition of module wide (CONST-) CONSTANTs - NOT for use in other modules
*****************************************************************************************************/

/*****************************************************************************************************
* Code of project wide FUNCTIONS
*****************************************************************************************************/

/**
 * \brief   main
 * \author  
 * \param  none
 * \return none
 * \todo
 * \warning
 */  

 int main (void)
{
  
    static char aux_fall_input,aux_clap_detected,auxclapmed,aux_tone_detected;
    static char fall_counter;
    
        TWR_GPIO_init();
  	printf("\n TWRK20D50M Demo Software Rev 1.4 \n");                
        printf("\n >"); 
        uart_app_init();
        buzzer_ftm_init(); //buzzer init
        PIT_init();
        rtc_init();
        accel_init();
        adc_test_init();
        tsi_test_init();
        irda_test_init();
        
        tone_filter_init();
        
        next_task(vfn_null);
        ti_fall = 3000;  //waits a least 3 seconds before detect falls
         
        cmt_init();
        EnableInterrupts;
        tone(_C7 ,300,10);
        while(tone_enable){};
        tone(_A5,300,10);
        while(tone_enable){};

        cmt_init();
       
 
	while(1)
	{
          
            ptr_next_task();  // do the actual function 
            vfn_test_accel();
            switch_test();
            tsi_test_service(); 
            /* tone detection */
          if (input_rise(tone_detected,&aux_tone_detected) && !tone_enable && !ti_tone_wait)
            {
               ti_tone_wait = 800;
               tone(_A5,180,10);
               printf("\n\r tone detected %3i\n\r ",tone_counter);
            } else  if (tone_detected==1 && !ti_tone_wait) tone_detected=0; 
              
            
            
           // Clap detection 
           if (input_rise(clap_detected,&aux_clap_detected) && !tone_enable && !ti_tone_wait)
            {
               ti_tone_wait = 400;
               tone(_A4,100,10);
               printf("\n\r clap/snap detected  %3i \n\r ",clap_counter);
               
            } else  if (clap_detected==1 && !ti_tone_wait) clap_detected=0; 
              
        
            
            /*fall detection */ 
         if (input_rise(fall_input,&aux_fall_input))
         {
           printf("\n\r i'm falling!!! %i \n\r",fall_counter++);
           ti_delay = 1500;
           tone(_A6,400,10);
         }
         
  if (input_rise(base_time125m&0x1,&auxclapmed))   
   {
    if (clap_energy>65535*2) clap_energy= 65535*2;
    energy_scaled = (clap_energy/2);
    median_energy = median(energy_scaled, &arr_medianasClap[0]);
   }

         
         
             
        } 
}


/*do nothing*/
void vfn_null(void)
{

}

void vfn_test_accel(void)
{
               if (ti_accel_sampling==0)
             {
               accel_read();              
               ti_accel_sampling = 10;            
               } 

}



/**
 * \brief   Turn on LED1 and turn off LED2 when SW2 is pressed, and invert when de-pressed
 * \brief   Turn on LED3 and turn off LED4 when SW3 is pressed, and invert when de-pressed
 * \author  b01252
 * \param  none
 * \return none
 */  


void  switch_test(void)
{
  static char aux_sw1, aux_sw2, sw1_count, sw2_count;
        if (input_rise(SW1_ON,&aux_sw1))
           {
              printf("\n\r push_button (SW3)  %i \n\r",sw1_count++);
              tone(_C6,100,10);                    
          }
  
  
        if (input_rise(SW2_ON,&aux_sw2))
           {
              printf("\n\r push_button (SW2) %i \n\r",sw2_count++);
              tone(_E6,100,10);                    
          }
        
        if (ti_test_leds==0)
        {   
/*          
         if (SW1_ON) LED1_ON; else LED1_OFF;
         if (SW2_ON) LED2_ON; else LED2_OFF;    
         if (ELECTRODE1_ON) LED3_ON; LED3_OFF;
         if (ELECTRODE2_ON) LED4_ON; LED4_OFF;
  */
         if (SW1_ON) LED3_ON; else LED3_OFF;
         if (SW2_ON) LED4_ON; else LED4_OFF;    
         if (ELECTRODE1_ON) LED1_ON; LED1_OFF;
         if (ELECTRODE2_ON) LED2_ON; LED2_OFF;

          
        

        }
        
}        




void accel_init(void)
{
    unsigned char tmp;
    printf("****Accelerometor init ****\n");
    //Initialize I2C modules
    hal_i2c_init();
   //Configure MMA8451 sensor //MMA8451Q
    tmp = hal_dev_mma8451_read_reg(0x2a);
    hal_dev_mma8451_write_reg(0x2a,tmp|0x01);
}



void tone_accel_demo(void)
{
  
    static int note,notez;
        note  = yz_angle/100;  //9-9
    
        if (!ti_delay && !tone_enable) buzzer_off();
        if (note != notez && xz_mag > 20 && !ti_delay)
        {
        if (note <-9) note=-9;
         if (note >9)  note =9;

         if (notez < note) notez++;
         if (notez > note) notez--;
         set_out_module(C_MAJ_SCALE[9 - notez]); 
         ti_delay = 75;
         buzzer_on();
        }
        

        
}

/*if read available update  accel_x,accel_y and accel_z */
void accel_read(void)
{
        if((hal_dev_mma8451_read_reg(0x00)&0xf) != 0)
        {
#if 0
            accel_x   = hal_dev_mma8451_read_reg(0x01)<<8;
            accel_x  |= hal_dev_mma8451_read_reg(0x02);
            accel_x >>= 2;

            accel_y   = hal_dev_mma8451_read_reg(0x03)<<8;
            accel_y  |= hal_dev_mma8451_read_reg(0x04);
            accel_y >>= 2;

            accel_z   = hal_dev_mma8451_read_reg(0x05)<<8;
            accel_z  |= hal_dev_mma8451_read_reg(0x06);
            accel_z >>= 2;
#endif
            resultx   = hal_dev_mma8451_read_reg(0x01)<<8;
            resultx  |= hal_dev_mma8451_read_reg(0x02);
            resultx >>= 8;

            resulty   = hal_dev_mma8451_read_reg(0x03)<<8;
            resulty  |= hal_dev_mma8451_read_reg(0x04);
            resulty >>= 8;

            resultz   = hal_dev_mma8451_read_reg(0x05)<<8;
            resultz  |= hal_dev_mma8451_read_reg(0x06);
            resultz >>= 8;
            
            
            
            angle_calculation(); //-900  to  900            
            detect_fall_detection();
            
            tone_accel_demo();
            
            
        }
        
    
}



