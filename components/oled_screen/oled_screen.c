/*
 * oled_screen.c
 *
 *  Created on: 04.05.2021
 *      Author: andriy@malyshenko.com
 */

#include <stddef.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <ctype.h>

#include "esp_log.h"
#include "vector.h"
#include "oled_screen.h"

#define TAG "oled_screen"

#if defined USE_I2C_DISPLAY
static const int I2CDisplayAddress = 0x3C;
static const int I2CDisplayWidth = 128;
static const int I2CDisplayHeight = 32;
static const int I2CResetPin = -1;

struct SSD1306_Device I2CDisplay;
#endif

#if defined USE_SPI_DISPLAY
static const int SPIDisplayChipSelect = 2;
static const int SPIDisplayWidth = 128;
static const int SPIDisplayHeight = 64;
static const int SPIResetPin = 21;

struct SSD1306_Device SPIDisplay;
#endif

bool oled_screen_init( void ) {
    #if defined USE_I2C_DISPLAY
        assert( SSD1306_I2CMasterInitDefault( ) == true );
        assert( SSD1306_I2CMasterAttachDisplayDefault( &I2CDisplay, I2CDisplayWidth, I2CDisplayHeight, I2CDisplayAddress, I2CResetPin ) == true );
    #endif

    #if defined USE_SPI_DISPLAY
        assert( SSD1306_SPIMasterInitDefault( ) == true );
        assert( SSD1306_SPIMasterAttachDisplayDefault( &SPIDisplay, SPIDisplayWidth, SPIDisplayHeight, SPIDisplayChipSelect, SPIResetPin ) == true );
    #endif

    return true;
}

void oled_screen_config( const struct SSD1306_FontDef* Font ) {
    SSD1306_Clear( &SPIDisplay, SSD_COLOR_BLACK );
    SSD1306_SetFont( &SPIDisplay, Font );
}

void oled_screen_say( const char* text ) {
    oled_screen_say_at( text, TextAnchor_Center );
}

void oled_screen_say_at( const char* text, TextAnchor anchor) {
    SSD1306_FontDrawAnchoredString( &SPIDisplay, anchor, text, SSD_COLOR_WHITE );
    SSD1306_Update( &SPIDisplay );
}

void oled_screen_clear( void ) {
    SSD1306_Clear( &SPIDisplay, SSD_COLOR_BLACK );
    SSD1306_Update( &SPIDisplay );
}
