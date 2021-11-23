/**
 * ===============================================================================
 * Name        	:	e8.3.c
 * Author      	:	Bonino, Francisco Ignacio
 * Version     	: 	1.0
 * Copyright   	: 	None
 * Description	: 	Fourth Timers excercise - Digital Electronics III
 * Activity	:	Using Timer 0 in capture mode and considering two normally
			open buttons placed conveniently, write a program that stores the time
			in milliseconds that takes to press the two buttons at the same time
			with a single finger.
 * Date		:	22/11/2021 (dd/mm/yyyy)
 * ===============================================================================
 */

#include "lpc17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_timer.h"

#define PORT(x) x
#define BIT(x) (1 << x)

void cfg_gpio_cap(uint8_t, uint8_t);
void cfg_timer(void);
void calc_time(void);

uint32_t cap0, cap1, shooter;

int main(void)
{
	for (uint8_t i = 0; i < 28; i++)
		cfg_gpio_cap(POTR(1), i);
	
	cfg_timer();
	
	cap0 = 0;
	cap1 = 0;
	shooter = 0;
	
	while (1)
		calc_time();
	
	return 0;
}

void cfg_gpio_cap(uint8_t port, uint8_t pin)
{
	PINSEL_CFG_Type cfg;
	
	cfg.Portnum = port;
	cfg.Pinnum = pin;
	cfg.Funcnum = 3; // CAP
	
	PINSEL_ConfigPin(&cfg);
}

void cfg_timer(void)
{
	TIM_TIMERCFG_Type tcfg;
	TIM_CAPTURECFG_Type ccfg;
	
	tcfg.PrescaleOption = TIM_PRESCALE_USVAL;
	tcfg.PrescaleValue = 1000; // Timer++ every 1[ms]
	
	ccfg.FallingEdge = ENABLE;
	ccfg.RisingEdge = DISABLE;
	ccfg.IntOnCaption = ENABLE;
	
	for (uint8_t i = 0; i < 2; i++)
	{
		ccfg.CaptureChannel = i;
		
		TIM_ConfigCapture(LPC_TIM0, &ccfg);
	}
	
	TIM_Cmd(LPC_TIM0, ENABLE);
	
	NVIC_EnableIRQ(TIMER0_IRQn);
}

void TIMER0_IRQHandler(void)
{
	if (TIM_GetIntStatus(LPC_TIM0, TIM_CR0_INT))
	{
		cap0 = TIM_GetCaptureValue(LPC_TIM0, 0);
		
		TIM_ClearIntPending(LPC_TIM0, TIM_CR0_INT);
	}
	else if (TIM_GetIntStatus(LPC_TIM0, TIM_CR1_INT))
	{
		cap1 = TIM_GetCaptureValue(LPC_TIM0, 1);
		
		TIM_ClearIntPending(LPC_TIM0, TIM_CR1_INT);
	}
}

void calc_time(void)
{
	if (cap0 < cap1)
		shooter = cap1 - cap0;
	else
		shooter = cap0 - cap1;
}