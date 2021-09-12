/**
 * ===============================================================================
 * Name        	:	Bonino, Francisco Ignacio
 * Author      	:	Bonino, Francisco Ignacio
 * Version     	: 	1.0
 * Copyright   	: 	None
 * Description	: 	First exercise - Digital Electronics III
 * Activity		:	Blink intermittently the LPC1769 integrated LED in P0.22
 * Date			:	05/08/2021 (dd/mm/yyyy)
 * ===============================================================================
 */

#include "LPC17xx.h"

#define TIMER 6000000

void configure_pins(void);
void delay(void);

int main(void)
{
	configure_pins();

	while (1)
	{
		LPC_GPIO0->FIOSET |= (1<<22); // p0.22 led on

		delay();

		LPC_GPIO0->FIOCLR |= (1<<22); // p0.22 led off

		delay();
	}

	return 0;
}

void configure_pins(void)
{
	LPC_PINCON->PINSEL1 &= ~(0b11<<12); // load '00' to set p0.22 as gpio
	LPC_GPIO0->FIODIR |= (1<<22); // set p0.22 as output
}

void delay(void)
{
	for (uint32_t i = 0; i < TIMER; i++); // ~0.5[s] delay
}
