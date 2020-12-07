/* -----------------------------------------------------------------------------
 * main.c - Application entry point
 *
 * @author  Jake Michael
 * @date    2020-12-07
 * @rev     1.3
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

/*
 * @brief   Application entry point.
 */
int main(void) {

  // initialize the system
  system_init();

  // run tests
  test_dsp();
  printf("all tests passed\r\n");

  uint32_t curr_led_colors[8];
  uint32_t init_led_colors[8] = {
      RED,
      PINK,
      PURPLE,
      BLUE,
      AQUA,
      GREEN,
      YELLOW,
      ORANGE
  };
  
  for (int i=0; i<8; i++) {
    curr_led_colors[i] = init_led_colors[i];
  }

  int thresh[NUM_PIXELS] = {
      4, 4, 4, 4, 4, 4, 4, 0
  };

  uint32_t bucket_indices[] = {
      0, 2, 4, 6, 10, 15, 20, 30, 255
  };

  uint16_t *samples;
  int16_t *fft_mags;
  fft_peaks curr;

  // update initial colors:
  tpm_pixl_update(&curr_led_colors, NUM_PIXELS);

  // main program loop
  while(1) {

      // wait until more samples are available
      while( !ain_is_adc_samples_avail() ) {;}
      // get ADC samples from microphone (also begins new sampling sequence)
      samples = ain_get_samples();
      // get fft magnitude (power spectrum of ADC samples)
      fft_mags = dsp_fft_mag(samples, 512);
      // find the peaks, delineate with bucket_indices
      dsp_find_peaks(fft_mags, &curr, bucket_indices);
      // loop through pixels
      for (int i=0; i<NUM_PIXELS; i++) {
        // if the peak magnitude is above some threshold
        if ( curr.mags[i] > thresh[i])  {
          curr_led_colors[i] = init_led_colors[i];
        } else {
          curr_led_colors[i] = 0x0;
        }
    }
    // update the pixels
    tpm_pixl_update(&curr_led_colors, NUM_PIXELS);
  }

  // will never return
  return -1;

}
