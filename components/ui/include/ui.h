/*
 * ui.h
 *
 *  Created on: 01.04.2017
 *      Author: michaelboeckling
 *  Modified on 04.05.2021
 *          by: andriy@malyshenko.com
 */

#ifndef _INCLUDE_UI_H_
#define _INCLUDE_UI_H_

typedef enum { 
    UI_NONE, 
    UI_CONNECTING, 
    UI_CONNECTED,
    UI_PLAYER_INIT,
    UI_PLAYER_RUNNING,
    UI_PLAYER_STOPPED,
    UI_PLAYER_UNINIT,
    UI_PLAYER_BUFF_PCT
} ui_event_type;

typedef struct { 
    ui_event_type evt;
    int value;
} ui_event_t;

void ui_queue_event(ui_event_t evt);

int ui_init();
int ui_init_with_led(gpio_num_t pin);

#endif /* _INCLUDE_UI_H_ */
