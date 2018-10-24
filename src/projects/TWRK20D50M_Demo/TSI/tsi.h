
#include "common.h"

extern uint16 lValidTouch;
extern uint16 TouchEvent;


#define ELECTRODE0  0
#define ELECTRODE1  1
#define ELECTRODE2  2
#define ELECTRODE3  3

#define ELECTRODE0_TOUCH  0x200
#define ELECTRODE1_TOUCH  0x200
#define ELECTRODE2_TOUCH  0x200
#define ELECTRODE3_TOUCH  0x200

#define ELECTRODE0_OUTRG_VAL  0xf000
#define ELECTRODE1_OUTRG_VAL  0xf000
#define ELECTRODE2_OUTRG_VAL  0xf000
#define ELECTRODE3_OUTRG_VAL  0xf000

/* Number of scans needed for a touch to remain high to be considered valid */
#define DBOUNCE_COUNTS  0x00000010

#define ELECTRODE_ENABLE_REG    TSI0_PEN

#ifdef CPU_MK60N512VMD100

  #define ELECTRODE0_COUNT  (uint16)((TSI0_CNTR5>>16)&0x0000FFFF)
  #define ELECTRODE1_COUNT  (uint16)((TSI0_CNTR7>>16)&0x0000FFFF)
  #define ELECTRODE2_COUNT  (uint16)((TSI0_CNTR9)&0x0000FFFF)
  #define ELECTRODE3_COUNT  (uint16)((TSI0_CNTR9>>16)&0x0000FFFF)

  #define ELECTRODE0_OUTRG  TSI0_THRESHLD5
  #define ELECTRODE1_OUTRG  TSI0_THRESHLD7
  #define ELECTRODE2_OUTRG  TSI0_THRESHLD8
  #define ELECTRODE3_OUTRG  TSI0_THRESHLD9

  #define ELECTRODE0_EN_MASK  TSI0_PEN_PEN5_MASK
  #define ELECTRODE1_EN_MASK  TSI0_PEN_PEN7_MASK
  #define ELECTRODE2_EN_MASK  TSI0_PEN_PEN8_MASK
  #define ELECTRODE3_EN_MASK  TSI0_PEN_PEN9_MASK

#elif (defined(CPU_MK40N512VMD100))

  #define ELECTRODE0_COUNT  (uint16)((TSI0_CNTR9>>16)&0x0000FFFF)
  #define ELECTRODE1_COUNT  (uint16)((TSI0_CNTR11)&0x0000FFFF)
  #define ELECTRODE2_COUNT  (uint16)((TSI0_CNTR11>>16)&0x0000FFFF)
  #define ELECTRODE3_COUNT  (uint16)((TSI0_CNTR13)&0x0000FFFF)

  #define ELECTRODE0_OUTRG  TSI0_THRESHLD9
  #define ELECTRODE1_OUTRG  TSI0_THRESHLD10
  #define ELECTRODE2_OUTRG  TSI0_THRESHLD11
  #define ELECTRODE3_OUTRG  TSI0_THRESHLD12

  #define ELECTRODE0_EN_MASK  TSI0_PEN_PEN9_MASK
  #define ELECTRODE1_EN_MASK  TSI0_PEN_PEN10_MASK
  #define ELECTRODE2_EN_MASK  TSI0_PEN_PEN11_MASK
  #define ELECTRODE3_EN_MASK  TSI0_PEN_PEN12_MASK
#elif (defined(CPU_MK10X256VLH72))

  #define ELECTRODE0_COUNT  (uint16)((TSI0_CNTR1)&0x0000FFFF)
  #define ELECTRODE1_COUNT  (uint16)((TSI0_CNTR7)&0x0000FFFF)
  #define ELECTRODE2_COUNT  (uint16)((TSI0_CNTR15>>16)&0x0000FFFF)
  #define ELECTRODE3_COUNT  (uint16)((TSI0_CNTR15)&0x0000FFFF)

  #define ELECTRODE0_OUTRG  TSI0_THRESHLD0
  #define ELECTRODE1_OUTRG  TSI0_THRESHLD6
  #define ELECTRODE2_OUTRG  TSI0_THRESHLD15
  #define ELECTRODE3_OUTRG  TSI0_THRESHLD14

  #define ELECTRODE0_EN_MASK  0//TSI_PEN_PEN0_MASK
  #define ELECTRODE1_EN_MASK  0//TSI_PEN_PEN6_MASK
  #define ELECTRODE2_EN_MASK  TSI_PEN_PEN15_MASK
  #define ELECTRODE3_EN_MASK  TSI_PEN_PEN14_MASK
#else

  #define ELECTRODE0_COUNT  (uint16)((TSI0_CNTR1)&0x0000FFFF)
  #define ELECTRODE1_COUNT  (uint16)((TSI0_CNTR7)&0x0000FFFF)
  #define ELECTRODE2_COUNT  (uint16)((TSI0_CNTR15>>16)&0x0000FFFF)
  #define ELECTRODE3_COUNT  (uint16)((TSI0_CNTR15)&0x0000FFFF)
/*
  #define ELECTRODE0_OUTRG  TSI0_THRESHLD0
  #define ELECTRODE1_OUTRG  TSI0_THRESHLD6
  #define ELECTRODE2_OUTRG  TSI0_THRESHLD15
  #define ELECTRODE3_OUTRG  TSI0_THRESHLD14
*/


  #define ELECTRODE0_OUTRG  TSI0_THRESHOLD
  #define ELECTRODE1_OUTRG  TSI0_THRESHOLD
  #define ELECTRODE2_OUTRG  TSI0_THRESHOLD
  #define ELECTRODE3_OUTRG  TSI0_THRESHOLD




  #define ELECTRODE0_EN_MASK  TSI_PEN_PEN0_MASK 
  #define ELECTRODE1_EN_MASK  TSI_PEN_PEN6_MASK
  #define ELECTRODE2_EN_MASK  !TSI_PEN_PEN15_MASK
  #define ELECTRODE3_EN_MASK  !TSI_PEN_PEN14_MASK
#endif



#define START_SCANNING  TSI0_GENCS |= TSI_GENCS_STM_MASK
#define ENABLE_EOS_INT  TSI0_GENCS |= (TSI_GENCS_TSIIE_MASK|TSI_GENCS_ESOR_MASK)
#define ENABLE_TSI      TSI0_GENCS |= TSI_GENCS_TSIEN_MASK
#define DISABLE_TSI     TSI0_GENCS &= ~TSI_GENCS_TSIEN_MASK


void TSI_Init(void);
void TSI_Deinit(void);
void tsi_isr(void);
void TSI_SelfCalibration(void);



#define ELECTRODE1_ON  (TouchEvent & (1<<ELECTRODE0))

#define ELECTRODE2_ON  (TouchEvent & (1<<ELECTRODE1))