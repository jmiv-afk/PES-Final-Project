/* -----------------------------------------------------------------------------
 * tpm_pixl.c - A module that allows KL25Z to speak "neopixel"
 *
 * The neopixel (WS2812B) communication specification is implemented via TPM1
 * and DMA1. Note: KL25Z is a 3.3V board and neopixels are typically supplied
 * with 5V. Make proper hardware considerations. 
 * 
 * @author  Jake Michael
 * @date    2020-12-07
 * @rev     1.3
 * -----------------------------------------------------------------------------
 */
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "MKL25Z4.h"
#include "tpm_pixl.h"

// defines for pixels / colors
#define PIXL_0          (0x1)       // the 0 bit for tpm output
#define PIXL_1          (0x3)       // the 1 bit for tpm output
#define BITS_PER_PIXEL  (24)        // 24-bit G-R-B output for neopixels
#define RED_SHIFT       (16)
#define GRN_SHIFT       (8)
#define BLU_SHIFT       (0)
#define RED_MASK        (0xFF0000)
#define GRN_MASK        (0x00FF00)
#define BLU_MASK        (0x0000FF)

// the board pins for TPM1,CH0 peripheral:
#define TPM_PORT     (PORTA)
#define TPM_PIN      (12)
#define TPM_MUX_ALT  (3)

// the bytes that will go over tpm to drive neopixels
static uint16_t tpm_output[NUM_PIXELS*BITS_PER_PIXEL];
static uint16_t tpm_reset = 0;

// flag indicating whether tpm/dma has finished transmitting
static volatile bool is_pixel_xmit_complete;

// see .h for more details
uint32_t tpm_pixl_rgb_to_24bit(color_t* col_rgb) {

  return ((uint32_t)(col_rgb->red)<<RED_SHIFT) + 
         ((uint32_t)(col_rgb->grn)<<GRN_SHIFT) + 
         ((uint32_t)(col_rgb->blu)<<BLU_SHIFT) ;
}

// see .h for more details
color_t tpm_pixl_24bit_to_rgb(uint32_t* col_24bit) {
  
  color_t rgb_pak;
  rgb_pak.red = (uint8_t)(*col_24bit & RED_MASK)>>RED_SHIFT;
  rgb_pak.grn = (uint8_t)(*col_24bit & GRN_MASK)>>GRN_SHIFT;
  rgb_pak.blu = (uint8_t)(*col_24bit & BLU_MASK)>>BLU_SHIFT;

  return rgb_pak;
}

// see .h for more details
int tpm_pixl_update(const uint32_t *rgb_24bit_colors, uint32_t npixels) {
  
  // error case: 
  if (rgb_24bit_colors == NULL || npixels <= 0) return -1;

  uint32_t mask;
  int tpm_idx = 0;
  const uint32_t *color;
  color = rgb_24bit_colors;
  
  while ( color-rgb_24bit_colors<npixels ) {

    // generate tpm bytes for green first
    for (mask=0x8000; mask>0x80; mask>>=1) {

       tpm_output[tpm_idx] = PIXL_0;
       if (*color & mask) {
         tpm_output[tpm_idx] = PIXL_1;
       }
       tpm_idx++;
    }

    // generate tpm bytes for red next
    for (mask=0x800000; mask>0x8000; mask>>=1) {
      tpm_output[tpm_idx] = PIXL_0;
      if (*color & mask) {
        tpm_output[tpm_idx] = PIXL_1;
      }
      tpm_idx++;
    }

    // generate tpm bytes for blue next
    for (mask=0x80; mask>0; mask>>=1) {
      tpm_output[tpm_idx] = PIXL_0;
      if (*color & mask) {
        tpm_output[tpm_idx] = PIXL_1;
      }
      tpm_idx++;
    }

  color++;
  } // end for loop over npixels

  // SEND THE BITPATTERN TO LATCH COLORS:
  // wait for reset to complete
  while(!is_pixel_xmit_complete) {;}
  // enable source increment for output
  DMA0->DMA[1].DCR |= DMA_DCR_SINC_MASK;
  // set byte count 
  DMA0->DMA[1].DSR_BCR |= DMA_DSR_BCR_BCR(BITS_PER_PIXEL*NUM_PIXELS*2);
  // setup source register to start at tpm_output
  DMA0->DMA[1].SAR = DMA_SAR_SAR((uint32_t)&(tpm_output[0]));
  // set flag 
  is_pixel_xmit_complete = false; 
  // re-enable peripheral request
  DMA0->DMA[1].DCR |= DMA_DCR_ERQ_MASK;

  // SEND THE RESET PATTERN TO DISPLAY COLORS:
  // wait for reset to complete
  while(!is_pixel_xmit_complete) {;}
  // disable source increment
  DMA0->DMA[1].DCR &= ~DMA_DCR_SINC_MASK;
  // set reset byte count
  DMA0->DMA[1].DSR_BCR |= DMA_DSR_BCR_BCR(30);
  // setup source register as reset
  DMA0->DMA[1].SAR = DMA_SAR_SAR((uint32_t)&(tpm_reset));
  // set flag
  is_pixel_xmit_complete = false;
  // re-enable peripheral request
  DMA0->DMA[1].DCR |= DMA_DCR_ERQ_MASK;

  return 0;
}

