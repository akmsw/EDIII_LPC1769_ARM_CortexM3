/**
 * ===============================================================================
 * Name        	:	Bonino, Francisco Ignacio
 * Author      	:	Bonino, Francisco Ignacio
 * Version     	: 	1.0
 * Copyright   	: 	None
 * Description	: 	Fourth exercise - Digital Electronics III
 * Activity		:	Using P0.4 as input with pull-up resistor, check its value
 * 					and:
 * 						- If it's '1', the value represented with P0.0-3 must be
 * 					 	  added to the variable 'accumulator'
 * 					 	- If it's '0', the value representer with P0.0-3 must be
 * 					 	  subtracted from the variable 'accumulator'
 * Date			:	12/08/2021 (dd/mm/yyyy)
 * ===============================================================================
 */

#include "LPC17xx.h"

#define TIMER 12000000

void configure_pins(void);
void delay(void);

int main(void)
{
	configure_pins();

	uint32_t accumulator = 0;

	while (1)
	{
		if (LPC_GPIO0->FIOPIN0 & (1<<4))
			accumlator += (LPC_GPIO0->FIOPIN0 & (0xF)); // if p0.4 is high, we add p0.0-3 to accumulator
		else
			accumlator -= (LPC_GPIO0->FIOPIN0 & (0xF)); // else, we subtract p0.0-3 from accumulator

		delay(); // we wait ~1[s] before reading p0.4 again to avoid possible overflows
	}

	return 0;
}

void configure_pins(void)
{
	LPC_PINCON->PINSEL0 &= ~(0x3ff); // p0.0-4 as gpio
	LPC_PINCON->PINMODE0 &= ~(0x3ff); // p0.0-4 with pull-up resistors

	LPC_GPIO0->FIODIR0 |= (0x1F); // p0.0-4 as outputs
}

void delay(void)
{
	for (uint32_t i = 0; i < TIMER; i++); // ~1[s]
}
