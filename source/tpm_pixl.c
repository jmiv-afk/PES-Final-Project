/* -----------------------------------------------------------------------------
 * tpm_pixl.c 
 *
 * @author  Jake Michael
 * @date    
 * @rev    
 * -----------------------------------------------------------------------------
 */
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "MKL25Z4.h"
#include "tpm_pixl.h"
#include "timer.h"

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
static uint16_t tpm_reset[16]; // 16 reset

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
uint8_t tpm_pixl_add_clamp(uint8_t val, int8_t adder) {
  if (val + adder > 255) {
    return 255;
  } else if (val + adder < 0) {
    return 0;
  } else {
    return val+adder;
  }
}

// see .h for more details
uint32_t tpm_pixl_dim(color_t *col_rgb, uint32_t *col_24bit, uint8_t dim_val) {

  color_t rgb;

  if (col_rgb==NULL && col_24bit==NULL) {return 0;}

  // if rgb color ptr is null, data was in 24bit format
  if (col_rgb == NULL) { 
    rgb = tpm_pixl_24bit_to_rgb(col_24bit); 
  } else {
    rgb = *col_rgb;
  }

  rgb.red = (rgb.red/dim_val);
  rgb.grn = (rgb.grn/dim_val);
  rgb.blu = (rgb.blu/dim_val);
  
  return tpm_pixl_rgb_to_24bit(&rgb);
}

// see .h for more details
void tpm_pixl_reset() {
  
  // wait while transmission completes
  /*while(!is_pixel_xmit_complete) {;}
  reset_timer();*/

  /*
  // wait while transmission completes
  while(!is_pixel_xmit_complete) {;}

  // enable source modulo increment - must be disabled after reset 
  DMA0->DMA[1].DCR |= DMA_DCR_SMOD(1); 
  
  // set reset byte count
  DMA0->DMA[1].DSR_BCR |= DMA_DSR_BCR_BCR(40);

  // setup source register as reset
  DMA0->DMA[1].SAR = DMA_SAR_SAR((uint32_t)&(tpm_reset[0]));

  // set flag 
  is_pixel_xmit_complete = false; 

  // re-enable peripheral request
  DMA0->DMA[1].DCR |= DMA_DCR_ERQ_MASK;
  */
}

// see .h for more details
int tpm_pixl_update(uint32_t *rgb_24bit_colors, uint32_t npixels) {
  
  // error case: 
  if (rgb_24bit_colors == NULL || npixels <= 0) return -1;

  //TPM1->CONTROLS[0].CnV = 0;
  //while(TPM1->CONTROLS[0].CnV) {;}
  //tpm_pixl_reset();

  int i;
  uint32_t mask;
  int tpm_idx = 0;
  uint32_t *color;
  color = rgb_24bit_colors;
  
  for (i=0; i<npixels; i++) {

    // generate tpm bytes for green first
    for (mask=0x8000; mask>0x80; mask>>=1) {
       if (*color & mask) {
         tpm_output[tpm_idx] = PIXL_1;
       } else {
         tpm_output[tpm_idx] = PIXL_0;
       }
       tpm_idx++;
    }

    // generate tpm bytes for red next
    for (mask=0x800000; mask>0x8000; mask>>=1) {
      if (*color & mask) {
        tpm_output[tpm_idx] = PIXL_1;
      } else {
        tpm_output[tpm_idx] = PIXL_0;
      }
      tpm_idx++;
    }

    // generate tpm bytes for blue next
    for (mask=0x80; mask>0; mask>>=1) {
      if (*color & mask) {
        tpm_output[tpm_idx] = PIXL_1;
      } else {
        tpm_output[tpm_idx] = PIXL_0;
      }
      tpm_idx++;
    }
  color++;
  } // end for loop over npixels

  // wait for reset to complete
  while(!is_pixel_xmit_complete) {;}

  // set byte count 
  DMA0->DMA[1].DSR_BCR |= DMA_DSR_BCR_BCR(BITS_PER_PIXEL*NUM_PIXELS*2);

  // setup source register to start at tpm_output
  DMA0->DMA[1].SAR = DMA_SAR_SAR((uint32_t)&(tpm_output[0]));

  // set flag 
  is_pixel_xmit_complete = false; 

  // re-enable timer
  //TPM1->SC |= TPM_SC_CMOD(1);

  // re-enable peripheral request
  DMA0->DMA[1].DCR |= DMA_DCR_ERQ_MASK;

  return 0;
}

