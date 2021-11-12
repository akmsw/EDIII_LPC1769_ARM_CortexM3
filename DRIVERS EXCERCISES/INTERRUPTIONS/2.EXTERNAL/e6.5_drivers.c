/**
 * ===============================================================================
 * Name        	:	e6.5_drivers.c
 * Author      	:	Bonino, Francisco Ignacio
 * Version     	: 	1.0
 * Copyright   	: 	None
 * Description	: 	First external interruptions example - Digital Electronics III
 * Activity	:	Using a normally open button, and considering it as an external
 * 			falling-edge interruptions source (specifically, EINT2), blink
 * 			intermittently the LPC1769 integrated LED in P0.22 changing the
 * 			blink frequency as you want every time the button is pressed
 * Date		:	26/08/2021 (dd/mm/yyyy)
 * ===============================================================================
 */

#include "lpc17xx.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_exti.h"

#define INPUT 0
#define OUTPUT 1
#define PORT(x) x
#define BIT(x) (1 << x)

uint16_t matchval;

void cfg_gpio(uint8_t, uint8_t, uint8_t);
void cfg_ext_int(uint8_t, uint8_t);
void cfg_timer(void);

int main(void)
{
	cfg_gpio(PORT(0), 22, OUTPUT);
	cfg_ext_int(PORT(2), 12);
	
	matchval = 1000;
	
	cfg_timer();
	
	while (1);
	
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

void cfg_ext_int(uint8_t, uint8_t)
{
	PINSEL_CFG_Type cfg;
	
	cfg.Portnum = port;
	cfg.Pinnunm = pin;
	cfg.Pinmode = PINSEL_PINMODE_TRISTATE;
	cfg.Funcnum = 1; // EINT
	cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	
	PINSEL_ConfigPin(&cfg);
	
	EXTI_Init();
	
	EXTI_InitTypeDef exticfg;
	
	exticfg.EXTI_Line = EXTI_EINT2;
	exticfg.EXTI_Mode = EXTI_MODE_EDGE_SENSITIVE;
	exticfg.EXTY_polarity = EXTY_POLARITY_LOW_ACTIVE_OR_FALLING_EDGE;
	
	NVIC_EnableIRQ(EINT2_IRQn);
	
	EXTI_Config(&exticfg),
}

void cfg_timer(void)
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
	mcfg.MatchValue = matchval;
	
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

void EINT2_IRQHandler(void)
{
	if (matchval == 1000)
		matchval = 500;
	else
		matchval = 1000;
	
	TIM_UpdateMatchValue(LPC_TIM0, TIM_MR0, matchval);
	TIM_RestartCounter(LPC_TIM0);
	
	EXTI_ClearEXTIFlag(EXTI_EINT2);
}