/**
 * ===============================================================================
 * Name        	:	e5.c
 * Author      	:	Bonino, Francisco Ignacio
 * Version     	: 	1.0
 * Copyright   	: 	None
 * Description	: 	Fifth exercise - Digital Electronics III
 * Activity	:	Calculate the mobile average with the last eight inputs on
 * 			P1, considering each input as a signed int conformed by
 * 			P2.0-15
 * 			Show the 16 bits result in P0.0-11 & P0.15-18
 * 			Remember: in a mobile average calculation, the oldest input
 * 			from the 8 buffer is discarded, a new input is registered
 * 			and the calculation is performed again with this new buffer
 * 			Consider a tiny delay before getting a new input from P1
 * Date		:	12/08/2021 (dd/mm/yyyy)
 * ===============================================================================
 */

#include "LPC17xx.h"

#define ARR_SIZE 8
#define TIMER 12000000

void configure_pins(void);
void delay(void);

uint16_t calc_avg(int[ARR_SIZE]);

int main(void)
{
	int data_array[ARR_SIZE];

	uint16_t avg, avgL, avgH;

	configure_pins();

	for (uint8_t i = 0; i < ARR_SIZE; i++)
		data_array[i] = LPC_GPIO1->FIOPINL; // take the p1 first 16 bits

	avg = calc_avg(data_array);

	avgL = avg & (0xfff);
	avgH = avg & (0xf000);

	LPC_GPIO0->FIOPIN |= avgL;
	LPC_GPIO0->FIOPIN |= (avgH<<4);

	delay();

	while (1)
	{
		for (uint8_t i = 0; i < ARR_SIZE; i++)
		{
			data_array[i] = LPC_GPIO1->FIOPINL;

			avg = calc_avg(data_array);

			avgL = avg & (0xfff);
			avgH = avg & (0xf000);

			LPC_GPIO0->FIOPIN |= avgL;
			LPC_GPIO0->FIOPIN |= (avgH<<4);

			delay();
		}
	}
}

void configure_pins(void)
{
	LPC_PINCON->PINSEL0 &= (0xc0ffffff); // p0.0-11 & p0.15 as gpio
	LPC_PINCON->PINSEL1 &= (0x3f); // p.16-18 as gpio
	LPC_PINCON->PINSEL2  = 0; // p1.0-15 as gpio

	LPC_GPIO0->FIODIR |= (0x78fff); // p0.0-11 & p0.15-18 as outputs
	LPC_GPIO1->FIODIRL = 0; // p1.0-15 as inputs

	LPC_GPIO0->FIOMASK |= (0x7000); // mask p0.12-14
}

void delay(void)
{
	for (uint8_t contador = 0; contador < TIMER; contador++); // ~1[s]
}

uint16_t calc_avg(int array[ARR_SIZE])
{
	int aux = 0;

	for (uint8_t i = 0; i < ARR_SIZE; i++)
		aux += array[i];

	return aux/ARR_SIZE;
}
