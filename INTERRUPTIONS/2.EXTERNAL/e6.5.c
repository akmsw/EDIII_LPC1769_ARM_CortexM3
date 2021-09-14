/**
 * ===============================================================================
 * Name        	:	e6.5.c
 * Author      	:	Bonino, Francisco Ignacio
 * Version     	: 	1.0
 * Copyright   	: 	None
 * Description	: 	First external interruptions example - Digital Electronics III
 * Activity		:	Using a normally open button, and considering it as an external
 * 					falling-edge interruptions source (specifically, EINT2), blink
 * 					intermittently the LPC1769 integrated LED in P0.22 changing the
 * 					blink frequency as you want every time the button is pressed
 * Date			:	26/08/2021 (dd/mm/yyyy)
 * ===============================================================================
 */

#include "LPC17xx.h"

#define BIT(x) (1 << x)

void conf_gpio(void);
void conf_eint(void);
void delay(void);
void EINT2_IRQHandler(void);

uint32_t timer = 0;
uint8_t inter = 0;

int main(void)
{
	conf_gpio();

	conf_eint();

	while(1)
	{
		if (inter % 2)
			timer = 6000000;
		else
			timer = 300000;

		LPC_GPIO0->FIOSET |= BIT(22); // p0.22 led on

		delay();

		LPC_GPIO0->FIOCLR |= BIT(22); // p0.22 led on

		delay();
	}

	return 0;
}

void conf_gpio(void)
{
	LPC_PINCON->PINSEL1 &= ~(0x3 << 12); // p0.22 as gpio

	LPC_GPIO0->FIODIRH |= (0x40); // p0.22 as output
}

void conf_eint(void)
{
	LPC_PINCON->PINSEL4 |= BIT(24); // p2.12 as eint2

	LPC_SC->EXTINT |= BIT(2); // eint2 interruption flag cleared
	LPC_SC->EXTMODE |= BIT(2); // edge interruptions selected
	LPC_SC->EXTPOLAR &= ~BIT(2); // falling-edge interruptions specified

	NVIC_EnableIRQ(EINT2_IRQn); // EINT2 interruptions enabled
}

void delay(void)
{
	for (uint32_t i = 0; i < timer; i++);
}

void EINT2_IRQHandler(void)
{
	inter++;

	LPC_SC->EXTINT |= BIT(2); // eint2 interruption flag cleared
}
