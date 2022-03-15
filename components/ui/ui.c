/*
 * ui.c
 *
 *  Created on: 01.04.2017
 *      Author: michaelboeckling
 */



#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <soc/rmt_struct.h>
#include <esp_system.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <driver/gpio.h>
#include <stdio.h>

#include "../user_driver/include/ws2812.h"
#include "ui.h"
#include "oled_screen.h"
//#include "common_utils.h"

#define TAG "ui"

#define delay_ms(ms) ((ms) / portTICK_RATE_MS)

typedef struct {
    int pin;
    int queue_size;
    QueueHandle_t ui_queue;
} ui_obj_t;

/** UI object instance */
static ui_obj_t *p_ui_obj = NULL;

static bool ledEnabled = false;
static rgbVal black;
static rgbVal white;
static rgbVal red;
static rgbVal green;
static rgbVal blue;

void task_ui(void *pvParameters)
{
    ui_event_t curr_anim = { UI_NONE };
    rgbVal *next_color = &black;
    // be lazy and use queue for blink speed
    TickType_t delay = portMAX_DELAY;

    while(1) {
        xQueueReceive((p_ui_obj->ui_queue), &curr_anim, delay);
        //ESP_LOGI(TAG, "ui evt: %d ", curr_anim.evt);
        char buf[3];

        switch(curr_anim.evt) {
            case UI_NONE:
                delay = portMAX_DELAY;
                if (ledEnabled)
                    ws2812_setColors(1, &black);
            break;

            case UI_CONNECTING:
                delay = delay_ms(250);
                oled_screen_clear();
                oled_screen_say("Connecting...");
                if (ledEnabled) {
                    next_color = (next_color == &white) ? &blue : &white;
                    ws2812_setColors(1, next_color);
                }
            break;

            case UI_CONNECTED:
                delay = portMAX_DELAY;
                if (ledEnabled)
                    ws2812_setColors(1, &white);
                oled_screen_clear();
            break;

            case UI_PLAYER_INIT:
                oled_screen_clear();
                oled_screen_say("Ready");
            break;

            case UI_PLAYER_RUNNING:
                oled_screen_clear();
                oled_screen_say("Playing");
            break;

            case UI_PLAYER_STOPPED:
                oled_screen_clear();
                oled_screen_say("Stopped");
            break;

            case UI_PLAYER_UNINIT:
                oled_screen_clear();
            break;

            case UI_PLAYER_BUFF_PCT:
                itoa(curr_anim.value, buf, 10);
                oled_screen_clear();
                oled_screen_say(buf);
            break;
        }
        //ESP_LOGI(TAG, "task_ui stack: %d\n", uxTaskGetStackHighWaterMark(NULL));
    }

    vTaskDelete(NULL);
}

void ui_queue_event(ui_event_t evt)
{
    if(p_ui_obj != NULL)
        xQueueSend(p_ui_obj->ui_queue, &evt, 0);
}

int ui_init_int(bool withLed, gpio_num_t pin)
{
    ledEnabled = withLed;
    if(p_ui_obj == NULL) {
        p_ui_obj = malloc(sizeof(ui_obj_t));
    } else {
        ESP_LOGE(TAG, "UI already initialized");
        return -1;
    }

    p_ui_obj->queue_size = 1;
    p_ui_obj->ui_queue = xQueueCreate(1, sizeof(ui_event_t));

    if (withLed) {
        black    = makeRGBVal(0, 0, 0);
        white    = makeRGBVal(32, 32, 32);
        red      = makeRGBVal(32, 0, 0);
        green    = makeRGBVal(0, 32, 0);
        blue     = makeRGBVal(0, 0, 32);

        ws2812_init(pin);
        ws2812_setColors(1, &black);
    }

    xTaskCreatePinnedToCore(&task_ui, "task_ui", 2048, NULL, 20, NULL, 0);

    return 0;
}

int ui_init()
{
    return ui_init_int(false, 0);
}

int ui_init_with_led(gpio_num_t pin)
{
    return ui_init_int(true, pin);
}