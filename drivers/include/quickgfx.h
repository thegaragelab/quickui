/*---------------------------------------------------------------------------*
* Defines the driver interface to the graphics hardware
*----------------------------------------------------------------------------*
* 24-Apr-2013 shaneg
*
* This file defines the interface to the graphics driver (the code that spits
* out pixels to the screen). Rather than make it overly generic there are a
* few assumptions:
*
* 1/ All pixels are 16 bits wide (the bit format is not important)
* 2/ The screen width is fixed, it's always 320x240
* 3/ The driver must provide (at a minimum) the ability to initialise the
*    screen and draw a single pixel of a fixed color. Other operations are
*    implemented by the common library but will not be optimised for the
*    target hardware.
*---------------------------------------------------------------------------*/
#ifndef __QUICKGFX_H
#define __QUICKGFX_H

/* Extra definitions we need */
#include <stdint.h>

/* Guard for C++ */
#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Core constants
//---------------------------------------------------------------------------

/** Size of palettes (number of colors) */
#define GFX_PALETTE_SIZE 16

//---------------------------------------------------------------------------
// Common types and structures
//---------------------------------------------------------------------------

/** Return codes
 */
typedef enum _GFX_RESULT {
  GFX_RESULT_OK = 0,   //! Operation succeeded
  GFX_RESULT_MEMORY,   //! Out of memory
  GFX_RESULT_INTERNAL, //! Internal error (driver specific)
  GFX_RESULT_FAILED,   //! Generic failure (when no other data is available)
  GFX_RESULT_BADARG,   //! Invalid argument
  } GFX_RESULT;

// These structures are all byte aligned (space is at a premium)
#pragma pack(push, 1)

/** A single 16 bit color for final display
 */
typedef uint16_t GFX_COLOR;

/** Palette to map 4 bit color information to native colors
 */
typedef GFX_COLOR GFX_PALETTE[GFX_PALETTE_SIZE];

/** Event types
 *
 * This enum defines the event types we recognise. The driver is expected to
 * provide an interface to the touch inputs for the screen as well as drive
 * the graphical output. As a result it is responsible for generating these
 * events to feed up to the framework.
 *
 * If there is no touch interface available for the screen the driver can
 * simply not generate any events.
 */
typedef enum _GFX_TOUCH_EVENT {
  GFX_EVENT_TOUCH = 0,    //! Touch event
  GFX_EVENT_DRAG,         //! A drag event (touched and moving)
  GFX_EVENT_RELEASE,      //! Release event
  } GFX_TOUCH_EVENT;

/** Represents a single event
 *
 * This structure holds information about an event.
 */
typedef struct _GFX_TOUCH_EVENT_INFO {
  GFX_TOUCH_EVENT m_event;  //! The type of event that was detected
  uint16_t        m_xpos;   //! The X position of the event (in screen co-ordinates)
  uint16_t        m_ypos;   //! The Y position of the event (in screen co-ordinates)
  } GFX_TOUCH_EVENT_INFO;

/** Supported image types
 *
 */
typedef enum _IMAGE_BPP {
  IMAGE_BPP_1  = 1,  //! 1 bpp (monochrome) image.
  IMAGE_BPP_4  = 4,  //! 4 bpp (palette) image.
  IMAGE_BPP_16 = 16, //! 16 bpp (rrrrrggggggbbbbb) image.
  } IMAGE_BPP;

/** Image header information
 *
 * This structure provides common information about an image regardless of
 * it's format.
 */
typedef struct _GFX_IMAGE_HEADER {
  uint8_t m_width;    //! Width of the image in pixels
  uint8_t m_height;   //! Height of the image in pixels
  uint8_t m_bpp;      //! Pixel size (see IMAGE_BPP enum for values)
  uint8_t m_reserved; //! Reserved for future use
  } GFX_IMAGE_HEADER;

/** An image
 *
 * This structure combines the image header with the raw data for the image.
 * TODO: Restrictions and requirements for data size
 */
typedef struct _GFX_IMAGE {
  GFX_IMAGE_HEADER m_header;  //! Image information
  uint8_t          m_data[1]; //! The actual image data
  } GFX_IMAGE;

/** Information about a single character in a font.
 *
 * For each character in a font this structure defines it's ASCII value,
 * it's width and the location of it's bitmap in the attached GFX_IMAGE
 * representing the font.
 */
