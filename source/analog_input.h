/* -----------------------------------------------------------------------------
 * analog_input.h - Interface for sampling ADC0 via DMA0 at ADC_SAMPLING_FREQ  
 *
 * Single ended 16-bit analog samples are recorded from KL25Z PortC, Pin0 (PTC0) 
 *
 * @author  Jake Michael
 * @date    2020-12-07 
 * @rev     1.3
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
 * @brief  Returns the most recent buffer of ADC samples 
 *
 * Internal ping-pong buffers are used. A call to this function initiates a 
 * new ADC recording (in the background) executed via DMA. It also returns the 
 * previously recorded data via a ping-pong buffering scheme. The buffer 
 * returned is safe to modify or process until the next call to ain_get_samples()
 * at which point it will be overwritten. 
 *
 * @param  none
 * @return uint16_t*, an ADC sample buffer with length 512 samples 
 *                    NULL if adc samples are not available 
 */
uint16_t* ain_get_samples();

/*
 * @brief   Returns boolean indicating whether samples are available 
 *
 * @param   none
 * @return  bool, true  - samples available.
 *                false - samples not available.
 */
bool ain_is_adc_samples_avail();

/*
 * @brief   Initializes ADC0, DMA0, and TPM0 
 *
 * ADC0 is set up to trigger on TPM0 overflow at ADC_SAMPLING_FREQ. DMA0 is 
 * triggered on ADC0 conversion completion to move the ADC data to one of the
 * two ping pong buffers (A or B) depending on which one was previously written 
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
 * @brief   Initializes ADC0 for sampling via TPM0 overflow
 *
 * Sets up ADC0 for 16 bit single-ended (unsigned) readings from the board
 * pin PTC0 
 *
 * @param   none
 * @return  none
 */
void _init_adc0();

/*
 * @brief  Initializes DMA0 for sampling ADC0 via TPM0 overflow
 *
 * @param   none
 * @return  none
 */
void _init_dma0();

/*
 * @brief   The DMA0 IRQ handler for automatically sampling ADC0 
 *
 * @param   none
 * @return  none
 */
void DMA0_IRQHandler();

/*
 * @brief   Initializes TPM0 to overflow at ADC_SAMPLING_FREQ in Hz
 *
 * @param   none
 * @return  none
 */
void _init_tpm0();

#endif // _ANALOG_INPUT_H_
