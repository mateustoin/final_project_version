#include "gpio_button.h"
#include "iot_button.h"
#include "esp_log.h"

static const char *TAG = "gpio_button";

static bool button_state = 1;

// create gpio button
button_config_t gpio_btn_cfg = {
    .type = BUTTON_TYPE_GPIO,
    .gpio_button_config = {
        .gpio_num = MAIN_BUTTON,
        .active_level = 0,
    },
};

static void button_single_click_cb()
{
    if (button_state) {
        ESP_LOGI(TAG, "button state: %d", button_state);
        button_state = !button_state;
    } else {
        ESP_LOGI(TAG, "button state: %d", button_state);
        button_state = !button_state;
    }
}

void init_gpio_button_config()
{
    ESP_LOGI(TAG, "Initializing button callback configuration...");
    // Configura botão do M5
    button_handle_t gpio_btn = iot_button_create(&gpio_btn_cfg);
    if(NULL == gpio_btn) {
        ESP_LOGE(TAG, "Button create failed");
    }
    ESP_LOGI(TAG, "Button create success");
    iot_button_register_cb(gpio_btn, BUTTON_SINGLE_CLICK, &button_single_click_cb, NULL);
}
