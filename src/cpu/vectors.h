/******************************************************************************
* File:    vectors.h
*
* Purpose: Provide custom interrupt service routines for Kinetis. 
*
* NOTE: This vector table is a superset table, so interrupt sources might be 
*       listed that are not available on the specific Kinetis device you are 
*       using.
******************************************************************************/

#ifndef __VECTORS_H
#define __VECTORS_H     1

#define  DMA0_irq_no        0         //16 
#define  DMA1_irq_no        1         //17 
#define  DMA2_irq_no        2         //18 
#define  DMA3_irq_no        3         //19 
#define  DMA4_irq_no        4         //20 
#define  DMA5_irq_no        5         //21 
#define  FCmdCom_irq_no     6         //22 
#define  FReadCol_irq_no    7         //23 
#define  LVD_irq_no         8         //24 
#define  LLWU_irq_no        9         //25 
#define  WDOG_irq_no        10        //26 
#define  I2C0_irq_no        11        //27 
#define  SPI0_irq_no        12        //28 
#define  I2S0_irq_no        13        //29 
#define  I2S1_irq_no        14        //30 
#define  UART0LON_irq_no    15        //31 
#define  UART0S_irq_no      16        //32 
#define  UART0E_irq_no      17        //33 
#define  UART1S_irq_no      18        //34 
#define  UART1E_irq_no      19        //35 
#define  UART2S_irq_no      20        //36 
#define  UART2E_irq_no      21        //37 
#define  ADC0_irq_no        22        //38 
#define  CMP0_irq_no        23        //39
#define  CMP1_irq_no        24        //40 
#define  FTM0_irq_no        25        //41 
#define  FTM1_irq_no        26        //42 
#define  CMT_irq_no         27        //43 
#define  RTCA_irq_no        28        //44 
#define  RTCS_irq_no        29        //45 
#define  PITC0_irq_no       30        //46 
#define  PITC1_irq_no       31        //47 
#define  PITC2_irq_no       32        //48 
#define  PITC3_irq_no       33        //49 
#define  PDB_irq_no         34        //50 
#define  USBOTG_irq_no      35        //51 
#define  USBCD_irq_no       36        //52 
#define  TSI_irq_no 	    37        //53 
#define  MCG_irq_no         38        //54 
#define  LPTMR_irq_no       39        //55 
#define  PortA_irq_no       40        //56 
#define  PortB_irq_no       41        //57 
#define  PortC_irq_no       42        //58 
#define  PortD_irq_no       43        //59 
#define  PortE_irq_no       44        //60 
#define  SWI_irq_no         45        //61 
// function prototype for default_isr in vectors.c
void default_isr(void);
void abort_isr(void);

void hard_fault_handler_c(unsigned int * hardfault_args);

/* Interrupt Vector Table Function Pointers */
typedef void pointer(void);

extern void __startup(void);

extern unsigned long __BOOT_STACK_ADDRESS[];
extern void __iar_program_start(void);                              //11-8-2011 pbd - fixed comments to match RM
                                        // Address     Vector IRQ   Source module   Source description
