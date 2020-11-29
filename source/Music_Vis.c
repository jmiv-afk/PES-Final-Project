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

/*
 * @brief   Application entry point.
 */
int main(void) {

  // initialize hardware
  BOARD_InitBootPins();
  BOARD_InitBootClocks();
  BOARD_InitBootPeripherals();
  #ifdef DEBUG
    // initialize debug console
    BOARD_InitDebugConsole();
  #endif

  while(1) {


  } // end while
}
