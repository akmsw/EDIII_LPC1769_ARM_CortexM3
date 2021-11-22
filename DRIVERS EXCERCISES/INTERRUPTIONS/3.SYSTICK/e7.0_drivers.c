/**
 * ===============================================================================
 * Name        	:	e7.0_drivers.c
 * Author      	:	Bonino, Francisco Ignacio
 * Version     	: 	1.0
 * Copyright   	: 	None
 * Description	: 	First SysTick interruptions example - Digital Electronics III
 * Activity	:	Using SysTick as interruptions source, toggle the LPC1769
 * 			integrated LED status (P0.22) every time SysTick interrupts
 * Date		:	22/11/2021 (dd/mm/yyyy)
 * ===============================================================================
 */

#include "lpc17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_systick.h"

#define OUTPUT 1
#define PORT(x) x
#define BIT(x) (1 << x)

void cfg_gpio(uint8_t, uint8_t, uint8_t);
void cfg_systick(void);

int main(void)
{
	cfg_gpio(PORT(0), BIT(22), OUTPUT);
	
	cfg_systick();
	
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

void cfg_systick(void)
{
    SYSTICK_Cmd(ENABLE);
    SYSTICK_IntCmd(ENABLE);
	SYSTICK_InternalInit(200);
}

void SysTick_Handler(void)}
{
    if (GPIO_ReadValue(PORT(0), BIT(22)))
        GPIO_ClearValue(PORT(0), BIT(22));
    else
        GPIO_SetValue(PORT(0), BIT(22));

    SYSTICK_ClearCounterFlag();
}