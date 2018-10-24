/*
* �������ƣ�water
* Ӳ�����ӣ�
* ����������
* ����Ŀ�ģ�
* ����˵����
* 
* �汾����	2012.8.17 V1.0	��ѭ��ʵ����ɣ�SPIͨ��ϸ����δ��ȫȷ����
* 		2012.8.30 V1.1	(1)��ʱ�ж�ģ�����ӣ���Ҫ���Ǻ�ʱ����RTC����232ͨ�����ø��õ���ϣ�
*				(2)ע��SPIͨ��93.2KSps�����ʣ��ɼ�5��ƽ�����ͣ�
*                               (3)��·�źŵ��趨
*               2012.10.31 V1.2 �����жϹ��ܽ�������������������
*TODO:    1)PIT��ʱ�жϡ�0.2~0.5�룬��ռ�ô���ʱ0.2��
*         2)SPI�жϽ��շ�ʽ����Ļ���������SPI���ݣ�ͬʱ��������
*         3)������Ϣ������ٽ���AD�ɼ��������¶���Ϣ
*/



//ͷ�ļ�
#include "common.h"

//������������ģ��ʹ��
uint8 checksum(uint32 len, uint8 *buff, uint8 computesum);

//DEFINE����������ģ��ʹ��
#define NUM 4 //��ѭ���вɼ� NUM ·�ź�

//������������ģ��ʹ��

//������������ģ��ʹ��
//����1��RS422Э����յ���������RX_422_buff[3], ���ʽΪ��
//Byte0: �ϴν��յ�������
//Byte1: �жϷ�ʽ���յ�������
//Byte2: �ϴθı�Ŵ���ʱ�ı�����С
uint8 RX_422_buff[3];//��λ�����͵�����1�����
uint8 RX_422_index;
uint8 RX_422_len;
uint8 RS422_rec_com_flag;//�жϽ���������ɱ�־

//ǰ20�ֽڴ�������Ƿ��ص��ֽڣ���16�ֽڴ�ŷ��͵���λ������Ϣ֡��֡��Ϊ10�ֽڻ�16�ֽ�
//buff[0]-buff[19]���20�ֽڵ����̶�λ��Ϣ������buff[6-7]buff[8-9]buff[16-17]���X��Ƕȡ�Y��Ƕȡ�������ű�ƫ�ǣ���ռ�����ֽڣ����ֽڴ�Ÿ�8λ�����ֽڴ�ŵ�8λ
//buff[20]-buff[38]�����Ϣ֡��buff[20-21]Ϊ��5A5A��֡ͷ��buff[22]Ϊ֡��
//buff[23-24]buff[25-26]buff[27-28]buff[29-30]��Ϊ�ɼ�����·���ݣ���ռ2���ֽڣ����ֽڴ�ŵ�8λ�����ֽڴ�Ÿ�8λ��buff[31]Ϊ����������Ϣʱ��У��
//������������ʱ��buff[31-32]buff[33-34]buff[35-36]buff[37]Ϊ������buff[6-7]buff[8-9]buff[16-17]buff[18]�����̵�X��Ƕȡ�Y��Ƕȡ�������ű�ƫ�ǣ��¶ȣ�buff[38]ΪУ��
//��������ʱ����֡Ϊ12�ֽ�
//��������ʱ����֡Ϊ19�ֽ�
uint8 TX_422_buff[39];//�����Ƿ��ظ�MCU�����ݣ����壬������˵���е�buff
uint8 TX_422_index;
uint8 TX_422_len;
uint8 RS232_rec_com_flag;//�жϽ��������Ƿ������ݽ�����ɱ�־

uint8 SPI_rec_int_flag;//�жϽ���SPI��ʽ�ɼ���AD������ɱ�־
uint16 AD_BUFF[NUM];//��� NUM ·�ź�ֵ�Ļ�����

//CONST������������ģ��ʹ��

//��Ŀʹ�õĺ���������Code of project wide FUNCTIONS

//�ⲿģ�������ı���
extern uint32 core_clk_khz; //�ں�Ƶ��48000�����ⲿ8Mʱ��4��Ƶ֮��24��Ƶ
extern uint32 periph_clk_khz;   //оƬ����ʱ��Ƶ��24000���ں�Ƶ�ʵ�2��Ƶ

//������

