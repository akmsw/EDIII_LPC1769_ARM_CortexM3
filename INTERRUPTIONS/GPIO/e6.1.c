/**
 * ===============================================================================
 * Name        	:	e6.1.c
 * Author      	:	Bonino, Francisco Ignacio
 * Version     	: 	1.0
 * Copyright   	: 	None
 * Description	: 	Second GPIO interruptions example - Digital Electronics III
 * Activity	:	Consider a normally open button connected to P2.3 and a LED
 * 			connected to P0.0
 * 			Every time the button generates a falling-edge interruption,
 * 			the LED status must change (on - off)
 * Date		:	26/08/2021 (dd/mm/yyyy)
 * ===============================================================================
 */

#include "LPC17xx.h"

#define BIT(x) (1 << x)

void conf_gpio(void);
void conf_gpio_int(void);

uint8_t led_on = 0;

int main(void)
{
	conf_gpio();
	conf_gpio_int();

	while (1);

	return 0;
}

void conf_gpio(void)
{
	LPC_PINCON->PINSEL0 &= ~(0x3); // p0.0 as gpio
	LPC_PINCON->PINSEL4 &= ~(0xc0); // p2.3 as gpio

	LPC_GPIO0->FIODIR |= (0x1); // p0.0 as output

	LPC_GPIO2->FIODIR &= ~(0x8); // p2.3 as input
}

void conf_gpio_int(void)
{
	LPC_GPIOINT->IO2IntEnF |= BIT(3); // falling-edge on p2.3 interruptions enabled
	LPC_GPIOINT->IO2IntClr |= BIT(3); // p2.3 interruption flag cleared

	NVIC_EnableIRQ(EINT3_IRQn); // EINT3 interruptions enabled
}

void EINT3_IRQHandler(void)
{
	if (LPC_GPIOINT->IO2IntStatF & BIT(3)) // toggle p0.0 led status on every interruption
	{
		if (!led_on)
		{
			LPC_GPIO0->FIOSET0 |= (0x1);
			led_on = 1;
		}
		else
		{
			LPC_GPIO0->FIOCLR0 |= (0x1);
			led_on = 0;
		}
	}

	LPC_GPIOINT->IO2IntClr |= BIT(3); // p2.3 interruption flag cleared
}
