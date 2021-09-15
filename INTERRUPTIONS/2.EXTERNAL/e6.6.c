/**
 * ===============================================================================
 * Name        	:	e6.6.c
 * Author      	:	Bonino, Francisco Ignacio
 * Version     	: 	1.0
 * Copyright   	: 	None
 * Description	: 	First external interruptions exercise - Digital Electronics III
 * Activity		:	Configure EINT1 & EINT2 such that EINT1 acts as a falling-edge
 * 					interruptions source and EINT2 acts as a rising-edge
 * 					interruptions source, and consider:
 * 						- Whenever EINT1 interrupts, SysTick must interrupt every
 * 						  25[ms], which will be also the integrated LPC1769 P0.22
 * 						  LED blink period
 * 						- Whenever EINT2 interrupts, SysTick must interrupt every
 * 						  60[ms], which will be also the integrated LPC1769 P0.22
 * 						  LED blink period
 * 						- EINT1 interruptions priority is bigger than EINT2
 * 						  interruptions priority
 * Date			:	09/09/2021 (dd/mm/yyyy)
 * ===============================================================================
 */

#include "LPC17xx.h"

#define BIT(x) (1 << x)

void conf_gpio(void);
void conf_eint(void);
void EINT1_IRQHandler(void);
void EINT2_IRQHandler(void);
void SysTick_Handler(void);

uint8_t err_f = 0;
uint8_t inter = 0;
uint8_t timer = 0;

int main(void)
{
	conf_gpio();
	conf_eint();

	while (1)
	{
		if (err_f)
			return 1; // systick error

		if (inter % 2)
			LPC_GPIO0->FIOSET |= BIT(22); // p0.22 led on
		else
			LPC_GPIO0->FIOCLR |= BIT(22); // p0.22 led off
	}

	return 0;
}

void conf_gpio(void)
{
	LPC_PINCON->PINSEL1 &= ~(0x3 << 12); // p0.22 as gpio

	LPC_GPIO0->FIODIR |= BIT(22); // p0.22 as output
}

void conf_eint(void)
{
	LPC_PINCON->PINSEL4 |= BIT(22); // p2.11 as eint1
	LPC_PINCON->PINSEL4 &= ~BIT(23);

	LPC_PINCON->PINSEL4 |= BIT(24); // p2.12 as eint2
	LPC_PINCON->PINSEL4 &= ~BIT(25);

	LPC_SC->EXTINT |= (BIT(11) | BIT(12)); // p2.11-12 interruptions flags cleared
	LPC_SC->EXTMODE |= (BIT(1) | BIT(2)); // both eint are edge-sensitive
	LPC_SC->EXTPOLAR |= BIT(1); // eint1 is falling-edge sensitive
	LPC_SC->EXTPOLAR &= ~BIT(2); // eint2 is rising-edge sensitive

	NVIC_EnableIRQ(EINT1_IRQn); // eint1 & eint2 interruptions enabled
	NVIC_EnableIRQ(EINT2_IRQn);

	NVIC_SetPriority(EINT1_IRQn, 1); // randomly chosen values to set eint1 priority higher than eint2
	NVIC_SetPriority(EINT2_IRQn, 6);
}

void EINT1_IRQHandler(void)
{
	if (SysTick_Config(2500000)) // 2500000 to get 25[ms]
		err_f = 1; // set error flag to 1

	LPC_SC->EXTINT |= BIT(1); // eint1 interruption flag cleared
}

void EINT2_IRQHandler(void)
{
	if (SysTick_Config(6000000)) // 6000000 to get 60[ms]
		err_f = 1; // set error flag to 1

	LPC_SC->EXTINT |= BIT(2); // eint1 interruption flag cleared
}

void SysTick_Handler(void)
{
	inter++;

	SysTick->CTRL &= (0xFFFFFFFF);
}
