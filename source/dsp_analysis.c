/* -----------------------------------------------------------------------------
 * dsp_analysis.c 
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
#include "arm_const_structs.h"
#include "arm_math.h"
#include "dsp_analysis.h"

#define NSAMPLES      (256)
#define FFT_MAG_LEN   (128)

static volatile uint16_t samples;
static arm_rfft_instance_q15 fft_q15_ctx = {0};
static volatile q15_t FFT_input[NSAMPLES];
static volatile q15_t FFT_output[2*NSAMPLES];
static volatile q15_t FFT_mag[NSAMPLES];
static volatile q15_t temp[NSAMPLES];
static const int16_t window[NSAMPLES];


// see .h for more details
void dsp_find_peaks(int16_t* fft_mag, fft_peaks* dest) {
 
  if (fft_mag==NULL) return;

  // algorithm only works for stride = 128/8 = 16 
  int stride = 16;
  int16_t peak = 0;
  int peak_idx = 0;

  for (int i=0; i<FFT_MAG_LEN; i+=stride) {
    peak = fft_mag[i];
    for (int j=0; j<stride; j++) {
      if (peak <= fft_mag[i+j]) {
        peak = fft_mag[i+j];
        peak_idx = i+j;
      }
    }
    dest->mags[i>>4] = peak;
    dest->indices[i>>4] = peak_idx;
  }
}

// see .h for more details
int16_t* dsp_fft_mag(uint16_t* samples, int nsamples) {
 
  // handle error:
  if (samples==NULL || nsamples!=NSAMPLES) return NULL;

  // normalize samples to q15_t type from uint16_t type
  for (int i=0; i<NSAMPLES; i++) {
    FFT_input[i] = (int16_t)(samples[i]-(1<<15));
    FFT_input[i] = ((q31_t)FFT_input[i]*window[i])>>15;
  }
  
  // initialize the real fft
  arm_rfft_init_q15(&fft_q15_ctx, NSAMPLES, 0, 1);
 
  // run the real fft, upscale by some bits afterwards
  // see arm_rfft_q15 at below link for more info:
  // https://www.keil.com/pack/doc/CMSIS/DSP/html/group__RealFFT.html
  arm_rfft_q15(&fft_q15_ctx, (q15_t*)FFT_input, (q15_t*)FFT_output);

  /*// RAW FFT OUTPUT FOR DEBUG:
  for (int i=0; i<NSAMPLES*2; i++) {
    printf("%d, %d\r\n", i/2, FFT_output[i]);
  }*/

  // take magnitude
  arm_cmplx_mag_squared_q15((q15_t*) FFT_output, (q15_t*) FFT_mag, NSAMPLES);

  return (int16_t*) FFT_mag;
}


// the smoothing window
static const int16_t window[NSAMPLES] = {
    0,     4,    19,    44,    79,   124,   178,   243,
    317,   401,   494,   598,   710,   833,   965,  1106,
    1256,  1416,  1585,  1762,  1949,  2144,  2348,  2561,
    2782,  3011,  3248,  3493,  3747,  4007,  4276,  4551,
    4834,  5124,  5420,  5724,  6034,  6350,  6672,  7000,
    7334,  7673,  8017,  8367,  8721,  9081,  9444,  9812,
    10184, 10559, 10938, 11321, 11706, 12094, 12485, 12878,
    13274, 13671, 14070, 14470, 14872, 15274, 15677, 16081,
    16484, 16888, 17291, 17694, 18096, 18497, 18897, 19295,
    19691, 20085, 20478, 20867, 21254, 21638, 22019, 22396,
    22770, 23140, 23505, 23867, 24223, 24575, 24923, 25265,
    25601, 25932, 26257, 26576, 26889, 27196, 27496, 27789,
    28075, 28355, 28626, 28891, 29148, 29397, 29638, 29872,
    30097, 30313, 30522, 30721, 30912, 31095, 31268, 31432,
    31587, 31733, 31869, 31997, 32114, 32222, 32321, 32409,
    32489, 32558, 32617, 32667, 32707, 32736, 32756, 32766,
    32766, 32756, 32736, 32707, 32667, 32617, 32558, 32489,
    32409, 32321, 32222, 32114, 31997, 31869, 31733, 31587,
    31432, 31268, 31095, 30912, 30721, 30522, 30313, 30097,
    29872, 29638, 29397, 29148, 28891, 28626, 28355, 28075,
    27789, 27496, 27196, 26889, 26576, 26257, 25932, 25601,
    25265, 24923, 24576, 24223, 23867, 23505, 23140, 22770,
    22396, 22019, 21638, 21254, 20867, 20478, 20085, 19691,
    19295, 18897, 18497, 18096, 17694, 17291, 16888, 16484,
    16081, 15677, 15274, 14872, 14470, 14070, 13671, 13274,
    12878, 12485, 12094, 11706, 11321, 10938, 10559, 10184,
    9812,  9444,  9081,  8721,  8367,  8017,  7673,  7334,
    7000,  6672,  6350,  6034,  5724,  5420,  5124,  4834,
    4551,  4276,  4007,  3747,  3493,  3248,  3011,  2782,
    2561,  2348,  2144,  1949,  1762,  1585,  1416,  1256,
    1106,   965,   833,   710,   598,   494,   401,   317,
    243,   178,   124,    79,    44,    19,     4,     0};
