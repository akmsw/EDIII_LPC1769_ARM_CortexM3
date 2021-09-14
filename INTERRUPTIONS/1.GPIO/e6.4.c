/**
 * ===============================================================================
 * Name        	:	e6.4.c
 * Author      	:	Bonino, Francisco Ignacio
 * Version     	: 	1.0
 * Copyright   	: 	None
 * Description	: 	Second GPIO interruptions excercise - Digital Electronics III
 * Activity	:	Configure P0 & P2 such as P0.0 & P2.0 as rising-edge interruptions
 * 			sources, and consider:
 * 				- Whenever P0.0 interrupts, show 0b1001 through P0.1-4
 * 				  for about 3[s]
 * 				- Whenever P2.0 interrupts, show 0b0110 through P2.1-4
 * 				  for about 3[s]
 * 				- If there's no interruption, P0.1-4 & P2.1-4 must be low
 * 				- Use different color leds for each sequence
 * Date		:	26/08/2021 (dd/mm/yyyy)
 * ===============================================================================
 */

#include "LPC17xx.h"

#define BIT(x) (1 << x)
#define TIMER 36000000

void conf_gpio(void);
void conf_gpio_int(void);
void delay(void);
void EINT3_IRQHandler(void);

int main(void)
{
	conf_gpio();

	LPC_GPIO0->FIOCLR0 = (0x1e); // p0.1-4 outputs as low
	LPC_GPIO2->FIOCLR0 = (0x1e); // p2.1-4 outputs as low

	conf_gpio_int();

	while(1);

	return 0;
}

void conf_gpio(void)
{
	LPC_PINCON->PINSEL0 &= ~(0x3ff); // p0.0-4 as gpio
	LPC_PINCON->PINSEL4 &= ~(0x3ff); // p2.0-4 as gpio

	LPC_GPIO0->FIODIR0 |= (0x1e); // p0.1-4 as output
	LPC_GPIO0->FIODIR0 &= ~(0x1); // p0.0 as input

	LPC_GPIO2->FIODIR0 |= (0x1e); // p2.1-4 as output
	LPC_GPIO2->FIODIR0 &= ~(0x1); // p2.0 as input
}

void conf_gpio_int(void)
{
	LPC_GPIOINT->IO0IntEnF |= (0x1); // rising-edge interruptions on p0.0 enabled
	LPC_GPIOINT->IO2IntEnF |= (0x1); // rising-edge interruptions on p2.0 enabled

	LPC_GPIOINT->IO0IntClr |= (0x1); // p0.0 interruption flag cleared
	LPC_GPIOINT->IO2IntClr |= (0x1); // p0.0 interruption flag cleared

	NVIC_EnableIRQ(EINT3_IRQn);
}

void delay(void)
{
	for (uint32_t i = 0; i < TIMER; i++); // ~3[s]
}

void EINT3_IRQHandler(void)
{
	// show patterns for ~3[s] and turn off leds
	if (LPC_GPIOINT->IO0IntStatF & BIT(0))
	{
		LPC_GPIO0->FIOSET0 |= (0b10010);

		delay();

		LPC_GPIO0->FIOCLR0 = (0x1e);

		LPC_GPIOINT->IO0IntClr |= (0x1);
	}
	else if (LPC_GPIOINT->IO2IntStatF & BIT(0))
	{
		LPC_GPIO2->FIOSET0 |= (0b01100);

		delay();

		LPC_GPIO2->FIOCLR0 = (0x1e);

		LPC_GPIOINT->IO2IntClr |= (0x1);
	}
}
