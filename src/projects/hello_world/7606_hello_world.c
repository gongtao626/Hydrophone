/*
* 工程名称：Paral_Four_channel_waterHearing
* 硬件连接：
* 程序描述：
* 工程目的：
* 工程说明：
* 
* 版本更新 
*  2012.12.07 V1.0  主循环实现完成
*  2012.12.14 V1.1  启用中断功能接收命令与罗盘仪数据
* TODO
*   1)TIME_DELAY_US(1) 实现
*   2)ad_reset信号,
*   3)SPI接收方式，相位、极性调整，连续片选信号？连续SCK信号？
*
*/

//头文件
#include "common.h"

//函数声明，本模块使用
uint8 checksum(uint32 len, uint8 *buff, uint8 computesum);

//DEFINE宏声明，本模块使用
#define NUM 4 //主循环中采集 NUM 路信号

//类型声明，本模块使用

//变量声明，本模块使用
//由程序负责那些未初始化的全局变量置为0

//串口1用RS422协议接收的命令存放在RX_422_buff[3], 其格式为：
//Byte0: 上次接收的命令字
//Byte1: 中断方式接收的新命令
//Byte2: 上次改变放大倍数时的倍数大小
uint8 RX_422_buff[4];//上位机发送到串口1的命令，
uint8 RX_422_index;
uint8 RX_422_len;
uint8 RS422_rec_com_flag;//中断接收命令完成标志

//前20字节存放罗盘仪返回的字节，后16字节存放发送到上位机的信息帧，帧长为10字节或16字节
//buff[0]-buff[19]存放20字节的罗盘定位信息，其中buff[6-7]buff[8-9]buff[16-17]存放X轴角度、Y轴角度、罗盘与磁北偏角，各占两个字节，低字节存放高8位，高字节存放低8位
//buff[20]-buff[38]存放信息帧，buff[20-21]为“5A5A”帧头，buff[22]为帧长
//buff[23-24]buff[25-26]buff[27-28]buff[29-30]，为采集的四路数据，各占2个字节，低字节存放低8位，高字节存放高8位。buff[31]为不含罗盘信息时的校验
//含罗盘仪数据时，buff[31-32]buff[33-34]buff[35-36]buff[37]为复制自buff[6-7]buff[8-9]buff[16-17]buff[18]的罗盘的X轴角度、Y轴角度、罗盘与磁北偏角，温度，buff[38]为校验
//无罗盘仪时数据帧为12字节
//含罗盘仪时数据帧为19字节
uint8 TX_422_buff[39];//罗盘仪返回给MCU的数据，缓冲，即上述说明中的buff
uint8 TX_422_index;
uint8 TX_422_len;
uint8 RS232_rec_com_flag;//中断接收罗盘仪返回数据接收完成标志

uint8 BUSY_SAMPLE_END_flag;//四路数据采样结束标志
uint8 CONVST_DOWN_PIT1_flag; //采样率控制信号CONVST计时标志，为1时拉低启动采样
uint8 SPI_rec_int_flag;//中断接收SPI方式采集的AD数据完成标志
uint16 AD_BUFF[NUM];//存放 NUM 路信号值的缓冲区, 用于存放adc7606采集到4个通道的数据

//CONST常量声明，本模块使用

//项目使用的函数声明，Code of project wide FUNCTIONS

//外部模块声明的变量
extern uint32 core_clk_khz; //内核频率48000，由外部8M时钟4分频之后，24倍频
extern uint32 periph_clk_khz;   //芯片总线时钟频率24000，内核频率的2分频

//主函数

