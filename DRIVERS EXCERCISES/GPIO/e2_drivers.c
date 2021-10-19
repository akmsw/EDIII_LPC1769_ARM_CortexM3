/**
 * ===============================================================================
 * Name        	:	e2_drivers.c
 * Author      	:	Bonino, Francisco Ignacio
 * Version     	: 	1.0
 * Copyright   	: 	None
 * Activity	:	Using P2.0-7, count from 0 to F in a 7-segments common cathode
 * 			display, using pull-up resistors
 * Date		:	19/10/2021 (dd/mm/yyyy)
 * ===============================================================================
 */

#include "lpc17xx.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_timer.h"

#define	OUTPUT	(uint8_t) 1
#define	INPUT	(uint8_t) 0
#define BIT(x)	(uint32_t) (1 << x)
#define PORT_2	(uint8_t) 2
#define SIZE	(uint8_t) 16

void conf_gpio(uint8_t, uint32_t, uint8_t);
void conf_timer(void);
void TIMER0_IRQHandler(void);

uint8_t index = 0;

// 0 - f in hexadecimal
uint8_t num_display[SIZE] =
{
	0x3f, 0x06, 0x5b, 0x4f,
	0x66, 0x6d, 0x7d, 0x07,
	0x7f, 0x67, 0x77, 0x7c,
	0x39, 0x5e, 0x79, 0x71
};

int main(void)
{
	// p2.0-9 configured as output gpio pins with pullup resistors
	for (uint8_t i = 0; i < 8; i++)
		conf_gpio(PORT_2, BIT(i), OUTPUT);

	FIO_HalfWordClearValue(PORT_2, 0, num_display[index]);

	conf_timer();

	while (1);

	return 0;
}

void conf_gpio(uint8_t port, uint32_t pin, uint8_t dir)
{
	PINSEL_CFG_Type conf_pin;

	conf_pin.Portnum = port;
	conf_pin.Pinnum = pin;
	conf_pin.Pinmode = PINSEL_PINMODE_PULLUP;
	conf_pin.Funcnum = 0;
	conf_pin.OpenDrain = PINSEL_PINMODE_NORMAL;

	PINSEL_ConfigPin(&conf_pin);

	GPIO_SetDir(port, pin, dir);
}

void conf_timer(void)
{
	TIM_TIMERCFG_Type struct_conf;
	TIM_MATCHCFG_Type struct_match;

	// every 100[us], the prescaler will overflow and the timer will increase by 1
	struct_conf.PrescaleOption = TIM_PRESCALE_USVAL;
	struct_conf.PrescaleValue = 100;

	// with a match value of 1000, every ~1[s] we'll get a match (100[us] * (10000 + 1) ~= 1[s])
	struct_match.MatchChannel = 0;
	struct_match.IntOnMatch = ENABLE;
	struct_match.ResetOnMatch = ENABLE;
	struct_match.StopOnMatch = DISABLE;
	struct_match.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
	struct_match.MatchValue = 10000;

	TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &struct_conf);
	TIM_ConfigMatch(LPC_TIM0, &struct_match);
	TIM_Cmd(LPC_TIM0, ENABLE);

	NVIC_EnableIRQ(TIMER0_IRQn);
}

void TIMER0_IRQHandler(void)
{
	FIO_HalfWordClearValue(PORT_2, 0, num_display[index]);

	if (index == SIZE)
		index = 0;
	else
		index++;

	FIO_HalfWordSetValue(PORT_2, 0, num_display[index]);

	TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);
}
