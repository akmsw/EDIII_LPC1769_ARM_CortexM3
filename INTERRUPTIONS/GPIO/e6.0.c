/**
 * ===============================================================================
 * Name        	:	e6.0.c
 * Author      	:	Bonino, Francisco Ignacio
 * Version     	: 	1.0
 * Copyright   	: 	None
 * Description	: 	First GPIO interruptions example - Digital Electronics III
 * Activity	:	Blink intermittently the LPC1769 integrated LED in P0.22
 * 			Change the blink frequency every time a rising-edge
 * 			interruption occurs on P0.15
 * Date		:	26/08/2021 (dd/mm/yyyy)
 * ===============================================================================
 */

#include "LPC17xx.h"

#define BIT(x) (1 << x)

void conf_gpio(void);
void conf_gpio_int(void);
void delay(uint32_t);
void EINT3_IRQHandler(void);

uint8_t inter; // interruptions counter

int main(void)
{
	conf_gpio();
	conf_gpio_int();

	uint32_t timer = 0;

	while (1)
	{
		if (inter % 2) // change the blink frequency depending on the interruption counter
			timer = 1000000;
		else
			timer = 4000000;

		LPC_GPIO0->FIOSET |= BIT(22); // p0.22 led on

		delay(timer);

		LPC_GPIO0->FIOCLR |= BIT(22); // p0.22 led off

		delay(timer);
	}

	return 0;
}

void conf_gpio(void)
{
	LPC_PINCON->PINSEL0 &= ~(0b11 << 30); // p0.15 as gpio
	LPC_PINCON->PINSEL1 &= ~(0b11 << 12); // p0.22 as gpio

	LPC_GPIO0->FIODIR |= BIT(22); // p0.22 as output
	LPC_GPIO0->FIODIR &= ~BIT(15); // p0.15 as input
}

void conf_gpio_int(void)
{
	LPC_GPIOINT->IO0IntEnR |= BIT(15); // rising-edge on p0.15 interruptions enabled
	LPC_GPIOINT->IO0IntClr |= BIT(15); // p0.15 interruption flag cleared

	NVIC_EnableIRQ(EINT3_IRQn); // EINT3 interruptions enabled
}

void delay(uint32_t timer)
{
	for (uint32_t i = 0; i < timer; i++);
}

void EINT3_IRQHandler(void)
{
	if (LPC_GPIOINT->IO0IntStatR & BIT(15)) // increment 'inter' if interruption source is p0.15
		inter++;

	LPC_GPIOINT->IO0IntClr |= BIT(15); // p0.15 interruption flag cleared
}
