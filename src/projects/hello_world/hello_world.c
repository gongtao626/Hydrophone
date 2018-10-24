/*
* 工程名称：water
* 硬件连接：
* 程序描述：
* 工程目的：
* 工程说明：
* 
* 版本更新	2012.8.17 V1.0	主循环实现完成，SPI通信细节尚未完全确定；
* 		2012.8.30 V1.1	(1)计时中断模块的添加，需要考虑何时启动RTC，与232通信配置更好的配合；
*				(2)注释SPI通信93.2KSps采样率，采集5次平均后发送；
*                               (3)三路信号的设定
*               2012.10.31 V1.2 启用中断功能接收命令与罗盘仪数据
*TODO:    1)PIT定时中断、0.2~0.5秒，不占用带宽定时0.2秒
*         2)SPI中断接收方式，大的缓冲区接收SPI数据，同时发送数据
*         3)罗盘信息先输出再进行AD采集，加入温度信息
*/



//头文件
#include "common.h"

//函数声明，本模块使用
uint8 checksum(uint32 len, uint8 *buff, uint8 computesum);

//DEFINE宏声明，本模块使用
#define NUM 4 //主循环中采集 NUM 路信号

//类型声明，本模块使用

//变量声明，本模块使用
//串口1用RS422协议接收的命令存放在RX_422_buff[3], 其格式为：
//Byte0: 上次接收的命令字
//Byte1: 中断方式接收的新命令
//Byte2: 上次改变放大倍数时的倍数大小
uint8 RX_422_buff[3];//上位机发送到串口1的命令，
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

uint8 SPI_rec_int_flag;//中断接收SPI方式采集的AD数据完成标志
uint16 AD_BUFF[NUM];//存放 NUM 路信号值的缓冲区

//CONST常量声明，本模块使用

//项目使用的函数声明，Code of project wide FUNCTIONS

//外部模块声明的变量
extern uint32 core_clk_khz; //内核频率48000，由外部8M时钟4分频之后，24倍频
extern uint32 periph_clk_khz;   //芯片总线时钟频率24000，内核频率的2分频

//主函数

