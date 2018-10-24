/* Support files for GNU libc.  Files in the system namespace go here.
   Files in the C namespace (ie those that do not start with an
   underscore) go in .c.  */

#include <stdio.h>
#include <errno.h>
#include <sys/unistd.h>
#include <sys/stat.h>

#include "common.h"
#include "uart.h"


int     _write 		_PARAMS ((int, char *, int));//

int putc(int ch, FILE *stream)
{
    (void)stream;
	uart_putchar(TERM_PORT,(unsigned int)ch);
	return ch;
}

int
_write (int    file,
	char * buffer,
	int    size)
{
	  size_t nChars = 0;

	  /*FSL: not checking file descriptor variable*/

	  if (buffer == 0)
	  {
	    /*
	     * This means that we should flush internal buffers.  Since we
	     * don't we just return.  (Remember, "handle" == -1 means that all
	     * handles should be flushed.)
	     */
	    return 0;
	  }

	  for (/* Empty */; size != 0; --size)
	  {
	    if( *buffer == '\n')
	      uart_putchar(TERM_PORT,'\r');//return carriage
	    uart_putchar(TERM_PORT,*buffer++);/*doesnt return error number*/
	    ++nChars;
	  }

	  return nChars;
}

/*
 read
 Read a character to a file.
 */
int
_read(int file, char *ptr, int len)
{
    int num = 0;

    /*FSL: not checking file descriptor variable*/
    *ptr = uart_getchar(TERM_PORT);
    num++;

    return num;
}

/*
 sbrk
 Increase program data space.
 Malloc and related functions depend on this
 */
caddr_t
_sbrk(int incr)
{
	  extern char __cs3_heap_start; // Defined by the linker
	  static unsigned char *heap = NULL;
	  unsigned char *prev_heap;

	  if (heap == NULL) {
	    heap = (unsigned char *)&__cs3_heap_start;
	  }
	  prev_heap = heap;

	  heap += incr;

	  return (caddr_t) prev_heap;
}

/************************Dummy Implementation*********************************/

#if 1
int
_close(int file)
{
    return -1;
}

/*
 fstat
 Status of an open file.
 */
int
_fstat(int file, struct stat *st)
{
    st->st_mode = S_IFCHR;
    return 0;
}

/*
 isatty
 Query whether output stream is a terminal
 */
int
_isatty(int file)
{
    switch (file){
    case STDOUT_FILENO:
    case STDERR_FILENO:
    case STDIN_FILENO:
        return 1;
    default:
        //errno = ENOTTY;
        errno = EBADF;
        return 0;
    }
}

/*
 lseek
 Set position in a file.
 */
int
_lseek(int file, int ptr, int dir)
{
    return 0;
}
#endif


