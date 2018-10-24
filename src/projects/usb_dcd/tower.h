/****************************************************************************************************/
/**
Copyright (c) 2011 Freescale Semiconductor
Freescale Confidential Proprietary
\file        tower.h
\brief      Kinetis tower CPU card definitions
\author     b01252
\version    1.0
\date       Sep 26, 2011
\warning    for PRE-Silicon Test     MCU_MK40DZ100 is defined instead of MCU_MK20DZ50, please uncommnet M
*/

#ifndef __TOWER_H__
#define __TOWER_H__

#include "mcg.h"

/********************************************************************/

/* Global defines to console support, ie IAR Terminal I/O */
//#define CONSOLE_SUPPORT

#if (defined(TWR_K40X256))
  #define MCU_MK40DZ100



  /*
   * System Bus Clock Info
   */
  #define MGG_OSC_MODE        XTAL_MODE    //EXTOSC_MODE or XTAL_MODE
  #define REF_CLK             XTAL8        // see available frequencies in osc_frequency   in mcg.h
  #define CORE_CLK_MHZ        PLL96        // see pll_options  frequencies in              in mcg.h      

  /* Serial Port Info */
  #define TERM_PORT           UART3_BASE_PTR
  #define TERMINAL_BAUD       115200
  #undef  HW_FLOW_CONTROL
#elif (defined(TWR_K53N512))
  #define MCU_MK53DZ100

  /*
   * System Bus Clock Info
   */

  #define MGG_OSC_MODE        EXTOSC_MODE   // EXTOSC_MODE or XTAL_MODE
  #define REF_CLK             EXTOSC50      // see available frequencies in osc_frequency   in mcg.h
  #define CORE_CLK_MHZ        PLL100        // see pll_options  frequencies in              in mcg.h      

  /* Serial Port Info */
  #define TERM_PORT           UART1_BASE_PTR
  #define TERMINAL_BAUD       115200
  #undef  HW_FLOW_CONTROL
#elif (defined(TWR_K60N512))
  #define MCU_MK60DZ100

  /*
   * System Bus Clock Info
   */
  #define MGG_OSC_MODE        EXTOSC_MODE   // EXTOSC_MODE or XTAL_MODE
  #define REF_CLK             EXTOSC50      // see available frequencies in osc_frequency   in mcg.h
  #define CORE_CLK_MHZ        PLL100        // see pll_options  frequencies in              in mcg.h

  /* Serial Port Info */
  #define TERM_PORT           UART3_BASE_PTR
  #define TERMINAL_BAUD       115200
  #undef  HW_FLOW_CONTROL
#elif (defined(TWR_K20DX256))//new!! P1
  #define MCU_MK20DZ100

  /*
   * System Bus Clock Info
   */
  #define MGG_OSC_MODE        XTAL_MODE    // EXTOSC_MODE or XTAL_MODE
  #define REF_CLK             XTAL8        // see available frequencies in osc_frequency   in mcg.h
  #define CORE_CLK_MHZ        PLL50        // see pll_options  frequencies in              in mcg.h

  /* Serial Port Info */
  #define TERM_PORT           UART0_BASE_PTR
  #define TERMINAL_BAUD       115200
  #undef  HW_FLOW_CONTROL


#elif (defined(TWR_K20D50M))//new!! P0
    #define MCU_MK20DZ50     // warning
    //#define MCU_MK40DZ100  // FOR PRE-SI TEST ONLY

  /*
   * Input Clock Info
   */
  #define CLK0_FREQ_HZ        8000000
  #define CLK0_TYPE           CRYSTAL

  /*
   * PLL Configuration Info
   */
//  #define NO_PLL_INIT

/* The expected PLL output frequency is:
 * PLL out = (((CLKIN/PRDIV) x VDIV) / 2)
 * where the CLKIN can be either CLK0_FREQ_HZ or CLK1_FREQ_HZ.
 * 
 * For more info on PLL initialization refer to the mcg driver files.
 */

  #define PLL0_PRDIV      4
  #define PLL0_VDIV       24



  /*
   * System Bus Clock Info
   */
  #define MGG_OSC_MODE        XTAL_MODE     // EXTOSC_MODE or XTAL_MODE
  #define REF_CLK             XTAL8         // see available frequencies in osc_frequency   in mcg.h
  #define CORE_CLK_MHZ        PLL50         // see pll_options  frequencies in              in mcg.h

  /* Serial Port Info */
  #define TERM_PORT           UART1_BASE_PTR
  #define TERMINAL_BAUD       115200
  #undef  HW_FLOW_CONTROL
#else
  #error "No valid tower CPU card defined"
#endif


#endif /* __TOWER_H__ */
