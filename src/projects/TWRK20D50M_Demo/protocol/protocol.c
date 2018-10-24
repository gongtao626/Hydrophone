/****************************************************************************************************/
/**
Copyright (c) 2011 Freescale Semiconductor
Freescale Confidential Proprietary
\file    protocol.c
\brief   check for sci input data and process command
\brief   command format >Tnn CR 
\author     
\author     
\version    1.0
\date       Sep 26, 2011
*/
#include "common.h"
#include "uif.h"
#include "stdio.h"
#include "uart.h"


#include "TWR_K20D50M_DEF.h"
#include "buzzer.h"
#include "rtc.h"
#include "hal_i2c.h"
#include "hal_dev_mma8451.h"
#include "adc_test.h"
#include "angle_cal.h"
#include "fft_test.h"
#include "irda_test.h"
#include "protocol.h"
#include "median.h"
#include "task_mgr.h"
#include "irda_test.h"
#include "tsi.h"
#include "cmt_test.h"


/* Test functions */

void vfn_tsk_rtc(void);
void vfn_tsk_buttons(void);
void vfn_tsk_adc(void);
void vfn_tsk_accel(void);
void vfn_tsk_accel(void);
void vfn_tst_dma_stat(void);
void vfn_tst_fft(void);
void vfn_tst_harmonic(void);
void vfn_tst_adc_buf(void);
void vfn_tst_irda(void);
void vfn_tst_tone_detec(void);
void vfn_tsk_null(void);
void vfn_tsk_leds(void);
void vfn_tst_clap_detec(void);

char auxdisp1;

void (*pfunc[])(void)=
 {
   vfn_tsk_null, //0
   vfn_tsk_rtc,  //1   32 KHz
   vfn_tsk_buttons, //2  SW1, SW 2 y electrodes
   vfn_tsk_leds, //3  
   vfn_tsk_accel,  //4
   vfn_tst_irda,     //5
   vfn_tst_adc_buf,  //6
   
   vfn_tst_harmonic, //7
   vfn_tst_dma_stat, //8
   vfn_tst_clap_detec,//9
   vfn_tst_fft,     //10
   vfn_tst_tone_detec, //11
 };


#define DISPLAY_FLAG1  input_rise(base_time125m&0x1,&auxdisp1)
#define DISPLAY_FLAG  input_rise(base_time125m&0x2,&auxdisp1)

extern void display_mode(char mode);
char irda_tx_counter;


struct tipo_mediana arr_medianas2[2];

struct tipo_mediana arr_medianasClap[2];

uint16 median_adc;
uint16 max_adc;


char myCmdline1 [80];
extern char cmdline2 [];
const char SYNTAX[] = "Syntax error";
const char INVCMD[] = "Invalid command";

const char BANNER[] = "\n TWR-K20D50M Test 01 \n";
const char PROMPT[] = "\r\n K20_Demo >";
void   run_cmd_not_blocking(char *line);

/*UIF Function prototypes */
void uif_cmd_disp (int argc, char **argv );
void lcd_print (int argc, char **argv ); //Print a message in the LCD
void uif_cmd_time (int argc, char **argv );
void uif_cmd_date (int argc, char **argv );

void   print_lcd_status(void);

UIF_CMD UIF_CMDTAB[] =
{
         {  "help" ,0 ,1 ,0 ,uif_cmd_help ,"Display this help" ,"<>"  },
         {  "Disp" ,0 ,1 ,0 ,uif_cmd_disp ,"select display fucntion" ,"disp nn"  },
         {  "time" ,0 ,4 ,0 ,uif_cmd_time ,"select set/display time" ,"time hr min sec "  },
         {  "date" ,0 ,4 ,0 ,uif_cmd_date ,"select set/display date" ,"date dd mm yyyy "  },

  };


UIF_SETCMD UIF_SETCMDTAB[] =
{
    {"sysclk", 0,1,lcd_print,""},
};



const int UIF_NUM_CMD    = UIF_CMDTAB_SIZE;
const int UIF_NUM_SETCMD = UIF_SETCMDTAB_SIZE;


