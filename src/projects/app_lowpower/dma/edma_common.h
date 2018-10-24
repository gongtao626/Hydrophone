//typedef signed char		s8;  /*  8 bits */ int8_t
//typedef signed short int	s16; /* 16 bits */ int16_t
//typedef signed long int	s32; /* 32 bits */ int32_t

#define DMA_TCD_SADDR(x)                            DMA_SADDR_REG(DMA_BASE_PTR,x) 
#define DMA_TCD_SOFF(x)                             DMA_SOFF_REG(DMA_BASE_PTR,x)
#define DMA_TCD_ATTR(x)                             DMA_ATTR_REG(DMA_BASE_PTR,x)
#define DMA_TCD_NBYTES_MLNO(x)                      DMA_NBYTES_MLNO_REG(DMA_BASE_PTR,x)
#define DMA_TCD_NBYTES_MLOFFNO(x)                   DMA_NBYTES_MLOFFNO_REG(DMA_BASE_PTR,x)
#define DMA_TCD_NBYTES_MLOFFYES(x)                  DMA_NBYTES_MLOFFYES_REG(DMA_BASE_PTR,x)
#define DMA_TCD_SLAST(x)                            DMA_SLAST_REG(DMA_BASE_PTR,x)
#define DMA_TCD_DADDR(x)                            DMA_DADDR_REG(DMA_BASE_PTR,x)
#define DMA_TCD_DOFF(x)                             DMA_DOFF_REG(DMA_BASE_PTR,x)
#define DMA_TCD_CITER_ELINKYES(x)                   DMA_CITER_ELINKYES_REG(DMA_BASE_PTR,x)
#define DMA_TCD_CITER_ELINKNO(x)                    DMA_CITER_ELINKNO_REG(DMA_BASE_PTR,x)
#define DMA_TCD_DLASTSGA(x)                         DMA_DLAST_SGA_REG(DMA_BASE_PTR,x)
#define DMA_TCD_CSR(x)                              DMA_CSR_REG(DMA_BASE_PTR,x)
#define DMA_TCD_BITER_ELINKNO(x)                    DMA_BITER_ELINKNO_REG(DMA_BASE_PTR,x)
#define DMA_TCD_BITER_ELINKYES(x)                   DMA_BITER_ELINKYES_REG(DMA_BASE_PTR,x)
//#define DMAMUX_CHCFG(x)                             DMAMUX_CHCFG_REG(DMAMUX_BASE_PTR,x)


typedef struct _dma_tcd {
	uint32 saddr;
	int16 soff;
	uint16 tcdAttr;
	uint32 nbytes;
	int32 slast;
	uint32 daddr;
	int16 doff;
	uint16 citer;
	int32 dlast_sga;
	uint16 csr;	
	uint16 biter;
	int channelno;
	uint32 testdata;
	int loopcount;
        uint32 link;
}dma_tcd;

enum COMMAND
{
	CONFIG_BASIC_XFR,
	MEM_FILL,
	DMA_SOFT_START,
	XFR_OVER_WAIT,
	CMP_MEM,
	CONFIG_BASIC_XFR_ERR,
	//CONFIG_BASIC_XFR_FULL_INT,
	//CONFIG_BASIC_XFR_HALF_INT,
	//CHK_HALF_POINT,
	MINOR_LOOP_CH_LINK,
	//MAJOR_LOOP_LINKED_CH_CFG,
	MAJOR_LOOP_LINK_EN,
	//TCD_CONFIG,
	SLAST_CHECK,
	DLAST_CHECK,
	SGA_TEST_CONFIG,
	SGA_ENB,
	XFR_WAIT_ERROR_CHECK,
	ERROR_CLR,
	INSERT_CPE_ERR,
	CONFIG_PRIORITY,
	CHECK_PRI
};

struct dummy_tcd {

	uint32 saddr;
	int16 soff;
	uint16 attr;
	uint32 nbytes;
	int32 slast;
	uint32 daddr;
	int16 doff;
	uint16 citer;
	int32 dlast_sga;
	uint16 csr;
	uint16 biter;
	
};
//struct edmaTcdType * dlast_sga;

void dma_config(int COMMAND ,dma_tcd *tcd_value);
