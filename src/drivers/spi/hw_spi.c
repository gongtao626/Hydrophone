#include "hw_spi.h"

//=================�ڲ���������=============================================

//=========================================================================
//�������ƣ�hw_spi_get_base_address                                                        
//���ܸ�Ҫ����ȡSPI�Ĵ����Ļ�ַ                                                
//����˵������      
//�������أ�SPI�Ļ�ֵַ                                                                  
//=========================================================================
SPI_MemMapPtr hw_spi_get_base_address();

//=================�ӿں���ʵ��=============================================

//=========================================================================
//�������ƣ�hw_spi_init
//����������Master:�Ƿ���������
//�������أ���
//���ܸ�Ҫ��SPI��ʼ����
//=========================================================================
//�ӻ��ļ�������λ�����趨�������趨ô��MCUֱ����Ϊ�������ӻ��Զ��趨��
void hw_spi_init(uint8 master)
{
    SPI_MemMapPtr BaseAdd = hw_spi_get_base_address();
    
    //ʹ��SPIģ��ʱ�ӣ�����SPI���Ź���
        SIM_SCGC6 |= SIM_SCGC6_SPI0_MASK;//��ʱ��
        PORTC_PCR4 = 0 | PORT_PCR_MUX(0x2) | PORT_PCR_DSE_MASK;//PC4 PCS0,DSE=1��������ʹ�ܣ�Ƭѡ
        PORTC_PCR5 = 0 | PORT_PCR_MUX(0x2) | PORT_PCR_DSE_MASK;//PC5 SCK
        PORTC_PCR6 = 0 | PORT_PCR_MUX(0x2) | PORT_PCR_DSE_MASK;//PC6 SOUT MOSI
        PORTC_PCR7 = 0 | PORT_PCR_MUX(0x2);//PC7 SIN MISO
    
    SPI_MCR_REG(BaseAdd) = 0 //��������ʱ�ӽ�ֹ
                 | SPI_MCR_CLR_TXF_MASK     //Clear the Tx FIFO counter.
                 | SPI_MCR_CLR_RXF_MASK     //Clear the Rx FIFO counter.
                 | SPI_MCR_PCSIS(1) //PCS0�͵�ƽ��Ч
                 | SPI_MCR_HALT_MASK;//HALT=1��ֹͣ����

    //�������ӻ�ģʽ���ù���ģʽ
    if(master == MASTER)
    {
        SPI_MCR_REG(BaseAdd) |= SPI_MCR_MSTR_MASK;//����ģʽ
        //ʱ��Ƶ�ʣ�(fSYS/Prescaler) x [(1+DBR)/Scaler]=(����ʱ�� /pbr)((1+DBR)/BR);����ʱ��24M��SCKƵ��12M   SCK = 83.3ns = 1000/12 ns
        //�趨PBR2��Ƶ����С����DBR˫�������ʣ���󣩣�BR2��Ƶ����С����(24/2)((1+1)/2)=12M������SCK
        //��ͬ����������ò�ͬ��ͨ����ʱ��������������ʱʱ��������ã�
        //tCSC =(1/fsys)��(2��PCSSCK+1)��2^(CSSCK+1) 	1/12=tsck PCSSCK��2λ��CSSCK��4λ  
        //tASC =(1/fsys)��(2��PASC+1)��2^(ASC+1) 		1/12=tsck PASC��2λ��ASC��4λ 
        //tDT=(1/fsys)��(2��PDT+1)��2^(DT+1)			(1/24)*7*32=9.33us PDT��2λ��DT��4λ 
        //tSCK = 1/12M; tCSC = 1/12M; tASC = 1/12M; tDT = 1*2/24M = 1/12M��
        //����16λ����ʱ��CNVΪ��ʱʱ��Ϊ��18/12M����1.5us��tDTΪ1/12M����0.0833us�����ܹ�1.583us����һ·�ź�
        //һ�β���ʱ��Ϊ19/12M����������CONVSTȷ����Ŀǰ�趨Ϊ5k��200us����һ��CS
        SPI_CTAR_REG(BaseAdd,0) = 0			//ʱ���봫���������ã�MSB first(0), PBR=00(2��Ƶ)
                       | SPI_CTAR_DBR_MASK //˫��������,1 bit
                       | SPI_CTAR_FMSZ(0x0F)//��֡��С0xF+1=16
                       |  SPI_CTAR_PDT_MASK//PDT=3 tASC tDT tCCS
                       | SPI_CTAR_DT(4) //DT=4, 92.3k
                       //| SPI_CTAR_DT(7) //DT=7, 13.13K
                       | SPI_CTAR_BR(0);//BR=0, CPOL=0, CPHA=0, �޷�æָʾ����ʱ��
                       //| SPI_CTAR_CPOL_MASK //CPOL=1
                       //| SPI_CTAR_CPHA_MASK;//CPHA=1

/*                SPI_CTAR_REG(BaseAdd,0) = 0			//ʱ���봫���������ã�MSB first(0), PBR=00(2��Ƶ)
                       | SPI_CTAR_DBR_MASK //˫��������,1 bit
                       | SPI_CTAR_FMSZ(0x0F)//��֡��С0xF+1=16
                       //| SPI_CTAR_PDT_MASK//PDT=7 tASC tDT tCCS
                       | 0x80000u //PDT = 5
                       | SPI_CTAR_DT(7) //DT=7, 18K
                       | SPI_CTAR_BR(0);//BR=0, CPOL=0, CPHA=0, �޷�æָʾ����ʱ��
                       //| SPI_CTAR_CPOL_MASK //CPOL=1
                       //| SPI_CTAR_CPHA_MASK;//CPHA=1
*/
    }

    SPI_SR_REG(BaseAdd) = (SPI_SR_EOQF_MASK	//End of Queue Flag��д1��ռĴ���SR�е����BITλ��������δ������Щ��ղο��ֲ�K10P64M50SF0RM, 40.3.5, P892
                         | SPI_SR_TFUF_MASK	//Transmit FIFO Underflow Flag
                         | SPI_SR_TFFF_MASK	//Transmit FIFO Fill Flag
                         | SPI_SR_RFOF_MASK	//Receive FIFO Overflow Flag
                         | SPI_SR_RFDF_MASK);	//Receive FIFO Drain Flag
    
    SPI_MCR_REG(BaseAdd) &= ~SPI_MCR_HALT_MASK;//HALTλ��0����ʼ����
}
   //ʱ��Ƶ�ʣ�(fSYS/Prescaler) x [(1+DBR)/Scaler]=(����ʱ�� /pbr)((1+DBR)/BR);����ʱ��24M��SCKƵ��12M   SCK = 83.3ns = 1000/12 ns
   //�趨PBR2��Ƶ����С����DBR˫�������ʣ���󣩣�BR2��Ƶ����С����(24/2)((1+1)/2)=12M������SCK
   //��ͬ����������ò�ͬ��ͨ����ʱ��������������ʱʱ��������ã�
   //tCSC =(1/fsys)��(2��PCSSCK+1)��2^(CSSCK+1) 	1/12=tsck PCSSCK��2λ��CSSCK��4λ  
   //tASC =(1/fsys)��(2��PASC+1)��2^(ASC+1) 		1/12=tsck PASC��2λ��ASC��4λ 
   //tDT=(1/fsys)��(2��PDT+1)��2^(DT+1)			(1/24)*7*32=9.33us PDT��2λ��DT��4λ 
   //tSCK = 1/12M; tCSC = 1/12M; tASC = 1/12M; tDT = 1*2/24M = 1/12M��
   //����16λ����ʱ��CNVΪ��ʱʱ��Ϊ��18/12M����1.5us��tDTΪ1/12M����0.0833us�����ܹ�1.583us����һ·�ź�
   //һ�β���ʱ��Ϊ19/12M����������CONVSTȷ����Ŀǰ�趨Ϊ5k��200us����һ��CS