typedef struct _GFX_FONT_CHAR {
  uint8_t m_char;  //! ASCII value of the character represented
  uint8_t m_width; //! Width of this character in pixels
  uint8_t m_x;     //! X position of the top left of the character
  uint8_t m_y;     //! Y position of the top left of the character
  } GFX_FONT_CHAR;

/** The font header
 *
 * The font header defines the number of characters in the font, the height
 * of the characters and the default character to use if there is no defined
 * character for the one specified.
 */
typedef struct _GFX_FONT_HEADER {
  uint8_t m_chars;    //! Number of characters in this font
  uint8_t m_height;   //! Height of the characters in this font
  uint8_t m_default;  //! ASCII value of the default character to display
  uint8_t m_reserved; //! Reserved for future use
  } GFX_FONT_HEADER;

/** A font structure
 *
 * A font consists of a GFX_FONT_HEADER structure followed by up to 256
 * GFX_FONT_CHAR entries followed by a 1 bpp GFX_IMAGE structure. The
 * GFX_IMAGE data is used to render the font.
 */
typedef struct _GFX_FONT {
  GFX_FONT_HEADER m_header;   //! Font header information
  GFX_FONT_CHAR   m_chars[1]; //! Font character data
  } GFX_FONT;

// Revert to normal structure packing
#pragma pack(pop)

//---------------------------------------------------------------------------
// Macros
//---------------------------------------------------------------------------

/** Create a 16 bit color from 3 color components
 *
 * The 16 bit color is in the format rrrrrggggggbbbbb, if the driver expects
 * a different format you will have to do the conversion on the fly.
 */
#define GFX_COLOR_FROM_RGB(r, g, b) \
  ((uint16_t)(( \
    ((((uint16_t)(r)) & 0xF8) << 11) | \
    ((((uint16_t)(g)) & 0xFC) << 3) | \
    ((((uint16_t)(b)) & 0xF8) >> 3) ) \
    & 0xFFFF \
    ))

/** Calculate the length (in bytes) of a line in an image
 */
#define GFX_LINE_LENGTH(image, bpp) \
  (((((image)->m_header.m_width + 1) * (bpp) - 1) / 8) + 1)

//---------------------------------------------------------------------------
// Graphics driver SPI
//---------------------------------------------------------------------------

/** Handle an event
 *
 * An implementation of this function is provided by the calling application
 * and is used to receive information about events from the driver.
 */
typedef GFX_RESULT (*_gfx_HandleEvent)(GFX_TOUCH_EVENT_INFO *pEventInfo);

/** Begin a multipart paint operation */
typedef GFX_RESULT (*_gfx_BeginPaint)();

/** Finish a multipart paint operation */
typedef GFX_RESULT (*_gfx_EndPaint)();

