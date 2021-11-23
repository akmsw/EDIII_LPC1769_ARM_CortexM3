/**
 * ===============================================================================
 * Name        	:	e6.6_drivers.c
 * Author      	:	Bonino, Francisco Ignacio
 * Version     	: 	1.0
 * Copyright   	: 	None
 * Description	: 	First external interruptions exercise - Digital Electronics III
 * Activity	:	Configure EINT1 & EINT2 such that EINT1 acts as a falling-edge
 * 			interruptions source and EINT2 acts as a rising-edge
 * 			interruptions source, and consider:
 * 				- Whenever EINT1 interrupts, SysTick must interrupt every
 * 				  25[ms], which will be also the integrated LPC1769 P0.22
 * 				  LED blink period
 * 				- Whenever EINT2 interrupts, SysTick must interrupt every
 * 				  60[ms], which will be also the integrated LPC1769 P0.22
 * 				  LED blink period
 * 				- EINT1 interruptions priority is bigger than EINT2
 * 				  interruptions priority
 * Date		:	09/09/2021 (dd/mm/yyyy)
 * ===============================================================================
 */

#include "lpc17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_exti.h"
#include "lpc17xx_systick.h"

#define INPUT 0
#define OUTPUT 1
#define PORT(x) x
#define BIT(x) (1 << x)

void cfg_gpio(uint8_t, uint8_t, uint8_t);
void cfg_ext_int(uint8_t, uint8_t);
void cfg_systick(void);
void cfg_priorities(void);

int main(void)
{
    cfg_gpio(PORT(0), 22, OUTPUT);

    cfg_ext_int(PORT(2), 11);
    cfg_ext_int(PORT(2), 12);

    cfg_systick();

    cfg_priorities();

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
	
	exticfg.EXTI_Line = (pin == 11) ? EXTI_EINT1 : EXTI_EINT2;
	exticfg.EXTI_Mode = EXTI_MODE_EDGE_SENSITIVE;
	exticfg.EXTY_polarity = (pin == 11) ? EXTI_POLARITY_LOW_ACTIVE_OR_FALLING_EDGE : EXTI_POLARITY_HIGH_ACTIVE_OR_RISING_EDGE;
	
	NVIC_EnableIRQ(EINT2_IRQn);
	
	EXTI_Config(&exticfg),
}

void cfg_systick(void)
{
    SYSTICK_Cmd(ENABLE);
    SYSTICK_IntCmd(ENABLE);
}

void cfg_priorities(void)
{
    NVIC_SetPriority(EINT1_IRQn, 1);
    NVIC_SetPriority(EINT2_IRQn, 5);
}

void EINT1_IRQHandler(void)
{
    // Falta antirrebote
    SYSTICK_InternalInit(25);

    EXTI_ClearEXTIFlag(EXTI_EINT1);
}

void EINT1_IRQHandler(void)
{
    // Falta antirrebote
    SYSTICK_InternalInit(25);

    EXTI_ClearEXTIFlag(EXTI_EINT1);
}

void EINT2_IRQHandler(void)
{
    // Falta antirrebote
    SYSTICK_InternalInit(60);

    EXTI_ClearEXTIFlag(EXTI_EINT2);
}

void SysTick_Handler(void)
{
    if (GPIO_ReadValue(PORT(0), BIT(22)))
        GPIO_ClearValue(PORT(0), BIT(22));
    else
        GPIO_SetValue(PORT(0), BIT(22));

    SYSTICK_ClearCounterFlag();
}
