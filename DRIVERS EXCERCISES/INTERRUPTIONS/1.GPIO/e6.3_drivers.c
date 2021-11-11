/**
 * ===============================================================================
 * Name        	:	e6.3_drivers.c
 * Author      	:	Bonino, Francisco Ignacio
 * Version     	: 	1.0
 * Copyright   	: 	None
 * Description	: 	First GPIO interruptions excercise - Digital Electronics III
 * Activity		:	Configure P2 so that P2.0 acts as a rising-edge interruptions
 * 					source and P2.1 acts as a falling-edge interruptions source
 * 					Consider:
 * 						- Whenever P2.0 interrupts, the program must show the
 * 						  sequence 0b010011010 on P0.0-8 for about 5[s]
 * 						- Whenever P2.1 interrupts, the program must show the
 * 				 		 sequence 0b011100110 on P2.0-8 for about 5[s]
 * 						- If there's no interruption, P0.0-8 must be high
 * Date			:	11/11/2021 (dd/mm/yyyy)
 * ===============================================================================
 */

#include "lpc17xx.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_timer.h"

#define OUTPUT 1
#define INPUT 0
#define BIT(x) (1 << x)
#define PORT(x) x
#define RISING 0
#define FALLING 1
#define LOWER 0
#define UPPER 1
#define FALSE 0
#define TRUE 1

uint8_t count;
uint8_t p2_0_ON, p2_1_ON;
uint16_t seq_1 = 0b010011010;
uint16_t seq_1 = 0b011100110;

void cfg_gpio(uint8_t, uint8_t, uint8_t);
void cfg_gpio_int(uint8_t, uint8_t, uint8_t);
void cfg_timer(void);

int maint(void)
{
	for (uint8_t i = 0; i < 2; i++)
		cfg_gpio(PORT(2), i, INPUT);
	
	for (uint8_t i = 0; i < 9; i++)
		cfg_gpio(PORT(0), i, OUTPUT);
	
	cfg_gpio_int(PORT(2), 0, RISING);
	cfg_gpio_int(PORT(2), 1, FALLING);
	
	NVIC_EnableIRQ(EINT3_IRQn);
	
	cfg_timer();
	
	FIO_HalfWordSetValue(PORT(0), LOWER, 0x1ff);
	
	count = 0;
	
	p2_0_ON = FALSE;
	p2_1_ON = FALSE;
	
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

void cfg_timer(void)
{
	TIM_TIMERCFG_Type tcfg;
	TIM_MATCHCFG_Type mcfg;
	
	tcfg.PrescaleOption = TIM_PRESCALE_USVAL;
	tcfg.PrescaleValue = 500; // TC++ every 500[us]
	
	mcfg.MatchChannel = 0;
	mcfg.IntOnMatch = ENABLE;
	mcfg.StopOnMatch = DISABLE;
	mcfg.ResetOnMatch = DISABLE;
	mcfg.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
	mcfg.MatchValue = 2000;
	
	TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &tcfg);
	TIM_ConfigMatch(LPC_TIM0, &mcfg);
	
	NVIC_EnableIRQ(TIMER0_IRQn);
}

void EINT3_IRQHandler(void)
{
	// Falta antirrebote
	if (GPIO_GetIntStatus(PORT(2), BIT(0), RISING))
	{
		p2_0_ON = TRUE;
		
		TIM_Cmd(LPC_TIM0, ENABLE);
		
		GPIO_ClearInt(PORT(2), BIT(0));
		
		FIO_HalfWordClearValue(PORT(0), LOWER, 0x1ff);
		FIO_HalfWordSetValue(PORT(0), LOWER, seq_1);
	}
	else if (GPIO_GetIntStatus(PORT(2), BIT(1), FALLING))
	{
		p2_1_ON = TRUE;
		
		TIM_Cmd(LPC_TIM0, ENABLE);
		
		GPIO_ClearInt(PORT(2), BIT(1));
		
		FIO_HalfWordClearValue(PORT(0), LOWER, 0x1ff);
		FIO_HalfWordSetValue(PORT(0), LOWER, seq_2);
	}
}

void TIMER0_IRQHandler(void)
{
	if (p2_0_ON)
	{
		if (count == 0)
		{
			count = 5;
			
			TIM_Cmd(LPC_TIM0, DISABLE);
			
			FIO_HalfWordClearValue(PORT(0), LOWER, 0x1ff);
			FIO_HalfWordSetValue(PORT(0), LOWER, 0x1ff);
			
			p2_0_ON = FALSE;
		}
		else
			count--;
		
		TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);
	}
	else if (p2_1_ON)
	{
		if (count == 0)
		{
			count = 5;
			
			TIM_Cmd(LPC_TIM0, DISABLE);
			
			FIO_HalfWordClearValue(PORT(0), LOWER, 0x1ff);
			FIO_HalfWordSetValue(PORT(0), LOWER, 0x1ff);
			
			p2_1_ON = FALSE;
		}
		else
			count--;
		
		TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);
	}
}