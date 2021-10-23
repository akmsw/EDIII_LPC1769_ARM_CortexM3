/**
 * ===============================================================================
 * Name        	:	e6.1.c
 * Author      	:	Bonino, Francisco Ignacio
 * Version     	: 	1.0
 * Copyright   	: 	None
 * Activity		:	Consider a normally open button connected to P2.3 and a LED
 * 					connected to P0.0
 * 					Every time the button generates a falling-edge interruption,
 * 					the LED status must change (on - off).
 * Date			:	26/08/2021 (dd/mm/yyyy)
 * ===============================================================================
 */

#include "lpc17xx.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"

#define OUTPUT 1
#define INPUT 0
#define BIT(x) (uint32_t) (1 << x)
#define PORT(x) (uint8_t) x

void conf_gpio(uint8_t, uint32_t, uint8_t);
void conf_inter(uint8_t, uint32_t, uint8_t);
void conf_timer(void);
void EINT3_IRQHandler(void);

int main(void)
{
	conf_gpio(PORT(2), BIT(3), INPUT); // p2.3 as input gpio pin
	conf_gpio(PORT(0), BIT(0), OUTPUT); // p0.0 as output gpio pin

	GPIO_ClearValue(PORT(0), BIT(0)); // p0.0 starts off

	conf_inter(PORT(2), BIT(3), 1); // enable p2.3 gpio interruptions

	while (1);

	return 0;
}

void conf_gpio(uint8_t port, uint32_t pin, uint8_t dir)
{
	PINSEL_CFG_Type gpio_conf;

	gpio_conf.Portnum = port;
	gpio_conf.Pinnum = pin;
	gpio_conf.Funcnum = 0;
	gpio_conf.OpenDrain = PINSEL_PINMODE_NORMAL;

	PINSEL_ConfigPin(&gpio_conf);

	GPIO_SetDir(port, pin, dir);
}

void conf_inter(uint8_t port, uint32_t pin, uint8_t edge)
{
	GPIO_IntCmd(port, pin, edge);
	GPIO_ClearInt(port, pin);

	NVIC_EnableIRQ(EINT3_IRQn);
}

void EINT3_IRQHandler(void)
{
	// a button debounce algorythm should be implemented for this code to work properly
	if (!GPIO_GetIntStatus(PORT(2), BIT(3), 1) && (stat != 0))
	{
		if (GPIO_ReadValue(PORT(0)) & BIT(0))
			GPIO_ClearValue(PORT(0), BIT(0));
		else
			GPIO_SetValue(PORT(0), BIT(0));
	}
}
