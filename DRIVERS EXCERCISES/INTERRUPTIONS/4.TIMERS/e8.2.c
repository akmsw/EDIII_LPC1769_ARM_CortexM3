/**
 * ===============================================================================
 * Name        	:	e8.2.c
 * Author      	:	Bonino, Francisco Ignacio
 * Version     	: 	1.0
 * Copyright   	: 	None
 * Description	: 	Third Timers excercise - Digital Electronics III
 * Activity	:	Using Timer 0 and considering a normally open button on EINT2,
				write a program that decreases by half the P0.22 integrated LED
				blink frequency due to a modification to the MR0 register.
 * Date		:	22/11/2021 (dd/mm/yyyy)
 * ===============================================================================
 */

#include "lpc17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_exti.h"

#define INPUT 0
#define OUTPUT 1
#define RISING 0
#define FALLING 1
#define PORT(x) x
#define BIT(x) (1 << x)

void cfg_gpio(uint8_t, uint8_t, uint8_t);
void cfg_ext_int(uint8_t, uint8_t, uint8_t);
void cfg_timer(void);

uint32_t match_val;

int main(void)
{
	cfg_gpio(PORT(0), 22, OUTPUT);
	cfg_ext_int(PORT(2), 12);
	
	match_val = 1000;
	
	cfg_timer();
	
	while (1);
	
	return 0;
}

void cfg_gpio(uint8_t port, uint8_t pin, uint8_t dir)
{
    PINSEL_CFG_Type cfg;

    cfg.Pinport = port;
    cfg.Pinnum = pin;
    cfg.Pinmode = PINSEL_PINMODE_TRISTATE;
    cfg.Funcnum = 0; // GPIO
    cfg.OpenDrain = PINSEL_PINMODE_NORMAL;

    GPIO_SetDir(port, BIT(pin), dir);
}

void cfg_ext_int(uint8_t port, uint8_t pin)
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
	
	exticfg.EXTI_Line = EXTI_EINT0;
	exticfg.EXTI_Mode = EXTI_MODE_EDGE_SENSITIVE;
	exticfg.EXTI_polarity = EXTI_POLARITY_LOW_ACTIVE_OR_FALLING_EDGE;
	
	EXTI_Config(&exticfg),
	EXTI_ClearEXTIFlag(EXTI_EINT2);
	
	NVIC_EnableIRQ(EINT2_IRQn);
}

void cfg_timer(void)
{
	TIM_TIMERCFG_Type tcfg;
	TIM_MATCHCFG_Type mcfg;
	
	tcfg.PrescaleOption = TIM_PRESCALE_USVAL;
	tcfg.PrescaleValue = 100; // Timer++ every 100[us]
	
	mcfg.MatchChannel = 0;
	mcfg.IntOnMatch = ENABLE;
	mcfg.ResetOnMatch = DISABLE;
	mcfg.StopOnMatch = DISABLE;
	mcfg.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
	mcfg.MatchValue = match_val;
	
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
	
	TIM_ClearIntPending(LPC_TIM0, TIM_MR0);
}

void EINT2_IRQHandler(void)
{
	match_val *= 2;
	
	TIM_UpdateMatchValue(LPC_TIM0, 0, match_val);
	TIM_ResetCounter(LPC_TIM0);
	
	EXTI_ClearEXTIFlag(EXTI_EINT2);
}