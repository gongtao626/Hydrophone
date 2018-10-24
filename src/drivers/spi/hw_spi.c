#include "hw_spi.h"

//=================内部函数声明=============================================

//=========================================================================
//函数名称：hw_spi_get_base_address                                                        
//功能概要：获取SPI寄存器的基址                                                
//参数说明：无      
//函数返回：SPI的基址值                                                                  
//=========================================================================
SPI_MemMapPtr hw_spi_get_base_address();

//=================接口函数实现=============================================

//=========================================================================
//函数名称：hw_spi_init
//函数参数：Master:是否是主机。
//函数返回：无
//功能概要：SPI初始化。
//=========================================================================
//从机的极性与相位怎样设定？不用设定么？MCU直接作为主机，从机自动设定？
void hw_spi_init(uint8 master)
{
    SPI_MemMapPtr BaseAdd = hw_spi_get_base_address();
    
    //使能SPI模块时钟，配置SPI引脚功能
        SIM_SCGC6 |= SIM_SCGC6_SPI0_MASK;//打开时钟
        PORTC_PCR4 = 0 | PORT_PCR_MUX(0x2) | PORT_PCR_DSE_MASK;//PC4 PCS0,DSE=1，高驱动使能，片选
        PORTC_PCR5 = 0 | PORT_PCR_MUX(0x2) | PORT_PCR_DSE_MASK;//PC5 SCK
        PORTC_PCR6 = 0 | PORT_PCR_MUX(0x2) | PORT_PCR_DSE_MASK;//PC6 SOUT MOSI
        PORTC_PCR7 = 0 | PORT_PCR_MUX(0x2);//PC7 SIN MISO
    
    SPI_MCR_REG(BaseAdd) = 0 //串行连续时钟禁止
                 | SPI_MCR_CLR_TXF_MASK     //Clear the Tx FIFO counter.
                 | SPI_MCR_CLR_RXF_MASK     //Clear the Rx FIFO counter.
                 | SPI_MCR_PCSIS(1) //PCS0低电平有效
                 | SPI_MCR_HALT_MASK;//HALT=1，停止传输

    //根据主从机模式设置工作模式
    if(master == MASTER)
    {
        SPI_MCR_REG(BaseAdd) |= SPI_MCR_MSTR_MASK;//主机模式
        //时钟频率：(fSYS/Prescaler) x [(1+DBR)/Scaler]=(总线时钟 /pbr)((1+DBR)/BR);总线时钟24M，SCK频率12M   SCK = 83.3ns = 1000/12 ns
        //设定PBR2分频（最小），DBR双倍波特率（最大），BR2分频（最小）。(24/2)((1+1)/2)=12M，最大的SCK
        //不同的外设可设置不同的通信延时，有下面三个延时时间可以设置：
        //tCSC =(1/fsys)×(2×PCSSCK+1)×2^(CSSCK+1) 	1/12=tsck PCSSCK共2位，CSSCK共4位  
        //tASC =(1/fsys)×(2×PASC+1)×2^(ASC+1) 		1/12=tsck PASC共2位，ASC共4位 
        //tDT=(1/fsys)×(2×PDT+1)×2^(DT+1)			(1/24)*7*32=9.33us PDT共2位，DT共4位 
        //tSCK = 1/12M; tCSC = 1/12M; tASC = 1/12M; tDT = 1*2/24M = 1/12M。
        //传输16位数据时，CNV为低时时间为：18/12M，即1.5us，tDT为1/12M，即0.0833us，即总共1.583us传输一路信号
        //一次采样时间为19/12M，采样率由CONVST确定，目前设定为5k，200us拉低一次CS
        SPI_CTAR_REG(BaseAdd,0) = 0			//时钟与传输属性设置：MSB first(0), PBR=00(2分频)
                       | SPI_CTAR_DBR_MASK //双倍波特率,1 bit
                       | SPI_CTAR_FMSZ(0x0F)//单帧大小0xF+1=16
                       |  SPI_CTAR_PDT_MASK//PDT=3 tASC tDT tCCS
                       | SPI_CTAR_DT(4) //DT=4, 92.3k
                       //| SPI_CTAR_DT(7) //DT=7, 13.13K
                       | SPI_CTAR_BR(0);//BR=0, CPOL=0, CPHA=0, 无繁忙指示器的时序
                       //| SPI_CTAR_CPOL_MASK //CPOL=1
                       //| SPI_CTAR_CPHA_MASK;//CPHA=1

/*                SPI_CTAR_REG(BaseAdd,0) = 0			//时钟与传输属性设置：MSB first(0), PBR=00(2分频)
                       | SPI_CTAR_DBR_MASK //双倍波特率,1 bit
                       | SPI_CTAR_FMSZ(0x0F)//单帧大小0xF+1=16
                       //| SPI_CTAR_PDT_MASK//PDT=7 tASC tDT tCCS
                       | 0x80000u //PDT = 5
                       | SPI_CTAR_DT(7) //DT=7, 18K
                       | SPI_CTAR_BR(0);//BR=0, CPOL=0, CPHA=0, 无繁忙指示器的时序
                       //| SPI_CTAR_CPOL_MASK //CPOL=1
                       //| SPI_CTAR_CPHA_MASK;//CPHA=1
*/
    }

    SPI_SR_REG(BaseAdd) = (SPI_SR_EOQF_MASK	//End of Queue Flag，写1清空寄存器SR中的五个BIT位，表明尚未发生这些清空参考手册K10P64M50SF0RM, 40.3.5, P892
                         | SPI_SR_TFUF_MASK	//Transmit FIFO Underflow Flag
                         | SPI_SR_TFFF_MASK	//Transmit FIFO Fill Flag
                         | SPI_SR_RFOF_MASK	//Receive FIFO Overflow Flag
                         | SPI_SR_RFDF_MASK);	//Receive FIFO Drain Flag
    
    SPI_MCR_REG(BaseAdd) &= ~SPI_MCR_HALT_MASK;//HALT位清0，开始传输
}
   //时钟频率：(fSYS/Prescaler) x [(1+DBR)/Scaler]=(总线时钟 /pbr)((1+DBR)/BR);总线时钟24M，SCK频率12M   SCK = 83.3ns = 1000/12 ns
   //设定PBR2分频（最小），DBR双倍波特率（最大），BR2分频（最小）。(24/2)((1+1)/2)=12M，最大的SCK
   //不同的外设可设置不同的通信延时，有下面三个延时时间可以设置：
   //tCSC =(1/fsys)×(2×PCSSCK+1)×2^(CSSCK+1) 	1/12=tsck PCSSCK共2位，CSSCK共4位  
   //tASC =(1/fsys)×(2×PASC+1)×2^(ASC+1) 		1/12=tsck PASC共2位，ASC共4位 
   //tDT=(1/fsys)×(2×PDT+1)×2^(DT+1)			(1/24)*7*32=9.33us PDT共2位，DT共4位 
   //tSCK = 1/12M; tCSC = 1/12M; tASC = 1/12M; tDT = 1*2/24M = 1/12M。
   //传输16位数据时，CNV为低时时间为：18/12M，即1.5us，tDT为1/12M，即0.0833us，即总共1.583us传输一路信号
   //一次采样时间为19/12M，采样率由CONVST确定，目前设定为5k，200us拉低一次CS