int main(void)
{
  //cfg[13:0]，其中各位的作用为
  //cfg[13]配置更新，cfg[12:10]输入通道配置，cfg[9:7]选择输入通道
  //cfg[6]选择带宽，cfg[5:3]基准电压源/缓冲器选择，cfg[2:1]通道序列器，cfg[0]回读CFG寄存器
  //spi传输16位，RAC模式，无繁忙指示器，MSB先传输，因此需要将cfg左移两位
  
  //参考内部电压源4.096V
  //uint16 ad_cfg = 0x3C49<<2;//改成输入通道扫描IN0,  覆盖寄存器，单极性GND参考，IN2, 全带宽，内部电压源（4.096），序列扫描，不回读
  //uint16 ad_cfg = 0x3CCF<<2;//改成输入通道扫描IN0 IN1， 覆盖寄存器，单极性GND参考，IN2, 全带宽，内部电压源（4.096），序列扫描，不回读 
  //uint16 ad_cfg = 0x3D4F<<2;//改成输入通道扫描IN0 IN1 IN2, 覆盖寄存器，单极性GND参考，IN2, 全带宽，内部电压源（4.096），序列扫描，不回读
  
  //参考外部电压源5V
  uint16 ad_cfg = 0x3DFF<<2; //改成输入通道扫描IN0 IN1 IN2 IN3, 覆盖寄存器，单极性GND参考，IN2, 全带宽，外部电压源（内缓冲、禁用温度），序列扫描，不回读
  //uint16 ad_cfg = 0x3D7F<<2; //改成输入通道扫描IN0 IN1 IN2, 覆盖寄存器，单极性GND参考，IN2, 全带宽，外部电压源（内缓冲、禁用温度），序列扫描，不回读
  //uint16 ad_cfg = 0x3C79<<2;//外部单通道IN0，其它与3D7F相同
  
  
  uint32 dummy_cfg=0;//SPI发送的第一个命令字ad_cfg，然后上升沿启动采集，上升沿
  uint32 unvalid_result1; //发送命令字ad_cfg，返回的无效数据
  uint32 unvalid_result2; //发送，
  uint16 signal_channel; //信号轮询变量，用于往AD_BUFF中存放数据时三路信号的计数
  
  RX_422_buff[2] = 0x01;//默认放大倍数
  TX_422_buff[20] = 0x5A; //字头
  TX_422_buff[21] = 0x5A;
  TX_422_buff[22] = 12;//TX_422_buff从TX_422_buff[20]开始存放发送的帧头
  
  //测量开始等待
  RX_422_buff[0] = uart_getchar(UART1_BASE_PTR);
  
  DisableInterrupts;//关总中断
  hw_uart0_232_init();//系统初始化时在sysinit.c中的函数sysinit已经配置好422的端口, 此处配置罗盘仪的串口，具体信息参见tower.h
  
  //  /*放大倍数程控初始化设定，默认10倍放大
  //  PTC8  = I0_L;    	PTC9  = I0_H;
  //  PTC10 = I1_L;    	PTC11 = I1_H;
  //  PTD0  = I2_L;    	PTD1  = I2_H;
  //  PTD2  = I3_L;    	PTD3  = I3_H;
  //  */
  hw_gpio_init(PORT_C, 8, 1, 1);	//设定程控选择端口PTC8，设定为输出，端口初始电平为1
  hw_gpio_init(PORT_C, 9, 1, 0);	//设定程控选择端口PTC9，设定为输出，端口初始电平为0
  hw_gpio_init(PORT_C,10, 1, 1);	//设定程控选择端口PTC10，设定为输出，端口初始电平为1
  hw_gpio_init(PORT_C,11, 1, 0);	//设定程控选择端口PTC11，设定为输出，端口初始电平为0
  hw_gpio_init(PORT_D, 0, 1, 1);	//设定程控选择端口PTD0，设定为输出，端口初始电平为1
  hw_gpio_init(PORT_D, 1, 1, 0);	//设定程控选择端口PTD1，设定为输出，端口初始电平为0
  hw_gpio_init(PORT_D, 2, 1, 1);	//设定程控选择端口PTD2，设定为输出，端口初始电平为1
  hw_gpio_init(PORT_D, 3, 1, 0);	//设定程控选择端口PTD3，设定为输出，端口初始电平为0
//解决下载不一致的问题??
  //启动串口0接收中断
  hw_uart0_232_re_int_open();//直接用下面的方式实现中断，涉及到配置寄存器
  //UART_PFIFO_REG(UART_0) |= UART_PFIFO_RXFE_MASK;//启动中断接收缓冲，参考K20P64M50SF0RM.PDF文件第1073页，设置缓冲大小
  
  //启动串口1接收中断  
  hw_uart1_422_re_int_open();
  hw_pit_init(PIT0, periph_clk_khz * 100);//每隔0.2s发送一次罗盘位置信息采集命令"A2"
  hw_enable_pit_int(PIT0);  
  
  EnableInterrupts;   

  uart_putchar(UART_0, 0xA2); //单次采集命令
  while(!RS232_rec_com_flag);
  
  //以下两行信息用于改变程序大小，当只有3路信号，可以通过改变程序大小暂时解决K20DX128VLH5的下载不稳定问题
  //time_delay_ms(1);
  //time_delay_ms(1);
  
  SPI0_Init(); //调整到中断启动语句之前？？
  dummy_cfg = hw_spi_send_receive(ad_cfg);
  unvalid_result1 = hw_spi_send_receive(ad_cfg);//cfg
  unvalid_result2 = hw_spi_send_receive(0); //保持cfg，DIN为低，参考RAC模式的序列时序图
  
  //主循环开始  
  for(;;)
  {
    for(signal_channel = 0; signal_channel < NUM; signal_channel++)//三路信号的采集
    {
      AD_BUFF[signal_channel] = hw_spi_send_receive(0);
      *(uint16 *)(TX_422_buff+23 + 2*signal_channel) = AD_BUFF[signal_channel];
    }
    
//    *(uint16 *)(TX_422_buff+23) = AD_BUFF[0];
//    *(uint16 *)(TX_422_buff+25) = AD_BUFF[1];
//    *(uint16 *)(TX_422_buff+27) = AD_BUFF[2];
//    *(uint16 *)(TX_422_buff+29) = AD_BUFF[3];
    dummy_cfg++;
    if(RS422_rec_com_flag == 1)//上位机的命令接收完毕
    {
      RX_422_buff[0] = RX_422_buff[1];
      RS422_rec_com_flag = 0;
    }
    
    
    if( ((RX_422_buff[0] & 0xF0) == 0x50))//校验
    {
      if(0 == BGET(3, RX_422_buff[0]))//改变增益
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
          hw_gpio_set(PORT_C,10, BGET(0,RX_422_buff[0]));//PTC10 = I1_L;
          hw_gpio_set(PORT_C,11, BGET(1,RX_422_buff[0]));//PTC11 = I1_H;
          hw_gpio_set(PORT_D, 0, BGET(0,RX_422_buff[0]));//PTD0  = I2_L;
          hw_gpio_set(PORT_D, 1, BGET(1,RX_422_buff[0]));//PTD1  = I2_H;
          hw_gpio_set(PORT_D, 2, BGET(0,RX_422_buff[0]));//PTD2  = I3_L
          hw_gpio_set(PORT_D, 3, BGET(1,RX_422_buff[0]));//PTD3  = I3_H
        }//END OF 增益改变是否与之前一样
      }//END OF 改变增益
      else//发送数据
      {
        if(RS232_rec_com_flag == 1)//已经接收完罗盘仪数据
        {
          if(1 == checksum(20, TX_422_buff, 0))//校验罗盘仪信息，由于罗盘操作手册所给出校验信息的不准确，删掉该验证
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
  DisableInterrupts;    //关总中断
  TX_422_buff[TX_422_index++] = uart_getchar(UART_0);
  if(TX_422_index == 20)
  {
    RS232_rec_com_flag = 1;
    TX_422_index = 0;
  }
  EnableInterrupts;     //开总中断
}
//命令接收中断服务程序
void UART1_isr(void)
{
  
  DisableInterrupts;    //关总中断
  
  RX_422_buff[1] = uart_getchar(UART_1);
  RS422_rec_com_flag = 1;
  
  EnableInterrupts;     //开总中断
}
//定时中断服务程序，
void pit0_isr(void)
{  
  if((PIT_TFLG(0)&PIT_TFLG_TIF_MASK)!=0)
  {
    uart_putchar(UART_0, 0xA2); //单次采集命令
    PIT_TFLG(0)|=PIT_TFLG_TIF_MASK; //清标志
  }
}
