
/*****************************************************************************************************
* Include files
*****************************************************************************************************/
#include "common.h"

//RS422接收的命令帧存放在RX_422_buff[13], 其格式为：
//Byte0: 422速率
//Byte1: 程控选择
//Byte2--ByteEnd: 232文本命令，以回车符即“\x0D\x0A”结束
//RS422返回的结果帧存放在TX_422_buff[80]中，其格式为：
//Byte0 Byte1:
//Byte2 Byte3
//Byte4 Byte5:
//Byte6 Byte7:
//Byte8--ByteEnd：RS232 返回的数据航向磁场等数据
//全局变量声明
//
uint8 RX_422_buff[13];//MCU发往罗盘仪的命令，缓冲
uint8 RX_422_index;
uint8 RX_422_len;
uint8 RS422_rec_com_flag;//中断接收完成标志

uint8 TX_422_buff[64];//罗盘仪返回给MCU的数据，缓冲
uint8 TX_422_index = AD_NUM;//TX_422_buff[08]开始存放
uint8 TX_422_len = AD_NUM;
uint8 SPI_rec_int_flag;//接收中断标志
uint8 RS232_rec_com_flag;//接收中断标志

uint8 uart_rec_ok=1;//串口接收成功判断标志，uart_rec_ok=0接收成功，uart_rec_ok=1接收失败


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
  //uint16 ad_cfg = 0x3FCF<<2;//MSB优先，所以要将其移至高14位
  //覆盖当前配置设置；单极性输入通道；输入通道选择至IN3；全带宽；
  //内部基准电压源，REF = 4.096v；通道序列器，扫描IN0至IN3；不回读配置内容
  uint16 ad_cfg = 0x3F4F<<2;//改成输入通道扫描IN0 IN1 IN2
  uint16 ad_cfg_hold = 0x0; //序列采样保持DIN处于低电平
  uint32 tmp1 = 0;//信道1 A/D采集数据临时存储，用于均值滤波
  uint32 tmp2 = 0;
  uint32 tmp3 = 0;
  uint16 spi_data = 0; //存放采集结果
  
  uint16 i = 0;//A/D多次采集循环计数
  uint8 spi_begin = 0;//SPI传输启动标志
  
  DisableInterrupts;      //禁止总中断
#ifdef DEBUG
  hw_uart1_422_init();
#else
  hw_uart0_232_init();
#endif
  
  hw_spi_init(MASTER);          //AD与MCU通讯，对应于SPI0，接收初始化，设定波特率为12M
  //hw_pit_init(PIT0,periph_clk_khz * 1000);                 //初始化pit0，周期1s 
  
  /*放大倍数程控初始化设定，默认10倍放大
  PTC8  = I0_L;    	PTC9  = I0_H;
  PTC10 = I1_L;    	PTC11 = I1_H;
  PTD0  = I2_L;    	PTD1  = I2_H;
  PTD2  = I3_L;    	PTD3  = I3_H;
  */
  hw_gpio_init(PORT_C, 8, 1, 0);	//设定程控选择端口PTC8，设定为输出，端口初始电平为0
  hw_gpio_init(PORT_C, 9, 1, 0);	//设定程控选择端口PTC9，设定为输出，端口初始电平为0
  //hw_gpio_init(PORT_C,10, 1, 0);	//设定程控选择端口PTC10，设定为输出，端口初始电平为0
  //hw_gpio_init(PORT_C,11, 1, 0);	//设定程控选择端口PTC11，设定为输出，端口初始电平为0
  //hw_gpio_init(PORT_D, 0, 1, 0);	//设定程控选择端口PTD0，设定为输出，端口初始电平为0
  //hw_gpio_init(PORT_D, 1, 1, 0);	//设定程控选择端口PTD1，设定为输出，端口初始电平为0
  
  //hw_gpio_init(PORT_D, 2, 1, 0);	//设定程控选择端口PTD2，设定为输出，端口初始电平为0
  //hw_gpio_init(PORT_D, 3, 1, 0);	//设定程控选择端口PTD3，设定为输出，端口初始电平为0
  
#ifdef DEBUG
  //hw_uart0_232_re_int_open();
#else
  hw_enable_pit_int(PIT0);                               //开pit中断    
  hw_uart0_232_re_int_open();
  hw_uart1_422_re_int_open();
#endif
  EnableInterrupts;			        //开总中断
  
  
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
    //主循环设计
    if(RS422_rec_com_flag == 1)//校验，初始代码，005A
    {
      //422速率变化
      //距离小于200m时，可以提高传输速率到300K
      //距离大于200m，可以降低传输速率到最低150K
      
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
      //程控选择：不修改时，该字节默认发送0，三路信号全是。
      //Ix_H=0 Ix_L=0, 10倍 
      //Ix_H=0 Ix_L=1, 100倍 
      //Ix_H=1 Ix_L=0, 1000倍 
      //Ix_H=1 Ix_L=1, 无定义，未使用，芯片上只控制三种放大倍数的选择
      if(RX_422_buff[1] != 0)//case 设定 ，switch(10/100/1000)
      {
        hw_gpio_set(PORT_C, 8, BGET(1,RX_422_buff[1]));//PTC8  = I0_L;
        hw_gpio_set(PORT_C, 9, BGET(1,RX_422_buff[1]));//PTC9  = I0_H;
        // hw_gpio_set(PORT_C,10, BGET(1,RX_422_buff[1]));//PTC10 = I1_L;
        // hw_gpio_set(PORT_C,11, BGET(1,RX_422_buff[1]));//PTC11 = I1_H;
        // hw_gpio_set(PORT_D, 0, BGET(1,RX_422_buff[1]));//PTD0  = I2_L;
        // hw_gpio_set(PORT_D, 1, BGET(1,RX_422_buff[1]));//PTD1  = I2_H;
        //hw_gpio_set(PORT_D, 2, BGET(1,RX_422_buff[1]));//PTD2  = I3_L;无第四路水听器
        //hw_gpio_set(PORT_D, 3, BGET(1,RX_422_buff[1]));//PTD3  = I3_H;
      }
      //232命令发送
      hw_uart_sendN(UART_232, (uint16)(RX_422_len-2), (uint8* )RX_422_buff+2);
      
      RX_422_len = 0;//命令缓冲区长度清零
      RS422_rec_com_flag = 0;//可以重新接受命令
      //配置完毕，启动SPI传输命令
      spi_begin = 1;
    }
    //spi发送1个配置开始,采样率为92.3K
    
    //TODO:修改SCK发送函数，HALT位清0，选通SS信号
    if(1 == spi_begin)// && 0 == RS422_rec_com_flag)
    {
      hw_spi_start_transfer();
      hw_spi_send(&ad_cfg, 1);//初始化AD7682，发送第二个哑元
      
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
        //TODO 出错处理
      }
      //结束CNV选通SS信号，HALT位置1
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
