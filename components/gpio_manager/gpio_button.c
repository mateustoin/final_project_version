#include "gpio_button.h"
#include "iot_button.h"
#include "project_fsm.h"
#include "esp_log.h"
#include "esp_system.h"

static const char *TAG = "gpio_button";

button_handle_t gpio_main_btn_handle;
static main_button_mode_t main_button_mode;

button_handle_t gpio_side_btn_handle;
static side_button_mode_t side_button_mode;

// create main gpio button configs
button_config_t gpio_main_btn_cfg = {
    .type = BUTTON_TYPE_GPIO,
    .gpio_button_config = {
        .gpio_num = MAIN_BUTTON,
        .active_level = 0,
    },
};

// create side gpio button configs
button_config_t gpio_side_btn_cfg = {
    .type = BUTTON_TYPE_GPIO,
    .gpio_button_config = {
        .gpio_num = SIDE_BUTTON,
        .active_level = 0,
    },
};

void set_button_main_mode(main_button_mode_t mode)
{
    switch (mode) {
        case MAIN_INACTIVE_MODE:
            ESP_LOGI(TAG, "Changing main button mode to INACTIVE_MODE");
            main_button_mode = MAIN_INACTIVE_MODE;
            break;
        case IDLE_APPLICATION_MODE:
            ESP_LOGI(TAG, "Changing main button mode to IDLE_APPLICATION_MODE");
            main_button_mode = IDLE_APPLICATION_MODE;
            break;
        case RUNNING_APPLICATION_MODE:
            ESP_LOGI(TAG, "Changing main button mode to RUNNING_APPLICATION_MODE");
            main_button_mode = RUNNING_APPLICATION_MODE;
            break;
        default:
            ESP_LOGE(TAG, "Wrong button_main_mode selected. Doing nothing...");
            main_button_mode = MAIN_INACTIVE_MODE;
            break;
    }
}

static void main_button_single_click_cb()
{
    switch(fsm_get_current_state()) {
        case INIT_GPIO_STATE:
            ESP_LOGI(TAG, "Button clicked while starting gpio. Is that even possible?! Nothing to do.");
            break;
        case INIT_WIFI_STATE:
            ESP_LOGI(TAG, "Starting WiFi, nothing to do with the button.");
            break;
        case INIT_MPU6886_STATE:
            ESP_LOGI(TAG, "Starting MPU6886, nothing to do with the button.");
            break;
        case WAIT_FOR_START_STATE:
            ESP_LOGI(TAG, "Starting project application, going to INIT_SUPABASE_CONN_STATE");
            fsm_set_next_state(INIT_SUPABASE_CONN_STATE);
            break;
        case INIT_SUPABASE_CONN_STATE:
            ESP_LOGI(TAG, "Doing nothing while initializing connection with Supabase.");
            break;
        case INIT_SAC_DM_ROUTINE_STATE:
            ESP_LOGI(TAG, "Cancelling state, cleaning data and going back to WAIT_FOR_START_STATE");
            fsm_set_next_state(EXIT_SUPABASE_CONN_STATE);
            break;
        case SAC_DM_DATA_COLLECTING_STATE:
            ESP_LOGI(TAG, "Cancelling state, cleaning data and going back to WAIT_FOR_START_STATE");
            fsm_set_next_state(EXIT_SUPABASE_CONN_STATE);
            break;
        case SEND_DATA_TO_SUPABASE_STATE:
            ESP_LOGI(TAG, "Cancelling state, cleaning data and going back to WAIT_FOR_START_STATE");
            fsm_set_next_state(EXIT_SUPABASE_CONN_STATE);
            break;
        case EXIT_SUPABASE_CONN_STATE:
            ESP_LOGI(TAG, "Already cleaning data and going back to WAIT_FOR_START_STATE");
            break;
        case IDLE_STATE:
            ESP_LOGI(TAG, "Leaving IDLE_STATE and restarting the board.");
            esp_restart();
            break;
        default:
            ESP_LOGI(TAG, "Starting default state, not recognized... going to IDLE_STATE");
            fsm_set_next_state(IDLE_STATE);
            break;
    }
}

esp_err_t init_gpio_main_button_config()
{
    ESP_LOGI(TAG, "Initializing button callback configuration...");
    esp_err_t ret;
    // Configura botão do M5
    gpio_main_btn_handle = iot_button_create(&gpio_main_btn_cfg);
    if(NULL == gpio_main_btn_handle) {
        ESP_LOGE(TAG, "Main Button create failed");
    }
    ESP_LOGI(TAG, "Main Button create success");
    ret = iot_button_register_cb(gpio_main_btn_handle, BUTTON_SINGLE_CLICK, &main_button_single_click_cb, NULL);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register button!");
        return ESP_FAIL;
    }
    set_button_main_mode(MAIN_INACTIVE_MODE);
    return ESP_OK;
}

esp_err_t init_gpio_side_button_config()
{
    ESP_LOGI(TAG, "Initializing button callback configuration...");
    // Configura botão do M5
    gpio_side_btn_handle = iot_button_create(&gpio_side_btn_cfg);
    if(NULL == gpio_side_btn_handle) {
        ESP_LOGE(TAG, "Side Button create failed");
    }
    ESP_LOGI(TAG, "Side Button create success");
    // iot_button_register_cb(gpio_side_btn_handle, BUTTON_SINGLE_CLICK, &main_button_single_click_cb, NULL);
    return ESP_OK;
}
