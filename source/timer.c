/* -----------------------------------------------------------------------------
 * @file timer.c
 * @brief configures the SysTick device for timing applications
 *
 * A general purpose timing system for generating an interrupt at
 * SYSTICK_INTERRRUPT_FREQ in Hz.
 *
 * @author Jake Michael
 * @date 2020-10-12
 * @rev 1.1
 * ------------------------------------------------------------------------------
 */

#include "MKL25Z4.h"
#include "core_cm0plus.h"
#include "timer.h"

// the desired frequency for SysTick interrupts:
#define SYSTICK_INTERRUPT_FREQ  10000UL // Hz

// the clock source frequency used for SysTick timer
// valid values are 48000000 (core clock) and 3000000 (core clock/16)
#define CLOCK_SOURCE_FREQ       3000000UL  // Hz

// the following count down setpoint must fit into 24-bits when evaluated
#define SYSTICK_LOAD_SETPOINT   ((CLOCK_SOURCE_FREQ/SYSTICK_INTERRUPT_FREQ) - 1)

// use a uint32_t to represent the tickers - the maximum time will be
// the max. uint32_t value
#define TIME_MAX UINT32_MAX

// static tickers
static ticktime_t g_start_ticks = 0;
static volatile ticktime_t g_system_ticks = 0;

void SysTick_Handler() {
	// increment the g_system_ticks counter
	g_system_ticks++;
}


void systick_init() {

	// enable the SysTick interrupt
	NVIC_EnableIRQ(SysTick_IRQn);

  // disable all interrupts - CRITICAL SECTION
	uint32_t masking_state = __get_PRIMASK();
	__disable_irq();

	// set SysTick reload value
	SysTick->LOAD = SYSTICK_LOAD_SETPOINT;

	// Force re-load the load value to begin counting down
	SysTick->VAL = SYSTICK_LOAD_SETPOINT;

	// set interrupt priority
	NVIC_SetPriority(SysTick_IRQn, 3);

	/* Enables SysTick timer with:
	 * 		interrupts enabled at frequency of SYSTICK_INTERRUPT_FREQ
	 * 		CLOCK_SOURCE_FREQ = 48 MHz then uses core clock @ 48 MHz
	 * 		CLOCK_SOURCE_FREQ = 3 MHz then uses divide-by-16 clock @ 3 MHz
	 */
	if (CLOCK_SOURCE_FREQ == 3000000UL) { // source clock at 3 MHz
			SysTick->CTRL =  SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
	}
	else { // default core clock at 48 MHz
		SysTick->CTRL =  SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk |
					     SysTick_CTRL_CLKSOURCE_Msk;
	}

	// re-set the interrupt masking state to what it was before
	__set_PRIMASK(masking_state);

	// initialize timing parameters
	g_start_ticks = 0;
	g_system_ticks = 0;
}


ticktime_t now_ticks() {
	return g_system_ticks;
}


uint32_t now_us() {
	if (now_ticks() == 0)
		return 0;
	return ( now_ticks()*1000000/SYSTICK_INTERRUPT_FREQ );
}

void reset_timer() {
	g_start_ticks = g_system_ticks;
}


ticktime_t get_timer_ticks() {

	// store the time
	ticktime_t curr_ticks = g_system_ticks;

	// check to make sure we have not rolled over
	if (curr_ticks >= g_start_ticks)
		return (curr_ticks - g_start_ticks);

	// return the proper value if a roll over occurred
	return (curr_ticks + (1 + TIME_MAX - g_start_ticks) );
}

uint32_t get_timer_us() {
	return ( get_timer_ticks()*100000/SYSTICK_INTERRUPT_FREQ );
}

void delay_ms(uint16_t ms) {

	// define the number of ticks to delay for based off ms
	ticktime_t delay_ticks = (SYSTICK_INTERRUPT_FREQ*ms)/1000;

	// reset the timer
	reset_timer();

	// wait for the timer to expire
	while(get_timer_ticks() < delay_ticks) {;}
}

