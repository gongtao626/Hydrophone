/**
  Copyright (c) 2006 Freescale Semiconductor
  Freescale Confidential Proprietary
  \file     	task_mgr.h
  \brief    	task manger functions
  \author   	Freescale Semiconductor
  \author     Luis Puebla
  \author   	Guadalajara Applications Laboratory RTAC Americas
  \version    1.0
  \date     	 13/jun/2007
  \warning    (If needed)

  * History:
  
*/


/*
*      main()
*      {
*        ... 
*        init_task_mgr();  // set the first task
*         ***
*       for(;;;)
*         {
*           ...
*           ptr_next_task();  // do the actual function 
*           ...
*          }
*        
*       }

* Board  : DEMO9S08QG8
*
*******************************************************************************/


#include "common.h" /* include peripheral declarations */

/*  Task manager */


#define next_task(funcion) ptr_next_task = funcion 
#define next_task_t(funcion,time) ptr_next_task = funcion, timer_task=time
#define timer_task_expires()   !timer_task 

extern void (*ptr_next_task)(void); //Apuntador a tareas
extern void (*resp_ptr_next_task)(void); //Respaldo para anuncios 

extern unsigned long timer_task;
