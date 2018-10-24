//============================================================================
//文件名称：hw_gpio.h
//功能概要：K60 GPIO底层驱动程序头文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//更新记录：2011-11-13   V1.0     初始版本
//          2011-11-17   V1.1    规范排版风格
//============================================================================

#include "hw_gpio.h"    

//============================================================================
//函数名称：hw_gpio_enable_port
//函数返回：无
//参数说明：无
//功能概要：加载所有GPIO端口的时钟。 供高层驱动调用。
//============================================================================
void hw_gpio_enable_port (void)
{
 	  SIM_SCGC5 |=   SIM_SCGC5_PORTA_MASK \
	               | SIM_SCGC5_PORTB_MASK \
	               | SIM_SCGC5_PORTC_MASK \
	               | SIM_SCGC5_PORTD_MASK \
	               | SIM_SCGC5_PORTE_MASK;
}

//============================================================================
//函数名称：hw_gpio_get_port_addr
//函数返回：端口号对应的端口寄存器组指针
//参数说明：无
//功能概要：将端口号转换成端口寄存器组指针。内部调用。
//============================================================================
PORT_MemMapPtr hw_gpio_get_port_addr (uint8 port)
{
	PORT_MemMapPtr p;
    
    switch(port)
    {
    case PORT_A:
        p = PORTA_BASE_PTR;
        break;
    case PORT_B:
        p = PORTB_BASE_PTR;
        break;
    case PORT_C:
        p = PORTC_BASE_PTR;
        break;
    case PORT_D:
        p = PORTD_BASE_PTR;
        break;
    case PORT_E:
        p = PORTE_BASE_PTR;
        break;
    default:
        return 0; //输入参数无效返回
    }
    
    return p;
}

//============================================================================
//函数名称：hw_gpio_get_pt_addr
//函数返回：端口号对应的GPIO端口寄存器组指针
//参数说明：无
//功能概要：将端口号转换成端口寄存器组指针。内部调用。
//============================================================================
GPIO_MemMapPtr hw_gpio_get_pt_addr(uint8 port)
{
	GPIO_MemMapPtr p;
    
    switch(port)
    {
    case PORT_A:
        p = PTA_BASE_PTR;
        break;
    case PORT_B:
        p = PTB_BASE_PTR;
        break;
    case PORT_C:
        p = PTC_BASE_PTR;
        break;
    case PORT_D:
        p = PTD_BASE_PTR;
        break;
    case PORT_E:
        p = PTE_BASE_PTR;
        break;
    default:
        return 0; //输入参数无效返回
    }
    
    return p;
}

//============================================================================
//函数名称：hw_gpio_init
//函数返回：函数执行状态。0=成功，其它为异常。
//参数说明：port：端口号。由宏定义。
//          pin：引脚号。0～31。
//          dir：引脚方向。 1=输出，0=输入。
//          state：引脚初始状态。1=高电平，0=低电平
//功能概要：初始化端口作为GPIO引脚的功能。
//============================================================================
uint8 hw_gpio_init(uint8 port, uint8 pin, uint8 dir, uint8 state)
{
    //将GPIO端口号转换成端口寄存器组指针
    GPIO_MemMapPtr pt = hw_gpio_get_pt_addr(port);
    PORT_MemMapPtr p  = hw_gpio_get_port_addr(port);
    
    if (!p) return 1;  //参数错误
     
    //设定通用端口引脚控制寄存器的值，设定为GPIO功能
    PORT_PCR_REG(p, pin) = (0|PORT_PCR_MUX(1));

    if(dir == 1) //若引脚被定义为输出
    {
        GPIO_PDDR_REG(pt) |= (1<<pin);
    	 
    	  //设定引脚初始化状态
        if(state == 1)
		    BSET(pin, GPIO_PDOR_REG(pt));
        else
        	BCLR(pin, GPIO_PDOR_REG(pt));
    }
    else //若引脚被定义为输入 
    {
    	BCLR(pin, GPIO_PDDR_REG(pt));
    }

    return 0;  //成功返回
}

//============================================================================
//函数名称：hw_gpio_get
//函数返回：指定引脚状态。0=低电平，1=高电平。
//参数说明：port：端口号。由宏定义。
//          pin：引脚号。0～31。
//功能概要：获取指定引脚状态。
//============================================================================
uint8 hw_gpio_get(uint8 port, uint8 pin)
{
    //将GPIO端口号转换成端口寄存器组指针
	GPIO_MemMapPtr pt = hw_gpio_get_pt_addr(port);
    
    //查看引脚状态
    if (BGET(pin, GPIO_PDIR_REG(pt)) == 0)
	{
        return 0;  //高电平
    }
    else
    {
        return 1;  //低电平
    }
}

//============================================================================
//函数名称：hw_gpio_set
//函数返回：无
//参数说明：port：端口号。由宏定义。
//          pin：引脚号。0～31。
//          state：引脚初始状态。1=高电平，0=低电平
//功能概要：获取指定引脚状态。
//============================================================================
void hw_gpio_set(uint8 port, uint8 pin, uint8 state)
{
    //将GPIO端口号转换成端口寄存器组指针
    GPIO_MemMapPtr pt = hw_gpio_get_pt_addr(port);
	  
	  if (state == 0) //控制为低电平
	      BCLR(pin, GPIO_PDOR_REG(pt));
	  else            //控制为高电平
	      BSET(pin, GPIO_PDOR_REG(pt));
}

//============================================================================
//函数名称：hw_gpio_reverse
//函数返回：无
//参数说明：port：端口号。由宏定义。
//          pin：引脚号。0～31。
//功能概要：反转指定引脚状态。
//============================================================================
void hw_gpio_reverse(uint8 port, uint8 pin)
{
    //将GPIO端口号转换成端口寄存器组指针
    GPIO_MemMapPtr pt = hw_gpio_get_pt_addr(port);
	  
    BSET(pin, GPIO_PTOR_REG(pt));
}
