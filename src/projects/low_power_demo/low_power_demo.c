
 /**
  \mainpage 
  \n Copyright (c) 2011 Freescale
  \brief 
  \author   	Freescale 
  \author    
  \version      m.n
  \date         Feb 15, 2012
  
  Put here all the information needed of the Project. Basic configuration as well as information on
  the project definition will be very useful 
*/
/****************************************************************************************************/
/*                                                                                                  */
/* All software, source code, included documentation, and any implied know-how are property of      */
/* Freescale Semiconductor and therefore considered CONFIDENTIAL INFORMATION.                       */
/* This confidential information is disclosed FOR DEMONSTRATION PURPOSES ONLY.                      */
/*                                                                                                  */
/* All Confidential Information remains the property of Freescale Semiconductor and will not be     */
/* copied or reproduced without the express written permission of the Discloser, except for copies  */
/* that are absolutely necessary in order to fulfill the Purpose.                                   */
/*                                                                                                  */
/* Services performed by FREESCALE in this matter are performed AS IS and without any warranty.     */
/* CUSTOMER retains the final decision relative to the total design and functionality of the end    */
/* product.                                                                                         */
/* FREESCALE neither guarantees nor will be held liable by CUSTOMER for the success of this project.*/
/*                                                                                                  */
/* FREESCALE disclaims all warranties, express, implied or statutory including, but not limited to, */
/* implied warranty of merchantability or fitness for a particular purpose on any hardware,         */
/* software ore advise supplied to the project by FREESCALE, and or any product resulting from      */
/* FREESCALE services.                                                                              */
/* In no event shall FREESCALE be liable for incidental or consequential damages arising out of     */
/* this agreement. CUSTOMER agrees to hold FREESCALE harmless against any and all claims demands or */
/* actions by anyone on account of any damage,or injury, whether commercial, contractual, or        */
/* tortuous, rising directly or indirectly as a result of the advise or assistance supplied CUSTOMER*/ 
/* in connectionwith product, services or goods supplied under this Agreement.                      */
/*                                                                                                  */
/****************************************************************************************************/

/****************************************************************************************************/
/****************************************************************************************************/
/****************************************NOTES*******************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/* This project was desing to work on TWR-k20DX50 board, configured with PEE at 50Mhz               */
/*      JJumper J25 may be used to measured to current consumption                                                                                             */
/****************************************************************************************************/

/*****************************************************************************************************
* Include files
*****************************************************************************************************/
#include "common.h"
#include "llwu.h"
#include "smc.h"
#include "mcg.h"
#include "pmc.h"

/*****************************************************************************************************
* Declaration of module wide FUNCTIONs - NOT for use in other modules
*****************************************************************************************************/
void LowPowerModes_test(void);
void LLWU_Init(void);
void SW_LED_Init(void);
void port_c_isr(void);

void uart(unsigned char state);
void uart_configure(int mcg_clock_hz);
int PEE_to_BLPE(void);
int BLPE_to_PEE(void);
void prepareToGetLowestPower(void);
void clockMonitor(unsigned char state);
void JTAG_TDO_PullUp_Enable(void);

/*****************************************************************************************************
* Definition of module wide MACROs / #DEFINE-CONSTANTs - NOT for use in other modules
*****************************************************************************************************/
#define UNDEF_VALUE  0xFF
/*****************************************************************************************************
* Declaration of module wide TYPEs - NOT for use in other modules
*****************************************************************************************************/

/*****************************************************************************************************
* Definition of module wide VARIABLEs - NOT for use in other modules
*****************************************************************************************************/
unsigned char measureFlag = 0;
unsigned char enterVLLSOflag = 0;
unsigned long ram_data[216];

extern int mcg_clk_hz;
extern int mcg_clk_khz;
extern int core_clk_khz;

/*****************************************************************************************************
* Definition of module wide (CONST-) CONSTANTs - NOT for use in other modules
*****************************************************************************************************/

/*****************************************************************************************************
* Code of project wide FUNCTIONS
*****************************************************************************************************/