void SPI0_Init(void) 
{ 
  SIM_SCGC6 |= SIM_SCGC6_SPI0_MASK;//打开spi0时钟 
  PORTC_PCR4 = PORT_PCR_MUX(2);//PC4，SPI0 PCS0，片选 
  PORTC_PCR5 = PORT_PCR_MUX(2);//PC5，SPI0 SCK 
  PORTC_PCR6 = PORT_PCR_MUX(2);//PC6，SPI0 SOUT MOSI 
  PORTC_PCR7 = PORT_PCR_MUX(2);//PC7，SPI0 SIN MISO 
  SPI0_MCR = 0 | SPI_MCR_MSTR_MASK;//主机模式 
  SPI0_MCR |= SPI_MCR_PCSIS(1);// PCS0低电平有效 
  SPI0_MCR &= ~SPI_MCR_MDIS_MASK;//使能DSPI时钟 
  SPI0_MCR &= ~SPI_MCR_HALT_MASK;//HALT位清0，开始传输, 启动SPI模块 
  //SPI0_MCR |= SPI_MCR_CONT_SCKE_MASK; //串行时钟连续工作
  SPI0_CTAR1 = 0 | SPI_CTAR_FMSZ(15);//帧大小16bit 此时PDT-00 DT-0000 tDT=1/24M=0.0416us
  SPI0_CTAR1 |= SPI_CTAR_CPOL_MASK;//时钟空闲高电平 
  SPI0_CTAR1 &= ~SPI_CTAR_CPHA_MASK;//数据在SCK的第一个边沿被捕捉，第二个边沿变化；参考手册P28，第一个边沿获取数据，第二个边沿输出
  //SPI0_CTAR1 |= SPI_CTAR_CPHA_MASK;//数据在SCK的第二个边沿被捕捉，第一个边沿变化
  
  SPI0_CTAR1 &= ~SPI_CTAR_LSBFE_MASK; //数据高位在前，低位在后

  //时钟频率：(fSYS/Prescaler) x [(1+DBR)/Scaler]=(总线时钟 /pbr)((1+DBR)/BR);总线时钟24M，SCK频率12M   SCK = 83.3ns 1000/12 ns
  //设定PBR2分频，DBR双倍波特率，BR2分频。(24/2)((1+1)/2)=12M
  SPI0_CTAR1 |= SPI_CTAR_DBR_MASK; //双倍波特率,1 bit；PBR=00 BR=0000
  //SPI0_CTAR1 |= SPI_CTAR_PBR(0);//预分频12M
  //SPI0_CTAR1 |= SPI_CTAR_BR(0);//最终分频12MHz
  
  //SPI0_CTAR1 |= SPI_CTAR_PDT_MASK; //pdt = 7 dt =4 92.3k //PDT = 5; dt=7; //位域'10'对应PDT=5 tASC tDT tCCS 18.237k
  //SPI0_CTAR1 |= SPI_CTAR_DT(0); //DT=7
} 
uint32 hw_spi_send_receive(uint16 ad_cfg)
{
  SPI0_PUSHR = 0 //| SPI_PUSHR_CONT_MASK //片选信号PCS0一直拉低
              | SPI_PUSHR_CTAS(1)
              | SPI_PUSHR_PCS(1)
              | SPI_PUSHR_TXDATA(ad_cfg);
  while((SPI0_SR & SPI_SR_TCF_MASK) == 0);
  SPI0_SR |= SPI_SR_TCF_MASK;
  return SPI0_POPR;
}


