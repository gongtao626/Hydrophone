/*
* �������ƣ�Paral_Four_channel_waterHearing
* Ӳ�����ӣ�
* ����������
* ����Ŀ�ģ�
* ����˵����
* 
* �汾���� 
*  2012.12.07 V1.0  ��ѭ��ʵ�����
*  2012.12.14 V1.1  �����жϹ��ܽ�������������������
* TODO
*   1)TIME_DELAY_US(1) ʵ��
*   2)ad_reset�ź�,
*   3)SPI���շ�ʽ����λ�����Ե���������Ƭѡ�źţ�����SCK�źţ�
*
*/

//ͷ�ļ�
#include "common.h"

//������������ģ��ʹ��
uint8 checksum(uint32 len, uint8 *buff, uint8 computesum);

//DEFINE����������ģ��ʹ��
#define NUM 4 //��ѭ���вɼ� NUM ·�ź�

//������������ģ��ʹ��

//������������ģ��ʹ��
//�ɳ�������Щδ��ʼ����ȫ�ֱ�����Ϊ0

//����1��RS422Э����յ���������RX_422_buff[3], ���ʽΪ��
//Byte0: �ϴν��յ�������
//Byte1: �жϷ�ʽ���յ�������
//Byte2: �ϴθı�Ŵ���ʱ�ı�����С
uint8 RX_422_buff[4];//��λ�����͵�����1�����
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

uint8 BUSY_SAMPLE_END_flag;//��·���ݲ���������־
uint8 CONVST_DOWN_PIT1_flag; //�����ʿ����ź�CONVST��ʱ��־��Ϊ1ʱ������������
uint8 SPI_rec_int_flag;//�жϽ���SPI��ʽ�ɼ���AD������ɱ�־
uint16 AD_BUFF[NUM];//��� NUM ·�ź�ֵ�Ļ�����, ���ڴ��adc7606�ɼ���4��ͨ��������

//CONST������������ģ��ʹ��

//��Ŀʹ�õĺ���������Code of project wide FUNCTIONS

//�ⲿģ�������ı���
extern uint32 core_clk_khz; //�ں�Ƶ��48000�����ⲿ8Mʱ��4��Ƶ֮��24��Ƶ
extern uint32 periph_clk_khz;   //оƬ����ʱ��Ƶ��24000���ں�Ƶ�ʵ�2��Ƶ

//������

