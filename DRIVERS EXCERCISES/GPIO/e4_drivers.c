/**
 * ===============================================================================
 * Name        	:	e4_drivers.c
 * Author      	:	Bonino, Francisco Ignacio
 * Version     	: 	1.0
 * Copyright   	: 	None
 * Description	: 	Fourth exercise - Digital Electronics III
 * Activity		:	Considering two normally-open buttons connected to P0.0-1,
 * 					identify which button has been pressed
 * 					There should be three states:
 * 						- None of them pressed
 * 						- P0.0 pressed
 * 						- P0.1 pressed
 * 					Show the state with two LEDs connected to P0.2-3
 * Date			:	06/11/2021 (dd/mm/yyyy)
 * ===============================================================================
 */

#include "lpc17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"

#define INPUT 0
#define OUTPUT 1
#define PORT(x) x
#define BIT(x) (1 << x)
#define RISING 0
#define FALLING 1

void cfg_gpio(uint8_t, uint8_t, uint8_t);
void cfg_gpio_int(void);

int main(void)
{
	for (uint8_t i = 0; i < 2; i++)
		cfg_gpio(PORT(0), i, INPUT);
	
	for (uint8_t i = 2; i < 4; i++)
		cfg_gpio(PORT(0), i, OUTPUT);
	
	cfg_gpio_int();
	
	while (1);
	
	return 0;
}

void cfg_gpio(uint8_t port, uint8_t pin, uint8_t dir)
{
	PINSEL_CFG_Type cfg;
	
	cfg.Portnum = port;
	cfg.Pinnnum = pin;
	cfg.Pinmode = PINSEL_PINMODE_TRISTATE;
	cfg.Funcnum = 0 // GPIO
	cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	
	PINSEL_ConfigPin(&cfg);
	
	GPIO_SetDir(port, BIT(pin), dir);
}

void cfg_gpio_int(void)
{
	for (uint8_t i = 0; i < 2; i++)
	{
		GPIO_IntCmd(PORT(0), BIT(i), FALLING);
		GPIO_ClearInt(PORT(0), BIT(i));
	}
	
	NVIC_EnableIRQ(EINT3_IRQn);
}

void EINT3_IRQHandler(void)
{
	if (GPIO_GetIntStatus(PORT(0), BIT(0), FALLING) == SET)
	{
		if (GPIO_ReadValue(PORT(0), BIT(2)))
			GPIO_ClearValue(PORT(0), BIT(2));
		else
			GPIO_SetValue(PORT(0), BIT(2));
		
		GPIO_ClearInt(PORT(0), BIT(0));
	}
	else if (GPIO_GetIntStatus(PORT(0), BIT(1), FALLING) == SET)
	{
		if (GPIO_ReadValue(PORT(0), BIT(3)))
			GPIO_ClearValue(PORT(0), BIT(3));
		else
			GPIO_SetValue(PORT(0), BIT(3));
		
		GPIO_ClearInt(PORT(0), BIT(1));
	}
}