int main(void)
{ 
  uint16 signal_channel; //信号轮询变量，用于往AD_BUFF中存放数据时四路信号的计数
  
  RX_422_buff[2] = 0x01;//默认放大倍数
  RX_422_buff[3] = 0x53;//默认不改变增益时放大倍数
  //TX_422_buff从TX_422_buff[20]开始存放发送的帧
  TX_422_buff[20] = 0x5A; //帧头
  TX_422_buff[21] = 0x5A; //帧头
  TX_422_buff[22] = 12; //无罗盘时的帧长
  time_delay_us(1);
  
  //测量开始等待
  RX_422_buff[0] = uart_getchar(UART1_BASE_PTR);
  
  DisableInterrupts;//关总中断
  hw_uart0_232_init();//系统初始化时在sysinit.c中的函数sysinit已经配置好422的端口, 此处配置罗盘仪的串口，具体信息参见tower.h
  
  //  /*放大倍数程控初始化设定，默认100倍放大
  //  PTC8  = I0_L;    	PTC9  = I0_H;
  hw_gpio_init(PORT_C, 8, 1, 1);	//设定程控选择端口PTC8，设定为输出，端口初始电平为1
  hw_gpio_init(PORT_C, 9, 1, 0);	//设定程控选择端口PTC9，设定为输出，端口初始电平为0
  
  // PTC10 = ADCS;    	
  // PTC11 = FRSTDATA;三态？
  // PTD0  = CONVST;
  // PTD1  = ADRESET;
  hw_gpio_init(PORT_C,10, 1, 1); //设定程控选择端口PTC10，设定为输出，ADCS初始电平为1
  hw_gpio_init(PORT_C,11, 0, 0); //设定程控选择端口PTC11，设定为输入，FRSTDATA端口初始电平为0？
  hw_gpio_init(PORT_D, 0, 1, 1); //设定程控选择端口PTD0，设定为输出，CONVST初始电平为1
  hw_gpio_init(PORT_D, 1, 1, 0); //设定程控选择端口PTD1，设定为输出，ADRESET初始电平为0, 上升沿AD7606复位
  
  //PTC0 = OS0 PTC1 = OS1 PTC2 = OS2
  //OS[2:0] 000-无过采样 001-2倍 010-4倍，2的幂次方倍过采样；111无效
  //59-2倍；5A-4倍；5B-8倍；5C-16倍；5D-32倍；5E-64倍
  if((RX_422_buff[0] > 0x58) &&(RX_422_buff[0] < 0x5F))//校验
  {
    hw_gpio_init(PORT_C, 0, 1, BGET(0,RX_422_buff[0])); //设定程控选择端口PTC0，设定为输出，OS0初始电平为BGET(0,RX_422_buff[0])
    hw_gpio_init(PORT_C, 1, 1, BGET(1,RX_422_buff[0])); //设定程控选择端口PTC1，设定为输出，OS1初始电平为BGET(1,RX_422_buff[0])
    hw_gpio_init(PORT_C, 2, 1, BGET(2,RX_422_buff[0])); //设定程控选择端口PTC2，设定为输出，OS2初始电平为BGET(2,RX_422_buff[0])
  }
  else
  {
    hw_gpio_init(PORT_C, 0, 1, 0); //设定程控选择端口PTC0，设定为输出，OS0初始电平为0
    hw_gpio_init(PORT_C, 1, 1, 0); //设定程控选择端口PTC1，设定为输出，OS1初始电平为0
    hw_gpio_init(PORT_C, 2, 1, 0); //设定程控选择端口PTC2，设定为输出，OS2初始电平为0
  }
  //PTC3 = ~STBY
  hw_gpio_init(PORT_C, 3, 1, 1); //设定程控选择端口PTC3，设定为输出，STBY初始电平为1, AD7606-4处于正常模式
  
  //调整优先级定义
  //UART1 通信命令    默认4
  //PTB1  采集        默认10=>优先级4
  //PIT0  开始采集CONVST拉低  默认优先级7
  //uart0 罗盘 16             默认4=>优先级10
  //PTB1 = BUSY
  hw_gpio_init(PORT_B, 1, 0, 0); //设定程控选择端口PTD1，设定为输入，端口初始电平为0  
  enable_irq(PortB_irq_no);
  PORT_PCR_REG(PORTB_BASE_PTR, 1) |= PORT_PCR_MUX(0x01) | 
    PORT_PCR_PE_MASK | // Pull Enable
      PORT_PCR_PS_MASK | //上拉
        PORT_PCR_IRQC(0x0A); //下降沿中断触发，参考手册P214
  set_irq_priority(41, 4);  
  //启动串口0接收中断
  hw_uart0_232_re_int_open();
  set_irq_priority(16, 10);
  
  //启动串口1接收中断
  hw_uart1_422_re_int_open();
  //set_irq_priority(18, 11);
  //hw_pit_init(PIT0, periph_clk_khz * 2222.5 / 10000 );   //每隔222us触发一次中断，降低CONVST管脚的逻辑电平，采样速率为4.5K，这四路信号通过422传输需要共80位，传输速率为400bps以上
  //hw_pit_init(PIT0, periph_clk_khz * 2631 / 10000 ); //受限于研华460800 ， 采样率对应为3.801k
  hw_pit_init(PIT0, periph_clk_khz * 5555 / 10000 );  //每隔5.555ms触发一次中断，采样率为1.8k  
  
  
  hw_enable_pit_int(PIT0);
  
  SPI0_Init(); //调整到中断启动语句之前？？
  
  EnableInterrupts;   
  
  uart_putchar(UART_0, 0xA4); //罗盘连续输出的命令
  while(!RS232_rec_com_flag); //清零一行语句必须添加下面
  RS232_rec_com_flag = 0;
  uart_putchar(UART_0, 0x01); //调整输出速度为4.5次/秒
  while(!RS232_rec_com_flag);
  
  adc_reset(); //重启AD7606
  
  //主循环开始  
  for(;;)
  {
    if(1 == RS422_rec_com_flag)//上位机的命令接收完毕
    {
      RX_422_buff[0] = RX_422_buff[1];
      RS422_rec_com_flag = 0;
    }
    
    /*    if(1 == CONVST_DOWN_PIT1_flag)
    {
    CONVST_DOWN_PIT1_flag = 0;
    hw_gpio_reverse(PORT_D, 0); //改变程控选择端口PTD0的电平；CONVST拉低
    //TODO:1us
    time_delay_us(1);
    hw_gpio_reverse(PORT_D, 0); //改变程控选择端口PTD0的电平；CONVST升高      
  }
    */
    if( ((RX_422_buff[0] & 0xF0) == 0x50))//校验
    {
      if(0 == BGET(3, RX_422_buff[0]))//改变采样率，传输速率，增益
      {
        if(1 == BGET(2, RX_422_buff[0]))//改变采样率、传输速率
        {
          //TODO
          //处理速度不快的话或者恰好罗盘收到数据，RS232_rec_com_flag=1;恰好花费了很长的时间
          if(RX_422_buff[0] != RX_422_buff[3])//本次命令与上次的命令不同
          {
            RX_422_buff[3] = RX_422_buff[0];
            if(RX_422_buff[0] == 0x54)
            {
              hw_disable_pit_int(PIT0);//关闭触发采样的定时中断
              hw_pit_init(PIT0, periph_clk_khz * 6666 / 10000 );  //每隔6.666ms触发一次中断，采样率为1.5k，对应最小传输比特率189600b/s
              hw_enable_pit_int(PIT0);
            }
            else 
              if(RX_422_buff[0] == 0x56)
              {
                hw_uart1_422_re_int_close();//停止中断接收命令
                hw_uart1_422_change(230400);//关闭串口收发，修改传输波特率，打开串口收发
                hw_uart1_422_re_int_open();//打开中断接收命令
              }
              else
                if(RX_422_buff[0] == 0x55)
                {
                  hw_disable_pit_int(PIT0);//关闭触发采样的定时中断
                  hw_pit_init(PIT0, periph_clk_khz * 5555 / 10000 );  //每隔5.555ms触发一次中断，采样率为1.8k，对应最低比特率225600b/s
                  hw_enable_pit_int(PIT0);
                }
                else 
                  if(RX_422_buff[0] == 0x57)
                  {
                    hw_uart1_422_re_int_close();//停止中断接收命令
                    hw_uart1_422_change(249600);//关闭串口收发，修改传输波特率，打开串口收发
                    hw_uart1_422_re_int_open();//打开中断接收命令
                  }
          }//END OF 本次命令与上次的命令不同
        }//END OF 改变采样率、传输速率
        else//增益变化3种情况 
        {  
          if((RX_422_buff[0] & 0x03) != RX_422_buff[2])//增益改变是否与之前一样
          {
            RX_422_buff[2] = RX_422_buff[0] & 0x03;
            //程控选择：不修改时，默认100，三路信号同时变化。
            //Ix_H=0 Ix_L=0, 10倍
            //Ix_H=0 Ix_L=1, 100倍
            //Ix_H=1 Ix_L=0, 1000倍
            //Ix_H=1 Ix_L=1, 无定义，未使用，芯片上只控制三种放大倍数的选择
            hw_gpio_set(PORT_C, 8, BGET(0,RX_422_buff[0]));//PTC8  = I0_L;
            hw_gpio_set(PORT_C, 9, BGET(1,RX_422_buff[0]));//PTC9  = I0_H;
          }//END OF 增益改变是否与之前一样
        }//END OF 改变增益
      }//END OF 改变采样率、传输速率、增益
      else //发送数据
      {
        if(1 == BUSY_SAMPLE_END_flag)//4路数据采集完毕到来
        {
          BUSY_SAMPLE_END_flag = 0;
          for(signal_channel = 0; signal_channel < NUM; signal_channel++)//四路信号
          {
            *(uint16 *)(TX_422_buff + 23 + 2 * signal_channel) = AD_BUFF[signal_channel];
          }
          
          if(RS232_rec_com_flag == 1)//已经接收完罗盘仪数据
          {
            if(1 == checksum(20, TX_422_buff, 0))//校验罗盘仪信息
            {
              RS232_rec_com_flag = 0; //全局变量置0，准备下一次罗盘信息的接收
              TX_422_buff[22] = 19;//帧长度
              TX_422_buff[31] = TX_422_buff[6];//参考TX_422_buff数组的说明
              TX_422_buff[32] = TX_422_buff[7];
              TX_422_buff[33] = TX_422_buff[8];
              TX_422_buff[34] = TX_422_buff[9];
              TX_422_buff[35] = TX_422_buff[16];
              TX_422_buff[36] = TX_422_buff[17];
              TX_422_buff[37] = TX_422_buff[18];//温度
              
              TX_422_buff[38] = checksum(TX_422_buff[22], TX_422_buff+20, 1);
              //添加温度信息、第四路的代码
              //TX_422_buff[22] = 19;//帧长度
              //TX_422_buff[37] = TX_422_buff[18];//温度
              //TX_422_buff[38] = checksum(TX_422_buff[22], TX_422_buff+20, 1);
            }
          }
          else//无罗盘信息
          {
            TX_422_buff[22] = 12;//长度
            TX_422_buff[31] = checksum(TX_422_buff[22], TX_422_buff+20, 1);
          }
          hw_uart_sendN(UART_1, TX_422_buff[22], TX_422_buff+20);
        }//4路数据采集完毕到来
      }//END OF 发送数据
    }//END OF 校验
  }//EDN OF for主循环 
}//END OF main()函数


