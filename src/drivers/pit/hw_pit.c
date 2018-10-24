//==========================================================================
//�ļ����ƣ�hw_pit.c
//���ܸ�Ҫ��K60 pit�ײ����������ļ�
//��Ȩ���У����ݴ�ѧ��˼����Ƕ��ʽ����(sumcu.suda.edu.cn)
//�汾���£�2011-11-13  V1.0  ��ʼ�汾��
//==========================================================================
    
#include "hw_pit.h"
//==========================================================================
//��������: hw_pit_init                                                         
//��������: ��                                          
//����˵��: pitno:��ʾpitͨ���š�  
//	    timeout:��ʾ��ʱ�����ʼ�ݼ���ֵ          
//���ܸ�Ҫ: ������ؼĴ�����ֵ        
//==========================================================================
void hw_pit_init(uint8 pitno,uint32 timeout)
{
	SIM_SCGC6|=SIM_SCGC6_PIT_MASK;              //ʹ��PITʱ��
	PIT_MCR&=~(PIT_MCR_MDIS_MASK);              //����ģʽ�½�ֹ
	PIT_MCR|=PIT_MCR_FRZ_MASK;                  //ʹ��PITģ��ʱ��
	PIT_LDVAL(pitno)=timeout;                   //��������
	PIT_TCTRL(pitno)|=PIT_TCTRL_TEN_MASK;       //ʹ��pitģ������
	PIT_TCTRL(pitno)&=~(PIT_TCTRL_TIE_MASK);    //��pit�ж�
}
    
//==========================================================================
//��������: hw_enable_pit_int                                                     
//��������: ��                                              
//����˵��: pitno: ��ʾpitͨ����      
//���ܸ�Ҫ: ��PIT�ж�                                                                                                     
//==========================================================================
void hw_enable_pit_int(uint8 pitno)
{
	PIT_TCTRL(pitno)|=(PIT_TCTRL_TIE_MASK); //��pit�ж�
	switch(pitno)
	{
		case 0:
			enable_irq(30);			      //���������ŵ�IRQ�ж�
		break;
		case 1:
			enable_irq(31);			      //���������ŵ�IRQ�ж�
		break;
		case 2:
			enable_irq(32);			      //���������ŵ�IRQ�ж�
		break;
		case 3:
			enable_irq(33);			      //���������ŵ�IRQ�ж�
		break;
	}
}

//==========================================================================
//��������: hw_disable_pit_int                                                    
//��������: ��                                              
//����˵��: pitno: ��ʾpitͨ����      
//���ܸ�Ҫ: �ر�PIT�ж�                                                                                                     
//==========================================================================
void hw_disable_pit_int(uint8 pitno)
{
	PIT_TCTRL(pitno)&=~(PIT_TCTRL_TIE_MASK);//��pit�ж�
	switch(pitno)
	{
		case 0:
			disable_irq(30);	              //�ؽ������ŵ�IRQ�ж�
		break;
		case 1:
			disable_irq(31);		         //�ؽ������ŵ�IRQ�ж�
		break;
		case 2:
			disable_irq(32);		         //�ؽ������ŵ�IRQ�ж�
		break;
		case 3:
			disable_irq(33);		         //�ؽ������ŵ�IRQ�ж�
		break;
	}
}
