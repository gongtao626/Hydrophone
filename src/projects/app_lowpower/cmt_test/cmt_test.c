/*  

 carrier modulator timer test 

  

 send a signal like UART modulated
 TONE space TONE et
 TONE is zero   at 1200 baudios


CMT frequency input is from BUSCLOCK in this case 50 MHZ

CMT IF must be in the 8MHZ range     50/8= 6.25


*/
#include "common.h"


#define BUSCLOCK 50000000
#define CMT_FREQ 8000000


uint16 cmt_data;
char cmt_data_count;
char cmt_isrv_count;

#define CMT_PPSDIV_VAL     5  //7      CMT freq = 8.333.333

#define CARRIER_HIGH      89  // 40KHz
#define CARRIER_LOW       89
// Not used on TIME BASE
#define CARRIER_HIGH2      104
#define CARRIER_LOW2       104


#define BIT_TIME        1736  // 600 Baud




#define CMT_CG1_TIME(x)   CMT_CGH1 = x>>8; CMT_CGL1 = x&0xFF;
#define CMT_CG2_TIME(x)   CMT_CGH2 = x>>8; CMT_CGL2 = x&0xFF;


#define CMT_MARK_TIME(x)   CMT_CMD1 = x>>8; CMT_CMD2 = x&0xFF;
#define CMT_SPACE_TIME(x)   CMT_CMD3 = x>>8; CMT_CMD4 = x&0xFF;

void cmt_init(void)
 {
   SIM_SCGC4 |= SIM_SCGC4_CMT_MASK;
  // SIM_SOPT2 |= SIM_SOPT2_CMTUARTPAD_MASK;   // Enable CMT_OUTPUT double buffer SOPT2[CMTUARTPAD]
   SIM_SOPT2 |= SIM_SOPT2_PTD7PAD_MASK;
   enable_irq(INT_CMT - 16);
  
   
   // Pin multilplexing configuration
   
   
   PORTD_PCR7 &= ~PORT_PCR_MUX_MASK;  
   PORTD_PCR7 |= PORT_PCR_MUX(2);  // pin for   CMT_IRO
   
   CMT_PPS = CMT_PPS_PPSDIV(CMT_PPSDIV_VAL);  // acepected values from 0*15   Divider 1-16
   
 
// Highpulse 100 (cycles)  Low Pulse = 100 cycles   
  
   CMT_CGH1 = CARRIER_HIGH; 
   CMT_CGL1 = CARRIER_LOW;
   
   
   CMT_CGH2 = CARRIER_HIGH2; 
   CMT_CGL2 = CARRIER_LOW2;
   
   CMT_OC |= (!CMT_OC_IROL_MASK | CMT_OC_CMTPOL_MASK | CMT_OC_IROPEN_MASK);
 
   CMT_MSC  = CMT_MSC_CMTDIV(0) | !CMT_MSC_FSK_MASK | CMT_MSC_EOCIE_MASK | !CMT_MSC_MCGEN_MASK;  // TIME BASE MODE
   
 }




void cmt_tx_data(char ch)
  {
    cmt_data_count = 10; //start 8 data 1 stop
    cmt_data = (0x100 | ch ) << 1; // introduce STOP bit and start bit
    CMT_MARK_TIME(BIT_TIME);
    CMT_SPACE_TIME(0);
    CMT_MSC  |= CMT_MSC_MCGEN_MASK;  // enable Module and carrier generator
  }


void cmt_isrv(void)
{
  (void) CMT_MSC;  // read CMT_MSC_EOCF_MASK as first pass to clear flag
   cmt_isrv_count++;
   if (cmt_data_count)
  { 
    if (cmt_data & 0x1)  //MSB first
    {
      CMT_MARK_TIME(BIT_TIME);
      CMT_SPACE_TIME(0);
     }
    else
    {
      CMT_SPACE_TIME(BIT_TIME);
      CMT_MARK_TIME(0);
     }  
    cmt_data_count--;
    cmt_data = cmt_data>>1;
    
  }
  else
  {
   CMT_SPACE_TIME(BIT_TIME);  // dummy write to clear CMT_MSC_EOCF_MASK
   CMT_MSC  &= ~CMT_MSC_MCGEN_MASK;  // Disabke Module and carrier generator
  }
  
}