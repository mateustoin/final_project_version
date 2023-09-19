#pragma once

#include "led_indicator.h"
#include "esp_check.h"

typedef enum {
    SUCCESS_EVENT,          /* Blink 1 time */
    FAIL_EVENT,             /* Blink 2 times */
    WAIT_FOR_START_EVENT,   /* Turn on LED constantly with half bright */
    IDLE_LED_STATE              /* Fade led constantly between 0 and half bright */
} led_status_evts_mode_t;

typedef enum {
    SUCESS_BEHAVIOR,
    FAIL_BEHAVIOR,
    WAIT_FOR_START_BEHAVIOR,
    IDLE_BEHAVIOR,
    INVALID_BEHAVIOR
} led_behavior_list_t;

esp_err_t init_led_configs();
esp_err_t deinit_led_configs();
void update_led_event_mode(led_status_evts_mode_t status);
