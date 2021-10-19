/**
 * ===============================================================================
 * Name        	:	e4.c
 * Author      	:	Bonino, Francisco Ignacio
 * Version     	: 	1.0
 * Copyright   	: 	None
 * Description	: 	Fourth exercise - Digital Electronics III
 * Activity	:	Considering two normally-open buttons connected to P0.0-1,
 * 			identify which button has been pressed
 * 			There should be three states:
 * 				- None of them pressed
 * 				- P0.0 pressed
 * 				- P0.1 pressed
 * 			Show the state with two LEDs connected to P0.2-3
 * Date		:	12/08/2021 (dd/mm/yyyy)
 * ===============================================================================
 */

#include "LPC17xx.h"

#define BIT(x) (1 << x)
#define TIMER 3000

void configure_pins(void);
void delay(void);

int main(void)
{
	configure_pins();

	uint8_t flag = 0;

	uint32_t curr_sample_0, curr_sample_1;
	uint32_t prev_sample_0 = LPC_GPIO0->FIOPIN & BIT(0);
	uint32_t prev_sample_1 = LPC_GPIO0->FIOPIN & BIT(1);

	while (1)
	{
		curr_sample_0 = LPC_GPIO0->FIOPIN & BIT(0);
		curr_sample_1 = LPC_GPIO0->FIOPIN & BIT(1);

		if ((curr_sample_0 != prev_sample_0) || (curr_sample_1 != prev_sample_1)) // changes detected on any button
		{
			delay(); // debouncing

			curr_sample_0 = LPC_GPIO0->FIOPIN & BIT(0); // test inputs again
			curr_sample_1 = LPC_GPIO0->FIOPIN & BIT(1);

			if (curr_sample_0 != prev_sample_0) // if button_0 changed
			{
				if (!flag) // toggle led_0 accroding to flag
				{
					LPC_GPIO0->FIOSET |= BIT(2);
					flag = 1;
				}
				else
				{
					LPC_GPIO0->FIOCLR |= BIT(2);
					flag = 0;
				}

				prev_sample_0 = curr_sample_0;
			}

			if (curr_sample_1 != prev_sample_1) // if button_1 changed
			{
				if (!flag) // toggle led_1 according to flag
				{
					LPC_GPIO0->FIOSET |= BIT(3);
					flag = 1;
				}
				else
				{
					LPC_GPIO0->FIOCLR |= BIT(3);
					flag = 0;
				}

				prev_sample_1 = curr_sample_1;
			}
		}
	}

	return 0;
}

void configure_pins(void)
{
	LPC_PINCON->PINSEL0 &= ~(0xff); // p0.0-3 as gpio

	LPC_GPIO0->FIODIR0 &= ~(0x3); // p0.0-1 as input
	LPC_GPIO0->FIODIR0 |= (0xc); // p0.2-3 as output
}

void delay(void)
{
	for (uint32_t i = 0; i < TIMER; i++); // ~1[s]
}