int main (void)
{
 
#ifdef KEIL
	start();
#endif
        /*Disable Clock Out*/        
        PORTC_PCR3 = ( PORT_PCR_MUX(0));
        
        /*Enable all operation modes because this is a write once register*/  
        SMC_PMPROT =  SMC_PMPROT_AVLLS_MASK |
                      SMC_PMPROT_ALLS_MASK  |    
                      SMC_PMPROT_AVLP_MASK;
        
        /*PTC1(SW2) is configured to wake up MCU from VLLSx and LLS modes, Interrup is ne*/
        LLWU_Init();
        /*Configure Tower hardware for the application*/
        SW_LED_Init();
        /*Start test*/
        LowPowerModes_test();
}

/*******************************************************************************************************/

void LowPowerModes_test(void)
{
  unsigned char op_mode;  
  unsigned char test_num = UNDEF_VALUE;
  char test_val;
   
  
        printf("*----------------------------------------*\n");
        printf("*        Low Power Mode Test             *\n");
        printf("*----------------------------------------*\n\n");
                       
	while(1)
	{
	    
            while (test_num > 11 | test_num < 0){
                printf("\nSelect the mode to enter \n");
                printf("0 to enter VLLS0 no POR\n");
                printf("1 to enter VLLS0\n");
                printf("2 to enter VLLS1\n");
                printf("3 to enter VLLS2\n");
                printf("4 to enter VLLS3\n"); 
                printf("5 to enter LLS\n");
                printf("6 to enter VLPS\n");
                printf("7 to enter VLPR\n");
                printf("8 exit VLPR\n");
                printf("9 to enter VLPW\n");
                printf("A to enter WAIT\n");
                printf("B to enter STOP\n");
                test_val = getchar();
		putchar(test_val);
                
                if((test_val>=0x30) && (test_val<=0x39))
                      test_num = test_val - 0x30;
                if((test_val>=0x41) && (test_val<=0x47))
                      test_num = test_val - 0x37;
                if((test_val>=0x61) && (test_val<=0x67))
                      test_num = test_val - 0x57;
              }
         
          
            switch(test_num){
              
              case 0://VLLS0
                  printf("Press any key to enter VLLS0 with POR disable\n ");
                getchar();
                  printf("Press SW2 to wake up from VLLS0\n ");
                prepareToGetLowestPower();
                enter_vlls0_nopor();
                break;
            
             case 1://VLLS0
                  printf("Press any key to enter VLLS0\n ");
                getchar();
                  printf("Press SW2 to wake up from VLLS0\n ");
                prepareToGetLowestPower();
                enter_vlls0();
                break;
                
                
              case 2://VLLS1
                  printf("Press any key to enter VLLS1\n ");
                getchar();
                  printf("Press SW2 to wake up from VLLS1\n ");
                prepareToGetLowestPower();          
                enter_vlls1();
                break;
              
              case 3://VLLS2
                  printf("Press any key to enter VLLS2\n ");
                getchar();
                  printf("Press SW2 to wake up from VLLS2\n ");
                prepareToGetLowestPower();
                enter_vlls2();
                break;
                
              case 4://VLLS3
                  printf("Press any key to enter VLLS3\n ");
                getchar();
                  printf("Press SW2 to wake up from VLLS3\n ");
                prepareToGetLowestPower();
                enter_vlls3();
                break;
                
              case 5://LLS
                  printf("Press any key to enter LLS\n ");
                getchar();
                  printf("Press SW2 to wake up from LLS\n ");
                prepareToGetLowestPower(); 
                enter_lls();
                /*After LLS wake up that was enter from PEE the exit will be on PBE */ 
                /*  for this reason after wake up make sure to get back to desired  */
                /*  clock mode                                                      */
                op_mode = what_mcg_mode();
                if(op_mode==PBE)
                {
                  mcg_clk_hz = pbe_pee(CLK0_FREQ_HZ);
                }
                clockMonitor(ON);
                uart(ON);
                uart_configure(mcg_clk_hz);
                break;
                
              case 6://VLPS
                  printf("Press any key to enter VLPS\n ");
                getchar();
                  printf("Force to BLPE mode before enter\n ");
                  printf("Press SW2 or SW3 to wake up from VLPS\n ");
                mcg_clk_hz = PEE_to_BLPE();
                uart_configure(mcg_clk_hz);  
                prepareToGetLowestPower();
                /*Go to VLPS*/
                enter_vlps(1);  //1 Means: Any interrupt could wake up from this mode             
                clockMonitor(ON);
                mcg_clk_hz = BLPE_to_PEE();
                uart(ON);
                uart_configure(mcg_clk_hz);
                  printf("\nNow in Run mode at 50MHz core clock, 25Mhz flash clock\n"); 
                break;
                
              case 7://VLPR
                /*Maximum clock frequency for this mode is core 4MHz and Flash 1Mhz*/
                  printf("Press any key to enter VLPR\n ");
                getchar();
                  printf("Configure clock frequency to 4MHz core clock and 1MHz flash clock\n ");
                mcg_clk_hz = PEE_to_BLPE();
                /*Configure UART for the new clock frequency*/
                uart_configure(mcg_clk_hz);                  
                /*Clock Monitor must be disable when run VLPR on BLPE*/
                clockMonitor(OFF);
                /*Go to VLPR*/
                enter_vlpr(0);   // 0 Means: Interruption does not wake up from VLPR
                  printf("\nNow in VLPR mode at 4MHz core clock, 1Mhz flash clock\n"); 
                break;


              case 8:// Exit VLPR
                  printf("Press any key to exit VLPR\n ");
                getchar();
                /*Exit VLPR*/
                exit_vlpr();
                clockMonitor(ON);
                /*Back to original clock frequency*/
                mcg_clk_hz = BLPE_to_PEE();
                /*Configure UART for original frequency*/
                uart_configure(mcg_clk_hz);
                  printf("\nNow in Run mode at 50MHz core clock, 25Mhz flash clock\n");
                break;

                
             case 9:// VLPW
                  printf("Press any key to enter VLPW\n ");
                getchar();
                  printf("Force to BLPE mode before enter\n ");
                  printf("Press SW2 or SW3 to wake up from VLPW\n ");  
                mcg_clk_hz = PEE_to_BLPE();
                uart_configure(mcg_clk_hz);  
                clockMonitor(OFF);
                /*Enter to VLPR*/
                enter_vlpr(1); //1 Means: Any interrupt could waku up from VLPR
                /*Enter to VLPW*/
                enter_wait();
                //exit_vlpr();
                clockMonitor(ON);
                mcg_clk_hz = BLPE_to_PEE();
                uart_configure(mcg_clk_hz);
                 printf("\nNow in Run mode at 50MHz core clock, 25Mhz flash clock\n");
                break;  
                
              case 10://WAIT
                  printf("Press any key to enter Wait\n ");
                getchar();
                  printf("Press SW2 or SW3 to wake up from Wait\n "); 
                prepareToGetLowestPower(); 
                /*Enter wait mode*/
                enter_wait();
                clockMonitor(ON);
                uart(ON);
                  printf("Back to Run mode\n");
                break;
              
              case 11://STOP
                  printf("Press any key to enter Stop\n ");
                getchar();
                  printf("Press SW2 or SW3 to wake up from Stop\n "); 
                prepareToGetLowestPower();
                /*Enter stop mode*/
                enter_stop();
                /*After LLS wake up that was enter from PEE the exit will be on PBE */ 
                /*  for this reason after wake up make sure to get back to desired  */
                /*  clock mode                                                      */
                op_mode = what_mcg_mode();
                if(op_mode==PBE)
                {
                  mcg_clk_hz = pbe_pee(CLK0_FREQ_HZ);
                }
                clockMonitor(ON);
                uart(ON);
                uart_configure(mcg_clk_hz);               
                  printf("Back to Run mode\n");
                break; 
                   
                
            }
               
            test_num = UNDEF_VALUE ;    

	} 
}

