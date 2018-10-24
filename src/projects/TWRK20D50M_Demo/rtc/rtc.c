
/**
Copyright (c) 2008 Freescale Semiconductor
Freescale Confidential Proprietary
\file       rtc.h
\brief      real time clock basic functionality
\author     Freescale Semiconductor
\version    1.0
\date       Sep 14, 2011
*/

#include "common.h"
#include "rtc.h"


/**  rtc_init
 * \brief    Initialize RTC module to count seconds
 * \author   
 * \param none
 * \return none
 */ 




time_date_t *current_time = (time_date_t *)(0x4003E000); //initialize current time pointer to FVBAT registers
//time_date_t  RFVBAT_BASE_PTR //0x4003E000u

//#define ap_time (*time_date_t)(time_date_t)FVBAT_BASE_PTR

void default_time_date(void)
 {
/*   time_date_t  *p_time;
   p_time = (time_date_t *)(0x4003E000);
   p_time->seconds = 59;*/
   
   current_time = (time_date_t *)(0x4003E000); //initialize current time pointer to FVBAT registers
   
   set_time(current_time, 12, 0, 0);
   set_time(current_time, 12, 0, 0);
   set_date(current_time, 14, 2, 2012);
 }



void set_time(time_date_t * td, char hr, char min, char sec)
{
  td->hours   = hr;
  td->minutes = min;
  td->seconds = sec;
}


/**/
void set_date(time_date_t * td, char day, char month, int year)
{
  td->day   = day;
  td->month = month;
  td->year = year;
}

/* day number between 1-7*/
void set_daynumber(time_date_t * td, char weekday)
 {
   td->weekday   = weekday;
 }
            

#define YEAR_BASE 2012

/* YEATS */
typedef struct 
 {
   uint16 daysPerYear;
   uint16 ElapseddaysPerYear;
 } str_daysPerYear;

typedef struct {
        uint8    daysPerMonth;
        uint16   elapsedDays;
        uint8    bisdaysPerMonth; 
        uint16   biselapsedDays;        
       } str_day_per_month;


const  str_daysPerYear YEAR_ELAPSED_DAY[] =
{
  //Days per year,  elapsedYears
      366, 366 ,  //2012
      365, 731 ,  //2013
      365, 1096 ,  //2014
      365, 1461 ,  //2015
      366, 1827 ,  //2016
      365, 2192 ,  //2017
      365, 2557 ,  //2018
      365, 2922 ,  //2019
      366, 3288 ,  //2020
      365, 3653 ,  //2021
      365, 4018 ,  //2022
};




const str_day_per_month  DAYS_PER_MONTH[] =
{
//DaysPerMonth,ElapDays,BisDaysPerMonth,BisElapDays	
       31,  31,31,  31,  //"offset31
       28,  59,29,  60,  //"offset29
       31,  90,31,  91,  //"offset31
       30,  120,30,  121,  //"offset30
       31,  151,31,  152,  //"offset31
       30,  181,30,  182,  //"offset30
       31,  212,31,  213,  //"offset31
       31,  243,31,  244,  //"offset31
       30,  273,30,  274,  //"offset30
       31,  304,31,  305,  //"offset31
       30,  334,30,  335,  //"offset30
       31,  365,31,  366,  //"offset31
};


void seconds_to_date(uint32 elapsed_seconds)
{ uint32 elapsed_days;
  uint32 remaining_sec;
  uint32 elapsed_hours;
  uint32 elapsed_min;
  uint8  off_year;
  uint8  off_month;
 
  uint8  bisiesto;
  uint16 year;
  uint16 daysofyear;
  uint16 dayofmonth;  
  
  elapsed_days  = elapsed_seconds/(60*60*24);
  remaining_sec = elapsed_seconds%(60*60*24);
  
  elapsed_hours = remaining_sec/(60*60);
  remaining_sec = elapsed_seconds%(60*60);
  
  elapsed_min = remaining_sec/(60);
  elapsed_seconds = elapsed_seconds%(60);  
  
  //search for year
  off_year=0;   
  while(elapsed_days>YEAR_ELAPSED_DAY[off_year].ElapseddaysPerYear) off_year++;
  year = YEAR_BASE + off_year;
  bisiesto = !(YEAR_ELAPSED_DAY[off_year].daysPerYear & 0x1);
 
  //search for month and day of the month
  daysofyear = elapsed_days - YEAR_ELAPSED_DAY[off_year].daysPerYear;
  
  if (bisiesto)
   {
     off_month = 0;
     while (daysofyear > DAYS_PER_MONTH[off_month].biselapsedDays)off_month++;
     dayofmonth = daysofyear;
     if (off_month>0) dayofmonth = daysofyear - DAYS_PER_MONTH[off_month].biselapsedDays;
   }
  else
  {
     off_month = 0;
     while (daysofyear > DAYS_PER_MONTH[off_month].elapsedDays)off_month++;
     dayofmonth = daysofyear;
     if (off_month>0) dayofmonth = daysofyear - DAYS_PER_MONTH[off_month].elapsedDays;
  }
  
   set_time(current_time, elapsed_hours, elapsed_min, elapsed_seconds);
   set_date(current_time, year, off_month+1, dayofmonth);
 }

