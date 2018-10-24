/**/
void adc_test_init(void);

void adc_start_conversion(uint8 channel);
extern uint32 adc_buffer[];

extern uint16 adc_sample0;
extern uint16 adc_sample1;  

extern char buffer;   //If buffer = 0x80 flag of data ready  bit0 for buffer selection
extern uint16 DMA_errorCount;
extern uint16 dmaIsrCount;