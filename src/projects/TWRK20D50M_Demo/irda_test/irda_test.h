/****************************************************************************************************/
/**
Copyright (c) 2011 Freescale Semiconductor
Freescale Confidential Proprietary
\file     irda_test.c
\brief    UART0 configured as IRDA interface  CMT output enabled (doubled buffer)

\author     
\author     
\version    1.0
\date       Sep 26, 2011

*/


#define DAC_INPUT      7
#define IRDA_CMP_INPUT 1   //CMP0_IN0

void irda_test_init(void );
void irda_tx(char ch);
char irda_get_char(void);
