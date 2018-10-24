/*
 * File:		uart_basic_test.c
 * Purpose:		UART basic transmit and receive test.
 *
 */

#include "common.h"
#include "uart.h"
#include "uart_tests.h"

/********************************************************************/
/* UART module basic transmit and receive test.
 * Tests all of the UART modules with the exception of the main TERM_PORT.
 * The TERM_PORT is being used for I/O for this test and all the others
 * so it does not need to be explicitly tested. 
 * 
 * NOTE: A second COM port on the PC is needed for this test. It should be 
 * configured for 8-N-1 at the TERMINAL_BAUD.
 * 
 * If user input is not correct (indicating incorrect transmit and/or incorrect
 * receive), then the test returns the number of errors. Otherwise returns 
 * 0 (pass).
 */
int uart_basic_test()
{
    UART_MemMapPtr module;
    char ch;
    int error = 0;
        
    /* Pointer for accessing a specific UART. Set to UART0 to start as long
     * as UART0 isn't the TERM_PORT. Otherwise start with UART1.
     */
    if (TERM_PORT != UART0_BASE_PTR)
      module = UART0_BASE_PTR;
    else
      module = UART1_BASE_PTR;   
    
    printf("\nStarting UART basic transmit and receive test.\n");
    
       printf("\nA second PC terminal is required for this test.");
       printf("\nPlease open a second 8-N-1 terminal at the same baud as the main terminal.");
       printf("\nThe second terminal should be connected to UART1 on the baseboard.");
       printf("\nTerminal 1 will give directions including connections to make on a Pioneer0 card.");
       printf("\nTerminal 2 will be used for communication with the UART module under test.");
       printf("\nPress any key when ready.\n");
       ch = in_char();
        
    /* Run basic transmit and receive test on all UARTs. */
     while( module )
    {  
        /* Give directions for connecting up the UART module currently under test */
        if (module == UART0_BASE_PTR)
        {  
            printf("\n\nTesting UART 0...");
            printf("\nConnect UART0 Rx and Tx signals");
        }  
        if (module == UART1_BASE_PTR)
        {  
            printf("\n\nTesting UART 1...");
            printf("\nConnect UART1 Rx and Tx signals");
        }  
        if (module == UART2_BASE_PTR)
        {  
            printf("\n\nTesting UART 2...");
            printf("\nConnect UART2 Rx and Tx signals");
        }
        
        printf("\nPress any key on terminal 1 once connections are complete.\n");
        ch = in_char();
        
        uart_putchar (module, 0xD);   // output a carriage return 
        uart_putchar (module, 0xA);   // output a line feed 
         /* Output characters "0123456789" to UART under test */
        for(ch = 0x30; ch <= 0x39; ch++)
            uart_putchar (module, ch);  
        uart_putchar (module, 0xD);   // output a carriage return 
        uart_putchar (module, 0xA);   // output a line feed 
        
        printf("\nThe characters \"0123456789\" should be displayed on terminal 2.");
        printf("\n\nPress SPACE on terminal 2 if characters are correct.\n");
        
        /* Wait for input on terminal 2 (UART under test) */
        ch = uart_getchar(module);
        
        /* If the input character isn't SPACE (0x20), then that indicates an 
         * error in the transmit and/or the recieve.
         */
        if (ch != 0x20)
        {  
            printf("\nERR! Incorrect input character received. Error counter has been incremented.\n");
            error++;
        }            
                    
        /* Advance the mem map ptr to the next available UART module to test */
        if (module == UART0_BASE_PTR)
        {  
          if (TERM_PORT == UART1_BASE_PTR)
              module = UART2_BASE_PTR;
          else
              module = UART1_BASE_PTR;
        }
        else if (module == UART1_BASE_PTR)
        {
          if (TERM_PORT == UART2_BASE_PTR)
              /* All common UART registers have been checked, so drop out of the loop */
              module = 0;
          else
              module = UART2_BASE_PTR;
        }
        else if (module == UART2_BASE_PTR)
        {  
          /* All common UART registers have been checked, so drop out of the loop */
          module = 0;
        }
    } 
    
    return error;    
} 
/********************************************************************/
   