void
uif_cmd_disp (int argc, char **argv )
 {
    uint32  result;
   
    if (argc>1)
    {  
     sscanf(argv[1],"%i",&result);
     if (result>sizeof(pfunc)) result=0;
      next_task(*pfunc[result]);
    }
    else
     {
   printf ("\n\r Available functions: disp n");
   printf ("\n\r 0: restore display"); //0
   printf ("\n\r 1: rtc  nnn        ; shows RTC seconds");  //1   32 KHz
   printf ("\n\r 2: Electrodes/sw B nn ; shows which Electrode/button us pressed "); //2  SW1, SW 2 y electrodes
   printf ("\n\r 3: leds         Led nn "); //3  
   printf ("\n\r 4: Accelerometer  Show  [magnitude] and <angle in degrees*10 ");  //4
   printf ("\n\r 5: irda  IR port:  Tx =   2,     Rx=   0 ");    //5
   printf ("\n\r 6: ADC Potentiometer  POT = 1425");  //6  
   printf ("\n\r 7: FFT harmonics  fft[ harmonic N]Macgnitude N,.... "); //7
   printf ("\n\r 8: DMA  transactions, errors "); //8
   printf ("\n\r 9: Clap analysis "); //8
   printf ("\n\r"); //8
//   printf ("\n\r 9 vfn_tst_fft");     //9
//   printf ("\n\r 10 vfn_tst_tone_detec, //10      
     }
    }



void
uif_cmd_time (int argc, char **argv )
 {
    uint32  result;
    char sec,min,hr,error=0;
   
    if (argc > 3)
    {  
      sscanf(argv[1],"%i",&result);
       hr = result;
       if (hr>23) error = 1;
      
      sscanf(argv[2],"%i",&result);
      min = result;
      if (min>59) error = 1;

      sscanf(argv[3],"%i",&result);      
      sec = result;
      if (sec>59) error = 1;
      if (!error)set_time(current_time, hr, min, sec);
    }
    else
    {
      printf ("\n\r time   hr mm ss");
      
    }
      printf("time: %02i:%02i:%02i  %02i/%02i/%02i ",current_time->hours, current_time->minutes, current_time->seconds,
      current_time->day, current_time->month, current_time->year );    
 }


void
uif_cmd_date (int argc, char **argv )
 {
    uint32  result;
    char day, month, error=0;
    int year;
   
    if (argc > 3)
    {  
      sscanf(argv[1],"%i",&result);
      day = result;
      if (day<1 || day>31) error = 1;

      sscanf(argv[2],"%i",&result);
      month = result;
      if (month<1 || month>12) error = 1;

      sscanf(argv[3],"%i",&result);      
      year = result;
      if (year<2000 || year>2100) error = 1;
      if (!error) set_date(current_time, day, month, year);
      else printf("\n\r error format date ");
    }
    else
    {
      printf ("\n\rformat error, use date dd mm yyyy");
      
    }
      printf("time: %02i:%02i:%02i  %02i/%02i/%02i ",current_time->hours, current_time->minutes, current_time->seconds,
      current_time->day, current_time->month, current_time->year );    
 }



#include "common.h"






/****

Return 0 
Return 1 if line is complete

line is the buffer where the command is received

****************************************************************/
char 
get_line2 (char ch, char *line)
{
  static int pos;     
    if (ch == 27 /* LF/NL */)
     {line[0] = 'D';
      line[1] = 'i';
      line[2] = 's';
      line[3] = 'p';
      line[4] = ' ';      
      line[5] = '0';
      pos = 6;
     }
    if ( (ch != 0x0D /* CR */) &&
         (ch != 0x0A /* LF/NL */) &&
         (ch != 27 /* LF/NL */) &&  
         (pos < UIF_MAX_LINE))
    {
        switch (ch)
        {
            case 0x08:      /* Backspace */
            case 0x7F:      /* Delete */
                if (pos > 0)
                {
                    pos -= 1;
                    out_char(0x08);    /* backspace */
                    out_char(' ');
                    out_char(0x08);    /* backspace */
                }
                break;
            default:
                if ((pos+1) < UIF_MAX_LINE)
                {
                    if ((ch > 0x1f) && (ch < 0x80))
                    {
                        line[pos++] = (char)ch;
                        out_char((char)ch);
                    }
                }
                break;
        }
    }
    else
    {  
     line[pos] = '\0';
     out_char(0x0D);    /* CR */
     out_char(0x0A);    /* LF */
     pos=0; // prepare for next command
     return 1;
    } 
   return 0; 
}

