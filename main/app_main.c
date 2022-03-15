
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "../components/http/include/http.h"
#include "driver/i2s.h"

#include "../components/vector/include/vector.h"
#include "ui.h"
#include "../components/fifo/include/spiram_fifo.h"
#include "../components/audio_renderer/include/audio_renderer.h"
#include "../components/web_radio/include/web_radio.h"
#include "../include/playerconfig.h"
#include "wifi.h"
#include "app_main.h"
#include "../components/oled_screen/include/oled_screen.h"

#ifdef CONFIG_BT_SPEAKER_MODE
#include "bt_speaker.h"
#endif
#include "../components/playlist/include/playlist.h"

#define WIFI_LIST_NUM 10
#define TAG "main"

//Priorities of the reader and the decoder thread. bigger number = higher prio
#define PRIO_READER configMAX_PRIORITIES - 3
#define PRIO_MQTT configMAX_PRIORITIES - 3
#define PRIO_CONNECT configMAX_PRIORITIES - 1

static void init_hardware()
{
    nvs_flash_init();

    // init UI
    ui_init(); 
    //ui_init_with_led(GPIO_NUM_32);

    //Initialize the SPI RAM chip communications and see if it actually retains some bytes. If it
    //doesn't, warn user.
    if (!spiRamFifoInit())
    {
        printf("\n\nSPI RAM chip fail!\n");
        while (1)
            ;
    }

    if (oled_screen_init() == true)
    {
        printf("BUS Init looking good...\n");
        oled_screen_config( &Font_droid_sans_mono_7x13 );
    }

    ESP_LOGI(TAG, "hardware initialized");
}

static void start_wifi()
{
    ESP_LOGI(TAG, "starting network");

    /* FreeRTOS event group to signal when we are connected & ready to make a request */
    EventGroupHandle_t wifi_event_group = xEventGroupCreate();

    /* init wifi */
    ui_queue_event((ui_event_t){ UI_CONNECTING, 0 });
    initialise_wifi(wifi_event_group);

    /* Wait for the callback to set the CONNECTED_BIT in the event group. */
    xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
                        false, true, portMAX_DELAY);

    ui_queue_event((ui_event_t){ UI_CONNECTED });
}

static renderer_config_t *create_renderer_config()
{
    renderer_config_t *renderer_config = calloc(1, sizeof(renderer_config_t));

    renderer_config->bit_depth = I2S_BITS_PER_SAMPLE_16BIT;
    renderer_config->i2s_num = I2S_NUM_0;
    renderer_config->sample_rate = 44100;
    renderer_config->sample_rate_modifier = 1.0;
    renderer_config->output_mode = AUDIO_OUTPUT_MODE;

    if (renderer_config->output_mode == I2S_MERUS)
    {
        renderer_config->bit_depth = I2S_BITS_PER_SAMPLE_32BIT;
    }

    if (renderer_config->output_mode == DAC_BUILT_IN)
    {
        renderer_config->bit_depth = I2S_BITS_PER_SAMPLE_16BIT;
    }

    return renderer_config;
}

static void start_web_radio()
{
    // init web radio
    web_radio_t *radio_config = calloc(1, sizeof(web_radio_t));
    radio_config->playlist = playlist_create();
    playlist_load_pls(radio_config->playlist);

    // init player config
    radio_config->player_config = calloc(1, sizeof(player_t));
    radio_config->player_config->command = CMD_NONE;
    radio_config->player_config->decoder_status = UNINITIALIZED;
    radio_config->player_config->decoder_command = CMD_NONE;
    radio_config->player_config->buffer_pref = BUF_PREF_SAFE;
    radio_config->player_config->media_stream = calloc(1, sizeof(media_stream_t));

    // init renderer
    renderer_init(create_renderer_config());

    // start radio
    web_radio_init(radio_config);
    web_radio_start(radio_config);
}

void app_main()
{
    ESP_LOGI(TAG, "starting app_main()");
    ESP_LOGI(TAG, "RAM left: %u", esp_get_free_heap_size());

    init_hardware();

#ifdef CONFIG_BT_SPEAKER_MODE
    bt_speaker_start(create_renderer_config());
#else
    start_wifi();
    start_web_radio();
#endif
    ESP_LOGI(TAG, "RAM left %d", esp_get_free_heap_size());
    ESP_LOGI(TAG, "app_main stack: %d\n", uxTaskGetStackHighWaterMark(NULL));
}
