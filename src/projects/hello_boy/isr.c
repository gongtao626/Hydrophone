//-------------------------------------------------------------------------
// 文件名称：isr.c                                                          
// 功能概要：中断处理例程
// 版权所有: 
// 版本更新:    时间                         版本                                 修改
//           2012-8-14     V1.0       422串口命令接收中断处理函数
//-------------------------------------------------------------------------
#include "common.h"

extern uint8 RX_422_buff[13];//MCU发往罗盘仪的命令，缓冲
extern uint8 RX_422_index;
extern uint8 RX_422_len;
extern uint8 TX_422_buff[80];//罗盘仪返回给MCU的数据，缓冲
extern uint8 TX_422_index;
extern uint8 TX_422_len;
extern uint8 SPI_rec_int_flag;//接收中断标志
extern uint8 RS232_rec_com_flag;//接收中断标志
extern uint8 RS422_rec_com_flag;//接收中断标志

//extern uint32 AlarmTimes; //报警计时器定时间隔

//============================================================================
//函数名称：uart_rs232_isr
//函数返回：无
//参数说明：无
//功能概要：RS232串口0数据接收中断例程。
//============================================================================
void uart_rs232_isr(void)
{
  //uint8 p = 0;
  //uint8 buff;
  DisableInterrupts;    //关总中断
  
  //接收一个字节数据
  
  TX_422_buff[TX_422_index] = uart_getchar (UART_232);
  uart_putchar (UART_232, TX_422_buff[TX_422_index]);
//  TX_422_index++;
//  
//
//  if(0 == RS232_rec_com_flag)
//  {
//    if((TX_422_index >= 10))
//    {
//        if('\x0A' == TX_422_buff[TX_422_index-1])
//        {
//          RS232_rec_com_flag = 1;
//          TX_422_len = TX_422_index;
//        }
//    }
//  }
  EnableInterrupts;     //开总中断
}
//============================================================================
//函数名称：uart_rs422_isr
//函数返回：无
//参数说明：无
//功能概要：RS422串口1数据接收中断例程。
//============================================================================
void uart_rs422_isr(void)
{
//  uint8 p;
//  //uint8 buff;
//  //uint8 i=0;
  DisableInterrupts;    //关总中断
  TX_422_buff[RX_422_index+1] = uart_getchar (UART_232);
  uart_putchar (UART_232, TX_422_buff[TX_422_index+1]);
//  //接收一个字节数据
//    RX_422_buff[RX_422_index] = hw_uart_re1 (UART_422,&p);
//  
//  if(0 == p && 0 == RS422_rec_com_flag )
//  {
//    RX_422_index++;
//    if((RX_422_index >= 2))
//    {
//      if(RX_422_index <= 13)
//      {
//        if(('\x0D' == RX_422_buff[RX_422_index-2]) && ('\x0A' == RX_422_buff[RX_422_index-1]))
//        {
//          RS422_rec_com_flag = 1;
//          RX_422_len = RX_422_index;
//          RX_422_index = 0;
//        }
//      }
//      else
//      {
//        RX_422_index = 0; 
//      }
//    }
//  }
//  
  EnableInterrupts;     //开总中断
}

//============================================================================
//函数名称：spi0_isr
//函数返回：无
//参数说明：无
//功能概要：spi0数据接收中断例程。
//============================================================================
/*
void spi0_isr(void)
{
uint8 i = 0,SPIRecDatalenght;   
uint16 SPIRecData;

根据需要改变
SPIRecDatalenght = hw_spi_re(&SPIRecData);
for(i = 0;i < SPIRecDatalenght;i++)
{
hw_uart_send1(UART_422,SPIRecData);
    }
}
*/


//==========================================================================
//函数名称：pit0_isr                                                        
//功能概要：中断例程                                                
//参数说明：无       
//函数返回：无                                                                  
//==========================================================================
void pit0_isr(void)
{ 
	if((PIT_TFLG(0)&PIT_TFLG_TIF_MASK)!=0)
	{
                hw_uart_sendN(UART_232, 4, "s\?\x0D\x0A");//发送罗盘仪命令
		PIT_TFLG(0)|=PIT_TFLG_TIF_MASK;       //清标志
	}
}


