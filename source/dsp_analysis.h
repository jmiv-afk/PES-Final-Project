/* -----------------------------------------------------------------------------
 * dsp_analysis.h - A digital signal processing module 
 *
 * Module for calculating a 512 sample FFT power spectrum via the CMSIS DSP 
 * library:
 *    https://www.keil.com/pack/doc/CMSIS/DSP/html/group__RealFFT.html
 *
 * @author  Jake Michael
 * @date    2020-12-07 
 * @rev     1.3
 * -----------------------------------------------------------------------------
 */
#include <stdint.h>

#ifndef _DSP_ANALYSIS_H_
#define _DSP_ANALYSIS_H_

#define NBUCKETS  (8)

typedef struct {
  int indices[NBUCKETS];  // contains the index of each peak
  int16_t mags[NBUCKETS]; // contains the magnitude of each peak
} fft_peaks;

/* @brief   Returns magnitude squared of a 512 sample real FFT  
 * 
 * The sampled data should be 512 samples long. Internally, the data is 
 * copied and scaled to a q15_t datatype and a Hanning window is applied 
 * before taking the transform. 
 *
 * @param   samples,  the sampled data as a uint16_t datatype
 *          nsamples, only 512 sample FFT's are currently supported
 * @return  int16_t, the complex magnitude squared of the FFT which has a 
 *          length of 512; however, the second half of this spectrum is 
 *          typically disregarded as it is a mirror image of the first half.
 */
int16_t* dsp_fft_mag(uint16_t* samples, int nsamples);

/* @brief  Finds the NBUCKETS peaks between bucket_indices
 *
 * Finds a single maximal peak via linear search between sequential pairs of
 * bucket_indices. The result is placed in an fft_peaks structure which wraps
 * the index and the magnitude of each peak found. If no maximum is found between
 * bucket_indices[i] and bucket_indices[i+1], then the result index/mag will 
 * simply be bucket_indices[i+1]. 
 *
 * @param  fft_mag, the magnitude (or magnitude squared) of the real FFT 
 *              spectrum
 *         dest, the destination fft_peaks structure indicating the index and 
 *              magnitude of each peak found
 *         bucket_indices, the sequential indices defining the regions of the peak
 *              search. Length must be NBUCKETS+1.
 * @return  0 on success, -1 on error
 */
int dsp_find_peaks(int16_t* fft_mag, fft_peaks* dest, uint32_t* bucket_indices);

#endif // _DSP_ANALYSIS_H_
