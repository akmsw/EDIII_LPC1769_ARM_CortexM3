/**
 * ===============================================================================
 * Name        	:	e6.2_drivers.c
 * Author      	:	Bonino, Francisco Ignacio
 * Version     	: 	1.0
 * Copyright   	: 	None
 * Description	: 	Third GPIO interruptions example - Digital Electronics III
 * Activity		:	Consider a normally open button connected to P2.9, a
 * 					7 segments common cathode display connected on P0.0-6 and
 *					a blue LED connected to P0.8
 * 					Every time the button generates a falling-edge interruption,
 * 					increment a counter variable from 0 to F and show the number
 *					on the 7 segments display
 * 					In case of reach the superior limit (F), the counter must be
 * 					reseted to 0 and the blue LED must be setted on
 * Date			:	11/11/2021 (dd/mm/yyyy)
 * ===============================================================================
 */

#include "lpc17xx.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"

#define OUTPUT 1
#define INPUT 0
#define BIT(x) (1 << x)
#define PORT(x) x
#define RISING 0
#define FALLING 1
#define LOWER 0
#define UPPER 1
#define SIZE 16

uint8_t index;
uint8_t nums[SIZE] =
{
	0x06, 0x5b, 0x4f, 0x77, // 1 2 3 A
	0x66, 0x6d, 0x7d, 0x7c, // 4 5 6 B
	0x07, 0x7f, 0x67, 0x39, // 7 8 9 C
	0x79, 0x3f, 0x71, 0x5E  // E 0 F D
}

void cfg_gpio(uint8_t, uint8_t, uint8_t);
void cfg_gpio_int(uint8_t, uint8_t, uint8_t);

int main(void)
{
	cfg_gpio(PORT(2), 9, INPUT);
	cfg_gpio(PORT(0), 8, OUTPUT);
	
	for (uint8_t i = 0; i < 7; i++)
		cfg_gpio(PORT(0), i, OUTPUT);
	
	cfg_gpio_int(PORT(2), 9, FALLING);
	
	NVIC_EnableIRQ(EINT3_IRQn);
	
	FIO_HalfWordClearValue(PORT(0), LOWER, 0x7f);
	
	while(1);
	
	return 0;
}

void cfg_gpio(uint8_t port, uint8_t pin, uint8_t dir)
{
	PINSEL_CFG_Type cfg;
	
	cfg.Portnum = port;
	cfg.Pinnunm = pin;
	cfg.Pinmode = PINSEL_PINMODE_TRISTATE;
	cfg.Funcnum = 0; // GPIO
	cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	
	PINSEL_ConfigPin(&cfg);
	
	GPIO_SetDir(port, BIT(pin), dir);
}

void cfg_gpio_int(uint8_t port, uint8_t pin, uint8_t edge)
{
	GPIO_IntCmd(port, BIT(pin), edge);
	GPIO_ClearInt(port, BIT(pin));
}

void EINT3_IRQHandler(void)
{
	//Falta antirrebote
	if (GPIO_GetIntStatus(PORT(2), BIT(9), FALLING) == SET)
	{
		FIO_HalfWordClearValue(PORT(0), LOWER, 0x7f);
		FIO_HalfWordSetValue(PORT(0), LOWER, nums[index]);
		
		if (index == SIZE)
		{
			index = 0;
			
			if (!GPIO_ReadValue(PORT(0), BIT(8)))
				GPIO_SetValue(PORT(0), BIT(8));
		}
		else
			index++;
		
		GPIO_ClearInt(PORT(2), BIT(9));
	}
}