#define VECTOR_000      (pointer*)__BOOT_STACK_ADDRESS	//          ARM core        Initial Supervisor SP
#define VECTOR_001      __startup	// 0x0000_0004 1 -          ARM core        Initial Program Counter
#define VECTOR_002      default_isr     // 0x0000_0008 2 -          ARM core        Non-maskable Interrupt (NMI)
#define VECTOR_003      default_isr     // 0x0000_000C 3 -          ARM core        Hard Fault
#define VECTOR_004      default_isr     // 0x0000_0010 4 -
#define VECTOR_005      default_isr     // 0x0000_0014 5 -          ARM core         Bus Fault
#define VECTOR_006      default_isr     // 0x0000_0018 6 -          ARM core         Usage Fault
#define VECTOR_007      default_isr     // 0x0000_001C 7 -                           
#define VECTOR_008      default_isr     // 0x0000_0020 8 -                           
#define VECTOR_009      default_isr     // 0x0000_0024 9 -
#define VECTOR_010      default_isr     // 0x0000_0028 10 -
#define VECTOR_011      default_isr     // 0x0000_002C 11 -         ARM core         Supervisor call (SVCall)
#define VECTOR_012      default_isr     // 0x0000_0030 12 -         ARM core         Debug Monitor
#define VECTOR_013      default_isr     // 0x0000_0034 13 -                          
#define VECTOR_014      default_isr     // 0x0000_0038 14 -         ARM core         Pendable request for system service (PendableSrvReq)
#define VECTOR_015      default_isr     // 0x0000_003C 15 -         ARM core         System tick timer (SysTick)
#define VECTOR_016      default_isr     // 0x0000_0040 16     0     DMA              DMA Channel 0 transfer complete
#define VECTOR_017      default_isr     // 0x0000_0044 17     1     DMA              DMA Channel 1 transfer complete
#define VECTOR_018      default_isr     // 0x0000_0048 18     2     DMA            38  DMA Channel 2 transfer complete
#define VECTOR_019      default_isr     // 0x0000_004C 19     3     DMA              DMA Channel 3 transfer complete
#define VECTOR_020      default_isr     // 0x0000_0050 20     4     DMA              DMA Channel 4 transfer complete
#define VECTOR_021      default_isr     // 0x0000_0054 21     5     DMA              DMA Channel 5 transfer complete
#define VECTOR_022      default_isr     // 0x0000_0058 22     6     Flash memory     Command Complete
#define VECTOR_023      default_isr     // 0x0000_005C 23     7     Flash memory     Read Collision
#define VECTOR_024      default_isr     // 0x0000_0060 24     8     Mode Controller  Low Voltage Detect,Low Voltage Warning, Low Leakage Wakeup
#define VECTOR_025      default_isr     // 0x0000_0064 25     9     LLWU
#define VECTOR_026      default_isr     // 0x0000_0068 26    10     WDOG
#define VECTOR_027      default_isr     // 0x0000_006C 27    11     I2C0
#define VECTOR_028      default_isr     // 0x0000_0070 28    12     SPI0             Single interrupt vector for all sources
#define VECTOR_029      default_isr     // 0x0000_0074 29    13     I2S0
#define VECTOR_030      default_isr     // 0x0000_0078 30    14     I2S1
#define VECTOR_031      default_isr     // 0x0000_007C 31    15     UART0            Single interrupt vector for CEA709.1-B (LON) status sources
#define VECTOR_032      default_isr     // 0x0000_0080 32    16     UART0            Single interrupt vector for UART status sources
#define VECTOR_033      default_isr     // 0x0000_0084 33    17     UART0            Single interrupt vector for UART error sources
#define VECTOR_034      default_isr     // 0x0000_0088 34    18     UART1            Single interrupt vector for UART status sources
#define VECTOR_035      default_isr     // 0x0000_008C 35    19     UART1            Single interrupt vector for UART error sources
#define VECTOR_036      default_isr     // 0x0000_0090 36    20     UART2            Single interrupt vector for UART status sources
#define VECTOR_037      default_isr     // 0x0000_0094 37    21     UART2            Single interrupt vector for UART error sources
#define VECTOR_038      default_isr     // 0x0000_0098 38    22     ADC0
#define VECTOR_039      default_isr     // 0x0000_009C 39    23	    CMP0             High-speed comparator 
#define VECTOR_040      default_isr     // 0x0000_00A0 40    24     CMP1
#define VECTOR_041      default_isr     // 0x0000_00A4 41    25     FTM0 			 Single interrupt vector for all sources 
#define VECTOR_042      default_isr     // 0x0000_00A8 42    26     FTM1 			 Single interrupt vector for all sources
#define VECTOR_043      default_isr     // 0x0000_00AC 43    27     CMT
#define VECTOR_044      default_isr     // 0x0000_00B0 44    28     RTC Alarm interrupt
#define VECTOR_045      default_isr     // 0x0000_00B4 45    29     RTC Seconds interrupt
#define VECTOR_046      default_isr     // 0x0000_00B8 46    30     PIT Channel 0
#define VECTOR_047      default_isr     // 0x0000_00BC 47    31     PIT Channel 1
#define VECTOR_048      default_isr     // 0x0000_00C0 48    32     PIT Channel 2
#define VECTOR_049      default_isr     // 0x0000_00C4 49    33     PIT Channel 3
#define VECTOR_050      default_isr     // 0x0000_00C8 50    34     PDB
#define VECTOR_051      default_isr     // 0x0000_00CC 51    35     USB OTG
#define VECTOR_052      default_isr     // 0x0000_00D0 52    36     USB Charger Detect
#define VECTOR_053      default_isr     // 0x0000_00D4 53    37     TSI 			 Single interrupt vector for all sources
#define VECTOR_054      default_isr     // 0x0000_00D8 54    38     MCG
#define VECTOR_055      default_isr     // 0x0000_00DC 55    39     Low Power Timer
#define VECTOR_056      default_isr     // 0x0000_00E0 56    40     Port control module Pin Detect (Port A) 
#define VECTOR_057      default_isr     // 0x0000_00E4 57    41     Port control module Pin Detect (Port B)
#define VECTOR_058      default_isr     // 0x0000_00E8 58    42     Port control module Pin Detect (Port C)
#define VECTOR_059      default_isr     // 0x0000_00EC 59    43     Port control module Pin Detect (Port D)
#define VECTOR_060      default_isr     // 0x0000_00F0 60    44     Port control module Pin Detect (Port E)
#define VECTOR_061      default_isr     // 0x0000_00F4 61    45     SWI
#define VECTOR_PADDING  (pointer*)0xffffffff/*not used*/

/* Flash configuration field values below */
/* Please be careful when modifying any of
 * the values below as it can secure the 
* flash (possibly permanently): 0x400-0x409.
 */
#define CONFIG_1	(pointer*)0xffffffff 
#define CONFIG_2	(pointer*)0xffffffff 
#define CONFIG_3	(pointer*)0xffffffff
#define CONFIG_4	(pointer*)0xfffffffe


#endif /*__VECTORS_H*/

/* End of "vectors.h" */
