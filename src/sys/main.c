/*************************************************************************************************/
/*
Copyright:		QingDao Eastsoft Communication Technology Co.,Ltd.
File Name:
Description:
Author:			Xzw
Version:		v1.0
Date:			2016/03/29
History:
*/
/************************************************************************************************/

#include "include.h"
#include "sec.h"
#include "uart.h"
#include "adaptor.h"

static void uart1_check(void)
{
    uint32_t len;
    uint8_t buf[0x200];

    len = esuart_peak_data(UART_CHN_1, buf, sizeof(buf));
    if (len > 0) 
    {
        len = adaptor_on_uart1_received(buf, len);
    }
    if (len > 0) 
    {
        esuart_read_data(UART_CHN_1, buf, len);
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
int __attribute__((section(".secstart.text"))) main(int argc, char *argv[])
{
	START_INIT();

    adaptor_init();
	while(1)
	{
        uart1_check();
        delay10ms(1);
	}
}
