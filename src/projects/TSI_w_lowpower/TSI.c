
#include "TSI.h"

extern uint32 __VECTOR_RAM[];

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
    SIM_SCGC5 |= (SIM_SCGC5_TSI_MASK); //Turn on clock to TSI module
    PORTA_PCR4 = PORT_PCR_MUX(0);      //Enable ALT0 for portA4

    TSI0_GENCS |= TSI_GENCS_EOSF_MASK;
    
    TSI0_GENCS |= ((TSI_GENCS_NSCN(10))|(TSI_GENCS_PS(4))|(TSI_GENCS_STPE_MASK)|
                   (TSI_GENCS_LPSCNITV(7)));
    TSI0_SCANC |= ((TSI_SCANC_EXTCHRG(9))| 
                  (TSI_SCANC_REFCHRG(15))|
                  (TSI_SCANC_SMOD(10))|
                  (TSI_SCANC_AMPSC(1))|
                  (TSI_SCANC_AMCLKS(0)));

    //TSI0_GENCS |= TSI_GENCS_LPCLKS_MASK;
  
    /* Init TSI interrupts */
    enable_irq(37);
    
    ELECTRODE_ENABLE_REG = ELECTRODE0_EN_MASK|ELECTRODE1_EN_MASK;
    TSI0_PEN |= TSI_PEN_LPSP(WAKEELECTRODE);
    TSI0_GENCS |= (TSI_GENCS_TSIEN_MASK);  //Enables TSI
}

/********************************************************************************
 *   TSI_GoToLP: Changes config to go to low power
 * Notes:
 *    -
 ********************************************************************************/
void TSI_GoToLP(void)
{
    DISABLE_TSI;
  
    while(!TSI0_GENCS&TSI_GENCS_EOSF_MASK){}; //Wait for last scan to happen
  
    TSI0_GENCS |= TSI_GENCS_OUTRGF_MASK;   //Clear all pending flags
    TSI0_GENCS |= TSI_GENCS_EOSF_MASK;
    
    TSI0_GENCS &= ~(TSI_GENCS_ESOR_MASK);
    ENABLE_TSI;

}


void TSI_GoToActive(void)
{
  DISABLE_TSI;
  
  //while(!TSI0_GENCS&TSI_GENCS_EOSF_MASK){}; //Wait for last scan to happen
  
  TSI0_GENCS |= TSI_GENCS_OUTRGF_MASK;   //Clear all pending flags
  TSI0_GENCS |= TSI_GENCS_EOSF_MASK;
  
  TSI0_GENCS |= (TSI_GENCS_ESOR_MASK);
  
  ENABLE_TSI;  //Enables TSI
}

/********************************************************************************
 *   TSI_SelfCalibration: Simple auto calibration version
 * Notes:
 *    - Very simple, only sums a prefixed amount to the current baseline
 ********************************************************************************/
void TSI_SelfCalibration(void)
{
  TSI0_GENCS |= TSI_GENCS_SWTS_MASK;

  while(!TSI0_GENCS&TSI_GENCS_EOSF_MASK){};

  delay(90000);
  
  g16ElectrodeBaseline[ELECTRODE0] = ELECTRODE0_COUNT;
  g16ElectrodeTouch[ELECTRODE0] = g16ElectrodeBaseline[ELECTRODE0] + ELECTRODE0_TOUCH;
  

  g16ElectrodeBaseline[ELECTRODE1] = ELECTRODE1_COUNT;
  g16ElectrodeTouch[ELECTRODE1] = g16ElectrodeBaseline[ELECTRODE1] + ELECTRODE1_TOUCH;

  ELECTRODE0_OUTRG = (uint32)g16ElectrodeTouch[WAKEELECTRODE];

  DISABLE_TSI;

}

/********************************************************************************
 *   TSI_isr: TSI interrupt subroutine
 * Notes:
 *    -
 ********************************************************************************/

uint32 GENCSInt1, GENCSInt2;

void TSI_isr(void)
{
  static uint16 TouchEvent = 0;
  uint16 lValidTouch = 0;
  uint16 l16Counter;

  GENCSInt1 = TSI0_GENCS;
  
  TSI0_GENCS |= TSI_GENCS_OUTRGF_MASK;
  TSI0_GENCS |= TSI_GENCS_EOSF_MASK;

  GENCSInt2 = TSI0_GENCS;

  /* Process electrode 0 */
  l16Counter = ELECTRODE0_COUNT;
  if(l16Counter>g16ElectrodeTouch[ELECTRODE0]) //See if detected a touch
  {
    TouchEvent |= (1<<ELECTRODE0); //Set touch event variable
    g32DebounceCounter[ELECTRODE0]--; //Decrement debounce counter
    if(!g32DebounceCounter[ELECTRODE0]) //If debounce counter reaches 0....
    {
      lValidTouch |= ((1<<ELECTRODE0)); //Signal that a valid touch has been detected
      TouchEvent &= ~(1<<ELECTRODE0);  //Clear touch event variable
    }
  }
  else
  {
    TouchEvent &= ~(1<<ELECTRODE0); //Clear touch event variable
    g32DebounceCounter[ELECTRODE0] = DBOUNCE_COUNTS; //Reset debounce counter
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

  if(lValidTouch&((1<<ELECTRODE0))) //If detected a valid touch...
  {
    LED0_TOGGLE; //Toggle LED
    lValidTouch &= ~((1<<ELECTRODE0)); //Clear valid touch
  }
  if(lValidTouch&((1<<ELECTRODE1)))
  {
    LED1_TOGGLE;
    lValidTouch &= ~((1<<ELECTRODE1));
  }


}

