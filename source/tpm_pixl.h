/* -----------------------------------------------------------------------------
 * tpm_pixl.h - A module that allows KL25Z to speak "neopixel"
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

#ifndef _TPM_PIXL_H_
#define _TPM_PIXL_H_

#include <stdint.h>

#define NUM_PIXELS  (8)

// Popular Neopixel colors:
#define RED     (0xff0000)
#define PINK    (0xd60018)
#define PURPLE  (0x1800de)
#define BLUE    (0x000cfa)
#define AQUA    (0x0039b5)
#define GREEN   (0x00f700)
#define YELLOW  (0x5a9c00)
#define ORANGE  (0xbd3900)


// color struct holding byte values for red, green, blue 
typedef struct {
  uint8_t red;
  uint8_t grn;
  uint8_t blu;
} color_t;

/* -----------------------------------------------------------------------------
 *     PUBLIC FUNCTIONS
 * -----------------------------------------------------------------------------
 */

/* @brief   Converts RGB color_t struct to a 24-bit packed color value
 *
 * @param   col_rgb, pointer to color_t struct
 * @return  uint32_t, a 24-bit packed number representing the color
 */
uint32_t tpm_pixl_rgb_to_24bit(color_t* col_rgb);


/* @brief   Converts 24-bit numbers to RGB color_t struct 
 *
 * @param   col_24bit, a pointer to a 24-bit packed number representing a color
 * @return  color_t, a rgb color_t struct
 */
color_t tpm_pixl_24bit_to_rgb(uint32_t* col_24bit);


/* @brief   Updates the neopixel strip via TPM1 and DMA1
 *
 * The logical output to the neopixels is on KL25Z Port A, Pin 12 (PTA12)
 *
 * @param   none
 * @return  -1 on error, 0 on success
 */
int tpm_pixl_update();


/* @brief   Initializes the neopixel output module
 *
 * Initializes DMA1 to update TPM1.CH0 edge-aligned pulse width for communication
 * to an external neopixel strip
 *
 * @param   none
 * @return  none
 */
void tpm_pixl_init();


/* -----------------------------------------------------------------------------
 *     PRIVATE FUNCTIONS
 * -----------------------------------------------------------------------------
 */

/* @brief   The interrupt service routine for DMA1 
 *
 * @param   none
 * @return  none
 */
void DMA1_IRQHandler();

/* @brief   Initializes TPM1 CH0
 *
 * TPM1 CH0 is output on PTA12 as an edge-aligned high true PWM signal with 
 * 3 MHz input clock
 *
 * @param   none
 * @return  none
 */
void _init_tpm1();

/* @brief   Initializes DMA1 
 *
 * DMA1 will update the pulse width CnV register of TPM1 upon TPM1 overflow
 * to output the 0 and 1 bit pulses that the neopixels need
 *
 * @param   none
 * @return  none
 */
void _init_dma1();

/* Note: Some colors were borrowed from the Arduino rainbow colorset:
0x00f700,
0x29ce00,
0x5a9c00,
0x8c6b00,
0xbd3900,
0xef0800,
0xd60018,
0xad004a,
0x7b007b,
0x4a00ad,
0x1800de,
0x0010e7,
0x0039b5,
0x006b8c,
0x009c5a,
0x00ce29,
*/

#endif // _TPM_PIXL_H_
