
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <xtensa/xtruntime.h>

#include "lightelf_gpio_reg.h"

#include "types.h"
#include "gpio.h"

__isr__ void isr_gpio(void *data)
{
/*
#ifdef ISR_NEST_TEST
	//delay_ms(10000);//10s
	int i;
	for(i=10;i>0;i--)
	{
		printf_s("isr_gpio  %d    \n", i);
		delay_ms(1000);
	}
#endif

	board_putchar(GET_GPIO_GPIO_EINT_STA(),0);
*/
}

/*****************************************************************************************************/
/*
	Function	:
	Description	:
	Input		:
	Output		:
	Return		:
	Notes		:
*/
/*****************************************************************************************************/
void gpio_out_high(uint32_t pin_num)
{
//	uint32_t i;
	WR_GPIO_GPIO_CTRL((~(0x1 << pin_num)) & RD_GPIO_GPIO_CTRL());	//gpio_pin_num output

	WR_GPIO_GPIO_DOUT((0x1 << pin_num) | RD_GPIO_GPIO_DOUT());		//output high

//	i = WR_GPIO_GPIO_DOUT((0x1 << pin_num) | RD_GPIO_GPIO_DOUT());
}

/*****************************************************************************************************/
/*
	Function	:
	Description	:
	Input		:
	Output		:
	Return		:
	Notes		:
*/
/*****************************************************************************************************/
void gpio_out_low(uint32_t pin_num)
{
	WR_GPIO_GPIO_CTRL((~(0x1 << pin_num)) & RD_GPIO_GPIO_CTRL());	//gpio_pin_num output

	WR_GPIO_GPIO_DOUT((~(0x1 << pin_num)) & RD_GPIO_GPIO_DOUT());	//output low

}

/*****************************************************************************************************/
/*
	Function	:
	Description	:
	Input		:
	Output		:
	Return		:
	Notes		:
*/
/*****************************************************************************************************/
void gpio_output(uint32_t pin_num, uint8_t opt)
{
	if(opt)
	{
		gpio_out_high(pin_num);
	}
	else
	{
		gpio_out_low(pin_num);
	}
}

/*****************************************************************************************************/
/*
	Function	:
	Description	:
	Input		:
	Output		:
	Return		:
	Notes		:
*/
/*****************************************************************************************************/
uint32_t gpio_input(uint32_t pin_num)
{
	uint32_t input;

	WR_GPIO_GPIO_CTRL((0x1 << pin_num) | RD_GPIO_GPIO_CTRL());		//gpio_pin_num input

	input = GET_GPIO_GPIO_DIN();

	if(input & (0x1 << pin_num))
		return 1;

	else return 0;
}