//=========================================================================
//函数名称：hw_spi_send
//函数参数：data[]：需要发送的数据。
//          len：数据长度。
//函数返回：无
//功能概要：SPI发送数据。
//=========================================================================
void hw_spi_send(uint16 data[],uint32 len)
{
    uint32 i = 0;
    SPI_MemMapPtr BaseAdd = hw_spi_get_base_address();
    
    SPI_SR_REG(BaseAdd) = (SPI_SR_EOQF_MASK
         | SPI_SR_TFUF_MASK
         | SPI_SR_TFFF_MASK
         | SPI_SR_RFOF_MASK
         | SPI_SR_RFDF_MASK)
      ;
    SPI_MCR_REG(BaseAdd) |= SPI_MCR_CLR_TXF_MASK 
              | SPI_MCR_CLR_RXF_MASK  
              ;
    for(i = 0;i < len;i++)
    {
        if(i == (len - 1))
        {
                SPI_PUSHR_REG(BaseAdd) = 0 //SPI_PUSHR_CONT为0，发送间隙片选信号PCS0恢复不活动状态(连接到AD7682的CNV)
                               | SPI_PUSHR_CTAS(0)//主机模式有两个CTAR，此处选择哪个CTAR
                               | SPI_PUSHR_EOQ_MASK//end of queue
                               | SPI_PUSHR_PCS(0x01)//电路设计中选择的是PTC4，对应PCS0，手册P189;宏SPI_PUSHR_PCS(X)，当X=1:选择PCS0、X=2选择PCS1
                               | SPI_PUSHR_TXDATA(data[i]);
        }
        else
        {
                SPI_PUSHR_REG(BaseAdd) = 0 //| SPI_PUSHR_CONT_MASK 发送间隙片选信号PCS0恢复不活动状态
                               | SPI_PUSHR_CTAS(0)
                               | SPI_PUSHR_PCS(1)
                               | SPI_PUSHR_TXDATA(data[i]);
        }
    }
}

