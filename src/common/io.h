/*
 * File:		io.h
 * Purpose:		Serial Input/Output routines
 *
 */

#ifndef _IO_H
#define _IO_H

/********************************************************************/
/*
char	
in_char(void);

void
out_char(char);
*/
#define in_char()      getchar()
#define out_char(x)   putchar(x)


int
char_present(void);

int		
printf(const char *, ... );

int
sprintf(char *, const char *, ... );

/********************************************************************/

#endif
