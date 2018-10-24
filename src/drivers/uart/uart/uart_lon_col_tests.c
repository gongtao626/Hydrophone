/*
 * File:		uart_lon_col_tests.c
 * Purpose:		LON mode collision tests.
 *
 */

#include "common.h"
#include "uart.h"
#include "uart_tests.h"


/********************************************************************/
/* UART module LON mode collision test.
 *
 * Tests the LON mode collision detect feature. The test will use a GPIO 
 * pin connected to the UART_COL signal to activate the collision
 * signal during different parts of the transfer. The delay loops
 * in the test are important because the are used to force the collision
 * toggle to occur during specific parts of the transfer. It is a good 
 * idea to use a scope to double check the timing of these delays anytime
 * this test is ported to a new processor. 
 *
 * Test will check both polarities of the COL signal and all four possible
 * states for collision detection available in S4[CDET]. Test returns the 
 * number of errors detected.
 */
int uart_lon_col_test(UART_MemMapPtr module)
{
    volatile char ch;
    uint32 i;
    int error = 0;        
            
    printf("\nStarting UART LON mode collision test.\n");
    
    
    printf("\nPlease connect a wire to (PTD5/UART0_COL).\n");
    printf("\nPlease connect a wire to (PTA13/GPIO).\n");
    
    printf("Press any key when ready.\n");
    ch = in_char();
    
    /* Configure PTA13 as a GPIO output driving high */
    PORTA_PCR13 = PORT_PCR_MUX(0x1); // GPIO is alt1 function
    
    GPIOA_PDDR |= 1<<13; // Configure as output
    
    GPIOA_PDOR |= 1<<13; // Drive pin high
    
    
    /* Configure the UART for LON mode */
    UART_C6_REG(module) = UART_C6_EN709_MASK;
    
    UART_PRE_REG(module) = 0x8; /* Set preamble length */
    
    UART_TPL_REG(module) = 0x01; /* Set packet length */

    /* Set beta1 timer to 1 as 0 seems to max out delay */
    UART_B1T_REG(module) = 1;
    
    /* Enable collision - active low COL is default */
    UART_C6_REG(module) |= UART_C6_CE_MASK;
    
    /* Make sure CDET field is cleared to start */
    if ( UART_S4_REG(module) != 0)
    {
        error++;
        printf("\nERR! S4 value is incorrect. Expected: 0x0 Read: 0x%02X", UART_S4_REG(module));
    }
    

    /* Test active low COL during preamble */
    printf("Testing active low COL during preamble...\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();
    
    UART_C6_REG(module) |= UART_C6_TX709_MASK;
    
    uart_putchar (module, 0xAA); 
    
    /* Small delay before driving PTD2 low */
    for(i=0; i < 0x400; i++);
    
    /* Toggle PTA13 low immediately */
    GPIOA_PDOR &= ~(1<<13); // Drive pin low
    
    /* Wait for transmit to complete */
    while( !(UART_S3_REG(module) & UART_S3_PTXF_MASK));

    /* Disable transmit to prevent a second packet from attempting to go out */
    UART_C6_REG(module) &= ~UART_C6_TX709_MASK;
    
    /* Clear the PTXF flag */
    UART_S3_REG(module) = UART_S3_PTXF_MASK;    
    
    /* Make sure CDET field is 01 indicating COL during preamble */
    if ( (UART_S4_REG(module) & UART_S4_CDET(0x3)) != 0x04)
    {
        error++;
        printf("\nERR! S4 value is incorrect. Expected: 0x04 Read: 0x%02X", UART_S4_REG(module));
    }
    
    /* Set PTA13 high again */
    GPIOA_PDOR |= 1<<13; // Drive pin high    
    
    /* Write 1s to S4 to clear CDET field */
    UART_S4_REG(module) = 0xF;


    /* Test active low COL during data */
    printf("Testing active low COL during data...\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();
    
    UART_C6_REG(module) |= UART_C6_TX709_MASK;
    
    uart_putchar (module, 0xAA); 
    
    /* delay before driving PTA13 low */
    //for(i=0; i < 0xC00; i++);
    for(i=0; i < 0x800; i++); // Modified for P0 MCU
    
    /* Toggle PTA13 low */
    GPIOA_PDOR &= ~(1<<13); // Drive pin low
    
    /* Wait for transmit to complete */
    while( !(UART_S3_REG(module) & UART_S3_PTXF_MASK));

    /* Disable transmit to prevent a second packet from attempting to go out */
    UART_C6_REG(module) &= ~UART_C6_TX709_MASK;
    
    /* Clear the PTXF flag */
    UART_S3_REG(module) = UART_S3_PTXF_MASK;    
    
    /* Make sure CDET field is 10 indicating COL during data */
    if ( (UART_S4_REG(module) & UART_S4_CDET(0x3)) != 0x08)
    {
        error++;
        printf("\nERR! S4 value is incorrect. Expected: 0x08 Read: 0x%02X", UART_S4_REG(module));
    }
    
    /* Set PTA13 high again */
    GPIOA_PDOR |= 1<<13; // Drive pin high    
    
    /* Write 1s to S4 to clear CDET field */
    UART_S4_REG(module) = 0xF;
    

    /* Test active low COL during line code violation */
    printf("Testing active low COL during line code violation...\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();
    
    UART_C6_REG(module) |= UART_C6_TX709_MASK;
    
    uart_putchar (module, 0xAA); 
    
    /* delay before driving PTD2 low */
    //for(i=0; i < 0x1400; i++);
    for(i=0; i < 0xA02; i++); // Modified for P0 MCU
    
    /* Toggle PTA13 low */
    GPIOA_PDOR &= ~(1<<13); // Drive pin low
    
    /* Wait for transmit to complete */
    while( !(UART_S3_REG(module) & UART_S3_PTXF_MASK));

    /* Disable transmit to prevent a second packet from attempting to go out */
    UART_C6_REG(module) &= ~UART_C6_TX709_MASK;
    
    /* Clear the PTXF flag */
    UART_S3_REG(module) = UART_S3_PTXF_MASK;    
    
    /* Make sure CDET field is 11 indicating COL during line code violation */
    if ( (UART_S4_REG(module) & UART_S4_CDET(0x3)) != 0x0C)
    {
        error++;
        printf("\nERR! S4 value is incorrect. Expected: 0x08 Read: 0x%02X", UART_S4_REG(module));
    }
    
    /* Set PTA13 high again */
    GPIOA_PDOR |= 1<<13; // Drive pin high    
    
    /* Write 1s to S4 to clear CDET field */
    UART_S4_REG(module) = 0xF;


    /* Test active low COL - no collision */
    printf("Testing active low COL - no collision...\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();
    
    UART_C6_REG(module) |= UART_C6_TX709_MASK;
    
    uart_putchar (module, 0xAA); 
    
    /* Wait for transmit to complete */
    while( !(UART_S3_REG(module) & UART_S3_PTXF_MASK));

    /* Disable transmit to prevent a second packet from attempting to go out */
    UART_C6_REG(module) &= ~UART_C6_TX709_MASK;
    
    /* Clear the PTXF flag */
    UART_S3_REG(module) = UART_S3_PTXF_MASK;    
    
    /* Make sure CDET field is 00 indicating no collision detected */
    if ( (UART_S4_REG(module) & UART_S4_CDET(0x3)) != 0x00)
    {
        error++;
        printf("\nERR! S4 value is incorrect. Expected: 0x00 Read: 0x%02X", UART_S4_REG(module));
    }

    /* Write 1s to S4 to clear CDET field */
    UART_S4_REG(module) = 0xF;


    /* Reconfigure UART for active high COL */
    UART_C6_REG(module) |= UART_C6_CP_MASK;
    
    /* Drive PTA13 low (inactive) */
    GPIOA_PDOR &= ~(1<<13); // Drive pin low
        
    
    /* Test active high COL during preamble */
    printf("Testing active high COL during preamble...\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();
    
    UART_C6_REG(module) |= UART_C6_TX709_MASK;
    
    uart_putchar (module, 0xAA); 
    
    /* Small delay before driving PTD2 high */
    for(i=0; i < 0x400; i++);
    
    /* Toggle PTA13 high  */
    GPIOA_PDOR |= 1<<13; // Drive pin high
    
    /* Wait for transmit to complete */
    while( !(UART_S3_REG(module) & UART_S3_PTXF_MASK));

    /* Disable transmit to prevent a second packet from attempting to go out */
    UART_C6_REG(module) &= ~UART_C6_TX709_MASK;
    
    /* Clear the PTXF flag */
    UART_S3_REG(module) = UART_S3_PTXF_MASK;    
    
    /* Make sure CDET field is 01 indicating COL during preamble */
    if ( (UART_S4_REG(module) & UART_S4_CDET(0x3)) != 0x04)
    {
        error++;
        printf("\nERR! S4 value is incorrect. Expected: 0x04 Read: 0x%02X", UART_S4_REG(module));
    }
    
    /* Set PTA13 low again */
    GPIOA_PDOR &= ~(1<<13); // Drive pin low    
    
    /* Write 1s to S4 to clear CDET field */
    UART_S4_REG(module) = 0xF;


    /* Test active high COL during data */
    printf("Testing active high COL during data...\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();
    
    UART_C6_REG(module) |= UART_C6_TX709_MASK;
    
    uart_putchar (module, 0xAA); 
    
    /* delay before driving PTD2 high */
    for(i=0; i < 0x800; i++);
    
    /* Toggle PTA13 high */
    GPIOA_PDOR |= 1<<13; // Drive pin high
    
    /* Wait for transmit to complete */
    while( !(UART_S3_REG(module) & UART_S3_PTXF_MASK));

    /* Disable transmit to prevent a second packet from attempting to go out */
    UART_C6_REG(module) &= ~UART_C6_TX709_MASK;
    
    /* Clear the PTXF flag */
    UART_S3_REG(module) = UART_S3_PTXF_MASK;    
    
    /* Make sure CDET field is 10 indicating COL during data */
    if ( (UART_S4_REG(module) & UART_S4_CDET(0x3)) != 0x08)
    {
        error++;
        printf("\nERR! S4 value is incorrect. Expected: 0x08 Read: 0x%02X", UART_S4_REG(module));
    }
    
    /* Set PTA13 low again */
    GPIOA_PDOR &= ~(1<<13); // Drive pin low    
    
    /* Write 1s to S4 to clear CDET field */
    UART_S4_REG(module) = 0xF;
    

    /* Test active high COL during line code violation */
    printf("Testing active high COL during line code violation...\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();
    
    UART_C6_REG(module) |= UART_C6_TX709_MASK;
    
    uart_putchar (module, 0xAA); 
    
    /* delay before driving PTD2 high */
    for(i=0; i < 0xA02; i++);
    
    /* Toggle PTA13 high */
    GPIOA_PDOR |= 1<<13; // Drive pin high
    
    /* Wait for transmit to complete */
    while( !(UART_S3_REG(module) & UART_S3_PTXF_MASK));

    /* Disable transmit to prevent a second packet from attempting to go out */
    UART_C6_REG(module) &= ~UART_C6_TX709_MASK;
    
    /* Clear the PTXF flag */
    UART_S3_REG(module) = UART_S3_PTXF_MASK;    
    
    /* Make sure CDET field is 11 indicating COL during line code violation */
    if ( (UART_S4_REG(module) & UART_S4_CDET(0x3)) != 0x0C)
    {
        error++;
        printf("\nERR! S4 value is incorrect. Expected: 0x08 Read: 0x%02X", UART_S4_REG(module));
    }
    
    /* Set PTA13 low again */
    GPIOA_PDOR &= ~(1<<13); // Drive pin low    
    
    /* Write 1s to S4 to clear CDET field */
    UART_S4_REG(module) = 0xF;


    /* Test active high COL - no collision */
    printf("Testing active high COL - no collision...\n");
    printf("Press any key when scope is ready.\n");
    ch = in_char();
    
    UART_C6_REG(module) |= UART_C6_TX709_MASK;
    
    uart_putchar (module, 0xAA); 
    
    /* Wait for transmit to complete */
    while( !(UART_S3_REG(module) & UART_S3_PTXF_MASK));

    /* Disable transmit to prevent a second packet from attempting to go out */
    UART_C6_REG(module) &= ~UART_C6_TX709_MASK;
    
    /* Clear the PTXF flag */
    UART_S3_REG(module) = UART_S3_PTXF_MASK;    
    
    /* Make sure CDET field is 00 indicating no collision detected */
    if ( (UART_S4_REG(module) & UART_S4_CDET(0x3)) != 0x00)
    {
        error++;
        printf("\nERR! S4 value is incorrect. Expected: 0x00 Read: 0x%02X", UART_S4_REG(module));
    }

    /* Write 1s to S4 to clear CDET field */
    UART_S4_REG(module) = 0xF;

    /* All tests complete. Return registers to default values */
    UART_C6_REG(module) = 0;   
    UART_PRE_REG(module) = 0;    
    UART_TPL_REG(module) = 0;  
        
    return error;    
} 
/********************************************************************/
