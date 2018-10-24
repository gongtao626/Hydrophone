#ifndef __Settings__
#define __Settings__

#define DCD_FLAG    0
#define VBUS_FLAG   1
#define DEBUG_FLAG  2

#define FLAG_SET(BitNumber, Register)        (Register |=(1<<BitNumber))
#define FLAG_CLR(BitNumber, Register)        (Register &=~(1<<BitNumber))
#define FLAG_CHK(BitNumber, Register)        (Register & (1<<BitNumber))

#define _USB     0


#endif /*__Settings__*/
