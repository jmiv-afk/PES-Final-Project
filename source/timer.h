/* -----------------------------------------------------------------------------
 * @file timer.h
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

#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h> // for UINT32_MAX definition

typedef uint32_t ticktime_t;

/*
 * @brief  initializes the Systick timer
 *
 * Timer is initialized for timer counting interrupt at the SYSTICK_INTERRUPT_FREQ
 * using either 3 MHz or 48 MHz clock depending on CLOCK_SOURCE_FREQ, defined
 * in the .c file.
 *
 * @param  none
 * @return none
 */
void systick_init();

/*
 * @brief  returns the ticks since timer initialization
 *
 * Returns system uptime g_system_ticks, or the current count of ticks since 
 * the last call to systick_init().
 *
 * @param  none
 * @return ticktime_t the uptime g_system_ticks - number of ticks since timer 
 *         initialized
 */
ticktime_t now_ticks();

/*
 * @brief  returns the msec since timer initialization
 *
 * calls now_ticks() and applies conversion to get approximate msec
 *
 * @param  none
 * @return uint32_t the total msec (system uptime) since timer initialized
 */
uint32_t now_ms();

/*
 * @brief  resets the current timer to zero
 *
 * @param  none
 * @return none
 */
void reset_timer();

/*
 * @brief  returns the ticks since the timer was reset
 *
 * @param  none
 * @return ticktime_t the number of ticks since last call to timer_reset()
 */
ticktime_t get_timer_ticks();

/*
 * @brief  returns the msec since timer initialization
 *
 * calls get_timer_ticks() and applies conversion to get approximate msec
 *
 * @param  none
 * @return uint32_t the msec since timer initialized
 */
uint32_t get_timer_ms();

/*
 * @brief  executes a hard-spin delay in milliseconds
 *
 * Note, the minimum delay possible is defined by the timer tick frequency. Always
 * ensure the tick frequency of the timer is adequate to represent the desired
 * delay period. For example, with a 10 Hz SYSTICK_INTERRUPT_FREQ, the minimum
 * delay period is 100 ms.
 *
 * @param  ms, the number of ms to hardspin delay for
 * @return none
 */
void delay_ms(uint16_t ms);

#endif // TIMER_H_
