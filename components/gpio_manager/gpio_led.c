#include "gpio_led.h"
#include <stdio.h>

static const char *TAG = "gpio_led";
led_behavior_list_t last_used_behavior;
bool led_already_used = false;

/*
    SUCCESS_EVENT - Blink 1 time
*/
const blink_step_t success_led_behavior[] = {
    {LED_BLINK_HOLD, LED_STATE_ON, 100},               // step1: turn on LED 50 ms
    {LED_BLINK_HOLD, LED_STATE_OFF, 100},             // step2: turn off LED 100 ms
    {LED_BLINK_STOP, 0, 0},                           // step3: loop from step1
};

/*
    FAIL_EVENT - Blink 2 time
*/
const blink_step_t fail_led_behavior[] = {
    {LED_BLINK_HOLD, LED_STATE_ON, 100},               // step1: turn on LED 50 ms
    {LED_BLINK_HOLD, LED_STATE_OFF, 100},             // step2: turn off LED 100 ms
    {LED_BLINK_HOLD, LED_STATE_ON, 100},               // step1: turn on LED 50 ms
    {LED_BLINK_HOLD, LED_STATE_OFF, 100},             // step2: turn off LED 100 ms
    {LED_BLINK_STOP, 0, 0},                           // step3: loop from step1
};

/*
    WAIT_FOR_START_EVENT - Turn ON with half bright
*/
const blink_step_t wait_for_start_led_behavior[] = {
    {LED_BLINK_HOLD, LED_STATE_ON, 100},                  // step1: set to half brightness 500 ms
    {LED_BLINK_LOOP, 0, 0},                           // step2: loop from step1
};

/*
    IDLE_EVENT - 3 blinks
*/
const blink_step_t idle_led_behavior[] = {
    {LED_BLINK_HOLD, LED_STATE_OFF, 500},             // step2: turn off LED 100 ms
    {LED_BLINK_HOLD, LED_STATE_ON, 100},               // step1: turn on LED 50 ms
    {LED_BLINK_HOLD, LED_STATE_OFF, 30},             // step2: turn off LED 100 ms
    {LED_BLINK_HOLD, LED_STATE_ON, 100},               // step1: turn on LED 50 ms
    {LED_BLINK_HOLD, LED_STATE_OFF, 30},             // step2: turn off LED 100 ms
    {LED_BLINK_HOLD, LED_STATE_ON, 100},               // step1: turn on LED 50 ms
    {LED_BLINK_HOLD, LED_STATE_OFF, 30},             // step2: turn off LED 100 ms
    {LED_BLINK_LOOP, 0, 0},                           // step3: loop from step1
};

/*
    INVALID_EVENT - Fade led constantly between 0 and half bright
*/
const blink_step_t invalid_led_behavior[] = {
    {LED_BLINK_BREATHE, LED_STATE_OFF, 50},             // step2: fade from on to off 500ms
    {LED_BLINK_LOOP, 0, 0},                              // step4: stop blink (50% brightness)
};

/*
    Lists creation
*/
blink_step_t const * led_indicator_blink_lists[] = {
    [SUCESS_BEHAVIOR] = success_led_behavior,
    [FAIL_BEHAVIOR] = fail_led_behavior,
    [WAIT_FOR_START_BEHAVIOR] = wait_for_start_led_behavior,
    [IDLE_BEHAVIOR] = idle_led_behavior,
    [INVALID_BEHAVIOR] = NULL
};

/*
    Led config creation
*/
led_indicator_gpio_config_t led_indicator_gpio_config = {
    .gpio_num = CONFIG_LED,              /**< num of GPIO */
    .is_active_level_high = 0,
};

led_indicator_config_t config = {
    .mode = LED_GPIO_MODE,
    .led_indicator_gpio_config = &led_indicator_gpio_config,
    .blink_lists = led_indicator_blink_lists,
    .blink_list_num = INVALID_BEHAVIOR,
};

led_indicator_handle_t led_handle;

esp_err_t init_led_configs()
{
    ESP_LOGI(TAG, "Creating led configs");
    led_handle = led_indicator_create(&config);
    if (led_handle == NULL) {
        ESP_LOGE(TAG, "Error while creating led handler.");
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t deinit_led_configs()
{
    ESP_LOGI(TAG, "Deleting led configs");
    esp_err_t ret = led_indicator_delete(led_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error while deleting led handler.");
        return ESP_FAIL;
    }
    return ESP_OK;
}

void update_led_event_mode(led_status_evts_mode_t status)
{
    esp_err_t ret;
    if (led_already_used == true)
        led_indicator_stop(led_handle, last_used_behavior);
    switch (status) {
    case SUCCESS_EVENT:
        ret = led_indicator_start(led_handle, SUCESS_BEHAVIOR);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Error set SUCCESS_BEHAVIOR for led...");
            break;
        }
        last_used_behavior = SUCESS_BEHAVIOR;
        break;
    case FAIL_EVENT:
        ret = led_indicator_start(led_handle, FAIL_BEHAVIOR);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Error set FAIL_BEHAVIOR for led...");
            break;
        }
        last_used_behavior = SUCESS_BEHAVIOR;
        break;
    case WAIT_FOR_START_EVENT:
        ret = led_indicator_start(led_handle, WAIT_FOR_START_BEHAVIOR);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Error set WAIT_FOR_START_BEHAVIOR for led...");
            break;
        }
        last_used_behavior = WAIT_FOR_START_BEHAVIOR;
        break;
    case IDLE_LED_STATE:
        ret = led_indicator_start(led_handle, IDLE_BEHAVIOR);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Error set IDLE_BEHAVIOR for led...");
            break;
        }
        last_used_behavior = IDLE_BEHAVIOR;
        break;
    default:
        ESP_LOGI(TAG, "Wrong argument for set new led behavior");
        ret = led_indicator_start(led_handle, INVALID_BEHAVIOR);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Error set INVALID_BEHAVIOR for led...");
            break;
        }
        last_used_behavior = INVALID_BEHAVIOR;
        break;
    }
    led_already_used = true;
}
