
#include "common.h"
#include "misc.h"
#include "pmc.h"

#define ELECTRODE0  0
#define ELECTRODE1  1
#define ELECTRODE2  2
#define ELECTRODE3  3
#define WAKEELECTRODE	0

#define ELECTRODE0_TOUCH  0x100
#define ELECTRODE1_TOUCH  0x100
#define ELECTRODE2_TOUCH  0x100
#define ELECTRODE3_TOUCH  0x100

#define ELECTRODE0_OVRRUN  0xf000
#define ELECTRODE1_OVRRUN  0xf000
#define ELECTRODE2_OVRRUN  0xf000
#define ELECTRODE3_OVRRUN  0xf000

/* Number of scans needed for a touch to remain high to be considered valid */
#define DBOUNCE_COUNTS  0x00000004

#define ELECTRODE_ENABLE_REG    TSI0_PEN

  #define ELECTRODE0_COUNT  (uint16)((TSI0_CNTR1)&0x0000FFFF)
  #define ELECTRODE1_COUNT  (uint16)((TSI0_CNTR7)&0x0000FFFF)
  /* These two are part of the TWRPI socket */
  #define ELECTRODE2_COUNT  (uint16)((TSI0_CNTR15)&0x0000FFFF)
  #define ELECTRODE3_COUNT  (uint16)((TSI0_CNTR5>>16)&0x0000FFFF)

  #define ELECTRODE0_OUTRG  TSI0_THRESHOLD

  #define ELECTRODE0_EN_MASK  TSI_PEN_PEN0_MASK
  #define ELECTRODE1_EN_MASK  TSI_PEN_PEN6_MASK
  /* These two are part of the TWRPI socket */
  #define ELECTRODE2_EN_MASK  TSI_PEN_PEN14_MASK
  #define ELECTRODE3_EN_MASK  TSI_PEN_PEN5_MASK



#define START_SCANNING  TSI0_GENCS |= TSI_GENCS_STM_MASK
#define ENABLE_EOS_INT  TSI0_GENCS |= (TSI_GENCS_TSIIE_MASK|TSI_GENCS_ESOR_MASK)
#define ENABLE_TSI      TSI0_GENCS |= TSI_GENCS_TSIEN_MASK
#define DISABLE_TSI     TSI0_GENCS &= ~TSI_GENCS_TSIEN_MASK


void TSI_Init(void);
void TSI_isr(void);
void TSI_SelfCalibration(void);
void TSI_GoToLP(void);
void TSI_GoToActive(void);