/** Set the clipping rectangle */
typedef GFX_RESULT (*_gfx_SetClip)(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

/** Set the color of a single pixel */
typedef GFX_RESULT (*_gfx_PutPixel)(uint16_t x, uint16_t y, GFX_COLOR color);

/** Fill a region with the specified color */
typedef GFX_RESULT (*_gfx_FillRegion)(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, GFX_COLOR color);

/** Draw a monochrome image to the display */
typedef GFX_RESULT (*_gfx_DrawIcon)(uint16_t x, uint16_t y, GFX_IMAGE *pIcon, uint8_t sx, uint8_t sy, uint8_t w, uint8_t h, GFX_IMAGE *pMask, GFX_COLOR color);

/** Draw a 4 bit image to the display */
typedef GFX_RESULT (*_gfx_DrawImage4)(uint16_t x, uint16_t y, GFX_IMAGE *pImage, uint8_t sx, uint8_t sy, uint8_t w, uint8_t h, GFX_IMAGE *pMask, GFX_PALETTE pPalette);

/** Draw a 16 bit image to the display */
typedef GFX_RESULT (*_gfx_DrawImage16)(uint16_t x, uint16_t y, GFX_IMAGE *pImage, uint8_t sx, uint8_t sy, uint8_t w, uint8_t h, GFX_IMAGE *pMask);

/** Draw an image to the display */
typedef GFX_RESULT (*_gfx_DrawImage)(uint16_t x, uint16_t y, GFX_IMAGE *pImage, uint8_t sx, uint8_t sy, uint8_t w, uint8_t h, GFX_IMAGE *pMask, GFX_COLOR color, GFX_PALETTE pPalette);

/** Draw a single character to the display */
typedef GFX_RESULT (*_gfx_DrawChar)(uint16_t x, uint16_t y, GFX_FONT *pFont, int color, char ch);

/** Draw a string to the display */
typedef GFX_RESULT (*_gfx_DrawString)(uint16_t x, uint16_t y, GFX_FONT *pFont, int color, const char *cszString);

/** Draw a line from one point to another */
typedef GFX_RESULT (*_gfx_DrawLine)(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, GFX_COLOR color);

/** Draw a box */
typedef GFX_RESULT (*_gfx_DrawBox)(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, GFX_COLOR color);

/** Process pending events
 */
typedef GFX_RESULT (*_gfx_CheckEvents)(_gfx_HandleEvent pfHandleEvent);

/** Add a new event to the event queue
 */
typedef GFX_RESULT (*_gfx_AddEvent)(GFX_TOUCH_EVENT evType, uint16_t x, uint16_t y);

/** The graphics driver API
 */
typedef struct _GFX_DRIVER {
  uint16_t              m_width;              //! Width of the display in pixels
  uint16_t              m_height;             //! Height of the display in pixels
  uint16_t              m_clipX1;             //! Top left X co-ordinate of clipping
  uint16_t              m_clipY1;             //! Top left Y co-ordinate of clipping
  uint16_t              m_clipX2;             //! Bottom right X co-ordinate of clipping
  uint16_t              m_clipY2;             //! Bottom right Y co-ordinate of clipping
  _gfx_BeginPaint       m_pfBeginPaint;       //! Begin a paint operation
  _gfx_EndPaint         m_pfEndPaint;         //! End a paint operation
  _gfx_SetClip          m_pfSetClip;          //! Set a clipping rectangle
  _gfx_PutPixel         m_pfPutPixel;         //! Place a single pixel
  _gfx_FillRegion       m_pfFillRegion;       //! Fill a region with a single color
  _gfx_DrawIcon         m_pfDrawIcon;         //! Draw a monochrome image
  _gfx_DrawImage4       m_pfDrawImage4;       //! Draw a 4 bit image
  _gfx_DrawImage16      m_pfDrawImage16;      //! Draw a 16 bit image
  _gfx_DrawImage        m_pfDrawImage;        //! Draw an image
  _gfx_DrawChar         m_pfDrawChar;         //! Draw a single character
  _gfx_DrawString       m_pfDrawString;       //! Draw a sequence of characters
  _gfx_DrawLine         m_pfDrawLine;         //! Draw a line
  _gfx_DrawBox          m_pfDrawBox;          //! Draw a box
  _gfx_CheckEvents      m_pfCheckEvents;      //! Check for pending events
  _gfx_AddEvent         m_pfAddEvent;         //! Add a new event to the queue
  } GFX_DRIVER;

//---------------------------------------------------------------------------
// Graphics driver API
//---------------------------------------------------------------------------

/** The driver singleton */
extern GFX_DRIVER g_GfxDriver;

/** Initialise the driver
 *
 * This function is implemented by the driver and is required to fill out the
 * driver SPI structure. You can request a specific size for the framebuffer
 * but the driver is not required to honor it. Always check the width and
 * height members for the driver structure to get the actual size.
 *
 * @param width the requested width of the framebuffer
 * @param height the requested height of the framebuffer
 *
 * @return GFX_RESULT_OK if the driver is ready to use, another error code if
 *                       things didn't go well.
 */
GFX_RESULT gfx_Init(uint16_t width, uint16_t height);

/** Get the framebuffer the driver is using
 *
 * This function may return a pointer to the framebuffer used by the driver.
 * The pixel format of the framebuffer (and if a framebuffer exists at all)
 * is driver independant so it should be used with care.
 *
 * @return a pointer to the framebuffer or NULL if the driver does not allow
 *         direct framebuffer access.
 */
const void *gfx_Framebuffer();

/** Begin a paint operation
 */
#define gfx_BeginPaint() (*g_GfxDriver.m_pfBeginPaint)()

/** End a paint operation
 */
#define gfx_EndPaint() (*g_GfxDriver.m_pfEndPaint)()

/** Set the clipping rectangle
 *
 * @param x1 the top left X co-ordinate of the clipping rectangle
 * @param y1 the top left Y co-ordinate of the clipping rectangle
 * @param x2 the bottom right X co-ordinate of the clipping rectangle
 * @param y2 the bottom right Y co-ordinate of the clipping rectangle
 *
 * @return GFX_RESULT_OK if everything was ok.
 */
#define gfx_SetClip(x1, y1, x2, y2) (*g_GfxDriver.m_pfSetClip)(x1, y1, x2, y2)

/** Set the color of a single pixel
 *
 * @param pDriver the SPI driver to use
 * @param x the x position of the pixel to set
 * @param y the y position of the pixel to set
 * @param color the color to set the pixel to
 *
 * @return GFX_RESULT_OK if everything was ok.
 */
#define gfx_PutPixel(x, y, color) (*g_GfxDriver.m_pfPutPixel)(x, y, color)

/** Fill a region with the specified color
 */
#define gfx_FillRegion(x1, y1, x2, y2, color) (*g_GfxDriver.m_pfFillRegion)(x1, y1, x2, y2, color)

/** Draw a portion of an icon to the display */
#define gfx_DrawIcon(x, y, pIcon, sx, sy, w, h, pMask, color) (*g_GfxDriver.m_pfDrawIcon)(x, y, pIcon, sx, sy, w, h, pMask, color)

/** Draw a portion of an image to the display */
#define gfx_DrawImage4(x, y, pImage, sx, sy, w, h, pMask, pPalette) (*g_GfxDriver.m_pfDrawImage4)(x, y, pImage, sx, sy, w, h, pMask, pPalette)

/** Draw a portion of an image to the display */
#define gfx_DrawImage16(x, y, pImage, sx, sy, w, h, pMask) (*g_GfxDriver.m_pfDrawImage16)(x, y, pImage, sx, sy, w, h, pMask)

/** Draw a portion of an image to the display */
#define gfx_DrawImage(x, y, pImage, sx, sy, w, h, pMask, color, pPalette) (*g_GfxDriver.m_pfDrawImage)(x, y, pImage, sx, sy, w, h, pMask, color, pPalette)

/** Draw a single character to the display */
#define gfx_DrawChar(x, y, pFont, color, ch) (*g_GfxDriver.m_pfDrawChar)(x, y, pFont, color, ch)

/** Draw a string to the display */
#define gfx_DrawString(x, y, pFont, color, cszString) (*g_GfxDriver.m_pfDrawString)(x, y, pFont, color, cszString)

/** Draw a line from one point to another */
#define gfx_DrawLine(x1, y1, x2, y2, color) (*g_GfxDriver.m_pfDrawLine)(x1, y1, x2, y2, color)

/** Draw a box */
#define gfx_DrawBox(x1, y1, x2, y2, color) (*g_GfxDriver.m_pfDrawBox)(x1, y1, x2, y2, color)

/** Check for pending events */
#define gfx_CheckEvents(pfHandleEvent) (*g_GfxDriver.m_pfCheckEvents)(pfHandleEvent)

/** Add a new event to the event queue */
#define gfx_AddEvent(evType, x, y) (*g_GfxDriver.m_pfAddEvent)(evType, x, y)

/** Clip a value
 *
 * @param x the x co-ordinate to clip
 * @param y the y co-ordinate to clip
 *
 * @return true if the co-ordinates can be displayed, false if not
 */
#define GFX_CLIP(x, y) \
  (((x)>=g_GfxDriver.m_clipX1)&&((x)<=g_GfxDriver.m_clipX2)&&((y)>=g_GfxDriver.m_clipY1)&&((y)<=g_GfxDriver.m_clipY2))

/** Clamp the X co-ordinate
 *
 * @param x the X co-ordinate to clamp
 *
 * @return the clamped value that will fit in the clipping region
 */
#define GFX_CLAMP_X(x) \
  (((x)<g_gfxDriver.m_clipX1)?g_gfxDriver.m_clipX1:(((x)>g_GfxDriver.m_clipX2)?g_GfxDriver.m_clipX2:(x)))

/** Clamp the Y co-ordinate
 *
 * @param y the Y co-ordinate to clamp
 *
 * @return the clamped value that will fit in the clipping region
 */
#define GFX_CLAMP_Y(y) \
  (((y)<g_gfxDriver.m_clipY1)?g_gfxDriver.m_clipY1:(((y)>g_GfxDriver.m_clipY2)?g_GfxDriver.m_clipY2:(y)))

/* Guard for C++ */
#ifdef __cplusplus
};
#endif

#endif /* __QUICKGFX_H */
