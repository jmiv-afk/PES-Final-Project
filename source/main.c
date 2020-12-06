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
#include "tpm_pixl.h"

#define ABS(N) ((N<0)?(-N):(N))

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
  tpm_pixl_init();
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

  uint32_t init_led_colors[8] = {
      0xFF0000, // red
      0xFF0000, // red
      0xFF0000, // red
      0xFF0000, // red
      0xFF0000, // red
      0xFF0000, // red
      0xFF0000, // red
      0xFF0000  // red
      };
  
  /*
  uint32_t init_led_colors[8] = {
      0xFF0000, // red
      0xFF8000, // orange
      0xFFFF00, // yellow
      0x00FF00, // green
      0x00FFFF, // aqua
      0x0000FF, // blue
      0x7F00FF, // purple
      0xFF00FF  // pink
      };
*/
  uint32_t *curr_led_colors;
  uint8_t dimmer[8] = {0,0,0,0,0,0,0,0};
  int thresh = 5;

  uint16_t *samples;
  int16_t *fft_mags;

  fft_peaks *fftpeaks_tmp; 
  fft_peaks prev;
  fft_peaks curr;
  
  tpm_pixl_update(&init_led_colors, NUM_PIXELS);
  
  curr_led_colors = init_led_colors;

  while(1) {

    // wait until more samples are available
    while( !ain_is_adc_samples_avail() ) {;}
    // get ADC samples from microphone 
    samples = ain_get_samples();
    // get fft magsnitude (power spectrum of ADC samples)
    fft_mags = dsp_fft_mag(samples, 256);
    // find the peaks 
    dsp_find_peaks(fft_mags, &prev);

    while(1) {
      // wait until more samples are available
      while( !ain_is_adc_samples_avail() ) {;}
      // get ADC samples from microphone 
      samples = ain_get_samples();
      // get fft magsnitude (power spectrum of ADC samples)
      fft_mags = dsp_fft_mag(samples, 256);
      // find the peaks 
      dsp_find_peaks(fft_mags, &curr);
      
      for (int i=0; i<NUM_PIXELS; i++) {
        if ( ABS(curr.mags[i] - prev.mags[i]) > thresh && 
             curr.mags[i] > prev.mags[i] ) {
          // current peak mags is greater than previous and above threshold
          dimmer[i] = (dimmer[i]+1);
        } else if ( ABS(curr.mags[i] - prev.mags[i]) > thresh && 
            curr.mags[i] < prev.mags[i]) {
          // current peak mags is less than previous and above threshold
          dimmer[i] = (dimmer[i]-1);
        }
        curr_led_colors[i] = tpm_pixl_dim(NULL, &init_led_colors[i], dimmer[i]);
      }

      tpm_pixl_update(&curr_led_colors, NUM_PIXELS);
      prev = curr;
    }
  }

  // will never return
  return -1;
#endif
}
