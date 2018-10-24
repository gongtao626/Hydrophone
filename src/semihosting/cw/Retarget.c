/* CW retarget support  */

#include <stdio.h>
#include <errno.h>

#include "common.h"
#include "uart.h"

#ifndef CONSOLE_SUPPORT

/*FSL: single character*/
void sys_writec(register void *cp)
{
	uint8 *ch = (uint8 *)cp;
	
    if( *ch == '\n' )
      uart_putchar(TERM_PORT,'\r');//return carriage
    uart_putchar(TERM_PORT,*ch);/*doesnt return error number*/	
}

/*FSL: multiple characters*/
void sys_write0(register void *str)
{
	  uint8 *ch = (uint8 *)str;
	
	  while(*ch != NULL)/*FSL: wait until end of string*/
	  {
	    if( *ch == '\n' )
	      uart_putchar(TERM_PORT,'\r');//return carriage
	    uart_putchar(TERM_PORT,*ch++);/*doesnt return error number*/
	  }
}

/*FSL: single character*/
unsigned char sys_readc(void)
{
    return uart_getchar(TERM_PORT);//return carriage
}

#endif
