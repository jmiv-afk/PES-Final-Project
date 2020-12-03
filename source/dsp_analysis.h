/* -----------------------------------------------------------------------------
 *  
 *
 * @author  Jake Michael
 * @date    
 * @rev    
 * -----------------------------------------------------------------------------
 */
#include <stdint.h>

#ifndef _DSP_ANALYSIS_H_
#define _DSP_ANALYSIS_H_

// do not change NBUCKETS define, algorithm works assuming 8 buckets
#define NBUCKETS  (8)

typedef struct {
  int indices[NBUCKETS];
  int16_t mags[NBUCKETS];
} fft_peaks;

int16_t* dsp_fft_mag(uint16_t* samples, int nsamples);

// assumes length of 128 and 8 buckets
void dsp_find_peaks(int16_t* fft_mag, fft_peaks* dest);

#endif // _DSP_ANALYSIS_H_
