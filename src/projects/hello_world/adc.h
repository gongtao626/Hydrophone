#ifndef __ADC_H
#define __ADC_H    1

//头文件
#include "common.h"

//宏定义


//读取四路信号
void adc_spi_read( void );
//AD7606复位
void adc_reset( void );

#endif //__ADC_H