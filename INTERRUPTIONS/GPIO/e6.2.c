/**
 * ===============================================================================
 * Name        	:	e6.2.c
 * Author      	:	Bonino, Francisco Ignacio
 * Version     	: 	1.0
 * Copyright   	: 	None
 * Description	: 	Third GPIO interruptions example - Digital Electronics III
 * Activity	:	Consider a normally open button connected to P2.9, a
 * 			7 segments common cathode display connected on P0.0-6 and
 *			a blue LED connected to P0.8
 * 			Every time the button generates a falling-edge interruption,
 * 			increment a counter variable from 0 to F
 * 			In case of reach the superior limit (F), the counter must be
 * 			reseted to 0 and the blue LED must be setted on
 * Date		:	26/08/2021 (dd/mm/yyyy)
 * ===============================================================================
 */

#include "LPC17xx.h"

#define BIT(x) (1 << x)
#define SIZE 16

void conf_gpio(void);
void conf_gpio_int(void);
void EINT3_IRQHandler(void);

uint8_t count = 0;

uint32_t num_display[SIZE] =	{	// 0 - f in hexadecimal
					0x3f, 0x06, 0x5b, 0x4f,
					0x66, 0x6d, 0x7d, 0x07,
					0x7f, 0x67, 0x77, 0x7c,
					0x39, 0x5e, 0x79, 0x71
				};

int main(void)
{
	conf_gpio();

	LPC_GPIO0->FIOPIN0 = 0;

	conf_gpio_int();

	while (1);

	return 0;
}

void conf_gpio(void)
{
	LPC_PINCON->PINSEL0 &= ~(0x3fff); // p0.0-6 as gpio
	LPC_PINCON->PINSEL1 &= ~(0x3); // p0.8 as gpio
	LPC_PINCON->PINSEL4 &= ~(0xc0000); // p2.9 as gpio

	LPC_GPIO0->FIODIR0 |= (0x7f); // p0.0-6 as output
	LPC_GPIO0->FIODIR1 |= (0x1); // p0.8 as output

	LPC_GPIO2->FIODIR &= ~(0x200); // p2.9 as input
}

void conf_gpio_int(void)
{
	LPC_GPIOINT->IO2IntEnF |= BIT(9); // p2.9 falling-edge interruptions enabled
	LPC_GPIOINT->IO2IntClr |= BIT(9); // p2.9 interruption flag cleared

	NVIC_EnableIRQ(EINT3_IRQn); // EINT3 interruptions enabled
}

void EINT3_IRQHandler(void)
{
	if (LPC_GPIOINT->IO2IntStatF & BIT(9))
	{
		if (count < SIZE)
		{
			LPC_GPIO0->FIOPIN0 = num_display[count];

			count++;
		}
		else
		{
			count = 0;

			LPC_GPIO0->FIOPIN0 = num_display[count];

			count++;

			if (!(LPC_GPIO0->FIOPIN & BIT(8)))
				LPC_GPIO0->FIOSET |= (0x100); // p0.8 led on
		}
	}

	LPC_GPIOINT->IO2IntClr |= BIT(9); // p2.9 interruption flag cleared
}
