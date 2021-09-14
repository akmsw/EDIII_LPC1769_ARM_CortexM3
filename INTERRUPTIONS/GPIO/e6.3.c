/**
 * ===============================================================================
 * Name        	:	e6.3.c
 * Author      	:	Bonino, Francisco Ignacio
 * Version     	: 	1.0
 * Copyright   	: 	None
 * Description	: 	First GPIO interruptions excercise - Digital Electronics III
 * Activity	:	Configure P2 so that P2.0 acts as a rising-edge interruptions
 * 			source and P2.1 acts as a falling-edge interruptions source
 * 			Consider:
 * 				- Whenever P2.0 interrupts, the program must show the
 * 				  sequence 0b010011010 on P0.0-8 for about 5[s]
 * 				- Whenever P2.1 interrupts, the program must show the
 * 				  sequence 0b011100110 on P2.0-8 for about 5[s]
 * 				- If there's no interruption, P0.0-8 must be high
 * Date		:	26/08/2021 (dd/mm/yyyy)
 * ===============================================================================
 */

#include "LPC17xx.h"

#define BIT(x) (1 << x)
#define TIMER 60000000

void conf_gpio(void);
void conf_gpio_int(void);
void delay(void);
void EINT3_IRQHandler(void);

int main(void)
{
	conf_gpio();

	conf_gpio_int();

	LPC_GPIO0->FIOPIN = (0x1FF); // p0.0-8 leds on

	while (1);

	return 0;
}

void conf_gpio(void)
{
	LPC_PINCON->PINSEL0 &= ~(0x3ffff); // p0.0-8 as gpio
	LPC_PINCON->PINSEL4 &= ~(0xf); // p2.0-1 as gpio

	LPC_GPIO0->FIODIR0 |= (0xff); // p0.0-7 as outputs
	LPC_GPIO0->FIODIR1 |= (0x1); // p0.8 as output

	LPC_GPIO2->FIODIR0 &= ~(0x3); // p2.0-1 as inputs
}

void conf_gpio_int(void)
{
	LPC_GPIOINT->IO2IntEnF |= (0x3); // falling-edge interruptions on p2.0-1 enabled
	LPC_GPIOINT->IO2IntClr |= (0x3); // p2.0-1 interruption flags cleared

	NVIC_EnableIRQ(EINT3_IRQn); // EINT3 interruptions enabled
}

void delay(void)
{
	for (uint32_t i = 0; i < TIMER; i++);
}

void EINT3_IRQHandler(void)
{
	if (LPC_GPIOINT->IO2IntStatF & BIT(0))
	{
		LPC_GPIO0->FIOPIN &= (0b010011010);

		delay();

		LPC_GPIO0->FIOPIN = (0x1FF);

		LPC_GPIOINT->IO2IntClr |= (0x1); // p2.0 interruption flag cleared
	}
	else if (LPC_GPIOINT->IO2IntStatF & BIT(1))
	{
		LPC_GPIO0->FIOPIN &= (0b011100110);

		delay();

		LPC_GPIO0->FIOPIN = (0x1FF);

		LPC_GPIOINT->IO2IntClr |= (0x2); // p2.1 interruption flag cleared
	}
}