// see .h for more details
void tpm_pixl_init() {

  // initialize subsystems
  _init_dma1();

  _init_tpm1();

  // set xmit complete flag
  is_pixel_xmit_complete = true;

}

// see .h for more details 
void DMA1_IRQHandler() {
  TPM1->CONTROLS[0].CnV = 0;
  // clear done flag
  DMA0->DMA[1].DSR_BCR |= DMA_DSR_BCR_DONE_MASK;
  // set flag that DMA TX complete 
  is_pixel_xmit_complete = true;
}

#define TPM1_CLK_INPUT_FREQ (48000000UL) // 48 MHz
// see .h for more details 
void _init_tpm1() {

  // configure clock gating for tpm0 on scgc6
  SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK; 

  // Configure TPM clock source - KL25Z datasheet sec. 12.2.3
  SIM->SOPT2 |= (SIM_SOPT2_TPMSRC(1) | SIM_SOPT2_PLLFLLSEL_MASK);

  // KL25Z datasheet sec. 12.2.3
  // TPM must be disabled to select prescale/counter bits:
  //   PS   - sets precaler to 16 - yields 3MHz clock
  //   DMA  - enables DMA transfers 
  TPM1->SC &= ~TPM_SC_CMOD_MASK;
  TPM1->SC |= (TPM_SC_PS(0b100) | TPM_SC_DMA_MASK );

  // Continue the TPM operation while in debug mode
  // KL25Z datasheet sec. 31.3.7
  TPM1->CONF |= TPM_CONF_DBGMODE(3);

  // set the overflow val - KL25Z datasheet sec. 31.3.3
  TPM1->MOD = 4;

  // enable edge-aligned high-true PWM mode
  TPM1->CONTROLS[0].CnSC = ( TPM_CnSC_MSB_MASK  |
                             TPM_CnSC_ELSB_MASK );
  
  TPM1->CONTROLS[0].CnV = 0;

  // enable TPM outputs to PORTA:
  //    PTA12 - TPM1CH0 - ALT3
  SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
  TPM_PORT->PCR[TPM_PIN] &= ~PORT_PCR_MUX_MASK;
  TPM_PORT->PCR[TPM_PIN] |= PORT_PCR_MUX(TPM_MUX_ALT);

  // start timer
  TPM1->SC |= TPM_SC_CMOD(1);
}

#define DMA_TPM1_OVRFLW_TRIG  (55)
// see .h for more details
void _init_dma1() {
  // enable clock gating to DMA
  SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
  SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;

  // disable during config.
  DMAMUX0->CHCFG[1] = 0;

  // see pg. 357 of datasheet
  // EINT  - Enable interrupts on transfer completion
  // SINC  - Enable source increment after transfer
  // SSIZE - sets source size to 16 bits
  // DSIZE - sets destination size to 16 bits
  // D_REQ - DCR ERQ bit is cleared when BCR is depleted
  // CS    - force single read/write per request (cycle steal)
  DMA0->DMA[1].DCR = ( DMA_DCR_EINT_MASK  |
                       DMA_DCR_SINC_MASK  |
                       DMA_DCR_SSIZE(2)   |
                       DMA_DCR_DSIZE(2)   |
                       DMA_DCR_D_REQ_MASK |
                       DMA_DCR_CS_MASK    );
  
  // set destination address as pwm duty cycle for TPM1CH0 
  DMA0->DMA[1].DAR = DMA_DAR_DAR((uint32_t)&(TPM1->CONTROLS[0].CnV));

  // configure the interrupt upon transfer complete, priority 
  NVIC_SetPriority(DMA1_IRQn, 3);
  NVIC_ClearPendingIRQ(DMA1_IRQn);
  NVIC_EnableIRQ(DMA1_IRQn);

  // enable DMA, triggered by TPM1 xmit datasheet 3.4.8.1
  DMAMUX0->CHCFG[1] = (DMAMUX_CHCFG_SOURCE(DMA_TPM1_OVRFLW_TRIG) |
                       DMAMUX_CHCFG_ENBL_MASK);

}


