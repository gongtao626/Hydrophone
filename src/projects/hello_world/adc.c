#include "adc.h"

extern uint16 AD_BUFF[4];

/******************************************************************************
函数名称：void adc_spi_read( void )
功能描述：数据采集
输入参数：无
输出参数：无
返 回 值：无
调用函数：无
全局变量：无
******************************************************************************/
void adc_spi_read( void )
{
   hw_gpio_reverse(PORT_C, 10);//PORTC_10对应ADCS信号, PORTC_10输出电平状态逆转；ADCS初始为高，逆转后拉低
   AD_BUFF[0] = hw_spi_send_receive(0);
   AD_BUFF[1] = hw_spi_send_receive(0);
   AD_BUFF[2] = hw_spi_send_receive(0);
   AD_BUFF[3] = hw_spi_send_receive(0);
   hw_gpio_reverse(PORT_C, 10);//PORTC_10对应ADCS信号, PORTC_10输出电平状态逆转；ADCS升高
}

/******************************************************************************
函数名称：void adc_reset( void )
功能描述：复位adc7606
输入参数：无
输出参数：无
返 回 值：无
调用函数：无
全局变量：无
******************************************************************************/
void adc_reset( void )
{
  hw_gpio_set(PORT_D, 1, 0); //已经初始化为低电平
  time_delay_us(1);
  hw_gpio_reverse(PORT_D, 1);   //升高
  time_delay_us(1); //最短RESET高电平脉冲宽度
  hw_gpio_reverse(PORT_D, 1);   //拉低
  time_delay_us(1);
}