//=========================================================================
//函数名称：hw_spi_re
//函数参数：data[]：需要接收的数据。
//函数返回：0：成功？？ 1：失败？？
//功能概要：SPI接收数据。
//=========================================================================
uint8 hw_spi_re(uint16 data[])
{
      SPI_MemMapPtr BaseAdd = hw_spi_get_base_address();

      if(SPI_SR_REG(BaseAdd) & SPI_SR_RFDF_MASK)
      {
    	      data[0] = (uint16)SPI_POPR_REG(BaseAdd);                
              SPI_SR_REG(BaseAdd) |= SPI_SR_RFDF_MASK;
              return 1;
      }
      SPI_SR_REG(BaseAdd) = (SPI_SR_EOQF_MASK
               | SPI_SR_TFUF_MASK
               | SPI_SR_TFFF_MASK
               | SPI_SR_RFOF_MASK
               | SPI_SR_RFDF_MASK);

      SPI_MCR_REG(BaseAdd) |= SPI_MCR_CLR_TXF_MASK     //Clear the Tx FIFO counter.
                | SPI_MCR_CLR_RXF_MASK;     //Clear the Rx FIFO counter.
      return 0;
}

//=========================================================================
//函数名称：hw_spi_enable_re_int
//参数说明：无
//函数返回： 无
//功能概要：开SPI接收中断
//=========================================================================
void hw_spi_enable_re_int(void)
{
    SPI_MemMapPtr BaseAdd = hw_spi_get_base_address();
    BSET(SPI_RSER_RFDF_RE_SHIFT, SPI_RSER_REG(BaseAdd));//开放SPI接收中断
    enable_irq(SPI0IRQ);			 //开接收引脚的IRQ中断
}

//=========================================================================
//函数名称：hw_spi_disable_re_int
//参数说明：无
//函数返回： 无
//功能概要：关SPI接收中断
//=========================================================================
void hw_spi_disable_re_int(void)
{
    SPI_MemMapPtr BaseAdd = hw_spi_get_base_address();
    BCLR(SPI_RSER_RFDF_RE_SHIFT, SPI_RSER_REG(BaseAdd));//关闭SPI接收中断
    disable_irq(SPI0IRQ);			 //开接收引脚的IRQ中断
}

//=================内部函数实现=============================================
//=========================================================================
//函数名称：hw_spi_get_base_address
//函数参数：spiNo：SPI通道号
//函数返回：无
//功能概要：将SPI通道号转换成对应基指针。
//=========================================================================
SPI_MemMapPtr hw_spi_get_base_address(void)
{
      return SPI0_BASE_PTR;
}
void hw_spi_stop_transfer(void)
{
	SPI_MemMapPtr BaseAdd = hw_spi_get_base_address();
	SPI_MCR_REG(BaseAdd) |= SPI_MCR_HALT_MASK;//HALT位置1，结束传输
}

void hw_spi_start_transfer(void)
{
	SPI_MemMapPtr BaseAdd = hw_spi_get_base_address();
	SPI_MCR_REG(BaseAdd) &= ~(SPI_MCR_HALT_MASK);//HALT位清0，开始传输
}


