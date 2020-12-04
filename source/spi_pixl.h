/* -----------------------------------------------------------------------------
 * spi_pixl.h
 *
 * @author  Jake Michael
 * @date    
 * @rev    
 * -----------------------------------------------------------------------------
 */

#include <stdint.h>

#ifndef _SPI_PIXL_H_
#define _SPI_PIXL_H_

#define NUM_PIXELS  (8)

void spi_pixl_reset();

int spi_pixl_write();

void spi_pixl_init();

void _init_spi0();

void _init_dma1();

#endif // _SPI_PIXL_H_