void SPI0_Init(void) 
{ 
  SIM_SCGC6 |= SIM_SCGC6_SPI0_MASK;//��spi0ʱ�� 
  PORTC_PCR4 = PORT_PCR_MUX(2);//PC4��SPI0 PCS0��Ƭѡ 
  PORTC_PCR5 = PORT_PCR_MUX(2);//PC5��SPI0 SCK 
  PORTC_PCR6 = PORT_PCR_MUX(2);//PC6��SPI0 SOUT MOSI 
  PORTC_PCR7 = PORT_PCR_MUX(2);//PC7��SPI0 SIN MISO 
  SPI0_MCR = 0 | SPI_MCR_MSTR_MASK;//����ģʽ 
  SPI0_MCR |= SPI_MCR_PCSIS(1);// PCS0�͵�ƽ��Ч 
  SPI0_MCR &= ~SPI_MCR_MDIS_MASK;//ʹ��DSPIʱ�� 
  SPI0_MCR &= ~SPI_MCR_HALT_MASK;//HALTλ��0����ʼ����, ����SPIģ�� 
  //SPI0_MCR |= SPI_MCR_CONT_SCKE_MASK; //����ʱ����������
  SPI0_CTAR1 = 0 | SPI_CTAR_FMSZ(15);//֡��С16bit ��ʱPDT-00 DT-0000 tDT=1/24M=0.0416us
  SPI0_CTAR1 |= SPI_CTAR_CPOL_MASK;//ʱ�ӿ��иߵ�ƽ 
  SPI0_CTAR1 &= ~SPI_CTAR_CPHA_MASK;//������SCK�ĵ�һ�����ر���׽���ڶ������ر仯���ο��ֲ�P28����һ�����ػ�ȡ���ݣ��ڶ����������
  //SPI0_CTAR1 |= SPI_CTAR_CPHA_MASK;//������SCK�ĵڶ������ر���׽����һ�����ر仯
  
  SPI0_CTAR1 &= ~SPI_CTAR_LSBFE_MASK; //���ݸ�λ��ǰ����λ�ں�

  //ʱ��Ƶ�ʣ�(fSYS/Prescaler) x [(1+DBR)/Scaler]=(����ʱ�� /pbr)((1+DBR)/BR);����ʱ��24M��SCKƵ��12M   SCK = 83.3ns 1000/12 ns
  //�趨PBR2��Ƶ��DBR˫�������ʣ�BR2��Ƶ��(24/2)((1+1)/2)=12M
  SPI0_CTAR1 |= SPI_CTAR_DBR_MASK; //˫��������,1 bit��PBR=00 BR=0000
  //SPI0_CTAR1 |= SPI_CTAR_PBR(0);//Ԥ��Ƶ12M
  //SPI0_CTAR1 |= SPI_CTAR_BR(0);//���շ�Ƶ12MHz
  
  //SPI0_CTAR1 |= SPI_CTAR_PDT_MASK; //pdt = 7 dt =4 92.3k //PDT = 5; dt=7; //λ��'10'��ӦPDT=5 tASC tDT tCCS 18.237k
  //SPI0_CTAR1 |= SPI_CTAR_DT(0); //DT=7
} 
uint32 hw_spi_send_receive(uint16 ad_cfg)
{
  SPI0_PUSHR = 0 //| SPI_PUSHR_CONT_MASK //Ƭѡ�ź�PCS0һֱ����
              | SPI_PUSHR_CTAS(1)
              | SPI_PUSHR_PCS(1)
              | SPI_PUSHR_TXDATA(ad_cfg);
  while((SPI0_SR & SPI_SR_TCF_MASK) == 0);
  SPI0_SR |= SPI_SR_TCF_MASK;
  return SPI0_POPR;
}


