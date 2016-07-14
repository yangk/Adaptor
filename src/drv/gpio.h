#ifndef _GPIO_H_
#define _GPIO_H_

#define GPIO_0			0
#define GPIO_1			1
#define GPIO_2			2
#define GPIO_3			3
#define GPIO_4			4
#define GPIO_5			5
#define GPIO_6			6
#define GPIO_7			7
#define GPIO_8			8
#define GPIO_9			9
#define GPIO_10			10
#define GPIO_11			11
#define GPIO_12			12
#define GPIO_13			13
#define GPIO_14			14
#define GPIO_15			15
#define GPIO_16			16
#define GPIO_17			17
#define GPIO_18			18
#define GPIO_19			19
#define GPIO_20			20
#define GPIO_21			21
#define GPIO_22			22
#define GPIO_23			23
#define GPIO_24			24
#define GPIO_25			25
#define GPIO_26			26
#define GPIO_27			27
#define GPIO_28			28
#define GPIO_29			29
#define GPIO_30			30
#define GPIO_31			31

#define BOOT_MODE		GPIO_0
#define GPIO_UART1TX	GPIO_17
#define GPIO_UART1RX	GPIO_18
#define GPIO_UART0TX	GPIO_20
#define GPIO_UART0RX	GPIO_21

#define TX_A_SEL		GPIO_19
#define TX_B_SEL		GPIO_28
#define TX_C_SEL		GPIO_29
#define RX_A_SEL		GPIO_30
#define RX_B_SEL		GPIO_31
#define RX_C_SEL		GPIO_0
#define RX_PHS_LED		GPIO_22

#define GPIO_SET_DIROUT(gpiox) (WR_GPIO_GPIO_CTRL((~(0x1 << gpiox)) & RD_GPIO_GPIO_CTRL()))	//gpio_pin_num output

#define GPIO_SET_DIRIN(gpiox) (WR_GPIO_GPIO_CTRL((0x1 << gpiox) | RD_GPIO_GPIO_CTRL()))	//gpio_pin_num intput

#define TX_LED GPIO_2
#define RX_LED GPIO_21

#define TXLED_ON ((0x1 << TX_LED) | RD_GPIO_GPIO_DOUT())
#define TXLED_OFF ((~(0x1 << TX_LED)) & RD_GPIO_GPIO_DOUT())

#define RXLED_ON ((0x1 << RX_LED) | RD_GPIO_GPIO_DOUT())
#define RXLED_OFF ((~(0x1 << RX_LED)) & RD_GPIO_GPIO_DOUT())

void gpio_out_high(uint32_t pin_num);

void gpio_out_low(uint32_t pin_num);

void gpio_output(uint32_t pin_num, uint8_t opt);

uint32_t gpio_input(uint32_t pin_num);

#endif

