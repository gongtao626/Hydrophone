/*
 * File:		uart_register_tests.c
 * Purpose:		UART register value test routines.
 *
 */

#include "common.h"
#include "uart.h"

/********************************************************************/
/* UART module register reset value test.
 * Compares each register value to the documented reset value for all
 * of the UART modules. If all values match the test passes and function
 * returns 0 otherwise returns the number of errors.
 * 
 * NOTE: Since one of the UARTs is already initialized in the sysinit,
 * the reset values for that UART cannot be tested here. This test
 * will skip over the registers that are impacted for the UART designated
 * as the terminal port. The reset values for the BDL, C2, S1, SFIFO, and
 * TCFIFO registers should be checked manually for the terminal port UART.
 */
int uart_reg_rst(void)
{
    int error = 0;
    
    /* Pointer for accessing a specific UART. Set to UART0 to start */
    UART_MemMapPtr module = UART0_BASE_PTR;
        
    printf("\nStarting UART register reset value tests.\n");
        
    /* Start off by testing all of the registers that are in common between
     * all of the UARTs. The 7816 and FIFO registers vary depending on the 
     * UART instantiation, so we'll test those last.
     */
    
    
    while( module )
    {  
      if (UART_BDH_REG(module) != 0x0 )  
      {
          error++;
          printf("ERR! UART_BDH value is incorrect on UART at 0x%08X: %02X\n", module, UART_BDH_REG(module));
      }
      /* This register is modified as part of the UART init so, if we are testing
       * the terminal port UART right now, we have to skip over this register.
       */
      if (module != TERM_PORT)
      {
        if (UART_BDL_REG(module) != 0x4 )  
        {
            error++;
            printf("ERR! UART_BDL value is incorrect on UART at 0x%08X: %02X\n", module, UART_BDL_REG(module));
        }
        if (UART_C1_REG(module) != 0x0 ) 
        {
          error++;
          printf("ERR! UART_C1 value is incorrect on UART at 0x%08X: %02X\n", module, UART_C1_REG(module));
        }
      }
      /* This register is modified as part of the UART init so, if we are testing
      * the terminal port UART right now, we have to skip over this register.
      */
      if (module != TERM_PORT)
        if (UART_C2_REG(module) != 0x0 ) 
        {
            error++;
            printf("ERR! UART_C2 value is incorrect on UART at 0x%08X: %02X\n", module, UART_C2_REG(module));
        }
      /* This register is modified as part of the UART init so, if we are testing
       * the terminal port UART right now, we have to skip over this register.
       */
      if (module != TERM_PORT)
      {
      if (UART_S1_REG(module) != 0xC0 )  
      {
            error++;
            printf("ERR! UART_S1 value is incorrect on UART at 0x%08X: %02X\n", module, UART_S1_REG(module));
      }
      if (UART_S2_REG(module) != 0x0 )  
      {
          error++;
         printf("ERR! UART_S2 value is incorrect on UART at 0x%08X: %02X\n", module, UART_S2_REG(module));
      }
      if (UART_C3_REG(module) != 0x0 )  
      {
          error++;
         printf("ERR! UART_C3 value is incorrect on UART at 0x%08X: %02X\n", module, UART_C3_REG(module));
      }
      if (UART_MA1_REG(module) != 0x0 )  
      {
          error++;
          printf("ERR! UART_MA1 value is incorrect on UART at 0x%08X: %02X\n", module, UART_MA1_REG(module));
      }
      if (UART_MA2_REG(module) != 0x0 )  
      {
          error++;
          printf("ERR! UART_MA2 value is incorrect on UART at 0x%08X: %02X\n", module, UART_MA2_REG(module));
      }
      }
      /* This register is modified as part of the UART init so, if we are testing
       * the terminal port UART right now, we have to skip over this register.
       */
      if (module != TERM_PORT)
      {
      if (UART_C4_REG(module) != 0x0 )  
      {
          error++;
          printf("ERR! UART_C4 value is incorrect on UART at 0x%08X: %02X\n", module, UART_C4_REG(module));
      }
      if (UART_C5_REG(module) != 0x0 )  
      {
          error++;
         printf("ERR! UART_C5 value is incorrect on UART at 0x%08X: %02X\n", module, UART_C5_REG(module));
      }
      if (UART_ED_REG(module) != 0x0 )  
      {
          error++;
         printf("ERR! UART_ED value is incorrect on UART at 0x%08X: %02X\n", module, UART_ED_REG(module));
      }
      if (UART_MODEM_REG(module) != 0x0 )  
      {
          error++;
          printf("ERR! UART_MODEM value is incorrect on UART at 0x%08X: %02X\n", module, UART_MODEM_REG(module));
      } 
      if (UART_IR_REG(module) != 0x0 )  
      {
          error++;
         printf("ERR! UART_IR value is incorrect on UART at 0x%08X: %02X\n", module, UART_IR_REG(module));
      }
      if (UART_CFIFO_REG(module) != 0x0 )  
      {
          error++;
         printf("ERR! UART_CFIFO value is incorrect on UART at 0x%08X: %02X\n", module, UART_CFIFO_REG(module));
      }
      }
      /* This register is modified as part of the UART init so, if we are testing
       * the terminal port UART right now, we have to skip over this register.
       */
      if (module != TERM_PORT)
      {
        if (UART_SFIFO_REG(module) != 0xC0 )  
        {
            error++;
            printf("ERR! UART_SFIFO value is incorrect on UART at 0x%08X: %02X\n", module, UART_SFIFO_REG(module));
        }
        if (UART_TWFIFO_REG(module) != 0x0 )  
        {
            error++;
            printf("ERR! UART_TWFIFO value is incorrect on UART at 0x%08X: %02X\n", module, UART_TWFIFO_REG(module));
        }
      }
      /* This register is modified as part of the UART init so, if we are testing
       * the terminal port UART right now, we have to skip over this register.
       */
      if (module != TERM_PORT)
      {
        if (UART_TCFIFO_REG(module) != 0x0 )  
        {
            error++;
            printf("ERR! UART_TCFIFO value is incorrect on UART at 0x%08X: %02X\n", module, UART_TCFIFO_REG(module));
        }      
        if (UART_RWFIFO_REG(module) != 0x01 )  
        { 
            error++;
            printf("ERR! UART_RWFIFO value is incorrect on UART at 0x%08X: %02X\n", module, UART_RWFIFO_REG(module));
        }
        if (UART_RCFIFO_REG(module) != 0x0 )  
        {
            error++;
            printf("ERR! UART_RCFIFO value is incorrect on UART at 0x%08X: %02X\n", module, UART_RCFIFO_REG(module));
        }
        /* Test UARTn_D last since reading it will cause an underflow and 
         * that will change the default UART_SFIFO[RXUF] value.
         */
        if (UART_D_REG(module) != 0x0 )  
        {
            error++;
            printf("ERR! UART0_D value is incorrect: %02X\n", UART0_D);
        }
      }
      /* Advance the mem map ptr to the next UART module to test */
      if (module == UART0_BASE_PTR)
          module = UART1_BASE_PTR;
      else if (module == UART1_BASE_PTR)
          module = UART2_BASE_PTR;
      else if (module == UART2_BASE_PTR)
          /* All common UART registers have been checked, so drop out of the loop */
          module = 0;
    }    
      
    /* Now we'll test the PFIFO register. The expected reset value is 0x22 for 
     * UART0, but it is 0x0 for UART1-UART2.
     */
     if (UART0_PFIFO != 0x22 )  
     {
         error++;
         printf("ERR! UART_PFIFO value is incorrect on UART at 0x4006A000: %02X\n", UART0_PFIFO);
     }
     if (UART1_PFIFO != 0x0 )  
     {
         error++;
         printf("ERR! UART_PFIFO value is incorrect on UART at 0x4006B000: %02X\n", UART1_PFIFO);
     }
     if (UART2_PFIFO != 0x0 )  
     {
         error++;
         printf("ERR! UART_PFIFO value is incorrect on UART at 0x4006C000: %02X\n", UART2_PFIFO);
     }
  
    
     /* Now test the 7816 registers. These are only available on UART0 for Pioneer0 */  
     module = UART0_BASE_PTR;    
    
     while (module)
     {        
        if (UART_C7816_REG(module) != 0x0 )  
        {
          error++;
          printf("ERR! UART_C7816 value is incorrect on UART at 0x%08X: %02X\n", module, UART_C7816_REG(module));
        }
        if (UART_IE7816_REG(module) != 0x0 )  
        {
          error++;
          printf("ERR! UART_IE7816 value is incorrect on UART at 0x%08X: %02X\n", module, UART_IE7816_REG(module));
        }
        if (UART_IS7816_REG(module) != 0x0 )  
        {
          error++;
          printf("ERR! UART_IS7816 value is incorrect on UART at 0x%08X: %02X\n", module, UART_IS7816_REG(module));
        }
        if (UART_WP7816_T_TYPE0_REG(module) != 0x0A )  
        {
          error++;
          printf("ERR! UART_WP7816_T_TYPE0 value is incorrect on UART at 0x%08X: %02X\n", module, UART_WP7816_T_TYPE0_REG(module));
        } 
        if (UART_WP7816_T_TYPE1_REG(module) != 0x0A )  
        {
          error++;
          printf("ERR! UART_WP7816_T_TYPE1 value is incorrect on UART at 0x%08X: %02X\n", module, UART_WP7816_T_TYPE1_REG(module));
        }
        if (UART_WN7816_REG(module) != 0x0 )  
        {
          error++;
          printf("ERR! UART_WN7816 value is incorrect on UART at 0x%08X: %02X\n", module, UART_WN7816_REG(module));
        }
        if (UART_WF7816_REG(module) != 0x01 )  
        {
          error++;
          printf("ERR! UART_WF7816 value is incorrect on UART at 0x%08X: %02X\n", module, UART_WF7816_REG(module));
        }
        if (UART_ET7816_REG(module) != 0x0 )  
        {
          error++;
          printf("ERR! UART_ET7816 value is incorrect on UART at 0x%08X: %02X\n", module, UART_ET7816_REG(module));
        }
        if (UART_TL7816_REG(module) != 0x0 )  
        {
          error++;
          printf("ERR! UART_TL7816 value is incorrect on UART at 0x%08X: %02X\n", module, UART_TL7816_REG(module));
        }

        /* Advance the mem map ptr to the next UART module to test */
        if (module == UART0_BASE_PTR)
         /* All ISO-7816 UART registers have been checked, so drop out of the loop */
          module = 0;
     }        
    

     /* Now test the LON registers. These are only available on UART0 */     
     module = UART0_BASE_PTR;    
    
     if (UART_C6_REG(module) != 0x0 )  
      {
          error++;
          printf("ERR! UART_C6 value is incorrect on UART at 0x%08X: %02X\n", module, UART_C6_REG(module));
      }    
     if (UART_PCTH_REG(module) != 0x0 )  
      {
          error++;
          printf("ERR! UART_PCTH value is incorrect on UART at 0x%08X: %02X\n", module, UART_PCTH_REG(module));
      }  
     if (UART_PCTL_REG(module) != 0x0 )  
      {
          error++;
          printf("ERR! UART_PCTL value is incorrect on UART at 0x%08X: %02X\n", module, UART_PCTL_REG(module));
      }
     if (UART_B1T_REG(module) != 0x0 )  
      {
          error++;
          printf("ERR! UART_B1T value is incorrect on UART at 0x%08X: %02X\n", module, UART_B1T_REG(module));
      }
     if (UART_SDTH_REG(module) != 0x0 )  
      {
          error++;
          printf("ERR! UART_SDTH value is incorrect on UART at 0x%08X: %02X\n", module, UART_SDTH_REG(module));
      }
     if (UART_SDTL_REG(module) != 0x0 )  
      {
          error++;
          printf("ERR! UART_SDTL value is incorrect on UART at 0x%08X: %02X\n", module, UART_SDTL_REG(module));
      }     
     if (UART_PRE_REG(module) != 0x0 )  
      {
          error++;
          printf("ERR! UART_PRE value is incorrect on UART at 0x%08X: %02X\n", module, UART_PRE_REG(module));
      }     
     if (UART_TPL_REG(module) != 0x0 )  
      {
          error++;
          printf("ERR! UART_TPL value is incorrect on UART at 0x%08X: %02X\n", module, UART_TPL_REG(module));
      }
     if (UART_IE_REG(module) != 0x0 )  
      {
          error++;
          printf("ERR! UART_IE value is incorrect on UART at 0x%08X: %02X\n", module, UART_IE_REG(module));
      }     
     if (UART_WB_REG(module) != 0x0 )  
      {
          error++;
          printf("ERR! UART_WB value is incorrect on UART at 0x%08X: %02X\n", module, UART_WB_REG(module));
      }     
     if (UART_S3_REG(module) != 0x0 )  
      {
          error++;
          printf("ERR! UART_S3 value is incorrect on UART at 0x%08X: %02X\n", module, UART_S3_REG(module));
      }     
     if (UART_S4_REG(module) != 0x0 )  
      {
          error++;
          printf("ERR! UART_S4 value is incorrect on UART at 0x%08X: %02X\n", module, UART_S4_REG(module));
      }     
     if (UART_RPL_REG(module) != 0x0 )  
      {
          error++;
          printf("ERR! UART_RPL value is incorrect on UART at 0x%08X: %02X\n", module, UART_RPL_REG(module));
      }     
     if (UART_RPREL_REG(module) != 0x0 )  
      {
          error++;
          printf("ERR! UART_RPREL value is incorrect on UART at 0x%08X: %02X\n", module, UART_RPREL_REG(module));
      }     
     if (UART_CPW_REG(module) != 0x0 )  
      {
          error++;
          printf("ERR! UART_CPW value is incorrect on UART at 0x%08X: %02X\n", module, UART_CPW_REG(module));
      }     
     if (UART_RIDT_REG(module) != 0x0 )  
      {
          error++;
          printf("ERR! UART_RIDT value is incorrect on UART at 0x%08X: %02X\n", module, UART_RIDT_REG(module));
      }     
     if (UART_TIDT_REG(module) != 0x0 )  
      {
          error++;
          printf("ERR! UART_TIDT value is incorrect on UART at 0x%08X: %02X\n", module, UART_TIDT_REG(module));
      } 
     /* End of LON register tests */

     return error;
}
/********************************************************************/
/* UART module register read/write test.
 * Writes the passed in value to each register, then reads back the register
 * value and compares to the expected value (need to mask out any read only
 * bits from the expected register value). If all values match the test passes 
 * and function returns 0 otherwise returns the number of errors.
 * 
 * NOTE: Since one of the UARTs is being used for terminal output, it cannot
 * be tested here. This test will be run once on the firebird and once on a 
 * tower board. Those systems are using different UARTs, so if the test passes
 * on both platforms, then all of the registers will be covered.
 */
