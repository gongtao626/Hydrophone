/******************************************************************************/
/* RETARGET.C: 'Retarget' layer for target-dependent low level functions      */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005-2006 Keil Software. All rights reserved.                */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

#include <stdio.h>
#include <rt_misc.h>

#include "common.h"
#include "uart.h"

#ifndef CONSOLE_SUPPORT

#pragma import(__use_no_semihosting_swi)


extern void uart_putchar (UART_MemMapPtr, char);
extern char uart_getchar (UART_MemMapPtr channel);


struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;


int fputc(int ch, FILE *f) {
  if( ch == '\n')
    uart_putchar(TERM_PORT,'\r');//return carriage
  uart_putchar(TERM_PORT,ch);/*doesnt return error number*/
  return 0;
}

int fgetc(FILE *f) {
  return uart_getchar(TERM_PORT);//return carriage
}

int ferror(FILE *f) {
  /* Your implementation of ferror */
  return EOF;
}


void _ttywrch(int ch) {
    if( ch == '\n')
      uart_putchar(TERM_PORT,'\r');//return carriage
    uart_putchar(TERM_PORT,ch);/*doesnt return error number*/
}


void _sys_exit(int return_code) {
label:  goto label;  /* endless loop */
}

#endif
