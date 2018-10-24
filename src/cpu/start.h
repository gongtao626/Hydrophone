/*
 * File:	start.h
 * Purpose:	Kinetis start up routines. 
 *
 * Notes:		
 */

#ifndef __START_H__
#define __START_H__

#include "common.h"

// Function prototypes
void start(void);
void cpu_identify(void);
void flash_identify(void);

#if defined(__IAR_SYSTEMS_ICC__)
__ramfunc
#elif defined(CW)
__relocate_code__
#endif
void SpSub(void);
void SpSubEnd(void);

#endif /*__START_H__*/