int main(void)
{ 
  uint16 signal_channel; //�ź���ѯ������������AD_BUFF�д������ʱ��·�źŵļ���
  
  RX_422_buff[2] = 0x01;//Ĭ�ϷŴ���
  RX_422_buff[3] = 0x53;//Ĭ�ϲ��ı�����ʱ�Ŵ���
  //TX_422_buff��TX_422_buff[20]��ʼ��ŷ��͵�֡
  TX_422_buff[20] = 0x5A; //֡ͷ
  TX_422_buff[21] = 0x5A; //֡ͷ
  TX_422_buff[22] = 12; //������ʱ��֡��
  time_delay_us(1);
  
  //������ʼ�ȴ�
  RX_422_buff[0] = uart_getchar(UART1_BASE_PTR);
  
  DisableInterrupts;//�����ж�
  hw_uart0_232_init();//ϵͳ��ʼ��ʱ��sysinit.c�еĺ���sysinit�Ѿ����ú�422�Ķ˿�, �˴����������ǵĴ��ڣ�������Ϣ�μ�tower.h
  
  //  /*�Ŵ����̿س�ʼ���趨��Ĭ��100���Ŵ�
  //  PTC8  = I0_L;    	PTC9  = I0_H;
  hw_gpio_init(PORT_C, 8, 1, 1);	//�趨�̿�ѡ��˿�PTC8���趨Ϊ������˿ڳ�ʼ��ƽΪ1
  hw_gpio_init(PORT_C, 9, 1, 0);	//�趨�̿�ѡ��˿�PTC9���趨Ϊ������˿ڳ�ʼ��ƽΪ0
  
  // PTC10 = ADCS;    	
  // PTC11 = FRSTDATA;��̬��
  // PTD0  = CONVST;
  // PTD1  = ADRESET;
  hw_gpio_init(PORT_C,10, 1, 1); //�趨�̿�ѡ��˿�PTC10���趨Ϊ�����ADCS��ʼ��ƽΪ1
  hw_gpio_init(PORT_C,11, 0, 0); //�趨�̿�ѡ��˿�PTC11���趨Ϊ���룬FRSTDATA�˿ڳ�ʼ��ƽΪ0��
  hw_gpio_init(PORT_D, 0, 1, 1); //�趨�̿�ѡ��˿�PTD0���趨Ϊ�����CONVST��ʼ��ƽΪ1
  hw_gpio_init(PORT_D, 1, 1, 0); //�趨�̿�ѡ��˿�PTD1���趨Ϊ�����ADRESET��ʼ��ƽΪ0, ������AD7606��λ
  
  //PTC0 = OS0 PTC1 = OS1 PTC2 = OS2
  //OS[2:0] 000-�޹����� 001-2�� 010-4����2���ݴη�����������111��Ч
  //59-2����5A-4����5B-8����5C-16����5D-32����5E-64��
  if((RX_422_buff[0] > 0x58) &&(RX_422_buff[0] < 0x5F))//У��
  {
    hw_gpio_init(PORT_C, 0, 1, BGET(0,RX_422_buff[0])); //�趨�̿�ѡ��˿�PTC0���趨Ϊ�����OS0��ʼ��ƽΪBGET(0,RX_422_buff[0])
    hw_gpio_init(PORT_C, 1, 1, BGET(1,RX_422_buff[0])); //�趨�̿�ѡ��˿�PTC1���趨Ϊ�����OS1��ʼ��ƽΪBGET(1,RX_422_buff[0])
    hw_gpio_init(PORT_C, 2, 1, BGET(2,RX_422_buff[0])); //�趨�̿�ѡ��˿�PTC2���趨Ϊ�����OS2��ʼ��ƽΪBGET(2,RX_422_buff[0])
  }
  else
  {
    hw_gpio_init(PORT_C, 0, 1, 0); //�趨�̿�ѡ��˿�PTC0���趨Ϊ�����OS0��ʼ��ƽΪ0
    hw_gpio_init(PORT_C, 1, 1, 0); //�趨�̿�ѡ��˿�PTC1���趨Ϊ�����OS1��ʼ��ƽΪ0
    hw_gpio_init(PORT_C, 2, 1, 0); //�趨�̿�ѡ��˿�PTC2���趨Ϊ�����OS2��ʼ��ƽΪ0
  }
  //PTC3 = ~STBY
  hw_gpio_init(PORT_C, 3, 1, 1); //�趨�̿�ѡ��˿�PTC3���趨Ϊ�����STBY��ʼ��ƽΪ1, AD7606-4��������ģʽ
  
  //�������ȼ�����
  //UART1 ͨ������    Ĭ��4
  //PTB1  �ɼ�        Ĭ��10=>���ȼ�4
  //PIT0  ��ʼ�ɼ�CONVST����  Ĭ�����ȼ�7
  //uart0 ���� 16             Ĭ��4=>���ȼ�10
  //PTB1 = BUSY
  hw_gpio_init(PORT_B, 1, 0, 0); //�趨�̿�ѡ��˿�PTD1���趨Ϊ���룬�˿ڳ�ʼ��ƽΪ0  
  enable_irq(PortB_irq_no);
  PORT_PCR_REG(PORTB_BASE_PTR, 1) |= PORT_PCR_MUX(0x01) | 
    PORT_PCR_PE_MASK | // Pull Enable
      PORT_PCR_PS_MASK | //����
        PORT_PCR_IRQC(0x0A); //�½����жϴ������ο��ֲ�P214
  set_irq_priority(41, 4);  
  //��������0�����ж�
  hw_uart0_232_re_int_open();
  set_irq_priority(16, 10);
  
  //��������1�����ж�
  hw_uart1_422_re_int_open();
  //set_irq_priority(18, 11);
  //hw_pit_init(PIT0, periph_clk_khz * 2222.5 / 10000 );   //ÿ��222us����һ���жϣ�����CONVST�ܽŵ��߼���ƽ����������Ϊ4.5K������·�ź�ͨ��422������Ҫ��80λ����������Ϊ400bps����
  //hw_pit_init(PIT0, periph_clk_khz * 2631 / 10000 ); //�������л�460800 �� �����ʶ�ӦΪ3.801k
  hw_pit_init(PIT0, periph_clk_khz * 5555 / 10000 );  //ÿ��5.555ms����һ���жϣ�������Ϊ1.8k  
  
  
  hw_enable_pit_int(PIT0);
  
  SPI0_Init(); //�������ж��������֮ǰ����
  
  EnableInterrupts;   
  
  uart_putchar(UART_0, 0xA4); //�����������������
  while(!RS232_rec_com_flag); //����һ���������������
  RS232_rec_com_flag = 0;
  uart_putchar(UART_0, 0x01); //��������ٶ�Ϊ4.5��/��
  while(!RS232_rec_com_flag);
  
  adc_reset(); //����AD7606
  
  //��ѭ����ʼ  
  for(;;)
  {
    if(1 == RS422_rec_com_flag)//��λ��������������
    {
      RX_422_buff[0] = RX_422_buff[1];
      RS422_rec_com_flag = 0;
    }
    
    /*    if(1 == CONVST_DOWN_PIT1_flag)
    {
    CONVST_DOWN_PIT1_flag = 0;
    hw_gpio_reverse(PORT_D, 0); //�ı�̿�ѡ��˿�PTD0�ĵ�ƽ��CONVST����
    //TODO:1us
    time_delay_us(1);
    hw_gpio_reverse(PORT_D, 0); //�ı�̿�ѡ��˿�PTD0�ĵ�ƽ��CONVST����      
  }
    */
    if( ((RX_422_buff[0] & 0xF0) == 0x50))//У��
    {
      if(0 == BGET(3, RX_422_buff[0]))//�ı�����ʣ��������ʣ�����
      {
        if(1 == BGET(2, RX_422_buff[0]))//�ı�����ʡ���������
        {
          //TODO
          //�����ٶȲ���Ļ�����ǡ�������յ����ݣ�RS232_rec_com_flag=1;ǡ�û����˺ܳ���ʱ��
          if(RX_422_buff[0] != RX_422_buff[3])//�����������ϴε����ͬ
          {
            RX_422_buff[3] = RX_422_buff[0];
            if(RX_422_buff[0] == 0x54)
            {
              hw_disable_pit_int(PIT0);//�رմ��������Ķ�ʱ�ж�
              hw_pit_init(PIT0, periph_clk_khz * 6666 / 10000 );  //ÿ��6.666ms����һ���жϣ�������Ϊ1.5k����Ӧ��С���������189600b/s
              hw_enable_pit_int(PIT0);
            }
            else 
              if(RX_422_buff[0] == 0x56)
              {
                hw_uart1_422_re_int_close();//ֹͣ�жϽ�������
                hw_uart1_422_change(230400);//�رմ����շ����޸Ĵ��䲨���ʣ��򿪴����շ�
                hw_uart1_422_re_int_open();//���жϽ�������
              }
              else
                if(RX_422_buff[0] == 0x55)
                {
                  hw_disable_pit_int(PIT0);//�رմ��������Ķ�ʱ�ж�
                  hw_pit_init(PIT0, periph_clk_khz * 5555 / 10000 );  //ÿ��5.555ms����һ���жϣ�������Ϊ1.8k����Ӧ��ͱ�����225600b/s
                  hw_enable_pit_int(PIT0);
                }
                else 
                  if(RX_422_buff[0] == 0x57)
                  {
                    hw_uart1_422_re_int_close();//ֹͣ�жϽ�������
                    hw_uart1_422_change(249600);//�رմ����շ����޸Ĵ��䲨���ʣ��򿪴����շ�
                    hw_uart1_422_re_int_open();//���жϽ�������
                  }
          }//END OF �����������ϴε����ͬ
        }//END OF �ı�����ʡ���������
        else//����仯3����� 
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
          }//END OF ����ı��Ƿ���֮ǰһ��
        }//END OF �ı�����
      }//END OF �ı�����ʡ��������ʡ�����
      else //��������
      {
        if(1 == BUSY_SAMPLE_END_flag)//4·���ݲɼ���ϵ���
        {
          BUSY_SAMPLE_END_flag = 0;
          for(signal_channel = 0; signal_channel < NUM; signal_channel++)//��·�ź�
          {
            *(uint16 *)(TX_422_buff + 23 + 2 * signal_channel) = AD_BUFF[signal_channel];
          }
          
          if(RS232_rec_com_flag == 1)//�Ѿ�����������������
          {
            if(1 == checksum(20, TX_422_buff, 0))//У����������Ϣ
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
        }//4·���ݲɼ���ϵ���
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
  
  //DisableInterrupts;    //�����ж�
  TX_422_buff[TX_422_index] = uart_getchar(UART_0);
  if(TX_422_index < 19)
  {
    TX_422_index++;
  }
  else
  {
    RS232_rec_com_flag = 1;
    TX_422_index = 0;
  }
  
  //EnableInterrupts;     //�����ж�
}
//��������жϷ������
void UART1_isr(void)
{
  
  //DisableInterrupts;    //�����ж�
  
  RX_422_buff[1] = uart_getchar(UART_1);
  RS422_rec_com_flag = 1;
  
  //EnableInterrupts;     //�����ж�
}

/******************************************************************************
�������ƣ�void pit0_isr( void )
����������PIT0�жϷ������222.25us��һ���ж�; ʹ�õڶ��ַ�ʽ
�����������
�����������
�� �� ֵ����
���ú�������
ȫ�ֱ�������
******************************************************************************/
void pit0_isr(void)
{  
  if((PIT_TFLG(0)&PIT_TFLG_TIF_MASK)!=0)
  {
    // 1 �������ʽ������ѭ���ı�˿ڵ�ƽ
    /*CONVST_DOWN_PIT1_flag = 1;*/
    
    //2 �����еȴ�1us�Ĵ���
    hw_gpio_reverse(PORT_D, 0); //�ı�˿�PTD0�ĵ�ƽ��CONVST����
    time_delay_us(1);
    hw_gpio_reverse(PORT_D, 0); //�ı�˿�PTD0�ĵ�ƽ��CONVST����      
    
    PIT_TFLG(0)|=PIT_TFLG_TIF_MASK; //���־
  }
}
/******************************************************************************
�������ƣ�void pit0_isr( void )
�������������������ʱ���͵��жϷ������
�����������
�����������
�� �� ֵ����
���ú�������
ȫ�ֱ�������
******************************************************************************/

void pit1_isr(void)
{  
  if((PIT_TFLG(1)&PIT_TFLG_TIF_MASK)!=0)
  {
    uart_putchar(UART_0, 0xA2); //������������Ϣ�ɼ�����
    PIT_TFLG(1)|=PIT_TFLG_TIF_MASK; //���־
  }
}


/******************************************************************************
�������ƣ�void PTB_1_isr( void )
����������AD_BUSY(PD9)�жϷ������
�����������
�����������
�� �� ֵ����
���ú�������
ȫ�ֱ�������
******************************************************************************/
void PTB_1_isr( void )
{
  DisableInterrupts;
  adc_spi_read();
  BUSY_SAMPLE_END_flag = 1;
  PORT_ISFR_REG(PORTB_BASE_PTR) = 0xFFFFFFFF; //����˿��ж�
  EnableInterrupts;
}