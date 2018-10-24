/****************************************************************************************************/
/**
\file       start.c
\brief      kinetis startup routines
\author     Freescale Semiconductor
\author     
\version    1.0
\date       Sep 14, 2011
\warning    need to check with Si
 
*/
#include "start.h"
#include "common.h"
#include "wdog.h"
#include "rcm.h"
#include "sysinit.h"

/********************************************************************/
/*!
 * \brief   Kinetis Start
 * \return  None
 *
 * This function calls all of the needed starup routines and then 
 * branches to the main process.
 */
void start(void)
{
	/* Disable the watchdog timer */
	wdog_disable();
        
	/* Copy any vector or data sections that need to be in RAM */
	common_startup();
        
	/* Perform processor initialization */
	sysinit();

        /* Determine the last cause(s) of reset */
        //outSRS();

	/* Determine specific Kinetis device and revision */
	//cpu_identify();

#ifndef KEIL	
	/* Jump to main process */
	main();

	/* No actions to perform after this so wait forever */
	while(1);
#endif
}
/********************************************************************/
/*!
 * \brief   Kinetis CPU Identify
 * \params  None
 *
 * This is primarly a reporting function that displays information
 * about the specific CPU to the default terminal including:
 * - Kinetis family
 * - package
 * - die revision
 * - P-flash size
 * - Ram size
 \warning   flash_identify() not implemented
 */
void cpu_identify (void)
{
    /* Determine the Kinetis family */
    printf("\n FAM_ID = 0x%x",(SIM_SDID & SIM_SDID_FAMID_MASK)>>SIM_SDID_FAMID_SHIFT);
    switch((SIM_SDID & SIM_SDID_FAMID_MASK)>>SIM_SDID_FAMID_SHIFT) 
 //    switch((SIM_SDID & SIM_SDID_DIEID(0x7))>>SIM_SDID_DIEID_SHIFT)   for P3
    { 
              
             
    	case 0x0:
    		printf("\nK10-");
    		break;
    	case 0x1:
    		printf("\nK20-");
    		break;
    	case 0x2:
    		printf("\nK30-");
    		break;
    	case 0x3:
    		printf("\nK40-");
    		break;
    	case 0x4:
    		printf("\nK60-");
    		break;
	default:
		printf("\nUnrecognized Kinetis family device.\n");  
		break;  	
    }

    printf("\n PIN_ID = 0x%x",(SIM_SDID & SIM_SDID_PINID_MASK)>>SIM_SDID_PINID_SHIFT);
 
    /* Determine the package size */
    switch((SIM_SDID & SIM_SDID_PINID_MASK)>>SIM_SDID_PINID_SHIFT) 
    {  
    	case 0x2:
    		printf("32pin\n");
    		break;
    	case 0x4:
    		printf("48pin\n");
    		break;
    	case 0x5:
    		printf("64pin\n");
    		break;
    	case 0x6:
    		printf("80pin\n");
    		break;
    	case 0x7:
    		printf("81pin\n");
    		break;
    	case 0x8:
    		printf("100pin\n");
    		break;
    	case 0x9:
    		printf("104pin\n");
    		break;
    	case 0xA:
    		printf("144pin\n");
    		break;
    	case 0xC:
    		printf("196pin\n");
    		break;
    	case 0xE:
    		printf("256pin\n");
    		break;
	default:
		printf("\nUnrecognized Kinetis package code .\n");  
		break;  	
    }                

       printf("\n REV_ID = 0x%x \n",(SIM_SDID & SIM_SDID_REVID_MASK)>>SIM_SDID_REVID_SHIFT);
 
    /* Determine the revision ID */
    switch((SIM_SDID & SIM_SDID_REVID_MASK)>>SIM_SDID_REVID_SHIFT) 
    {
        case 0x0:
                printf("Silicon rev 1.0\n");
                break;
        case 0x1:
                printf("Silicon rev 1.1\n");
                break;
        case 0x2:
                printf("Silicon rev 1.2\n");
                break;
        default:
                printf("\nThis version of software doesn't recognize the revision code.");
                break;
    }

      /* Determine the flash revision */
    flash_identify();   


    /* Determine the RAM size */
   printf("\n RAM_SIZE_ID = 0x%x \n",(SIM_SOPT1 & SIM_SOPT1_RAMSIZE_MASK)>>SIM_SOPT1_RAMSIZE_SHIFT);

    switch((SIM_SOPT1 & SIM_SOPT1_RAMSIZE_MASK)>>SIM_SOPT1_RAMSIZE_SHIFT)
    {
      
   	case 0x1:
    		printf("8 kBytes of RAM\n\n");
    		break;
    	case 0x3:
    		printf("16 kBytes of RAM\n\n");
    		break;    
   //LP following are not present on P0     
    	case 0x5:
    		printf("32 kBytes of RAM\n\n");
    		break;
    	case 0x7:
    		printf("64 kBytes of RAM\n\n");
    		break;
    	case 0x8:
    		printf("96 kBytes of RAM\n\n");
    		break;
    	case 0x9:
    		printf("128 kBytes of RAM\n\n");
    		break;
            
		default:
			printf(" ERR!! Undefined RAM size\n\n");  
			break;  		
    }
}


