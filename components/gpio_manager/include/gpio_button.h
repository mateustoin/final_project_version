#pragma once

#include "esp_check.h"

#define MAIN_BUTTON CONFIG_BUTTON_A
#define SIDE_BUTTON CONFIG_BUTTON_B

typedef enum {
    MAIN_INACTIVE_MODE,
    IDLE_APPLICATION_MODE,
    RUNNING_APPLICATION_MODE
} main_button_mode_t;

typedef enum {
    SIDE_INACTIVE_MODE,
    DEBUG_MODE,
    IDLE_MODE
} side_button_mode_t;

esp_err_t init_gpio_main_button_config();
esp_err_t init_gpio_side_button_config();

void set_button_main_mode(main_button_mode_t mode);