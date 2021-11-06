/**
 * ===============================================================================
 * Name        	:	e3_drivers.c
 * Author      	:	Bonino, Francisco Ignacio
 * Version     	: 	1.0
 * Copyright   	: 	None
 * Description	: 	Third exercise - Digital Electronics III
 * Activity		:	Using P0.4 as input with pull-up resistor, check its value
 * 					and:
 * 					- If it's '1', the value represented in P0.0-3 must be
 * 			 	  	added to the variable 'accumulator'
 * 			 		- If it's '0', the value represented in P0.0-3 must be
 * 			 	 	 subtracted from the variable 'accumulator'
 * Date			:	06/11/2021 (dd/mm/yyyy)
 * ===============================================================================
 */

#include "lpc17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_timer.h"

#define INPUT 0
#define OUTPUT 1
#define PORT(x) x
#define BIT(x) (1 << x)

void cfg_gpio(uint8_t, uint8_t, uint8_t);
void cfg_timer(void);

uint32_t acc;

int main(void)
{
	acc = 0;
	
	cfg_gpio(PORT(0), 4, INPUT);
	
	for (uint8_t i = 0; i < 4; i++)
		cfg_gpio(PORT(0), i, INPUT);
	
	while (1);
	
	return 0;
}

void cfg_gpio(uint8_t port, uint8_t pin, uint8_t dir)
{
	PINSEL_CFG_Type cfg;
	
	cfg.Portnum = port;
	cfg.Pinnum = pin;
	cfg.Pinmode = PINSEL_PINMODE_PULLUP;
	cfg.Funcnum = 0; // GPIO
	cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	
	PINSEL_ConfigPin(&cfg);
	
	GPIO_SetDir(port, BIT(pin), dir);
}

void cfg_timer(void)
{
	TIM_TIMERCFG_Type tcfg;
	TIM_MATCHCFG_Type mcfg;
	
	tcfg.PrescaleOption = TIM_PRESCALE_USVAL;
	tcfg.PrescaleValue = 100; // timer++ cada 100[us]
	
	mcfg.MatchChannel = 0;
	mcfg.IntOnMatch = ENABLE;
	mcfg.StopOnMatch = DISABLE;
	mcfg.ResetOnMatch = ENABLE;
	mcfg.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
	mcfg.MatchValue = 1000; // match cada 100[ms]
	
	TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &tcfg);
	TIM_ConfigMatch(LPC_TIM0, &mcfg);
	TIM_Cmd(LPC_TIM0, ENABLE);
	
	NVIC_EnableIRQ(TIMER0_IRQn);
}

void TIMER0_IRQHandler(void)
{
	if (GPIO_ReadValue(PORT(0), BIT(4)))
		acc += (FIO_HalfWordReadValue(PORT(0), 0) & 0xf);
	else
		acc -= (FIO_HalfWordReadValue(PORT(0), 0) & 0xf);
	
	TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);
}