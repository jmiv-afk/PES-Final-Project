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

void system_init() {
  // initialize hardware
  BOARD_InitBootPins();
  BOARD_InitBootClocks();
  BOARD_InitBootPeripherals();
#ifdef DEBUG
  // initialize debug console
  BOARD_InitDebugConsole();
#endif

  //systick_init();
  ain_init();

}

#define TESTING
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

  uint16_t *samples;
  int16_t *fft_mag;

  while(1) {
    ain_set_adc_sampling_hold_flag(true);
    samples = ain_get_prev_samples();
    fft_mag = dsp_fft_mag(samples, 256);
    for (int i=0; i<256; i++) {
      printf("%d\r\n", fft_mag[i]);
    }
    ain_set_adc_sampling_hold_flag(false);
  }

  // will never return
  return -1;
#endif
}
