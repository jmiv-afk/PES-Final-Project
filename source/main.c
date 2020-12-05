/* -----------------------------------------------------------------------------
 * main.c - Application entry point
 *
 * @author  Jake Michael
 * @date    2020-11-23
 * @rev     1.2
 * -----------------------------------------------------------------------------
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL25Z4.h"
#include "fsl_debug_console.h"
#include "analog_input.h"
#include "test_dsp_analysis.h"
#include "dsp_analysis.h"
#include "spi_pixl.h"

void system_init() {
  // initialize hardware
  BOARD_InitBootPins();
  BOARD_InitBootClocks();
  BOARD_InitBootPeripherals();

#ifdef DEBUG
  // initialize debug console
  BOARD_InitDebugConsole();
#endif
  
  // initialize analog input module
  ain_init();

  // initialize the neopixels
  spi_pixl_init();
}

//#define TESTING
/*
 * @brief   Application entry point.
 */
int main(void) {

  // initialize the system
  system_init();

#ifdef TESTING
  test_dsp();
  printf("all tests passed\r\n");
#else

  uint32_t led_colors[8] = {
      0xFF0000, // red
      0xFF8000, // orange
      0xFFFF00, // yellow
      0x00FF00, // green
      0x00FFFF, // aqua
      0x0000FF, // blue
      0x7F00FF, // purple
      0xFF00FF  // pink
      };

  uint16_t *samples;
  int16_t *fft_mag;
  
  fft_peaks *temp;
  fft_peaks prev;
  fft_peaks curr;

  spi_pixl_update(&led_colors, 8);

  while(1) {

    // wait until more samples are available
    while( !ain_is_adc_samples_avail() ) {;}
    // get ADC samples from microphone 
    samples = ain_get_samples();
    // get fft magnitude (power spectrum of ADC samples)
    fft_mag = dsp_fft_mag(samples, 256);
    // find the peaks 
    dsp_find_peaks(fft_mag, &prev);

    // wait until more samples are available
    while( !ain_is_adc_samples_avail() ) {;}
    // get ADC samples from microphone 
    samples = ain_get_samples();
    // get fft magnitude (power spectrum of ADC samples)
    fft_mag = dsp_fft_mag(samples, 256);
    // find the peaks 
    dsp_find_peaks(fft_mag, &curr);
    
    spi_pixl_update(&led_colors, 8);
  }

  // will never return
  return -1;
#endif
}
