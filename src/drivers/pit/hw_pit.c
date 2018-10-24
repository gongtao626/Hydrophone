//==========================================================================
//文件名称：hw_pit.c
//功能概要：K60 pit底层驱动程序文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//版本更新：2011-11-13  V1.0  初始版本。
//==========================================================================
    
#include "hw_pit.h"
//==========================================================================
//函数名称: hw_pit_init                                                         
//函数返回: 无                                          
//参数说明: pitno:表示pit通道号。  
//	    timeout:表示定时溢出开始递减的值          
//功能概要: 设置相关寄存器的值        
//==========================================================================
void hw_pit_init(uint8 pitno,uint32 timeout)
{
	SIM_SCGC6|=SIM_SCGC6_PIT_MASK;              //使能PIT时钟
	PIT_MCR&=~(PIT_MCR_MDIS_MASK);              //调试模式下禁止
	PIT_MCR|=PIT_MCR_FRZ_MASK;                  //使能PIT模块时钟
	PIT_LDVAL(pitno)=timeout;                   //设置周期
	PIT_TCTRL(pitno)|=PIT_TCTRL_TEN_MASK;       //使能pit模块运行
	PIT_TCTRL(pitno)&=~(PIT_TCTRL_TIE_MASK);    //关pit中断
}
    
//==========================================================================
//函数名称: hw_enable_pit_int                                                     
//函数返回: 无                                              
//参数说明: pitno: 表示pit通道号      
//功能概要: 开PIT中断                                                                                                     
//==========================================================================
void hw_enable_pit_int(uint8 pitno)
{
	PIT_TCTRL(pitno)|=(PIT_TCTRL_TIE_MASK); //开pit中断
	switch(pitno)
	{
		case 0:
			enable_irq(30);			      //开接收引脚的IRQ中断
		break;
		case 1:
			enable_irq(31);			      //开接收引脚的IRQ中断
		break;
		case 2:
			enable_irq(32);			      //开接收引脚的IRQ中断
		break;
		case 3:
			enable_irq(33);			      //开接收引脚的IRQ中断
		break;
	}
}

//==========================================================================
//函数名称: hw_disable_pit_int                                                    
//函数返回: 无                                              
//参数说明: pitno: 表示pit通道号      
//功能概要: 关闭PIT中断                                                                                                     
//==========================================================================
void hw_disable_pit_int(uint8 pitno)
{
	PIT_TCTRL(pitno)&=~(PIT_TCTRL_TIE_MASK);//关pit中断
	switch(pitno)
	{
		case 0:
			disable_irq(30);	              //关接收引脚的IRQ中断
		break;
		case 1:
			disable_irq(31);		         //关接收引脚的IRQ中断
		break;
		case 2:
			disable_irq(32);		         //关接收引脚的IRQ中断
		break;
		case 3:
			disable_irq(33);		         //关接收引脚的IRQ中断
		break;
	}
}
