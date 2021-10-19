/**
 * ===============================================================================
 * Name        	:	e7.0.c
 * Author      	:	Bonino, Francisco Ignacio
 * Version     	: 	1.0
 * Copyright   	: 	None
 * Description	: 	First SysTick interruptions example - Digital Electronics III
 * Activity	:	Using SysTick as interruptions source, toggle the LPC1769
 * 			integrated LED status (P0.22) every time SysTick interrupts
 * Date		:	02/09/2021 (dd/mm/yyyy)
 * ===============================================================================
 */

#include "LPC17xx.h"

#define BIT(x) (1 << x)

void conf_gpio(void);
void SysTick_Handler(void);

uint8_t inter = 0;

int main(void)
{
	conf_gpio();

	if (SysTick_Config(SystemCoreClock / 10))
		return 1; // error

	while (1)
	{
		if (inter % 2)
			LPC_GPIO0->FIOSET |= BIT(22); // p0.22 led on
		else
			LPC_GPIO0->FIOCLR |= BIT(22);
	}

	return 0;
}

void conf_gpio(void)
{
	LPC_PINCON->PINSEL1 &= ~(0x3 << 12); // p0.22 as gpio

	LPC_GPIO0->FIODIR |= BIT(22); // p0.22 as output
}

void SysTick_Handler(void)
{
	inter++;

	SysTick->CTRL &= (0xffffffff); // reading CTRL register clears systick interruption flag
}