// see .h for more details
void tpm_pixl_init() {

  // initialize reset array
  __builtin_memset(&tpm_reset[0], 0, 16*2);

  //systick_init();

  _init_dma1();

  _init_tpm1();

  // set xmit complete flag
  is_pixel_xmit_complete = true;

  //reset_timer();
}

// see .h for more details 
void DMA1_IRQHandler() {
  //
  TPM1->CONTROLS[0].CnV = (uint16_t) 0x0;
  // clear done flag
  DMA0->DMA[1].DSR_BCR |= DMA_DSR_BCR_DONE_MASK;
  // set flag that DMA TX complete 
  is_pixel_xmit_complete = true;
  //TPM1->SC &= ~TPM_SC_CMOD_MASK;
  // send pwm low

}


#define TPM1_CLK_INPUT_FREQ (48000000UL) // 48 MHz
// see .h for more details 
void _init_tpm1() {

  // configure clock gating for tpm0 on scgc6
  SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK; 

  // Configure TPM clock source - KL25Z datasheet sec. 12.2.3
  // use OSCERCLK - 8 MHz oscillator clock
  SIM->SOPT2 |= (SIM_SOPT2_TPMSRC(1) | SIM_SOPT2_PLLFLLSEL_MASK);
  //SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);

  // KL25Z datasheet sec. 12.2.3
  // TPM must be disabled to select prescale/counter bits:
  //   PS   - sets precaler to 2 - yields 4MHz clock
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

  // enable TPM outputs to PORTB:
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
  // ERQ   - Enable peripheral request 
  // SINC  - Enable source increment after transfer
  // SSIZE - sets source size to 8 bits 
  // DSIZE - sets destination size to 8 bits
  // D_REQ - DCR ERQ bit is cleared when BCR is depleted
  // CS    - force single read/write per request (cycle steal)
  DMA0->DMA[1].DCR = ( DMA_DCR_EINT_MASK  |
                       //DMA_DCR_ERQ_MASK   |
                       DMA_DCR_SINC_MASK  |
                       DMA_DCR_SSIZE(2)   |
                       DMA_DCR_DSIZE(2)   |
                       DMA_DCR_D_REQ_MASK |
                       DMA_DCR_CS_MASK    );
  
  // setup source register as reset
  //DMA0->DMA[1].SAR = DMA_SAR_SAR((uint32_t)&(tpm_reset[0]));

  // set destination address as pwm duty cycle for TPM1CH0 
  DMA0->DMA[1].DAR = DMA_DAR_DAR((uint32_t)&(TPM1->CONTROLS[0].CnV));

  // setup byte count
  //DMA0->DMA[1].DSR_BCR = DMA_DSR_BCR_BCR(16);
  
  // configure the interrupt upon transfer complete, priority 
  NVIC_SetPriority(DMA1_IRQn, 3);
  NVIC_ClearPendingIRQ(DMA1_IRQn);
  NVIC_EnableIRQ(DMA1_IRQn);

  // enable DMA, triggered by TPM1 xmit datasheet 3.4.8.1
  DMAMUX0->CHCFG[1] = (DMAMUX_CHCFG_SOURCE(DMA_TPM1_OVRFLW_TRIG) |
                       DMAMUX_CHCFG_ENBL_MASK);

  // clear the flag to stop any transfer
  //DMA0->DMA[1].DSR_BCR |= DMA_DSR_BCR_DONE_MASK;
}
