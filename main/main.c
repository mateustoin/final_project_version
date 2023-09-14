#include <stdio.h>
#include <string.h>

// Bibliotecas do ESP
#include "esp_system.h"
//#include "esp_spi_flash.h"
#include "spi_flash_mmap.h"
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "esp_timer.h"
#include "sdkconfig.h"

// Bibliotecas do freeRTOS
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Bibliotecas próprias para o projeto
#include "connect_sta.h"
#include "supabase_client.h"
#include "sacdm_manager.h"

// Lib do botão e led
#include "driver/gpio.h"
#include "iot_button.h"

// Create timer config
const esp_timer_create_args_t esp_timer_create_args = {
        .callback = sacdm_calculate,
        .name = "SAC Timer"
};
esp_timer_handle_t esp_timer_handle;

#define M5_BUTTON_A 37
#define M5_BUTTON_B 39
#define M5_LED GPIO_NUM_10

static int led_state = 1;

// create gpio button
button_config_t gpio_btn_cfg = {
    .type = BUTTON_TYPE_GPIO,
    .gpio_button_config = {
        .gpio_num = M5_BUTTON_A,
        .active_level = 0,
    },
};

static void button_single_click_cb()
{
    if (esp_timer_is_active(esp_timer_handle) == 0) {
        ESP_LOGI("Button_A", "Starting SAC-DM System");
        esp_timer_start_periodic(esp_timer_handle, 2*1000);
        gpio_set_level(M5_LED, led_state);
        led_state = !led_state;
    } else {
        ESP_LOGI("Button_A", "Stoping SAC-DM System");
        esp_timer_stop(esp_timer_handle);
        sacdm_reset();
        gpio_set_level(M5_LED, led_state);
        led_state = !led_state;
    }
}

void receive_http_notification(void *params)
{
    // Configura e inicializa supabase
    supabase_config sbp_config = {
        .table_url = "https://nuopbiwoomjqqfgdasxh.supabase.co",
        .api_key = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6Im51b3BiaXdvb21qcXFmZ2Rhc3hoIiwicm9sZSI6ImFub24iLCJpYXQiOjE2NTM3ODE5NzYsImV4cCI6MTk2OTM1Nzk3Nn0.OhT45KrI62zmA8TVxabm1dfeuyZhLD2O7tPp6NMXD2s"
    };
    spb_open(&sbp_config);

    while(true) {
        ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
        spb_write(create_sacdm_payload_body());
    }
}

TaskHandle_t notify_handler = NULL;

void app_main(void) 
{
    // SAC_DM manager
    sacdm_init(&notify_handler);

    // Inicializa WiFi e conecta no AP
    ESP_LOGI("main", "Inicializando WiFi");
    esp_err_t ret = nvs_flash_init();        //Initialize NVS

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);
    wifi_init_sta();

    xTaskCreatePinnedToCore(&receive_http_notification, "SupabaseClient", 4096*2, NULL, 5, &notify_handler, 1);

    // Inicia timer para calculo do SAC-DM
    ESP_ERROR_CHECK(esp_timer_create(&esp_timer_create_args, &esp_timer_handle));

    // Configura led
    //gpio_pad_select_gpio(M5_LED);
    esp_rom_gpio_pad_select_gpio(M5_LED);
    gpio_set_direction(M5_LED, GPIO_MODE_OUTPUT);
    gpio_set_level(M5_LED, 0);

    // Configura botão do M5
    button_handle_t gpio_btn = iot_button_create(&gpio_btn_cfg);
    if(NULL == gpio_btn) {
        ESP_LOGE("Button_A", "Button create failed");
    }
    iot_button_register_cb(gpio_btn, BUTTON_SINGLE_CLICK, &button_single_click_cb, NULL);
}
