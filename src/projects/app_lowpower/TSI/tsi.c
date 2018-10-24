
#include "common.h"
#include "TSI.h"

extern uint32 __VECTOR_RAM[];
uint16 lValidTouch;
uint16 TouchEvent = 0;


uint16  g16ElectrodeTouch[16] = {0};
uint16  g16ElectrodeBaseline[16] = {0};

uint32  g32DebounceCounter[16] = {DBOUNCE_COUNTS};

/********************************************************************************
 *   TSI_Init: Initializes TSI module
 * Notes:
 *    -
 ********************************************************************************/
void TSI_Init(void)
{
    SIM_SCGC5 |= (SIM_SCGC5_TSI_MASK);
    enable_irq(INT_TSI0-16);
    
    /*  Ensures pin configured for TSI operation
    
    */
    PORTB_PCR0 = PORT_PCR_MUX(0);
    PORTB_PCR1 = PORT_PCR_MUX(0);

    
    TSI0_GENCS |= ((TSI_GENCS_NSCN(10))|(TSI_GENCS_PS(3)));
    TSI0_SCANC |= ((TSI_SCANC_EXTCHRG(3))|(TSI_SCANC_REFCHRG(31))/*|(TSI_SCANC_DELVOL(7))|*/| (TSI_SCANC_SMOD(0))|(TSI_SCANC_AMPSC(0)));
    
    ELECTRODE_ENABLE_REG = ELECTRODE0_EN_MASK |ELECTRODE1_EN_MASK | ELECTRODE2_EN_MASK |ELECTRODE3_EN_MASK;
    
    
    
    /* Init TSI interrupts */
    TSI0_GENCS |= (TSI_GENCS_TSIEN_MASK);  //Enables TSI



}

void TSI_Deinit(void)
{
    disable_irq(INT_TSI0-16);
    TSI0_GENCS &= ~(TSI_GENCS_TSIEN_MASK);
    SIM_SCGC5 &= ~(SIM_SCGC5_TSI_MASK);
}

/********************************************************************************
 *   delay: delay
 * Notes:
 *    - 
 ********************************************************************************/
void tsi_delay(uint32 i)
{
  uint32 cnt;
  for(cnt=0;cnt<i;cnt++)  //delay
  {
    asm("nop");
  }
}

/********************************************************************************
 *   TSI_SelfCalibration: Simple auto calibration version
 * Notes:
 *    - Very simple, only sums a prefixed amount to the current baseline
 ********************************************************************************/
void TSI_SelfCalibration(void)
{
    TSI0_GENCS |= TSI_GENCS_SWTS_MASK;
    while(!TSI0_GENCS & TSI_GENCS_EOSF_MASK){};
    
    tsi_delay(100000);
    
    g16ElectrodeBaseline[ELECTRODE0] = ELECTRODE0_COUNT;
    ELECTRODE0_OUTRG = (uint32)((g16ElectrodeBaseline[ELECTRODE0]+ELECTRODE0_OUTRG_VAL));
    g16ElectrodeTouch[ELECTRODE0] = g16ElectrodeBaseline[ELECTRODE0] + ELECTRODE0_TOUCH;
    
    g16ElectrodeBaseline[ELECTRODE1] = ELECTRODE1_COUNT;
    ELECTRODE1_OUTRG = (uint32)((g16ElectrodeBaseline[ELECTRODE1]+ELECTRODE1_OUTRG_VAL));
    g16ElectrodeTouch[ELECTRODE1] = g16ElectrodeBaseline[ELECTRODE1] + ELECTRODE1_TOUCH;
    
    g16ElectrodeBaseline[ELECTRODE2] = ELECTRODE2_COUNT;
    ELECTRODE2_OUTRG = (uint32)((g16ElectrodeBaseline[ELECTRODE2]+ELECTRODE2_OUTRG_VAL));
    g16ElectrodeTouch[ELECTRODE2] = g16ElectrodeBaseline[ELECTRODE2] + ELECTRODE2_TOUCH;
    
    g16ElectrodeBaseline[ELECTRODE3] = ELECTRODE3_COUNT;
    ELECTRODE3_OUTRG = (uint32)((g16ElectrodeBaseline[ELECTRODE3]+ELECTRODE3_OUTRG_VAL));
    g16ElectrodeTouch[ELECTRODE3] = g16ElectrodeBaseline[ELECTRODE3] + ELECTRODE3_TOUCH;
    
    DISABLE_TSI;

}

/********************************************************************************
 *   TSI_isr: TSI interrupt subroutine
 * Notes:
 *    - lValidTouch        
               1
               2 
               4
               8
 ********************************************************************************/


void tsi_isr(void)
{


  uint16 l16Counter;

  TSI0_GENCS |= TSI_GENCS_OUTRGF_MASK;
  TSI0_GENCS |= TSI_GENCS_EOSF_MASK;


  /* Process electrode 0 */
  l16Counter = ELECTRODE0_COUNT;
  if(l16Counter>g16ElectrodeTouch[ELECTRODE0])
  {
    TouchEvent |= (1<<ELECTRODE0);
    g32DebounceCounter[ELECTRODE0]--;
    if(!g32DebounceCounter[ELECTRODE0])
    {
      lValidTouch |= ((1<<ELECTRODE0));
      TouchEvent &= ~(1<<ELECTRODE0);
    }
  }
  else
  {
    TouchEvent &= ~(1<<ELECTRODE0);
    g32DebounceCounter[ELECTRODE0] = DBOUNCE_COUNTS;
  }
  /***********************/

  /* Process electrode 1 */
  l16Counter = ELECTRODE1_COUNT;
  if(l16Counter>g16ElectrodeTouch[ELECTRODE1])
  {
    TouchEvent |= (1<<ELECTRODE1);
    g32DebounceCounter[ELECTRODE1]--;
    if(!g32DebounceCounter[ELECTRODE1])
    {
      lValidTouch |= ((1<<ELECTRODE1));
      TouchEvent &= ~(1<<ELECTRODE1);
    }
  }
  else
  {
    TouchEvent &= ~(1<<ELECTRODE1);
    g32DebounceCounter[ELECTRODE1] = DBOUNCE_COUNTS;
  }
  /***********************/

  /* Process electrode 2 */
  l16Counter = ELECTRODE2_COUNT;
  if(l16Counter>g16ElectrodeTouch[ELECTRODE2])
  {
    TouchEvent |= (1<<ELECTRODE2);
    g32DebounceCounter[ELECTRODE2]--;
    if(!g32DebounceCounter[ELECTRODE2])
    {
      lValidTouch |= ((1<<ELECTRODE2));
      TouchEvent &= ~(1<<ELECTRODE2);
    }
  }
  else
  {
    TouchEvent &= ~(1<<ELECTRODE2);
    g32DebounceCounter[ELECTRODE2] = DBOUNCE_COUNTS;
  }
  /***********************/

  /* Process electrode 3 */
  l16Counter = ELECTRODE3_COUNT;
  if(l16Counter>g16ElectrodeTouch[ELECTRODE3])
  {
    TouchEvent |= (1<<ELECTRODE3);
    g32DebounceCounter[ELECTRODE3]--;
    if(!g32DebounceCounter[ELECTRODE3])
    {
      lValidTouch |= ((1<<ELECTRODE3));
      TouchEvent &= ~(1<<ELECTRODE3);
    }
  }
  else
  {
    TouchEvent &= ~(1<<ELECTRODE3);
    g32DebounceCounter[ELECTRODE3] = DBOUNCE_COUNTS;
  }

 // TSI0_STATUS = 0xFFFFFFFF;
}

