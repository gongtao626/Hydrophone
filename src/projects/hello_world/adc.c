#include "adc.h"

extern uint16 AD_BUFF[4];

/******************************************************************************
�������ƣ�void adc_spi_read( void )
�������������ݲɼ�
�����������
�����������
�� �� ֵ����
���ú�������
ȫ�ֱ�������
******************************************************************************/
void adc_spi_read( void )
{
   hw_gpio_reverse(PORT_C, 10);//PORTC_10��ӦADCS�ź�, PORTC_10�����ƽ״̬��ת��ADCS��ʼΪ�ߣ���ת������
   AD_BUFF[0] = hw_spi_send_receive(0);
   AD_BUFF[1] = hw_spi_send_receive(0);
   AD_BUFF[2] = hw_spi_send_receive(0);
   AD_BUFF[3] = hw_spi_send_receive(0);
   hw_gpio_reverse(PORT_C, 10);//PORTC_10��ӦADCS�ź�, PORTC_10�����ƽ״̬��ת��ADCS����
}

/******************************************************************************
�������ƣ�void adc_reset( void )
������������λadc7606
�����������
�����������
�� �� ֵ����
���ú�������
ȫ�ֱ�������
******************************************************************************/
void adc_reset( void )
{
  hw_gpio_set(PORT_D, 1, 0); //�Ѿ���ʼ��Ϊ�͵�ƽ
  time_delay_us(1);
  hw_gpio_reverse(PORT_D, 1);   //����
  time_delay_us(1); //���RESET�ߵ�ƽ������
  hw_gpio_reverse(PORT_D, 1);   //����
  time_delay_us(1);
}