/*
  Check if there a character from UART
  if a character is available procces it
  if a line is complete
*/
void
run_cmd_not_blocking (char *cmdline1)
{
    /*
     * Global array of pointers to emulate C argc,argv interface
     */
    int argc;
    char cmdline2[80];
    char *argv[UIF_MAX_ARGS + 1];   /* one extra for null terminator */
    
    
    if (!(argc = make_argv(cmdline1,argv)))
    {
        /* no command entered, just a blank line */
        strcpy(cmdline1,cmdline2);
        argc = make_argv(cmdline1,argv);
    }
    cmdline2[0] = '\0';

    if (argc)
    {
        int i;
        for (i = 0; i < UIF_NUM_CMD; i++)
        {
            if (strcasecmp(UIF_CMDTAB[i].cmd,argv[0]) == 0)
            {
                if (((argc-1) >= UIF_CMDTAB[i].min_args) &&
                    ((argc-1) <= UIF_CMDTAB[i].max_args))
                {
                    if (UIF_CMDTAB[i].flags & UIF_CMD_FLAG_REPEAT)
                    {
                        strcpy(cmdline2,argv[0]);
                    }
                    UIF_CMDTAB[i].func(argc,argv);
                    printf(PROMPT);
                    return;
                }
                else
                {
                    printf(SYNTAX,argv[0]);
                    return;
                }
            }
        }
        printf(INVCMD,argv[0]);
        printf(HELPMSG);
    }
}



/*
  provides a UART RX service
*/


void vfnUART_Rx_Isrv(void)
{
  char RxCh;
    while (!(UART_S1_REG(TERM_PORT) & UART_S1_RDRF_MASK));  //  To clear proper flag
    /* Return the 8-bit data from the receiver */
    RxCh = UART_D_REG(TERM_PORT);
    if (get_line2(RxCh,myCmdline1)) run_cmd_not_blocking(myCmdline1);
  
}


/*
initialize UART to work with Reception interrupts
*/

void  uart_app_init(void)
{
    enable_irq(INT_UART1_RX_TX - 16);
    UART_C2_REG(TERM_PORT)|= UART_C2_RIE_MASK;

}   

/**************************************************/

void vfn_tsk_null(void)
{
};


void vfn_tsk_leds(void)
{
  static char led_count;
   // Pin multilplexing configuration  J7 ON,  J4 OFF
  if (((PORTC_PCR7 & PORT_PCR_MUX_MASK)>>PORT_PCR_MUX_SHIFT)!= 1)
  {
    PORTC_PCR7 &= ~PORT_PCR_MUX_MASK;  
    PORTC_PCR7 |= PORT_PCR_MUX(1);  // pin for   LED output
   };
  
  ti_test_leds = 2000;
  if (DISPLAY_FLAG)
  {
    led_count++;
    printf("\r LEDs %02x  ",led_count);
    if  (led_count&0x01) LED1_ON;else LED1_OFF;  //Orange
    if  (led_count&0x02) LED2_ON;else LED2_OFF;  //Yellow
    if  (led_count&0x04) LED4_ON;else LED4_OFF;  //blue
    if  (led_count&0x08) LED3_ON;else LED3_OFF;  //green
  }
  
  
}


void vfn_tsk_rtc(void)
{
  printf("\r rtc %4i  ;   elapsed time with Vbat %6i", seconds_count,RTC_TSR);
 /* 
  printf("time: %02i:%02i:%02i  day,mm,year: %02i/%02i/%02i  delta seg = %i",current_time->hours, current_time->minutes, current_time->seconds,
         current_time->day, current_time->month, current_time->year,RFVBAT_REG7 );
   */
  
}