/********************************************************************/
/*!
 * \brief   flash Identify
 * \return  None
 *
 * This is primarly a reporting function that displays information
 * about the specific flash parameters and flash version ID for 
 * the current device. These parameters are obtained using a special
 * flash command call "read resource." The first four bytes returned
 * are the flash parameter revision, and the second four bytes are
 * the flash version ID.
 */
void flash_identify (void)
{

   #if 1
  
    FTFL_FSTAT = 0x70;    
    
      
      
    /* Get the flash parameter version */
    /* Write the flash FCCOB registers with the values for a Read resource command */
    FTFL_FCCOB0 = 0x03;   
    FTFL_FCCOB1 = 0x00;
    FTFL_FCCOB2 = 0x00;
    FTFL_FCCOB3 = 0x00;
    FTFL_FCCOB8 = 0x01;       // Resource Select Code   1= Version ID 8 bytes.

    /* Launch command */
    SpSub();

    printf("Flash parameter version %d.%d.%d.%d\n",FTFL_FCCOB4,FTFL_FCCOB5,FTFL_FCCOB6,FTFL_FCCOB7);

    /* Get the flash version ID */
    /* Write the flash FCCOB registers with the values for a read resource command */
    FTFL_FCCOB0 = 0x03;
    FTFL_FCCOB1 = 0x00;
    FTFL_FCCOB2 = 0x00;
    FTFL_FCCOB3 = 0x04;
    FTFL_FCCOB8 = 0x01;    

    /* Launch command */
    SpSub();

    printf("Flash version ID %d.%d.%d.%d\n",FTFL_FCCOB4,FTFL_FCCOB5,FTFL_FCCOB6,FTFL_FCCOB7);  

    /* Clear all of the flags in the flash status register */
    
    FTFL_FSTAT = !FTFL_FSTAT_CCIF_MASK | FTFL_FSTAT_RDCOLERR_MASK | FTFL_FSTAT_ACCERR_MASK | FTFL_FSTAT_FPVIOL_MASK;
    #endif
    
}
/********************************************************************/

/*******************************************************************************
 * Function:        SpSub
 *
 * Description:     Execute the Flash write while running out of SRAM
 *
 * Returns:         none
 *
 * Notes:
 *
 *******************************************************************************/
#if defined(__IAR_SYSTEMS_ICC__)
__ramfunc
#elif defined(CW)
__relocate_code__
#endif
void 
SpSub(void)
{
    /* All required FCCOBx registers are written, so launch the command */
    FTFL_FSTAT = FTFL_FSTAT_CCIF_MASK;
    /* Wait for the command to complete */
    while(!(FTFL_FSTAT & FTFL_FSTAT_CCIF_MASK)){};
    
}
/* Leave this immediately after SpSub */
void SpSubEnd(void) {}

