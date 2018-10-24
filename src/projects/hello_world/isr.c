//-------------------------------------------------------------------------
// �ļ����ƣ�isr.c                                                          
// ���ܸ�Ҫ���жϴ�������
// ��Ȩ����: 
// �汾����:    ʱ��                         �汾                                 �޸�
//           2012-8-14     V1.0       422������������жϴ�����
//-------------------------------------------------------------------------
#include "common.h"

extern uint8 RX_422_buff[13];//MCU���������ǵ��������
extern uint8 RX_422_index;
extern uint8 RX_422_len;
extern uint8 TX_422_buff[80];//�����Ƿ��ظ�MCU�����ݣ�����
extern uint8 TX_422_index;
extern uint8 TX_422_len;
extern uint8 SPI_rec_int_flag;//�����жϱ�־
extern uint8 RS232_rec_com_flag;//�����жϱ�־
extern uint8 RS422_rec_com_flag;//�����жϱ�־

//extern uint32 AlarmTimes; //������ʱ����ʱ���

//============================================================================
//�������ƣ�uart_rs232_isr
//�������أ���
//����˵������
//���ܸ�Ҫ��RS232����0���ݽ����ж����̡�
//============================================================================
void uart_rs232_isr(void)
{
  //uint8 p = 0;
  //uint8 buff;
  DisableInterrupts;    //�����ж�
  
  //����һ���ֽ�����
  
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
  EnableInterrupts;     //�����ж�
}
//============================================================================
//�������ƣ�uart_rs422_isr
//�������أ���
//����˵������
//���ܸ�Ҫ��RS422����1���ݽ����ж����̡�
//============================================================================
void uart_rs422_isr(void)
{
//  uint8 p;
//  //uint8 buff;
//  //uint8 i=0;
  DisableInterrupts;    //�����ж�
  TX_422_buff[RX_422_index+1] = uart_getchar (UART_232);
  uart_putchar (UART_232, TX_422_buff[TX_422_index+1]);
//  //����һ���ֽ�����
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
  EnableInterrupts;     //�����ж�
}

//============================================================================
//�������ƣ�spi0_isr
//�������أ���
//����˵������
//���ܸ�Ҫ��spi0���ݽ����ж����̡�
//============================================================================
/*
void spi0_isr(void)
{
uint8 i = 0,SPIRecDatalenght;   
uint16 SPIRecData;

������Ҫ�ı�
SPIRecDatalenght = hw_spi_re(&SPIRecData);
for(i = 0;i < SPIRecDatalenght;i++)
{
hw_uart_send1(UART_422,SPIRecData);
    }
}
*/


//==========================================================================
//�������ƣ�pit0_isr                                                        
//���ܸ�Ҫ���ж�����                                                
//����˵������       
//�������أ���                                                                  
//==========================================================================
void pit0_isr(void)
{ 
	if((PIT_TFLG(0)&PIT_TFLG_TIF_MASK)!=0)
	{
                hw_uart_sendN(UART_232, 4, "s\?\x0D\x0A");//��������������
		PIT_TFLG(0)|=PIT_TFLG_TIF_MASK;       //���־
	}
}


