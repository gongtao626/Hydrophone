//============================================================================
//�ļ����ƣ�hw_gpio.h
//���ܸ�Ҫ��K60 GPIO�ײ���������ͷ�ļ�
//��Ȩ���У����ݴ�ѧ��˼����Ƕ��ʽ����(sumcu.suda.edu.cn)
//���¼�¼��2011-11-13   V1.0     ��ʼ�汾
//          2011-11-17   V1.1    �淶�Ű���
//============================================================================

#include "hw_gpio.h"    

//============================================================================
//�������ƣ�hw_gpio_enable_port
//�������أ���
//����˵������
//���ܸ�Ҫ����������GPIO�˿ڵ�ʱ�ӡ� ���߲��������á�
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
//�������ƣ�hw_gpio_get_port_addr
//�������أ��˿ںŶ�Ӧ�Ķ˿ڼĴ�����ָ��
//����˵������
//���ܸ�Ҫ�����˿ں�ת���ɶ˿ڼĴ�����ָ�롣�ڲ����á�
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
        return 0; //���������Ч����
    }
    
    return p;
}

//============================================================================
//�������ƣ�hw_gpio_get_pt_addr
//�������أ��˿ںŶ�Ӧ��GPIO�˿ڼĴ�����ָ��
//����˵������
//���ܸ�Ҫ�����˿ں�ת���ɶ˿ڼĴ�����ָ�롣�ڲ����á�
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
        return 0; //���������Ч����
    }
    
    return p;
}

//============================================================================
//�������ƣ�hw_gpio_init
//�������أ�����ִ��״̬��0=�ɹ�������Ϊ�쳣��
//����˵����port���˿ںš��ɺ궨�塣
//          pin�����źš�0��31��
//          dir�����ŷ��� 1=�����0=���롣
//          state�����ų�ʼ״̬��1=�ߵ�ƽ��0=�͵�ƽ
//���ܸ�Ҫ����ʼ���˿���ΪGPIO���ŵĹ��ܡ�
//============================================================================
uint8 hw_gpio_init(uint8 port, uint8 pin, uint8 dir, uint8 state)
{
    //��GPIO�˿ں�ת���ɶ˿ڼĴ�����ָ��
    GPIO_MemMapPtr pt = hw_gpio_get_pt_addr(port);
    PORT_MemMapPtr p  = hw_gpio_get_port_addr(port);
    
    if (!p) return 1;  //��������
     
    //�趨ͨ�ö˿����ſ��ƼĴ�����ֵ���趨ΪGPIO����
    PORT_PCR_REG(p, pin) = (0|PORT_PCR_MUX(1));

    if(dir == 1) //�����ű�����Ϊ���
    {
        GPIO_PDDR_REG(pt) |= (1<<pin);
    	 
    	  //�趨���ų�ʼ��״̬
        if(state == 1)
		    BSET(pin, GPIO_PDOR_REG(pt));
        else
        	BCLR(pin, GPIO_PDOR_REG(pt));
    }
    else //�����ű�����Ϊ���� 
    {
    	BCLR(pin, GPIO_PDDR_REG(pt));
    }

    return 0;  //�ɹ�����
}

//============================================================================
//�������ƣ�hw_gpio_get
//�������أ�ָ������״̬��0=�͵�ƽ��1=�ߵ�ƽ��
//����˵����port���˿ںš��ɺ궨�塣
//          pin�����źš�0��31��
//���ܸ�Ҫ����ȡָ������״̬��
//============================================================================
uint8 hw_gpio_get(uint8 port, uint8 pin)
{
    //��GPIO�˿ں�ת���ɶ˿ڼĴ�����ָ��
	GPIO_MemMapPtr pt = hw_gpio_get_pt_addr(port);
    
    //�鿴����״̬
    if (BGET(pin, GPIO_PDIR_REG(pt)) == 0)
	{
        return 0;  //�ߵ�ƽ
    }
    else
    {
        return 1;  //�͵�ƽ
    }
}

//============================================================================
//�������ƣ�hw_gpio_set
//�������أ���
//����˵����port���˿ںš��ɺ궨�塣
//          pin�����źš�0��31��
//          state�����ų�ʼ״̬��1=�ߵ�ƽ��0=�͵�ƽ
//���ܸ�Ҫ����ȡָ������״̬��
//============================================================================
void hw_gpio_set(uint8 port, uint8 pin, uint8 state)
{
    //��GPIO�˿ں�ת���ɶ˿ڼĴ�����ָ��
    GPIO_MemMapPtr pt = hw_gpio_get_pt_addr(port);
	  
	  if (state == 0) //����Ϊ�͵�ƽ
	      BCLR(pin, GPIO_PDOR_REG(pt));
	  else            //����Ϊ�ߵ�ƽ
	      BSET(pin, GPIO_PDOR_REG(pt));
}

//============================================================================
//�������ƣ�hw_gpio_reverse
//�������أ���
//����˵����port���˿ںš��ɺ궨�塣
//          pin�����źš�0��31��
//���ܸ�Ҫ����תָ������״̬��
//============================================================================
void hw_gpio_reverse(uint8 port, uint8 pin)
{
    //��GPIO�˿ں�ת���ɶ˿ڼĴ�����ָ��
    GPIO_MemMapPtr pt = hw_gpio_get_pt_addr(port);
	  
    BSET(pin, GPIO_PTOR_REG(pt));
}
