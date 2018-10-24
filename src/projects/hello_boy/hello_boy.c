
/*****************************************************************************************************
* Include files
*****************************************************************************************************/
#include "common.h"

//RS422���յ�����֡�����RX_422_buff[13], ���ʽΪ��
//Byte0: 422����
//Byte1: �̿�ѡ��
//Byte2--ByteEnd: 232�ı�����Իس�������\x0D\x0A������
//RS422���صĽ��֡�����TX_422_buff[80]�У����ʽΪ��
//Byte0 Byte1:
//Byte2 Byte3
//Byte4 Byte5:
//Byte6 Byte7:
//Byte8--ByteEnd��RS232 ���ص����ݺ���ų�������
//ȫ�ֱ�������
//
uint8 RX_422_buff[13];//MCU���������ǵ��������
uint8 RX_422_index;
uint8 RX_422_len;
uint8 RS422_rec_com_flag;//�жϽ�����ɱ�־

uint8 TX_422_buff[64];//�����Ƿ��ظ�MCU�����ݣ�����
uint8 TX_422_index = AD_NUM;//TX_422_buff[08]��ʼ���
uint8 TX_422_len = AD_NUM;
uint8 SPI_rec_int_flag;//�����жϱ�־
uint8 RS232_rec_com_flag;//�����жϱ�־

uint8 uart_rec_ok=1;//���ڽ��ճɹ��жϱ�־��uart_rec_ok=0���ճɹ���uart_rec_ok=1����ʧ��


