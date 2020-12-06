/* -----------------------------------------------------------------------------
 * tpm_pixl.h
 *
 * @author  Jake Michael
 * @date    
 * @rev    
 * -----------------------------------------------------------------------------
 */

#ifndef _TPM_PIXL_H_
#define _TPM_PIXL_H_

#include <stdint.h>

#define NUM_PIXELS  (8)

// color struct holding byte values for red, green, blue 
typedef struct {
  uint8_t red;
  uint8_t grn;
  uint8_t blu;
} color_t;

/* @brief   
 *
 * @return
 * @param
 */
uint32_t tpm_pixl_rgb_to_24bit(color_t* col_rgb);


/* @brief   
 *
 * @return
 * @param
 */
color_t tpm_pixl_24bit_to_rgb(uint32_t* col_24bit);

/* @brief   
 *
 * @return
 * @param
 */
uint32_t tpm_pixl_dim(color_t *col_rgb, uint32_t* col_24bit, uint8_t dim_val);

/* @brief   
 *
 * @return
 * @param
 */
int tpm_pixl_update();

/* @brief   
 *
 * @return
 * @param
 */
void tpm_pixl_init();

/* @brief   
 *
 * @return
 * @param
 */
void _init_tpm1();

/* @brief   
 *
 * @return
 * @param
 */
void _init_dma1();

#endif // _TPM_PIXL_H_