uint32 date_to_seconds(time_date_t * td)
 {
   
   char bisiesto;
   char off_year;
   uint16 dayofyear;
   uint32 elapsed_seconds=0;
   uint32 elapsed_days=0;
   
   
   off_year = td->year - YEAR_BASE;
   bisiesto = !(YEAR_ELAPSED_DAY[off_year].daysPerYear & 0x1);
   
   if (off_year>0)
   elapsed_days = YEAR_ELAPSED_DAY[off_year-1].ElapseddaysPerYear;
   
   
   if (bisiesto)
   {
    dayofyear = DAYS_PER_MONTH[td->month-1].biselapsedDays;
    dayofyear += td->day;
   }else{
    dayofyear = DAYS_PER_MONTH[td->month-1].biselapsedDays;
    dayofyear += td->day;
   }

   elapsed_days += dayofyear;
    
   elapsed_seconds = elapsed_days*(60*60*24) + td->hours*60*60 + td->minutes*60 + td->seconds;
   return elapsed_seconds;
   
 }


/* This funcion must be called every second*/
void clock_service(time_date_t * td)
{
   if (++td->seconds >59 )
   {
      td->seconds =  0;
      if (++td->minutes >59 ) 
      {
        td->minutes =  0;
        if (++td->hours >23 ) 
          {  
             td->hours = 0; 
             if (++td->weekday >7 ) 
             {
               td->weekday =1;
             } 
             td->day++;             
             if (td->month==2)
             { 
               if ((td -> year%4)  == 0)
               {
                if  (td->day > 29)      td->day = 1;
               }
               else  if (td->day > 28) td->day = 1; 
             }  
             else if (td->month==4 || td->month==6 || td->month==9  ||   td->month==11)
             {
              if (td->day  > 30) td->day = 1;
             }
             else // if (td->month==1 || td->month==3 || td->month==5 ||  td->month==7 || td->month==8  ||   td->month==10     ||  td->month==12)
             { 
               if (td->day > 31) td->day = 1;
             } 
             if ( td->hours == 0 && td->day==1) 
             { 
              if (++td->month > 12) 
              {
                td->month = 1; 
                td->year++;
              } //year 
             }//month
          }//hour
       }//minutes
      }//seconds
   }








uint32 seconds_count;

void rtc_init(void)
{
     SIM_SCGC6|=SIM_SCGC6_RTC_MASK;
     printf("Initialize RTC\n");
    //Check to see if RTC is already enabled, meaning the RTC battery backup
    //  saved the RTC values. If not enabled, then we have to initalize RTC
    if((RTC_SR&RTC_SR_TCE_MASK)==0)  //Time Counter Enable
    {
        default_time_date();
        RTC_TSR = 0x00;
        RFVBAT_REG7 = 0x00;
        //set the Timer Alarm Register to 0 to disable RTC flag
        RTC_TAR = 0x01;  // set this register to other than 01 so set TAF
       // enable the oscillator and set load capacitors
        RTC_CR |=  RTC_CR_OSCE_MASK ;
    }
    else
    {
        RTC_TAR = RTC_TSR + 1;
        printf("RTC already initialized  \n");
        RFVBAT_REG7 = RTC_TSR - RFVBAT_REG7;
        
    }
    enable_irq(INT_RTC-16);
    RTC_SR |= RTC_SR_TCE_MASK; // enable counter
   
}


void deinit_rtc(void)
{    
    disable_irq(INT_RTC-16);
}



void rtc_isrv(void)
{
    uint32 rtc_sr = RTC_SR;
    
    if(rtc_sr & RTC_SR_TAF_MASK) // RTC timer alarm flag is set
    {
      RTC_TAR = RTC_TSR + 1;     // write new value to TAR to clear TAF
      seconds_count++;
      clock_service(current_time);

      
    }
    
    if(rtc_sr & RTC_SR_TOF_MASK) // RTC timer Overlow flag is set
    {
     RTC_SR |= RTC_SR_TOF_MASK;
    }

    
    if (rtc_sr & RTC_SR_TIF_MASK) // Timer Invalid flag
    {
        RTC_SR &= ~RTC_SR_TCE_MASK;  //Disable timer
        RTC_TSR = 0x00; // write to clear TOF or TIF
        RTC_SR |= RTC_SR_TCE_MASK;   //re-enable timer
    }
}

