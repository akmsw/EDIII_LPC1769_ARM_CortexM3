/**
 * ===============================================================================
 * Name        	:	Bonino, Francisco Ignacio
 * Author      	:	Bonino, Francisco Ignacio
 * Version     	: 	1.0
 * Copyright   	: 	None
 * Description	: 	Third exercise - Digital Electronics III
 * Activity	:	Using P2.0-7, count from 0 to F in a 7-segments common cathode
 * 			display, using pull-up resistors
 * Date		:	12/08/2021 (dd/mm/yyyy)
 * ===============================================================================
 */

#include "LPC17xx.h"

#define TIMER 12000000
#define SIZE 16

void configure_pins(void);
void delay(void);

int main(void)
{
	uint32_t num_display[SIZE] =	{	// 0 - f in hexadecimal
						0x3f, 0x06, 0x5b, 0x4f,
						0x66, 0x6d, 0x7d, 0x07,
						0x7f, 0x67, 0x77, 0x7c,
						0x39, 0x5e, 0x79, 0x71
					};

	configure_pins();

	while (1)
		for (uint8_t i = 0; i < SIZE; i++)
		{
			LPC_GPIO2->FIOSETL |= num_display[i]; // i-th number corresponding leds on

			delay();

			LPC_GPIO2->FIOCLRL |= num_display[i]; // i-th number corresponding leds off
		}

	return 0;
}

void configure_pins(void)
{
	LPC_PINCON->PINSEL4 &= ~(0x3fff); // p2.0-6 as gpio
	LPC_PINCON->PINMODE4 &= ~(0x3fff); // p2.0-6 with pull-up resistors

	LPC_GPIO2->FIODIRL |= (0x7f); // p2.0-6 as outputs
}

void delay(void)
{
	for (uint32_t i = 0; i < TIMER; i++); // ~1[s]
}