/**
* \brief   main
* \author  
* \param  none
* \return none
* \todo
* \warning
*/  
int main (void)
{
  //uint16 ad_cfg = 0x3FCF<<2;//MSB���ȣ�����Ҫ����������14λ
  //���ǵ�ǰ�������ã�����������ͨ��������ͨ��ѡ����IN3��ȫ����
  //�ڲ���׼��ѹԴ��REF = 4.096v��ͨ����������ɨ��IN0��IN3�����ض���������
  uint16 ad_cfg = 0x3F4F<<2;//�ĳ�����ͨ��ɨ��IN0 IN1 IN2
  uint16 ad_cfg_hold = 0x0; //���в�������DIN���ڵ͵�ƽ
  uint32 tmp1 = 0;//�ŵ�1 A/D�ɼ�������ʱ�洢�����ھ�ֵ�˲�
  uint32 tmp2 = 0;
  uint32 tmp3 = 0;
  uint16 spi_data = 0; //��Ųɼ����
  
  uint16 i = 0;//A/D��βɼ�ѭ������
  uint8 spi_begin = 0;//SPI����������־
  
  DisableInterrupts;      //��ֹ���ж�
#ifdef DEBUG
  hw_uart1_422_init();
#else
  hw_uart0_232_init();
#endif
  
  hw_spi_init(MASTER);          //AD��MCUͨѶ����Ӧ��SPI0�����ճ�ʼ�����趨������Ϊ12M
  //hw_pit_init(PIT0,periph_clk_khz * 1000);                 //��ʼ��pit0������1s 
  
  /*�Ŵ����̿س�ʼ���趨��Ĭ��10���Ŵ�
  PTC8  = I0_L;    	PTC9  = I0_H;
  PTC10 = I1_L;    	PTC11 = I1_H;
  PTD0  = I2_L;    	PTD1  = I2_H;
  PTD2  = I3_L;    	PTD3  = I3_H;
  */
  hw_gpio_init(PORT_C, 8, 1, 0);	//�趨�̿�ѡ��˿�PTC8���趨Ϊ������˿ڳ�ʼ��ƽΪ0
  hw_gpio_init(PORT_C, 9, 1, 0);	//�趨�̿�ѡ��˿�PTC9���趨Ϊ������˿ڳ�ʼ��ƽΪ0
  //hw_gpio_init(PORT_C,10, 1, 0);	//�趨�̿�ѡ��˿�PTC10���趨Ϊ������˿ڳ�ʼ��ƽΪ0
  //hw_gpio_init(PORT_C,11, 1, 0);	//�趨�̿�ѡ��˿�PTC11���趨Ϊ������˿ڳ�ʼ��ƽΪ0
  //hw_gpio_init(PORT_D, 0, 1, 0);	//�趨�̿�ѡ��˿�PTD0���趨Ϊ������˿ڳ�ʼ��ƽΪ0
  //hw_gpio_init(PORT_D, 1, 1, 0);	//�趨�̿�ѡ��˿�PTD1���趨Ϊ������˿ڳ�ʼ��ƽΪ0
  
  //hw_gpio_init(PORT_D, 2, 1, 0);	//�趨�̿�ѡ��˿�PTD2���趨Ϊ������˿ڳ�ʼ��ƽΪ0
  //hw_gpio_init(PORT_D, 3, 1, 0);	//�趨�̿�ѡ��˿�PTD3���趨Ϊ������˿ڳ�ʼ��ƽΪ0
  
#ifdef DEBUG
  //hw_uart0_232_re_int_open();
#else
  hw_enable_pit_int(PIT0);                               //��pit�ж�    
  hw_uart0_232_re_int_open();
  hw_uart1_422_re_int_open();
#endif
  EnableInterrupts;			        //�����ж�
  
  
  //char ch;
  printf("\nRunning the hello_boy project in K1050MHz family\n");
  
  //  for(;;)
  //  {
  //    time_delay_ms(10);
  //    ch = uart_getchar (UART_232);
  //    uart_putchar (UART_232, ch);
  //    
  //  }
  for (;;)
  {
    //��ѭ�����
    if(RS422_rec_com_flag == 1)//У�飬��ʼ���룬005A
    {
      //422���ʱ仯
      //����С��200mʱ��������ߴ������ʵ�300K
      //�������200m�����Խ��ʹ������ʵ����150K
      
      switch(RX_422_buff[0])
      {
      case 1:
        //hw_uart_change_baudrate(UART_422, 300000);
        break;
      case 2:
        //hw_uart_change_baudrate(UART_422, 150000);
        break;
      case 0:
        break;
      default:
        break;
      }
      //�̿�ѡ�񣺲��޸�ʱ�����ֽ�Ĭ�Ϸ���0����·�ź�ȫ�ǡ�
      //Ix_H=0 Ix_L=0, 10�� 
      //Ix_H=0 Ix_L=1, 100�� 
      //Ix_H=1 Ix_L=0, 1000�� 
      //Ix_H=1 Ix_L=1, �޶��壬δʹ�ã�оƬ��ֻ�������ַŴ�����ѡ��
      if(RX_422_buff[1] != 0)//case �趨 ��switch(10/100/1000)
      {
        hw_gpio_set(PORT_C, 8, BGET(1,RX_422_buff[1]));//PTC8  = I0_L;
        hw_gpio_set(PORT_C, 9, BGET(1,RX_422_buff[1]));//PTC9  = I0_H;
        // hw_gpio_set(PORT_C,10, BGET(1,RX_422_buff[1]));//PTC10 = I1_L;
        // hw_gpio_set(PORT_C,11, BGET(1,RX_422_buff[1]));//PTC11 = I1_H;
        // hw_gpio_set(PORT_D, 0, BGET(1,RX_422_buff[1]));//PTD0  = I2_L;
        // hw_gpio_set(PORT_D, 1, BGET(1,RX_422_buff[1]));//PTD1  = I2_H;
        //hw_gpio_set(PORT_D, 2, BGET(1,RX_422_buff[1]));//PTD2  = I3_L;�޵���·ˮ����
        //hw_gpio_set(PORT_D, 3, BGET(1,RX_422_buff[1]));//PTD3  = I3_H;
      }
      //232�����
      hw_uart_sendN(UART_232, (uint16)(RX_422_len-2), (uint8* )RX_422_buff+2);
      
      RX_422_len = 0;//���������������
      RS422_rec_com_flag = 0;//�������½�������
      //������ϣ�����SPI��������
      spi_begin = 1;
    }
    //spi����1�����ÿ�ʼ,������Ϊ92.3K
    
    //TODO:�޸�SCK���ͺ�����HALTλ��0��ѡͨSS�ź�
    if(1 == spi_begin)// && 0 == RS422_rec_com_flag)
    {
      hw_spi_start_transfer();
      hw_spi_send(&ad_cfg, 1);//��ʼ��AD7682�����͵ڶ�����Ԫ
      
      for(i = 0; i < AD_COUNT; i++)
      {
        hw_spi_send(&ad_cfg_hold, 1);
        
        if(1 == hw_spi_re(&spi_data))
        {
          tmp1 += spi_data;
          hw_spi_send(&ad_cfg_hold, 1);
        }
        if(1 == hw_spi_re(&spi_data))
        {
          tmp2 += spi_data;
          hw_spi_send(&ad_cfg_hold, 1);
        }
        if(1 == hw_spi_re(&spi_data))
        {
          tmp3 += spi_data;
        }
        //TODO ������
      }
      //����CNVѡͨSS�źţ�HALTλ��1
      hw_spi_stop_transfer();	    
      spi_begin = 0;
      *(uint16 *)TX_422_buff = tmp1/AD_COUNT;
      *(uint16 *)(TX_422_buff+2) = tmp2/AD_COUNT;
      *(uint16 *)(TX_422_buff+4) = tmp3/AD_COUNT;

    }
    
      //hw_uart_sendN(UART_232, 4, "b\?\x0D\x0A");
      //delay(1000);
    if( 0 == spi_begin)
    {
      if(1 == RS232_rec_com_flag)
      {
        if('$' == TX_422_buff[AD_NUM])
        {
#ifdef DEBUG
          hw_uart_sendN(UART_232, (uint16)(TX_422_len), (uint8* )TX_422_buff);
#else
          hw_uart_sendN(UART_422, (uint16)(TX_422_len), (uint8* )TX_422_buff);
#endif            
        }
        TX_422_buff[AD_NUM] = '\x0';
        TX_422_buff[TX_422_index-1] = '\x0';
        RS232_rec_com_flag = 0;
        TX_422_len = AD_NUM;
        TX_422_index = AD_NUM;
      }
      else
      {
        TX_422_buff[AD_NUM]= '\x0';
#ifdef DEBUG
        hw_uart_sendN(UART_232, AD_NUM, (uint8* )TX_422_buff);
#else
        hw_uart_sendN(UART_422, AD_NUM, (uint8* )TX_422_buff);
#endif        
      }
      spi_begin = 1;
    }
  } 
}
/********************************************************************/