//计算或验证校验和
//computesum = 1，计算校验和
//computesum = 0， 验证校验和
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
////函数名称：uart_rs232_isr
////函数返回：无
////参数说明：无
////功能概要：RS232串口0数据接收中断例程。
////============================================================================
void UART0_isr(void)
{
  
  //DisableInterrupts;    //关总中断
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
  
  //EnableInterrupts;     //开总中断
}
//命令接收中断服务程序
void UART1_isr(void)
{
  
  //DisableInterrupts;    //关总中断
  
  RX_422_buff[1] = uart_getchar(UART_1);
  RS422_rec_com_flag = 1;
  
  //EnableInterrupts;     //开总中断
}

/******************************************************************************
函数名称：void pit0_isr( void )
功能描述：PIT0中断服务程序，222.25us进一次中断; 使用第二种方式
输入参数：无
输出参数：无
返 回 值：无
调用函数：无
全局变量：无
******************************************************************************/
void pit0_isr(void)
{  
  if((PIT_TFLG(0)&PIT_TFLG_TIF_MASK)!=0)
  {
    // 1 宏变量方式，在主循环改变端口电平
    /*CONVST_DOWN_PIT1_flag = 1;*/
    
    //2 程序中等待1us的代码
    hw_gpio_reverse(PORT_D, 0); //改变端口PTD0的电平；CONVST拉低
    time_delay_us(1);
    hw_gpio_reverse(PORT_D, 0); //改变端口PTD0的电平；CONVST升高      
    
    PIT_TFLG(0)|=PIT_TFLG_TIF_MASK; //清标志
  }
}
/******************************************************************************
函数名称：void pit0_isr( void )
功能描述：罗盘仪命令定时发送的中断服务程序
输入参数：无
输出参数：无
返 回 值：无
调用函数：无
全局变量：无
******************************************************************************/

void pit1_isr(void)
{  
  if((PIT_TFLG(1)&PIT_TFLG_TIF_MASK)!=0)
  {
    uart_putchar(UART_0, 0xA2); //单次罗盘仪信息采集命令
    PIT_TFLG(1)|=PIT_TFLG_TIF_MASK; //清标志
  }
}


/******************************************************************************
函数名称：void PTB_1_isr( void )
功能描述：AD_BUSY(PD9)中断服务程序
输入参数：无
输出参数：无
返 回 值：无
调用函数：无
全局变量：无
******************************************************************************/
void PTB_1_isr( void )
{
  DisableInterrupts;
  adc_spi_read();
  BUSY_SAMPLE_END_flag = 1;
  PORT_ISFR_REG(PORTB_BASE_PTR) = 0xFFFFFFFF; //清除端口中断
  EnableInterrupts;
}