/* -----------------------------------------------------------------------------
 * spi_pixl.h
 *
 * @author  Jake Michael
 * @date    
 * @rev    
 * -----------------------------------------------------------------------------
 */

#ifndef _SPI_PIXL_H_
#define _SPI_PIXL_H_

#include <stdint.h>

#define NUM_PIXELS  (8)

/* @brief   
 *
 * @return
 * @param
 */
int spi_pixl_update();

/* @brief   
 *
 * @return
 * @param
 */
void spi_pixl_init();

/* @brief   
 *
 * @return
 * @param
 */
void _init_spi0();

/* @brief   
 *
 * @return
 * @param
 */
void _init_dma1();

#endif // _SPI_PIXL_H_