/*******************************************************************************************************/
void LLWU_Init(void)
{
    enable_irq(INT_LLW-16);
    llwu_configure(0x0040/*PTC1*/, LLWU_PIN_FALLING, 0x0);
}

/*******************************************************************************************************/
void SW_LED_Init(void)
{
     SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTC_MASK; /* PORTC clock enablement */ 
 
    /*Configure NVIC for PortC Isr*/ 
    enable_irq(INT_PORTC-16);
       
    /*Configure SW2*/
    PORTC_PCR1 = PORT_PCR_PS_MASK|PORT_PCR_PE_MASK|PORT_PCR_PFE_MASK|PORT_PCR_IRQC(10)|PORT_PCR_MUX(1); /* IRQ Falling edge */   
    //PORTC_DFER  |= (1<<1) //  digital filter not available on PORTC of K20D50M
    //PORTC_DFWR  = 0x1F;//  digital filter not available on PORTC of K20D50M
    
    /*Configure SW3*/
    PORTC_PCR2 = PORT_PCR_PS_MASK|PORT_PCR_PE_MASK|PORT_PCR_PFE_MASK|PORT_PCR_IRQC(10)|PORT_PCR_MUX(1); /* IRQ Falling edge */   
    //PORTC_DFER  |= (1<<2);  digital filter not available on PORTC of K20D50M
    //PORTC_DFWR  = 0x1F;  digital filter not available on PORTC of K20D50M
       
}