int main(void)
{
  //cfg[13:0]�����и�λ������Ϊ
  //cfg[13]���ø��£�cfg[12:10]����ͨ�����ã�cfg[9:7]ѡ������ͨ��
  //cfg[6]ѡ�����cfg[5:3]��׼��ѹԴ/������ѡ��cfg[2:1]ͨ����������cfg[0]�ض�CFG�Ĵ���
  //spi����16λ��RACģʽ���޷�æָʾ����MSB�ȴ��䣬�����Ҫ��cfg������λ
  
  //�ο��ڲ���ѹԴ4.096V
  //uint16 ad_cfg = 0x3C49<<2;//�ĳ�����ͨ��ɨ��IN0,  ���ǼĴ�����������GND�ο���IN2, ȫ�����ڲ���ѹԴ��4.096��������ɨ�裬���ض�
  //uint16 ad_cfg = 0x3CCF<<2;//�ĳ�����ͨ��ɨ��IN0 IN1�� ���ǼĴ�����������GND�ο���IN2, ȫ�����ڲ���ѹԴ��4.096��������ɨ�裬���ض� 
  //uint16 ad_cfg = 0x3D4F<<2;//�ĳ�����ͨ��ɨ��IN0 IN1 IN2, ���ǼĴ�����������GND�ο���IN2, ȫ�����ڲ���ѹԴ��4.096��������ɨ�裬���ض�
  
  //�ο��ⲿ��ѹԴ5V
  uint16 ad_cfg = 0x3DFF<<2; //�ĳ�����ͨ��ɨ��IN0 IN1 IN2 IN3, ���ǼĴ�����������GND�ο���IN2, ȫ�����ⲿ��ѹԴ���ڻ��塢�����¶ȣ�������ɨ�裬���ض�
  //uint16 ad_cfg = 0x3D7F<<2; //�ĳ�����ͨ��ɨ��IN0 IN1 IN2, ���ǼĴ�����������GND�ο���IN2, ȫ�����ⲿ��ѹԴ���ڻ��塢�����¶ȣ�������ɨ�裬���ض�
  //uint16 ad_cfg = 0x3C79<<2;//�ⲿ��ͨ��IN0��������3D7F��ͬ
  
  
  uint32 dummy_cfg=0;//SPI���͵ĵ�һ��������ad_cfg��Ȼ�������������ɼ���������
  uint32 unvalid_result1; //����������ad_cfg�����ص���Ч����
  uint32 unvalid_result2; //���ͣ�
  uint16 signal_channel; //�ź���ѯ������������AD_BUFF�д������ʱ��·�źŵļ���
  
  RX_422_buff[2] = 0x01;//Ĭ�ϷŴ���
  TX_422_buff[20] = 0x5A; //��ͷ
  TX_422_buff[21] = 0x5A;
  TX_422_buff[22] = 12;//TX_422_buff��TX_422_buff[20]��ʼ��ŷ��͵�֡ͷ
  
  //������ʼ�ȴ�
  RX_422_buff[0] = uart_getchar(UART1_BASE_PTR);
  
  DisableInterrupts;//�����ж�
  hw_uart0_232_init();//ϵͳ��ʼ��ʱ��sysinit.c�еĺ���sysinit�Ѿ����ú�422�Ķ˿�, �˴����������ǵĴ��ڣ�������Ϣ�μ�tower.h
  
  //  /*�Ŵ����̿س�ʼ���趨��Ĭ��10���Ŵ�
  //  PTC8  = I0_L;    	PTC9  = I0_H;
  //  PTC10 = I1_L;    	PTC11 = I1_H;
  //  PTD0  = I2_L;    	PTD1  = I2_H;
  //  PTD2  = I3_L;    	PTD3  = I3_H;
  //  */
  hw_gpio_init(PORT_C, 8, 1, 1);	//�趨�̿�ѡ��˿�PTC8���趨Ϊ������˿ڳ�ʼ��ƽΪ1
  hw_gpio_init(PORT_C, 9, 1, 0);	//�趨�̿�ѡ��˿�PTC9���趨Ϊ������˿ڳ�ʼ��ƽΪ0
  hw_gpio_init(PORT_C,10, 1, 1);	//�趨�̿�ѡ��˿�PTC10���趨Ϊ������˿ڳ�ʼ��ƽΪ1
  hw_gpio_init(PORT_C,11, 1, 0);	//�趨�̿�ѡ��˿�PTC11���趨Ϊ������˿ڳ�ʼ��ƽΪ0
  hw_gpio_init(PORT_D, 0, 1, 1);	//�趨�̿�ѡ��˿�PTD0���趨Ϊ������˿ڳ�ʼ��ƽΪ1
  hw_gpio_init(PORT_D, 1, 1, 0);	//�趨�̿�ѡ��˿�PTD1���趨Ϊ������˿ڳ�ʼ��ƽΪ0
  hw_gpio_init(PORT_D, 2, 1, 1);	//�趨�̿�ѡ��˿�PTD2���趨Ϊ������˿ڳ�ʼ��ƽΪ1
  hw_gpio_init(PORT_D, 3, 1, 0);	//�趨�̿�ѡ��˿�PTD3���趨Ϊ������˿ڳ�ʼ��ƽΪ0
//������ز�һ�µ�����??
  //��������0�����ж�
  hw_uart0_232_re_int_open();//ֱ��������ķ�ʽʵ���жϣ��漰�����üĴ���
  //UART_PFIFO_REG(UART_0) |= UART_PFIFO_RXFE_MASK;//�����жϽ��ջ��壬�ο�K20P64M50SF0RM.PDF�ļ���1073ҳ�����û����С
  
  //��������1�����ж�  
  hw_uart1_422_re_int_open();
  hw_pit_init(PIT0, periph_clk_khz * 100);//ÿ��0.2s����һ������λ����Ϣ�ɼ�����"A2"
  hw_enable_pit_int(PIT0);  
  
  EnableInterrupts;   

  uart_putchar(UART_0, 0xA2); //���βɼ�����
  while(!RS232_rec_com_flag);
  
  //����������Ϣ���ڸı�����С����ֻ��3·�źţ�����ͨ���ı�����С��ʱ���K20DX128VLH5�����ز��ȶ�����
  //time_delay_ms(1);
  //time_delay_ms(1);
  
  SPI0_Init(); //�������ж��������֮ǰ����
  dummy_cfg = hw_spi_send_receive(ad_cfg);
  unvalid_result1 = hw_spi_send_receive(ad_cfg);//cfg
  unvalid_result2 = hw_spi_send_receive(0); //����cfg��DINΪ�ͣ��ο�RACģʽ������ʱ��ͼ
  
  //��ѭ����ʼ  
  for(;;)
  {
    for(signal_channel = 0; signal_channel < NUM; signal_channel++)//��·�źŵĲɼ�
    {
      AD_BUFF[signal_channel] = hw_spi_send_receive(0);
      *(uint16 *)(TX_422_buff+23 + 2*signal_channel) = AD_BUFF[signal_channel];
    }
    
//    *(uint16 *)(TX_422_buff+23) = AD_BUFF[0];
//    *(uint16 *)(TX_422_buff+25) = AD_BUFF[1];
//    *(uint16 *)(TX_422_buff+27) = AD_BUFF[2];
//    *(uint16 *)(TX_422_buff+29) = AD_BUFF[3];
    dummy_cfg++;
    if(RS422_rec_com_flag == 1)//��λ��������������
    {
      RX_422_buff[0] = RX_422_buff[1];
      RS422_rec_com_flag = 0;
    }
    
    
    if( ((RX_422_buff[0] & 0xF0) == 0x50))//У��
    {
      if(0 == BGET(3, RX_422_buff[0]))//�ı�����
      {
        if((RX_422_buff[0] & 0x03) != RX_422_buff[2])//����ı��Ƿ���֮ǰһ��
        {
          RX_422_buff[2] = RX_422_buff[0] & 0x03;
          //�̿�ѡ�񣺲��޸�ʱ��Ĭ��100����·�ź�ͬʱ�仯��
          //Ix_H=0 Ix_L=0, 10�� 
          //Ix_H=0 Ix_L=1, 100�� 
          //Ix_H=1 Ix_L=0, 1000�� 
          //Ix_H=1 Ix_L=1, �޶��壬δʹ�ã�оƬ��ֻ�������ַŴ�����ѡ��
          hw_gpio_set(PORT_C, 8, BGET(0,RX_422_buff[0]));//PTC8  = I0_L;
          hw_gpio_set(PORT_C, 9, BGET(1,RX_422_buff[0]));//PTC9  = I0_H;
          hw_gpio_set(PORT_C,10, BGET(0,RX_422_buff[0]));//PTC10 = I1_L;
          hw_gpio_set(PORT_C,11, BGET(1,RX_422_buff[0]));//PTC11 = I1_H;
          hw_gpio_set(PORT_D, 0, BGET(0,RX_422_buff[0]));//PTD0  = I2_L;
          hw_gpio_set(PORT_D, 1, BGET(1,RX_422_buff[0]));//PTD1  = I2_H;
          hw_gpio_set(PORT_D, 2, BGET(0,RX_422_buff[0]));//PTD2  = I3_L
          hw_gpio_set(PORT_D, 3, BGET(1,RX_422_buff[0]));//PTD3  = I3_H
        }//END OF ����ı��Ƿ���֮ǰһ��
      }//END OF �ı�����
      else//��������
      {
        if(RS232_rec_com_flag == 1)//�Ѿ�����������������
        {
          if(1 == checksum(20, TX_422_buff, 0))//У����������Ϣ���������̲����ֲ�������У����Ϣ�Ĳ�׼ȷ��ɾ������֤
          {
            RS232_rec_com_flag = 0; //ȫ�ֱ�����0��׼����һ��������Ϣ�Ľ���
            TX_422_buff[22] = 19;//֡����
            TX_422_buff[31] = TX_422_buff[6];//�ο�TX_422_buff�����˵��
            TX_422_buff[32] = TX_422_buff[7];
            TX_422_buff[33] = TX_422_buff[8];
            TX_422_buff[34] = TX_422_buff[9];
            TX_422_buff[35] = TX_422_buff[16];
            TX_422_buff[36] = TX_422_buff[17];
            TX_422_buff[37] = TX_422_buff[18];//�¶�
            
            TX_422_buff[38] = checksum(TX_422_buff[22], TX_422_buff+20, 1);
            //����¶���Ϣ������·�Ĵ���
            //TX_422_buff[22] = 19;//֡����
            //TX_422_buff[37] = TX_422_buff[18];//�¶�
            //TX_422_buff[38] = checksum(TX_422_buff[22], TX_422_buff+20, 1);
          }
        }
        else//��������Ϣ
        {
          TX_422_buff[22] = 12;//����
          TX_422_buff[31] = checksum(TX_422_buff[22], TX_422_buff+20, 1);
        }
        hw_uart_sendN(UART_1, TX_422_buff[22], TX_422_buff+20);
      }//END OF ��������
    }//END OF У��
  }//EDN OF for��ѭ�� 
}//END OF main()����


//�������֤У���
//computesum = 1������У���
//computesum = 0�� ��֤У���
uint8 checksum(uint32 len, uint8 *buff, uint8 computesum)
{
  uint16 sum = 0;
  uint8 checksum, i;
  for(i = 0; i < len-1; i++)
  {
    sum += buff[i];
  }
  checksum = sum & 0xFF;
  if(1 == computesum)
  {
    return checksum;
  }
  else
  {
    if(checksum == buff[len-1])
      return 1;
    else
      return 0;
  }
}
////============================================================================
////�������ƣ�uart_rs232_isr
////�������أ���
////����˵������
////���ܸ�Ҫ��RS232����0���ݽ����ж����̡�
////============================================================================
void UART0_isr(void)
{
  DisableInterrupts;    //�����ж�
  TX_422_buff[TX_422_index++] = uart_getchar(UART_0);
  if(TX_422_index == 20)
  {
    RS232_rec_com_flag = 1;
    TX_422_index = 0;
  }
  EnableInterrupts;     //�����ж�
}
//��������жϷ������
void UART1_isr(void)
{
  
  DisableInterrupts;    //�����ж�
  
  RX_422_buff[1] = uart_getchar(UART_1);
  RS422_rec_com_flag = 1;
  
  EnableInterrupts;     //�����ж�
}
//��ʱ�жϷ������
void pit0_isr(void)
{  
  if((PIT_TFLG(0)&PIT_TFLG_TIF_MASK)!=0)
  {
    uart_putchar(UART_0, 0xA2); //���βɼ�����
    PIT_TFLG(0)|=PIT_TFLG_TIF_MASK; //���־
  }
}