void vfn_tsk_buttons(void)
{ char buttons=0;
  static char aux1, aux2, aux3, aux4;

 
  if (SW1_ON) buttons |= 0x01;
  if (SW2_ON) buttons |= 0x02;
  if (ELECTRODE1_ON) buttons |= 0x04;
  if (ELECTRODE2_ON) buttons |= 0x08;
  
  
  if (DISPLAY_FLAG)
  printf("\r B %02x", buttons);
  
  if (input_rise(SW1_ON,&aux1)) tone(_C5,160,10);    
  if (input_rise(SW2_ON,&aux2)) tone(_E5,160,10);    
  if (input_rise(ELECTRODE1_ON,&aux3)) tone(_G5,160,10);    //tone on tsi_test_service
  if (input_rise(ELECTRODE2_ON,&aux4)) tone(_C6,160,10);    //tone on tsi_test_service
}                       




void vfn_tsk_adc(void)
{

         median_adc = median(adc_buffer[0], &arr_medianas2[0]);
         if (adc_buffer[0]>max_adc)max_adc = adc_buffer[0];
         
         if (DISPLAY_FLAG)
         printf("\r adc = %04i, med= %04i , Max = %i" ,adc_buffer[0],median_adc,max_adc);
}



void vfn_tsk_accel(void)
{
  if (DISPLAY_FLAG1)
  printf("\r xy=[%3d]<%3d,  xz=[%3d]<%3d,  yz =[%3d]<%3d ",xy_mag, xy_angle,xz_mag, xz_angle,yz_mag, yz_angle);
}

void vfn_tst_dma_stat(void)
{
  if (DISPLAY_FLAG)
  printf("\r Status DMA %i,  %i,   ",dmaIsrCount, DMA_errorCount);
}

void vfn_tst_fft(void)
{
  if (DISPLAY_FLAG)
  display_fft_info();    
}


void vfn_tst_harmonic(void)
{   int i;
   static char aux3;
   static char harmonic_group;
// ´print initial 10 harmonics
     // printf("\033[2J"); Home and clear screen
     if (DISPLAY_FLAG)
     {  
      printf("\r fft");
       if (input_rise(SW1_ON,&aux3)) 
       {
         harmonic_group+=8;
         if(harmonic_group>=64)harmonic_group=0;
       }
      for (i=1;i<9;i++)
      {   
        printf("[%2i]%4i,",i+harmonic_group,rfft128Magnitud[i+harmonic_group]);
      }
     } 
}

void vfn_tst_adc_buf(void)
{   

     ti_test_adc = 2000;   // force to read POT signal while in this tsk
     if (DISPLAY_FLAG)
     printf("\r ADC  POT = %05i  ;", adc_buffer[0]);
      
}





void vfn_tst_irda(void)
{
  static char aux2;
  static char rx_ch;
  char ch;

       // Pin multilplexing configuration  J4 OFF  J7 ON
  if (((PORTC_PCR7 & PORT_PCR_MUX_MASK)>>PORT_PCR_MUX_SHIFT)!= 0)
  {
    cmt_init();
    irda_test_init();
   };
  
     if (input_rise(seconds_count&0x1,&aux2))
       {
     //   irda_tx(++irda_tx_counter);
        cmt_tx_data(++irda_tx_counter);
        }
        ch = irda_get_char();
        if (ch) rx_ch = ch;  //%store last received value
        
        if (DISPLAY_FLAG1)
        printf("\r IR port:  Tx = %03i,     Rx= %03i ;",irda_tx_counter, rx_ch);      
  
}


void vfn_tst_tone_detec(void)
{   char i;
    // tone_filter.filter_output
    // clap_filter.filter_output
      printf("\r Max ");
      for (i=0;i<6;i++)
      {
        printf(" %i,", peak_table[i].magnitud);
      }
     //   printf("Tone detection T= %i, C = %i, filter %i",tone_filter.filter_output, clap_filter.filter_output,  clap_filter.filter_count);
      
}



void vfn_tst_clap_detec(void)
{   

    
  if (DISPLAY_FLAG1)   
  {
   printf("\r Clap Peak energy=%7i  energy=%7i    median %5i clap count= %3i", clap_peak_energy/2,clap_energy/2,median_energy,clap_counter);  
  }
  

  
}