int uart_reg_rw(uint8 reg_val)
{
    int error = 0;
    UART_MemMapPtr module;
    
    /* Pointer for accessing a specific UART. Set to UART0 to start as long
     * as UART0 isn't the TERM_PORT. Otherwise start with UART1.
     */
    if (TERM_PORT != UART0_BASE_PTR)
      module = UART0_BASE_PTR;
    else
      module = UART1_BASE_PTR;
        
    printf("\nStarting UART register read write test.\n");
        
    /* Start off by testing all of the registers that are in common between
     * all of the UARTs. The 7816 registers are only available on UART0 so 
     * we'll test those last.
     */
     while( module )
    {  
        /* The write to the lower bits of BDH don't take effect until BDL is
         * is written, so we we'll check the BDH value after the BDL test. 
         */
        UART_BDH_REG(module) = reg_val;
        UART_BDL_REG(module) = reg_val;
	if(UART_BDL_REG(module) != reg_val)
	{ 
	    error++;
            printf("ERR! UART_BDL read value is incorrect on UART at 0x%08X: %02X\n", module, UART_BDL_REG(module));
 	}
	if((UART_BDH_REG(module)&0xDF) != (reg_val&0xDF))
	{ 
	    error++;
            printf("ERR! UART_BDH read value is incorrect on UART at 0x%08X: %02X\n", module, UART_BDH_REG(module));
 	}
         UART_C1_REG(module) = reg_val;
	if(UART_C1_REG(module) != reg_val)
	{ 
	    error++;
            printf("ERR! UART_C1 read value is incorrect on UART at 0x%08X: %02X\n", module, UART_C1_REG(module));
 	}
        UART_C2_REG(module) = reg_val;
	if(UART_C2_REG(module) != reg_val)
	{ 
	    error++;
            printf("ERR! UART_C2 read value is incorrect on UART at 0x%08X: %02X\n", module, UART_C2_REG(module));
 	}
        UART_S2_REG(module) = reg_val;
	if((UART_S2_REG(module)&0x3E) != (reg_val&0x3E))
	{ 
	    error++;
            printf("ERR! UART_S2 read value is incorrect on UART at 0x%08X: %02X\n", module, UART_S2_REG(module));
 	}
        UART_C3_REG(module) = reg_val;
	if((UART_C3_REG(module)&0x7F) != (reg_val&0x7F))
	{ 
	    error++;
            printf("ERR! UART_C3 read value is incorrect on UART at 0x%08X: %02X\n", module, UART_C3_REG(module));
 	}
        UART_MA1_REG(module) = reg_val;
	if(UART_MA1_REG(module) != reg_val)
	{ 
	    error++;
            printf("ERR! UART_MA1 read value is incorrect on UART at 0x%08X: %02X\n", module, UART_MA1_REG(module));
 	}
        UART_MA2_REG(module) = reg_val;
	if(UART_MA2_REG(module) != reg_val)
	{ 
	    error++;
            printf("ERR! UART_MA2 read value is incorrect on UART at 0x%08X: %02X\n", module, UART_MA2_REG(module));
 	}
        UART_C4_REG(module) = reg_val;
	if(UART_C4_REG(module) != reg_val)
	{ 
	    error++;
            printf("ERR! UART_C4 read value is incorrect on UART at 0x%08X: %02X\n", module, UART_C4_REG(module));
 	}
        UART_C5_REG(module) = reg_val;
	if((UART_C5_REG(module)&0xA0) != (reg_val&0xA0))
	{ 
	    error++;
            printf("ERR! UART_C5 read value is incorrect on UART at 0x%08X: %02X\n", module, UART_C5_REG(module));
 	}
        UART_MODEM_REG(module) = reg_val;
	if((UART_MODEM_REG(module)&0x0F) != (reg_val&0x0F))
	{ 
	    error++;
            printf("ERR! UART_MODEM read value is incorrect on UART at 0x%08X: %02X\n", module, UART_MODEM_REG(module));
 	}
        UART_IR_REG(module) = reg_val;
	if((UART_IR_REG(module)&0x07) != (reg_val&0x07))
	{ 
	    error++;
            printf("ERR! UART_IR read value is incorrect on UART at 0x%08X: %02X\n", module, UART_IR_REG(module));
 	}
        UART_PFIFO_REG(module) = reg_val;
	if((UART_PFIFO_REG(module)&0x88) != (reg_val&0x88))
	{ 
	    error++;
            printf("ERR! UART_PFIFO read value is incorrect on UART at 0x%08X: %02X\n", module, UART_PFIFO_REG(module));
 	}
        UART_CFIFO_REG(module) = reg_val;
	if((UART_CFIFO_REG(module)&0x03) != (reg_val&0x03))
	{ 
	    error++;
            printf("ERR! UART_CFIFO read value is incorrect on UART at 0x%08X: %02X\n", module, UART_CFIFO_REG(module));
 	}
        UART_TWFIFO_REG(module) = reg_val;
	if(UART_TWFIFO_REG(module) != reg_val)
	{ 
	    error++;
            printf("ERR! UART_TWFIFO read value is incorrect on UART at 0x%08X: %02X\n", module, UART_TWFIFO_REG(module));
 	}
        UART_RWFIFO_REG(module) = reg_val;
	if(UART_RWFIFO_REG(module) != reg_val)
	{ 
	    error++;
            printf("ERR! UART_RWFIFO read value is incorrect on UART at 0x%08X: %02X\n", module, UART_RWFIFO_REG(module));
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
    
    /* Now we'll test the 7816 registers that are only available on UART0 and UART1. */
    module = UART0_BASE_PTR;

     while (module)
     {        
        UART_C7816_REG(module) = reg_val;
        if((UART_C7816_REG(module)&0x1F) != (reg_val&0x1F))
        { 
            error++;
            printf("ERR! UART_C7816 read value is incorrect on UART at 0x%08X: %02X\n", module, UART_C7816_REG(module));
        }
        UART_IE7816_REG(module) = reg_val;
        if((UART_IE7816_REG(module)&0xF7) != (reg_val&0xF7))
        { 
            error++;
            printf("ERR! UART_IE7816 read value is incorrect on UART at 0x%08X: %02X\n", module, UART_IE7816_REG(module));
        }
        UART_WP7816_T_TYPE0_REG(module) = reg_val;
        if(UART_WP7816_T_TYPE0_REG(module) != reg_val)
        { 
            error++;
            printf("ERR! UART_WP7816_T_TYPE0 read value is incorrect on UART at 0x%08X: %02X\n", module, UART_WP7816_T_TYPE0_REG(module));
        }
        UART_WP7816_T_TYPE1_REG(module) = reg_val;
        if(UART_WP7816_T_TYPE1_REG(module) != reg_val)
        { 
            error++;
            printf("ERR! UART_WP7816_T_TYPE1 read value is incorrect on UART at 0x%08X: %02X\n", module, UART_WP7816_T_TYPE1_REG(module));
        }
        UART_WN7816_REG(module) = reg_val;
        if(UART_WN7816_REG(module) != reg_val)
        { 
            error++;
            printf("ERR! UART_WN7816 read value is incorrect on UART at 0x%08X: %02X\n", module, UART_WN7816_REG(module));
        }
        UART_WF7816_REG(module) = reg_val;
        if(UART_WF7816_REG(module) != reg_val)
        { 
            error++;
            printf("ERR! UART_WF7816 read value is incorrect on UART at 0x%08X: %02X\n", module, UART_WF7816_REG(module));
        }
        UART_ET7816_REG(module) = reg_val;
        if(UART_ET7816_REG(module) != reg_val)
        { 
            error++;
            printf("ERR! UART_ET7816 read value is incorrect on UART at 0x%08X: %02X\n", module, UART_ET7816_REG(module));
        }
        UART_TL7816_REG(module) = reg_val;
        if(UART_TL7816_REG(module) != reg_val)
        { 
            error++;
            printf("ERR! UART_TL7816 read value is incorrect on UART at 0x%08X: %02X\n", module, UART_TL7816_REG(module));
        }

        /* Advance the mem map ptr to the next UART module to test */
        if (module == UART0_BASE_PTR)
         /* All ISO-7816 UART registers have been checked, so drop out of the loop */
          module = 0;
     }        
        
    /* Now we'll test the 7816 registers that are only available on UART0 and UART1. */
    module = UART0_BASE_PTR;

    UART_C6_REG(module) = reg_val;
    if((UART_C6_REG(module)&0xF0) != (reg_val&0xF0))
    { 
        error++;
        printf("ERR! UART_C6 read value is incorrect on UART at 0x%08X: %02X\n", module, UART_C6_REG(module));
    }
    UART_PCTH_REG(module) = reg_val;
    if(UART_PCTH_REG(module) != reg_val)
    { 
        error++;
        printf("ERR! UART_PCTH read value is incorrect on UART at 0x%08X: %02X\n", module, UART_PCTH_REG(module));
    }
    UART_PCTL_REG(module) = reg_val;
    if(UART_PCTL_REG(module) != reg_val)
    { 
        error++;
        printf("ERR! UART_PCTL read value is incorrect on UART at 0x%08X: %02X\n", module, UART_PCTL_REG(module));
    }
    UART_B1T_REG(module) = reg_val;
    if(UART_B1T_REG(module) != reg_val)
    { 
        error++;
        printf("ERR! UART_B1T read value is incorrect on UART at 0x%08X: %02X\n", module, UART_B1T_REG(module));
    }
    UART_SDTH_REG(module) = reg_val;
    if(UART_SDTH_REG(module) != reg_val)
    { 
        error++;
        printf("ERR! UART_SDTH read value is incorrect on UART at 0x%08X: %02X\n", module, UART_SDTH_REG(module));
    }
    UART_SDTL_REG(module) = reg_val;
    if(UART_SDTL_REG(module) != reg_val)
    { 
        error++;
        printf("ERR! UART_SDTL read value is incorrect on UART at 0x%08X: %02X\n", module, UART_SDTL_REG(module));
    }
    UART_PRE_REG(module) = reg_val;
    if(UART_PRE_REG(module) != reg_val)
    { 
        error++;
        printf("ERR! UART_PRE read value is incorrect on UART at 0x%08X: %02X\n", module, UART_PRE_REG(module));
    }
    UART_TPL_REG(module) = reg_val;
    if(UART_TPL_REG(module) != reg_val)
    { 
        error++;
        printf("ERR! UART_TPL read value is incorrect on UART at 0x%08X: %02X\n", module, UART_TPL_REG(module));
    }
    UART_IE_REG(module) = reg_val;
    if((UART_IE_REG(module)&0x7F) != (reg_val&0x7F))
    { 
        error++;
        printf("ERR! UART_IE read value is incorrect on UART at 0x%08X: %02X\n", module, UART_IE_REG(module));
    }
    UART_WB_REG(module) = reg_val;
    if(UART_WB_REG(module) != reg_val)
    { 
        error++;
        printf("ERR! UART_WB read value is incorrect on UART at 0x%08X: %02X\n", module, UART_WB_REG(module));
    }
    UART_CPW_REG(module) = reg_val;
    if(UART_CPW_REG(module) != reg_val)
    { 
        error++;
        printf("ERR! UART_CPW read value is incorrect on UART at 0x%08X: %02X\n", module, UART_CPW_REG(module));
    }
    UART_RIDT_REG(module) = reg_val;
    if(UART_RIDT_REG(module) != reg_val)
    { 
        error++;
        printf("ERR! UART_RIDT read value is incorrect on UART at 0x%08X: %02X\n", module, UART_RIDT_REG(module));
    }
    UART_TIDT_REG(module) = reg_val;
    if(UART_TIDT_REG(module) != reg_val)
    { 
        error++;
        printf("ERR! UART_TIDT read value is incorrect on UART at 0x%08X: %02X\n", module, UART_TIDT_REG(module));
    }

        

     return error;
}
/********************************************************************/
