/*
 * File:        common.h
 * Purpose:     File to be included by all project files
 *
 * Notes:
 */

#ifndef _COMMON_H_
#define _COMMON_H_

/********************************************************************/

/*
 * Debug prints ON (#define) or OFF (#undef)
 */
#define DEBUG
#define DEBUG_PRINT

/* 
 * Include the generic CPU header file 
 */
#include "arm_cm4.h"

/* 
 * Include the platform specific header file 
 */
#if (defined(FIREBIRD))
  #include "firebird.h"
#elif (defined(TOWER))
  #include "tower.h"
#elif (defined(BACES))
  #include "baces.h"
#elif (defined(TSIEVB))
  #include "TSIEVB.h"
#else
  #error "No valid platform defined"
#endif

/* 
 * Include the cpu specific header file 
 */
#if (defined(MCU_MK10DZ50))//new!
  #include "MK10D5.h"
#elif (defined(MCU_MK20DZ100))//new!
  #include "MK20DZ10.h"
 
#elif (defined(MCU_MK20DZ50))//new!
  #include "MK20D5.h"
#elif (defined(MCU_MK40DZ100))
  #include "MK40DZ10.h"
#elif (defined(MCU_MK53DZ100))
  #include "MK53DZ10.h"
#elif (defined(MCU_MK60DZ100))
  #include "MK60DZ10.h"
#elif (defined(CPU_MK70F120))
  #include "MK70F15.h"
#elif (defined(CPU_MK60F120))
  #include "MK70F15.h"
#else
  #error "No valid CPU defined"
#endif

/* 
 * Include any toolchain specfic header files 
 */
#if (defined(CW))
  #include "cw.h"
#elif (defined(IAR))
  #include "iar.h"
#elif (defined(__GNUC__))
  #include "gnu.h"
#elif (defined(KEIL))
  //#include "armcc.h"
#elif (defined(MULTI))
  //#include "ghs.h"
#else
#warning "No toolchain specific header included"
#endif

/* 
 * Include common utilities
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assert.h"
#include "startup.h"

#if (defined(IAR))
	#include "intrinsics.h"
#else

#endif



/********************************************************************/

#endif /* _COMMON_H_ */
