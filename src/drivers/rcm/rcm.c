/*
 * File:        rcm.c
 * Purpose:     Provides routines for the reset controller module
 *              
 */

#include "common.h"
#include "rcm.h"

/* OutSRS routine - checks the value in the SRS registers and sends
 * messages to the terminal announcing the status at the start of the 
 * code.
 */
void outSRS(void){                         //[outSRS]

  
  /* 	if (RCM_SRS1 & RCM_SRS1_TAMPER_MASK)
		printf("Tamper Detect Reset\n");*/
  
	if (RCM_SRS1 & RCM_SRS1_SACKERR_MASK)
		printf("Stop Mode Acknowledge Error Reset\n");
	if (RCM_SRS1 & RCM_SRS1_EZPT_MASK)
		printf("EzPort Reset\n");
	if (RCM_SRS1 & RCM_SRS1_MDM_AP_MASK)
		printf("MDM-AP Reset\n");
	if (RCM_SRS1 & RCM_SRS1_SW_MASK)
		printf("Software Reset\n");
	if (RCM_SRS1 & RCM_SRS1_LOCKUP_MASK)
		printf("Core Lockup Event Reset\n");
	if (RCM_SRS1 & RCM_SRS1_JTAG_MASK)
		printf("JTAG Reset\n");
	
	if (RCM_SRS0 & RCM_SRS0_POR_MASK)
		printf("Power-on Reset\n");
	if (RCM_SRS0 & RCM_SRS0_PIN_MASK)
		printf("External Pin Reset\n");
	if (RCM_SRS0 & RCM_SRS0_WDOG_MASK)
		printf("Watchdog(COP) Reset\n");
	if (RCM_SRS0 & RCM_SRS0_LOC_MASK)
		printf("Loss of Clock Reset\n");
	if (RCM_SRS0 & RCM_SRS0_LVD_MASK)
		printf("Low-voltage Detect Reset\n");
	if (RCM_SRS0 & RCM_SRS0_WAKEUP_MASK)
        {
          printf("[outSRS]Wakeup bit set from low power mode exit\n");
          printf("[outSRS]SMC_PMPROT   = %#02X \r\n", (SMC_PMPROT))  ;
          printf("[outSRS]SMC_PMCTRL   = %#02X \r\n", (SMC_PMCTRL))  ;
          printf("[outSRS]SMC_VLLSCTRL   = %#02X \r\n", (SMC_VLLSCTRL))  ;
          printf("[outSRS]SMC_PMSTAT   = %#02X \r\n", (SMC_PMSTAT))  ;

          if ((SMC_PMCTRL & SMC_PMCTRL_STOPM_MASK)== 3)
            printf("[outSRS] LLS exit \n") ;
          if (((SMC_PMCTRL & SMC_PMCTRL_STOPM_MASK)== 4) && ((SMC_VLLSCTRL & SMC_VLLSCTRL_VLLSM_MASK)== 0))
            printf("[outSRS] VLLS0 exit \n") ;
          if (((SMC_PMCTRL & SMC_PMCTRL_STOPM_MASK)== 4) && ((SMC_VLLSCTRL & SMC_VLLSCTRL_VLLSM_MASK)== 1))
            printf("[outSRS] VLLS1 exit \n") ;
          if (((SMC_PMCTRL & SMC_PMCTRL_STOPM_MASK)== 4) && ((SMC_VLLSCTRL & SMC_VLLSCTRL_VLLSM_MASK)== 2))
            printf("[outSRS] VLLS2 exit \n") ;
          if (((SMC_PMCTRL & SMC_PMCTRL_STOPM_MASK)== 4) && ((SMC_VLLSCTRL & SMC_VLLSCTRL_VLLSM_MASK)== 3))
            printf("[outSRS] VLLS3 exit \n") ; 
	}

        if ((RCM_SRS0 == 0) && (RCM_SRS1 == 0)) 
        {
	       printf("[outSRS]RCM_SRS0 is ZERO   = %#02X \r\n", (RCM_SRS0))  ;
	       printf("[outSRS]RCM_SRS1 is ZERO   = %#02X \r\n", (RCM_SRS1))  ;	 
        }
  }

