/* -----------------------------------------------------------------------------
 * analog_input.h - Interface for sampling ADC0 via DMA0 at ADC_SAMPLING_FREQ  
 *
 * @author  Jake Michael
 * @date    2020-11-23
 * @rev     1.2
 * -----------------------------------------------------------------------------
 */

#ifndef _ANALOG_INPUT_H_
#define _ANALOG_INPUT_H_

#include <stdint.h>

/* 
 * -----------------------------------------------------------------------------
 *    PUBLIC FUNCTIONS
 * -----------------------------------------------------------------------------
 */

/*
 * @brief   
 *
 * @param   
 *          
 * @return  
 */
uint16_t* ain_get_prev_samples();


/*
 * @brief   
 *
 * @param   
 *          
 * @return  
 */
void ain_set_adc_sampling_hold_flag(bool state);

void ain_analyze();


/*
 * @brief   Initializes ADC0, DMA0, and TPM1 
 *
 * ADC0 is set up to trigger on TPM1 overflow at ADC_SAMPLING_FREQ. DMA0 is 
 * triggered on ADC0 conversion completion to move the ADC data to either 
 * adc_samples_ping or adc_samples_pong, depending on is_acq_into_pong bool. 
 *
 * @param   none
 * @return  none
 */
void ain_init();


/* 
 * -----------------------------------------------------------------------------
 *    PRIVATE FUNCTIONS 
 * -----------------------------------------------------------------------------
 */

/*
 * @brief   Initializes ADC0 for sampling via TPM1 overflow
 *
 * Sets up ADC0 for 16 bit single-ended (unsigned) readings from the DAC 
 *
 * @param   none
 * @return  none
 */
void _init_adc0();

/*
 * @brief   
 *
 *
 * @param   none
 * @return  none
 */
void _init_dma0();

/*
 * @brief   
 *
 *
 * @param   none
 * @return  none
 */
void _restart_dma0();

/*
 * @brief   
 *
 *
 * @param   none
 * @return  none
 */
void DMA0_IRQHandler();

/*
 * @brief   Initializes TPM1 to overflow at ADC_SAMPLING_FREQ in Hz
 *
 * @param   none
 * @return  none
 */
void _init_tpm1();

#endif // _ANALOG_INPUT_H_
