/*---------------------------------------------------------------------------*
* Common implementation of graphics functions
*----------------------------------------------------------------------------*
* 24-Apr-2013 shaneg
*
* This file implements a common version of the function that can be used by
* any graphics driver.
*---------------------------------------------------------------------------*/
#include <stdint.h>
#include <quickgfx.h>
#include <gfxdriver.h>

/** Draw an image to the display
 */
GFX_RESULT gfx_common_DrawImage(uint16_t x, uint16_t y, GFX_IMAGE *pImage, GFX_PALETTE pPalette) {
  return GFX_RESULT_OK;
  }
