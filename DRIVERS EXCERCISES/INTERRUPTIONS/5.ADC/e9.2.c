/**
 * ===============================================================================
 * Name        	:	e9.2.c
 * Author      	:	Bonino, Francisco Ignacio
 * Version     	: 	1.0
 * Copyright   	: 	None
 * Description	: 	Second ADC excercise - Digital Electronics III
 * Activity	:	Write a program that uses four ADC conversion channels in burst
			mode, considering CCLK = 100MHz and 50KHz on each channel.
 * Date		:	22/11/2021 (dd/mm/yyyy)
 * ===============================================================================
 */

/*
Cálculos:

f_muest = 20KHz >> t_conv = (1 / f_muest) = (1 / 20(10^3)) = 50[us]

Configuramos TIM0 para que haga match cada (t_conv / 2)[s] y configuramos
al ADC para que convierta por flanco de subida, teniendo así cada conversión
cada t_conv[s]*/

#include "lpc17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_timer.h"

#define PORT(x) x
#define BIT(x) (1 << x)

void cfg_gpio(void);
void cfg_adc_pin(uint8_t, uint8_t);
void cfg_adc(void);
void cfg_tim_pin(uint8_t, uint8_t);
void cfg_timer(void);

uint16_t result;

int main(void)
{
	cfg_gpio();
	cfg_adc_pin(PORT(0), 23); // AD0.0
	cfg_adc();
	cfg_tim_pin(PORT(3), 26); // MAT0.1
	cfg_timer();
	
	while (1);
	
	return 0;
}

void cfg_gpio(void)
{
	PINSEL_CFG_Type cfg;
	
	cfg.Portnum = 0;
	cfg.Funcnum = 0; // GPIO
	
	for (uint8_t i = 0; i < 12; i++) // P0.0-11
	{
		cfg.Pinnum = i;
		
		PINSEL_ConfigPin(&cfg);
	}
	
	FIO_HalfWordSetDir(PORT(0), 0, 0xfff, 1);
}
void cfg_tim_pin(uint8_t port, uint8_t pin)
{
	PINSEL_CFG_Type cfg;
	
	cfg.Portnum = port;
	cfg.Pinnum = pin;
	cfg.Funcnum = 2; // MATCH
	
	PINSEL_ConfigPin(&cfg);
}

void cfg_adc_pin(uint8_t port, uint8_t pin)
{
	PINSEL_CFG_Type cfg;
	
	cfg.Portnum = port;
	cfg.Pinnum = pin;
	cfg.Pinmode = PINSEL_PINMODE_TRISTATE;
	cfg.Funcnum = 1; // AD0.0
	
	PINSEL_ConfigPin(&cfg);
}

void cfg_adc(void)
{
	ADC_Init(LPC_ADC, 200000);
	ADC_BurstCmd(LPC_ADC, 0); // No burst
	ADC_ChannelCmd(LPC_ADC, 0);
	ADC_IntConfig(LPC_ADC, ADGINTEN, SET);
	ADC_StartCmd(LPC_ADC, ADC_START_ON_MAT01);
	ADC_EdgeStartConfig(LPC_ADC, ADC_START_ON_RISING);
	
	NVIC_EnableIRQ(ADC_IRQn);
}

void cfg_timer(void)
{
	TIM_TIMERCFG_Type tcfg;
	TIM_MATCHCFG_Type mcfg;
	
	tcfg.PrescaleOption = TIM_PRESCALE_USVAL;
	tcfg.PrescaleValue = 1; // Timer++ every 1[us]
	
	mcfg.MatchChannel = 1;
	mcfg.IntOnMatch = DISABLE;
	mcfg.ResetOnMatch = ENABLE;
	mcfg.StopOnMatch = DISABLE;
	mcfg.ExtMatchOutputType = TIM_EXTMATCH_TOGGLE;
	mcfg.MatchValue = 24; // Match every 25[us]
	
	TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &tcfg);
	TIM_ConfigMatch(LPC_TIM0, &mcfg);
	TIM_Cmd(LPC_TIM0, ENABLE);
	
	NVIC_EnableIRQ(TIMER0_IRQn);
}

void ADC_IRQHandler(void)
{
	result = ADC_ChannelGetData(LPC_ADC, 0);
	
	FIO_HalfWordClearValue(PORT(0), 0, 0xfff);
	FIO_HalfWordSetValue(PORT(0), 0, result);
	
	LPC_ADC->ADSTAT &=~ BIT(16); // Limpio flag
}