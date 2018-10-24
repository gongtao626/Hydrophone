/****************************************************************************************************/
/**
Copyright (c) 2008 Freescale Semiconductor
Freescale Confidential Proprietary
\file       tsi_test.c
\brief      configure tsi to read TSI CH6 and TSI CH0,
\author     FSL
\version    1.0
\date       Sep 29 2011
*/

#include "tsi.h"
#include "buzzer.h"


/********************************************************************/
void tsi_test_init(void)
{
    printf("****Kinetis TSI Demo****\r\n");
    TSI_Init();
    TSI_SelfCalibration();
    
    START_SCANNING;
    ENABLE_EOS_INT;
    ENABLE_TSI;
}   
      




  /***
param lValidTouch   per electrode bit


********************/
void  tsi_test_service(void)
{
  if(lValidTouch&((1<<ELECTRODE0)))
  {
      lValidTouch &= ~((1<<ELECTRODE0));
      printf("\n\r Electrode 1  \n\r");     
      tone(_G6,100,10);      
  }
  if(lValidTouch&((1<<ELECTRODE1)))
  {
      printf("\n\r Electrode 2 \n\r");
      lValidTouch &= ~((1<<ELECTRODE1));
      tone(_C7,100,10);
  }
  if(lValidTouch&((1<<ELECTRODE2)))
  {
    printf("\n\r Electrode 3  \n\r");
    lValidTouch &= ~((1<<ELECTRODE2));
    
    
  }
  if(lValidTouch&((1<<ELECTRODE3)))
  {
    printf("Electrode 4  \n\r");
    lValidTouch &= ~((1<<ELECTRODE3));
  }
}