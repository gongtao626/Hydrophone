/*  

 carrier modulator timer test 

  

 send a signal like UART modulated
 TONE space TONE et
 TONE is zero   at 1200 baudios


CMT frequency input is from BUSCLOCK in this case 50 MHZ

CMT IF must be in the 8MHZ range     50/8= 6.25


*/


#define BUSCLOCK 50000000
#define CMT_FREQ 8000000

void cmt_init(void);

void cmt_tx_data(char ch);

void cmt_isrv(void);

extern char cmt_isrv_count;