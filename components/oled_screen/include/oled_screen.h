/*
 * oled_screen.h
 *
 *  Created on: 04.05.2021
 *      Author: andriy@malyshenko.com
 */

#ifndef _INCLUDE_OLED_SCREEN_H_
#define _INCLUDE_OLED_SCREEN_H_

#include "../../tarablessd1306/ssd1306.h"
#include "../../tarablessd1306/ssd1306_draw.h"
#include "../../tarablessd1306/ssd1306_font.h"
#include "../../tarablessd1306/ssd1306_default_if.h"

//#define USE_I2C_DISPLAY
#define USE_SPI_DISPLAY

bool oled_screen_init( void );

void oled_screen_config( const struct SSD1306_FontDef* Font );
void oled_screen_clear( void );
void oled_screen_say( const char* text );
void oled_screen_say_at( const char* text, TextAnchor anchor );

#endif /* _INCLUDE_OLED_SCREEN_H_ */
