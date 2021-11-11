/**
 * ===============================================================================
 * Name        	:	e6.0_drivers.c
 * Author      	:	Bonino, Francisco Ignacio
 * Version     	: 	1.0
 * Copyright   	: 	None
 * Description	: 	First GPIO interruptions example - Digital Electronics III
 * Activity		:	Blink intermittently the LPC1769 integrated LED in P0.22
 * 					Change the blink frequency every time a rising-edge
 * 					interruption occurs on P0.15
 * Date			:	11/11/2021 (dd/mm/yyyy)
 * ===============================================================================
 */

#include "lpc17xx.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_timer.h"

#define INPUT 0
#define OUTPUT 1
#define PORT(x) x
#define BIT(x) (1 << x)
#define RISING 0
#define FALLING 1

uint16_t match;

void cfg_gpio(uint8_t, uint8_t, uint8_t);
void cfg_gpio_int(uint8_t, uint8_t, uint8_t);
void cfg_timer(uint16_t);

int main(void)
{
	cfg_gpio(PORT(0), 22, OUTPUT);
	cfg_gpio(PORT(0), 15, INPUT);
	
	cfg_gpio_int(PORT(0), 15, INPUT);
	
	NVIC_EnableIRQ(EINT3_IRQn);
	
	match = 1000;
	
	cfg_timer(match);
	
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

void cfg_timer(uint16_t match)
{
	TIM_TIMERCFG_Type tcfg;
	TIM_MATCHCFG_Type mcfg;
	
	tcfg.PrescaleOption = TIM_PRESCALE_USVAL;
	tcfg.PrescaleValue = 100; // TC++ every 100[us]
	
	mcfg.MatchChannel = 0;
	mcfg.IntOnMatch = ENABLE;
	mcfg.StopOnMatch = DISABLE;
	mcfg.ResetOnMatch = DISABLE;
	mcfg.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
	mcfg.MatchValue = match;
	
	TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &tcfg);
	TIM_ConfigMatch(LPC_TIM0, &mcfg);
	TIM_Cmd(LPC_TIM0, ENABLE);
	
	NVIC_EnableIRQ(TIMER0_IRQn);
}

void TIMER0_IRQHandler(void)
{
	if (GPIO_ReadValue(PORT(0), BIT(22)))
		GPIO_ClearValue(PORT(0), BIT(22));
	else
		GPIO_SetValue(PORT(0), BIT(22));
	
	TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);
}

void EINT3_IRQHandler(void)
{
	// Falta antirrebote
	if (GPIO_GetIntStatus(PORT(0), BIT(15), RISING)
	{
		if (match == 1000)
			match = 5000;
		else
			match = 1000;
		
		TIM_UpdateMatchValue(LPC_TIM0, TIM_MR0, match);
		
		GPIO_ClearInt(PORT(0), BIT(15));
	}
}