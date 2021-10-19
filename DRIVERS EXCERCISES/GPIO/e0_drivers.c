/**
 * ===============================================================================
 * Name        	:	e0_drivers.c
 * Author      	:	Bonino, Francisco Ignacio
 * Version     	: 	1.0
 * Copyright   	: 	None
 * Activity	:	Blink intermittently the LPC1769 integrated LED in P0.22 using
 * 			drivers. The delay must be implemented with timer and
 * 			interruptions.
 * Date		:	19/10/2021 (dd/mm/yyyy)
 * ===============================================================================
 */

// Used libraries.
#include "lpc17xx.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_timer.h"

// Useful constants definitions.
#define	OUTPUT	(uint8_t) 1
#define INPUT	(uint8_t) 0
#define BIT(x)	((uint32_t)(1 << x))
#define PORT_0	(uint8_t) 0
#define PIN_22	(uint32_t) 22

// Functions prototypes.
void conf_gpio(uint8_t, uint32_t, uint8_t);
void conf_timer(void);
void TIMER0_IRQHandler(void);

/**
 * @brief The main program.
 */
int main(void)
{
	conf_gpio(PORT_0, BIT(PIN_22), OUTPUT);
	conf_timer();

	GPIO_SetValue(PORT_0, BIT(PIN_22));

	while (1);

	return 0;
}

/**
 * @brief This function configures a specific pin as gpio
 *
 * @param port The port number.
 * @param pin The pin number.
 * @param dir If the pin should be output or input.
 */
void conf_gpio(uint8_t port, uint32_t pin, uint8_t dir)
{
	PINSEL_CFG_Type pin_conf;

	// P[port].[pin] as GPIO with pull-up resistor.
	pin_conf.Portnum 	= port;
	pin_conf.Pinnum 	= pin;
	pin_conf.Pinmode 	= PINSEL_PINMODE_PULLUP;
	pin_conf.Funcnum 	= PINSEL_FUNC_0; // GPIO.
	pin_conf.OpenDrain	= PINSEL_PINMODE_NORMAL; // Not open drain.

	// Apply configuration.
	PINSEL_ConfigPin(&pin_conf);

	// Set GPIO P[port].[pin] as output.
	GPIO_SetDir(port, pin, dir);
}

void conf_timer(void)
{
	TIM_TIMERCFG_Type struct_config; // General timer configurations.
	TIM_MATCHCFG_Type struct_match; // Match configurations.

	/**
	 * Every 100[us], the prescaler will overflow and the timer will increase by 1.
	 * The prescaler load is automatically defined.
	 */
	struct_config.PrescaleOption	= TIM_PRESCALE_USVAL;
	struct_config.PrescaleValue	= 100;

	/**
	 * We'll use the match channel 0.
	 * We want to generate an interruption whenever a match occurs.
	 * We want the timer to reset whenever a match occurs.
	 * We don't want the timer to stop whenever a match occurs.
	 * We won't do anything on the timer output whenever a match occurs.
	 * With a match value of 1000, every ~100[ms] we'll get a match (100[us] * (1000 + 1) ~= 0.1[s]).
	 * Every time we calculate the match time, we have to consider the next equation:
	 * 		MatchTime = PrescaleValue * (MatchValue + 1)
	 */
	struct_match.MatchChannel	= 0;
	struct_match.IntOnMatch		= ENABLE;
	struct_match.ResetOnMatch	= ENABLE;
	struct_match.StopOnMatch	= DISABLE;
	struct_match.ExtMatchOutputType	= TIM_EXTMATCH_NOTHING;
	struct_match.MatchValue 	= 1000;

	/**
	 * Apply general & match configuration to timer 0.
	 * We'll always use the 'timer' mode for timers.
	 */
	TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &struct_config);
	TIM_ConfigMatch(LPC_TIM0, &struct_match);

	// We enable the timer.
	TIM_Cmd(LPC_TIM0, ENABLE);

	// We enable the timer interruptions.
	NVIC_EnableIRQ(TIMER0_IRQn);
}

/**
 * @brief We toggle the P0.22 value every time a match occures.
 */
void TIMER0_IRQHandler(void)
{
	if (GPIO_ReadValue(PORT_0) & BIT(PIN_22))
		GPIO_ClearValue(PORT_0, BIT(PIN_22));
	else
		GPIO_SetValue(PORT_0, BIT(PIN_22));

	// We clear the timer 0 match channel 0 interruption flag.
	TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);

	return;
}
