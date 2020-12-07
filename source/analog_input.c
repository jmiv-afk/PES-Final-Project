/* -----------------------------------------------------------------------------
 * analog_input.c - Interface for sampling ADC0 via DMA0 at ADC_SAMPLING_FREQ  
 *
 * Single ended 16-bit analog samples are recorded from KL25Z PortC, Pin0 (PTC0) 
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
#include "analog_input.h"

#define START_CRITICAL_SECTION \
          uint32_t masking_state = __get_PRIMASK(); \
          __disable_irq()

#define END_CRITICAL_SECTION \
          __set_PRIMASK(masking_state)

#define ADC_SAMPLING_FREQ  (48000U) // in Hz
#define ADC_MAX_SAMPLES    (512)  


// use a ping-pong buffer approach
static uint16_t adc_samplesA[ADC_MAX_SAMPLES];
static uint16_t adc_samplesB[ADC_MAX_SAMPLES];
// the status flags
static volatile bool is_adc_samplesA_recording;
static volatile bool is_adc_samples_avail;

// see .h for more details
void ain_init() {

  // init dma0, tpm0, adc0
  _init_dma0();
  _init_tpm0();
  _init_adc0();

  // samplesA is recording first according to DMA config
  is_adc_samplesA_recording = true;
  is_adc_samples_avail = false;
  
  // turn on the TPM0 timer - to kick DMA0
  TPM0->SC |= TPM_SC_CMOD(1);
}


// see .h for more details
bool ain_is_adc_samples_avail() {
  return is_adc_samples_avail;
}


// see .h for more details
uint16_t* ain_get_samples() {

  // if samples are not available, we are still recording
  // so return NULL
  if (!is_adc_samples_avail) {
    return NULL;
  }
  
  // the return buffer:
  uint16_t* process_buffer_return;

  START_CRITICAL_SECTION;

  // if samplesA was previously recording, return samplesA
  if (is_adc_samplesA_recording) {
    process_buffer_return = adc_samplesA;
    // setup DMA destination
    DMA0->DMA[0].DAR = DMA_DAR_DAR((uint32_t) (&(adc_samplesB[0])));
    is_adc_samplesA_recording = false;

  // if samplesB was previously recording, return samplesB
  } else {
    process_buffer_return = adc_samplesB;
    // setup DMA destination
    DMA0->DMA[0].DAR = DMA_DAR_DAR((uint32_t) (&(adc_samplesA[0])));
    is_adc_samplesA_recording = true;
  }
  
  // need to record a new buffer before more samples become avail
  is_adc_samples_avail = false;

  // re-start DMA0
  DMA0->DMA[0].DSR_BCR |= DMA_DSR_BCR_BCR(2*ADC_MAX_SAMPLES);
  DMA0->DMA[0].DCR |= DMA_DCR_ERQ_MASK;

  END_CRITICAL_SECTION;

  // return buffer for processing
  return process_buffer_return;
}

// see .h for more details 
void DMA0_IRQHandler() {
  // clear done flag
  DMA0->DMA[0].DSR_BCR |= DMA_DSR_BCR_DONE_MASK;
  // samples are now available
  is_adc_samples_avail = true;
}

// see .h for more details
void _init_adc0() {
  // enable clock gating
  SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;
  // enable alternate trigger pg. 201
  // select TPM0 as trigger
  SIM->SOPT7  = SIM_SOPT7_ADC0ALTTRGEN(1) | SIM_SOPT7_ADC0TRGSEL(9);

  // pg. 466 datasheet
  // use bus clock (24 MHz) and divide by 4
  // sets to 16-bit single ended conversion
  // note: input clock must be between 2 and 12 MHz for 16-bit mode
  ADC0->CFG1  = ADC_CFG1_ADICLK(0) | ADC_CFG1_ADIV(2)  |
                ADC_CFG1_MODE(3)   | ADC_CFG1_ADLPC(0) |
                ADC_CFG1_ADLSMP(0);

  ADC0->CFG2 = 0;

  // select reference voltages
  ADC0->SC2 = ADC_SC2_REFSEL(0) |  ADC_SC2_ADTRG(0) |
              ADC_SC2_ACFE(0)   |  ADC_SC2_DMAEN(0);

  // set adc0 input to SE14
  // AIEN and DIFF
  ADC0->SC1[0] = ADC_SC1_ADCH(14) | ADC_SC1_AIEN(0) | ADC_SC1_DIFF(0);

  // run calibration datasheet sec 28.4.6:
  // turn on averaging (32) temporarily for calibration sequence
  ADC0->SC3 |= ADC_SC3_CAL_MASK | ADC_SC3_AVGS(3);
  // start calibration
  while(!(ADC0->SC1[0] & ADC_SC1_COCO_MASK)) {;} // wait for calibration complete

  // plus-side calibration
  uint16_t cal = 0;
  cal = ADC0->CLP0+ADC0->CLP1+ADC0->CLP2+ADC0->CLP3+ADC0->CLP4+ADC0->CLPS;
  cal = cal/2;
  cal |= (1<<15); // set MSB
  ADC0->PG = cal; // write calibration

  // minus-side calibration
  cal = 0;
  cal = ADC0->CLM0+ADC0->CLM1+ADC0->CLM2+ADC0->CLM3+ADC0->CLM4+ADC0->CLMS;
  cal = cal/2;
  cal |= (1<<15); // set MSB
  ADC0->MG = cal; // write calibration

  // disable averaging
  ADC0->SC3 &= ~(ADC_SC3_AVGS_MASK);
  // re-enable hardware triggering
  // enable dma request
  ADC0->SC2 |= ADC_SC2_ADTRG(1) | ADC_SC2_DMAEN(1);
}

#define DMA_ADC0_COCO_TRIG  (40)
// see .h for more details
void _init_dma0() {
  // enable clock gating to DMA
  SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
  SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;

  // disable during config.
  DMAMUX0->CHCFG[0] = 0;

  // see pg. 357 of datasheet
  // EINT  - Enable interrupts on transfer completion
  // ERQ   - Enable peripheral request (from ADC0)
  // DINC  - Enable destination increment after transfer
  // SSIZE - sets source size to 16 bits (from ADC0)
  // DSIZE - sets destination size to 16 bits
  // D_REQ - DCR ERQ bit is cleared when BCR is depleted
  // CS    - force single read/write per request (cycle steal)
  DMA0->DMA[0].DCR = ( DMA_DCR_EINT_MASK  |
                       DMA_DCR_ERQ_MASK   |
                       DMA_DCR_DINC_MASK  |
                       DMA_DCR_SSIZE(2)   |
                       DMA_DCR_DSIZE(2)   |
                       DMA_DCR_D_REQ_MASK |
                       DMA_DCR_CS_MASK    );

  // setup source from adc0, dest to adc_samples
  DMA0->DMA[0].SAR = DMA_SAR_SAR((uint32_t)&(ADC0->R[0]));
  DMA0->DMA[0].DAR = DMA_DAR_DAR((uint32_t)&(adc_samplesA[0]));
  // load BCR with ADC_MAX_SAMPLES*2 bytes (16-bits) per transfer 
  DMA0->DMA[0].DSR_BCR |= DMA_DSR_BCR_BCR(2*ADC_MAX_SAMPLES);
  
  // configure the interrupt upon transfer complete, priority 
  NVIC_SetPriority(DMA0_IRQn, 2);
  NVIC_ClearPendingIRQ(DMA0_IRQn);
  NVIC_EnableIRQ(DMA0_IRQn);

  // turn on the DMA, triggered by ADC0 conversion complete datasheet 3.4.8.1
  DMAMUX0->CHCFG[0] = (DMAMUX_CHCFG_SOURCE(DMA_ADC0_COCO_TRIG) |
                       DMAMUX_CHCFG_ENBL_MASK);
}


#define TPM0_CLK_INPUT_FREQ (48000000UL) // 48 MHz
// see .h for more details 
void _init_tpm0() {

  // configure clock gating for tpm0 on scgc6
  SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK; 
  // Configure TPM clock source - KL25Z datasheet sec. 12.2.3
  SIM->SOPT2 |= (SIM_SOPT2_TPMSRC(1) | SIM_SOPT2_PLLFLLSEL(1));
  // set TPM count direction to up with prescaler
  // KL25Z datasheet sec. 12.2.3
  // TPM must be disabled to select prescale/counter bits:
  TPM0->SC &= ~TPM_SC_CMOD_MASK;
  TPM0->SC |= TPM_SC_PS(0) | TPM_SC_CPWMS(0);
  // Continue the TPM operation while in debug mode
  // KL25Z datasheet sec. 31.3.7
  TPM0->CONF |= TPM_CONF_DBGMODE(0b11);
  // set the overflow - KL25Z datasheet sec. 31.3.3
  TPM0->MOD = TPM0_CLK_INPUT_FREQ/ADC_SAMPLING_FREQ - 1;
  // clear counter 
  TPM0->CNT = 0;
  // note: TPM0 will be started when reading samples
}