//=========================================================================
//�������ƣ�hw_spi_send
//����������data[]����Ҫ���͵����ݡ�
//          len�����ݳ��ȡ�
//�������أ���
//���ܸ�Ҫ��SPI�������ݡ�
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
                SPI_PUSHR_REG(BaseAdd) = 0 //SPI_PUSHR_CONTΪ0�����ͼ�϶Ƭѡ�ź�PCS0�ָ����״̬(���ӵ�AD7682��CNV)
                               | SPI_PUSHR_CTAS(0)//����ģʽ������CTAR���˴�ѡ���ĸ�CTAR
                               | SPI_PUSHR_EOQ_MASK//end of queue
                               | SPI_PUSHR_PCS(0x01)//��·�����ѡ�����PTC4����ӦPCS0���ֲ�P189;��SPI_PUSHR_PCS(X)����X=1:ѡ��PCS0��X=2ѡ��PCS1
                               | SPI_PUSHR_TXDATA(data[i]);
        }
        else
        {
                SPI_PUSHR_REG(BaseAdd) = 0 //| SPI_PUSHR_CONT_MASK ���ͼ�϶Ƭѡ�ź�PCS0�ָ����״̬
                               | SPI_PUSHR_CTAS(0)
                               | SPI_PUSHR_PCS(1)
                               | SPI_PUSHR_TXDATA(data[i]);
        }
    }
}

//=========================================================================
//�������ƣ�hw_spi_re
//����������data[]����Ҫ���յ����ݡ�
//�������أ�0���ɹ����� 1��ʧ�ܣ���
//���ܸ�Ҫ��SPI�������ݡ�
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
//�������ƣ�hw_spi_enable_re_int
//����˵������
//�������أ� ��
//���ܸ�Ҫ����SPI�����ж�
//=========================================================================
void hw_spi_enable_re_int(void)
{
    SPI_MemMapPtr BaseAdd = hw_spi_get_base_address();
    BSET(SPI_RSER_RFDF_RE_SHIFT, SPI_RSER_REG(BaseAdd));//����SPI�����ж�
    enable_irq(SPI0IRQ);			 //���������ŵ�IRQ�ж�
}

//=========================================================================
//�������ƣ�hw_spi_disable_re_int
//����˵������
//�������أ� ��
//���ܸ�Ҫ����SPI�����ж�
//=========================================================================
void hw_spi_disable_re_int(void)
{
    SPI_MemMapPtr BaseAdd = hw_spi_get_base_address();
    BCLR(SPI_RSER_RFDF_RE_SHIFT, SPI_RSER_REG(BaseAdd));//�ر�SPI�����ж�
    disable_irq(SPI0IRQ);			 //���������ŵ�IRQ�ж�
}

//=================�ڲ�����ʵ��=============================================
//=========================================================================
//�������ƣ�hw_spi_get_base_address
//����������spiNo��SPIͨ����
//�������أ���
//���ܸ�Ҫ����SPIͨ����ת���ɶ�Ӧ��ָ�롣
//=========================================================================
SPI_MemMapPtr hw_spi_get_base_address(void)
{
      return SPI0_BASE_PTR;
}
void hw_spi_stop_transfer(void)
{
	SPI_MemMapPtr BaseAdd = hw_spi_get_base_address();
	SPI_MCR_REG(BaseAdd) |= SPI_MCR_HALT_MASK;//HALTλ��1����������
}

void hw_spi_start_transfer(void)
{
	SPI_MemMapPtr BaseAdd = hw_spi_get_base_address();
	SPI_MCR_REG(BaseAdd) &= ~(SPI_MCR_HALT_MASK);//HALTλ��0����ʼ����
}