/*******************************************************************************************************/
void port_c_isr(void)
{
    
  if(PORTC_ISFR == (1<<1))
  {
   PORTC_PCR1 |= PORT_PCR_ISF_MASK;    
  }
  
  if(PORTC_ISFR == (1<<2))
  {
   PORTC_PCR2 |= PORT_PCR_ISF_MASK;
  }
   PORTC_ISFR = 0xFFFFFFFF;
}

/*******************************************************************************************************/
void uart(unsigned char state)
{
    if(state)
    {
       PORTE_PCR0 = PORT_PCR_MUX(3); // UART is alt3 function for this pin
       PORTE_PCR1 = PORT_PCR_MUX(3); // UART is alt3 function for this pin
    }else
    {
       PORTE_PCR0 = PORT_PCR_MUX(0); // Analog is alt0 function for this pin
       PORTE_PCR1 = PORT_PCR_MUX(0); // Analog is alt0 function for this pin
    }
  
 
}

/*******************************************************************************************************/
void uart_configure(int mcg_clock_hz)
{
     int mcg_clock_khz;
     int core_clock_khz;
  
     mcg_clock_khz = mcg_clock_hz / 1000;
     core_clock_khz = mcg_clock_khz / (((SIM_CLKDIV1 & SIM_CLKDIV1_OUTDIV1_MASK) >> 28)+ 1);
     uart_init (TERM_PORT, core_clock_khz, TERMINAL_BAUD);
}

/*******************************************************************************************************/
int PEE_to_BLPE(void)
{
     int mcg_clock_hz;    
  
      mcg_clock_hz = pee_pbe(CLK0_FREQ_HZ);
      mcg_clock_hz = pbe_blpe(CLK0_FREQ_HZ);
      /* Reduce Flash clock to 1MHz- Currently commented out due to errata KINETIS50MHZ_1N86B-4473*/
      //SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(1)|SIM_CLKDIV1_OUTDIV2(1)|SIM_CLKDIV1_OUTDIV4(0x7);
      /*Reduce Flash clock to 500KHz*/
      SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(1)|SIM_CLKDIV1_OUTDIV2(1)|SIM_CLKDIV1_OUTDIV4(0xF);

      return mcg_clock_hz; 
}

/*******************************************************************************************************/
int BLPE_to_PEE(void)
{
      int mcg_clock_hz;    
  
      SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0)|SIM_CLKDIV1_OUTDIV2(0)|SIM_CLKDIV1_OUTDIV4(0x1);
      /*After wake up back to the original clock frequency*/            
      mcg_clock_hz = blpe_pbe(CLK0_FREQ_HZ, PLL0_PRDIV,PLL0_VDIV);
      mcg_clock_hz = pbe_pee(CLK0_FREQ_HZ);
      
      return mcg_clock_hz;
}

/*******************************************************************************************************/
void prepareToGetLowestPower(void)
{
     uart(OFF);
     clockMonitor(OFF);
     JTAG_TDO_PullUp_Enable();          
}

/*******************************************************************************************************/
void JTAG_TDO_PullUp_Enable(void)
{
    PORTA_PCR2 = PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;//JTAG_TDO      
}


/*******************************************************************************************************/
void clockMonitor(unsigned char state)
{
    if(state)
      MCG_C6 |= MCG_C6_CME0_MASK;
    else
      MCG_C6 &= ~MCG_C6_CME0_MASK;
}
  
