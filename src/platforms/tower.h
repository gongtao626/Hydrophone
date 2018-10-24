/****************************************************************************************************/
/**
Copyright (c) 2011 Freescale Semiconductor
Freescale Confidential Proprietary
\file       tower.h
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
  //
//#define NO_PLL_INIT //�����ⲿPLL��ֻ��Ĭ��MCG

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
//Modified by ����
  #define UART_0 UART0_BASE_PTR
  #define UART_1 UART1_BASE_PTR

  #define TERM_PORT           UART_1//�����ã���MAX3490����֮��Ӧ����UART_422
  #define TERMINAL_BAUD       115200
  #undef  HW_FLOW_CONTROL
     
  #define LUOPAN_PORT UART_0
  #define LUOPAN_BAUD 9600
  #undef  HW_FLOW_CONTROL


  #define NO_CLKOUT_SUPPORT//û�����ϵͳʱ����Ϣ�Ĺܽţ�k10��֧��
 //Modified by ����
#else
  #error "No valid tower CPU card defined"
#endif

//2.3 ��λ����λ����üĴ���һλ��״̬
#define BSET(bit,Register)  ((Register)|= (1<<(bit)))    //�üĴ�����һλ
#define BCLR(bit,Register)  ((Register) &= ~(1<<(bit)))  //��Ĵ�����һλ
#define BGET(bit,Register)  (((Register) >> (bit)) & 1)  //��üĴ���һλ��״̬

#define AD_NUM 6 //��·�ź���ռ�ݵ��ֽ���
#define AD_COUNT 5

//2 �궨�� 
#define UART_232 UART_0 //DEMOʹ�ô���3�շ����ݣ���ʽ��ʹ�ô���0
#define UART_422 UART_1 //DEMO ʹ�ô���3�շ����ݣ���ʽ��ʹ�ô���1	
#define LUOPAN_BIT 2
#define CAIJI_BIT 3

#endif /* __TOWER_H__ */
