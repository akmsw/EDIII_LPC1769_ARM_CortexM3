/**
 * ===============================================================================
 * Name        	:	e2.c
 * Author      	:	Bonino, Francisco Ignacio
 * Version     	: 	1.0
 * Copyright   	: 	None
 * Description	: 	Second exercise - Digital Electronics III
 * Activity	:	Using P2.0-9, make two different light sequences and switch
 * 			from one to another indefinitely
 * Date		:	12/08/2021 (dd/mm/yyyy)
 * ===============================================================================
 */

#include "LPC17xx.h"

#define TIMER 3000000

void configure_pins(void);
void delay(void);
void seq_1(void);
void seq_2(void);

int main(void)
{
	configure_pins();

	while (1)
	{
		seq_1();
		seq_2();
	}

	return 0;
}

void configure_pins(void)
{
	LPC_PINCON->PINSEL4 &= ~(0xfffff); // p2.0-9 configured as gpio

	LPC_GPIO2->FIODIRL |= (0x3ff); // p2.0-9 configured as output
}

void delay()
{
	for (uint32_t i = 0; i < TIMER; i++); // ~0.25[s]
}

void seq_1(void)
{
	// p2.0-9 leds on and off one by one
	for (uint8_t i = 0; i < 10; i++)
	{
		LPC_GPIO2->FIOSETL |= (1<<i);

		delay();

		LPC_GPIO2->FIOCLRL |= (1<<i);
	}
}

void seq_2(void)
{
	// p2.0-9 leds blink
	for (uint8_t i = 0; i < 3; i++)
	{
		LPC_GPIO2->FIOSETL |= (0x3ff); // p2.0-9 leds on

		delay();

		LPC_GPIO2->FIOCLRL |= (0x3ff); // p2.0-9 leds off

		delay();
	}
}