void outsrs(void){
	char i;
	/* OutSRS routine - checks the value in the SRS registers and sends
	 * messages to two pins that indicate the bits set in the two SRS  
	 * registers.  PORT C19 and PORT E26 are used in this driver
         * initialized in Port_init(); 
	 */ 
	//	void outSRS(void){                         //[outSRS]
        //used for wakeup testing
        PORTB_PCR19 = (PORT_PCR_MUX(01) |    //  GPIO
              PORT_PCR_IRQC(0x00) |  //  falling edge enable
              PORT_PCR_DSE_MASK);    // enable high drive strength
        GPIOB_PDDR  |= 0x00080000;           // set Port B 19 as output
        GPIOB_PCOR =   0x00080000;           // set Port B 19 indicate wakeup - clears it in llwu_isr
        //used for wakeup testing
        PORTB_PCR18 = (PORT_PCR_MUX(01) |    //  GPIO
                       PORT_PCR_IRQC(0x00) |  //  falling edge enable
                       PORT_PCR_DSE_MASK);    // enable high drive strength
        GPIOB_PDDR  |= 0x00040000;           // set Port B18 as output
        GPIOB_PSOR   = 0x00040000;             // set Port B18 SRS output outsrs uses this pin
	
 	if (RCM_SRS0 & RCM_SRS0_POR_MASK){
		for (i=0;i<8;i++){
                          GPIOB_PTOR = 0x00080000;           
                          GPIOB_PTOR = 0x00080000;           
		}
	}
	if (RCM_SRS0 & RCM_SRS0_PIN_MASK){
			for (i=0;i<7;i++){
                          GPIOB_PTOR = 0x00080000;           
                          GPIOB_PTOR = 0x00080000;           
			}
	}
	if (RCM_SRS0 & RCM_SRS0_WDOG_MASK){
			for (i=0;i<6;i++){
                          GPIOB_PTOR = 0x00080000;           
                          GPIOB_PTOR = 0x00080000;           
			}
	}
	if (RCM_SRS0 & RCM_SRS0_LOC_MASK){
			for (i=0;i<3;i++){
                          GPIOB_PTOR = 0x00080000;           
                          GPIOB_PTOR = 0x00080000;           
			}
	}
	if (RCM_SRS0 & RCM_SRS0_LVD_MASK){
			for (i=0;i<2;i++){
                          GPIOB_PTOR = 0x00080000;           
                          GPIOB_PTOR = 0x00080000;           
			}
	}
	if (RCM_SRS0 & RCM_SRS0_WAKEUP_MASK){
                          GPIOB_PTOR = 0x00080000;           
                          GPIOB_PTOR = 0x00080000;           
	}
        GPIOB_PCOR   = 0x00040000;             // set Port B18 SRS output outsrs uses this pin

/*	if (RCM_SRS1 & RCM_SRS1_TAMPER_MASK){
			for (i=0;i<8;i++){
                          GPIOB_PTOR = 0x00080000;           
                          GPIOB_PTOR = 0x00080000;           
			}
	}*/
        
        
	if (RCM_SRS1 & RCM_SRS1_SACKERR_MASK){
			for (i=0;i<6;i++){
                          GPIOB_PTOR = 0x00080000;           
                          GPIOB_PTOR = 0x00080000;           
			}
	}
	if (RCM_SRS1 & RCM_SRS1_EZPT_MASK){
			for (i=0;i<5;i++){
                          GPIOB_PTOR = 0x00080000;           
                          GPIOB_PTOR = 0x00080000;           
			}
	}
	if (RCM_SRS1 & RCM_SRS1_MDM_AP_MASK){
			for (i=0;i<4;i++){
                          GPIOB_PTOR = 0x00080000;           
                          GPIOB_PTOR = 0x00080000;           
			}
	}
	if (RCM_SRS1 & RCM_SRS1_SW_MASK){
			for (i=0;i<3;i++){
                          GPIOB_PTOR = 0x00080000;           
                          GPIOB_PTOR = 0x00080000;           
			}
	}
	if (RCM_SRS1 & RCM_SRS1_LOCKUP_MASK){
			for (i=0;i<2;i++){
                          GPIOB_PTOR = 0x00080000;           
                          GPIOB_PTOR = 0x00080000;           
			}
	}
	if (RCM_SRS1 & RCM_SRS1_JTAG_MASK){
                          GPIOB_PTOR = 0x00080000;           
                          GPIOB_PTOR = 0x00080000;           
	}
 	GPIOB_PSOR = 0x000C0000;   // output SRS1 done
}
