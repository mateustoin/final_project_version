#include <stdio.h>
#include "project_fsm.h"

// Bibliotecas do ESP
#include "esp_system.h"
#include "esp_log.h"
#include "esp_err.h"
#include "sdkconfig.h"

// Bibliotecas do freeRTOS
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Bibliotecas do projeto
#include "connect_sta.h"
#include "gpio_button.h"
#include "gpio_led.h"
#include "supabase_client.h"
#include "sacdm_acc_provider.h"
#include "sacdm_manager.h"

static const char *TAG = "Project_FSM";

project_states_t currentFsmState;
project_states_t eNextState;
eSystemEvent currentEvt;

project_states_t fsm_get_current_state()
{
    return currentFsmState;
}

void fsm_set_current_state(project_states_t newState)
{
    currentFsmState = newState;
}

void fsm_set_next_state(project_states_t newState)
{
    eNextState = newState;
}

void run_project_fsm()
{
    eNextState = INIT_GPIO_STATE;

    while(true) {
        esp_err_t ret_code;

        switch(eNextState) {
        case INIT_GPIO_STATE:
            if (eNextState != currentFsmState)
                ESP_LOGI(TAG, ">>> Starting INIT_GPIO_STATE <<<");
            currentFsmState = INIT_GPIO_STATE;
            ret_code = init_led_configs();
            if (ret_code != ESP_OK) {
                ESP_LOGE(TAG, "Failed to configure LED Indicator!");
            }
            ret_code = init_gpio_main_button_config();
            if (ret_code != ESP_OK) {
                ESP_LOGE(TAG, "Failed to configure Main Button GPIO!");
                update_led_event_mode(FAIL_EVENT);
                eNextState = IDLE_STATE;
                break;
            }
            update_led_event_mode(SUCCESS_EVENT);
            eNextState = INIT_WIFI_STATE;
            break;
        case INIT_WIFI_STATE:
            if (eNextState != currentFsmState)
                ESP_LOGI(TAG, ">>> Starting INIT_WIFI_STATE <<<");
            currentFsmState = INIT_GPIO_STATE;
            ret_code = wifi_config_sta();
            if (ret_code != ESP_OK) {
                ESP_LOGE(TAG, "Failed to init WiFi!");
                eNextState = IDLE_STATE;
                break;
            }
            update_led_event_mode(SUCCESS_EVENT);
            eNextState = INIT_MPU6886_STATE;
            break;
        case INIT_MPU6886_STATE:
            if (eNextState != currentFsmState)
                ESP_LOGI(TAG, ">>> Starting INIT_MPU6886_STATE <<<");
            currentFsmState = INIT_MPU6886_STATE;
            ret_code = sacdm_mpu6886_init();
            if (ret_code != ESP_OK) {
                ESP_LOGE(TAG, "Failed to init MPU6886!");
                eNextState = IDLE_STATE;
                break;
            }
            update_led_event_mode(SUCCESS_EVENT);
            eNextState = WAIT_FOR_START_STATE;
            break;
        case WAIT_FOR_START_STATE:
            if (eNextState != currentFsmState)
                ESP_LOGI(TAG, ">>> Starting WAIT_FOR_START_STATE <<<");
            currentFsmState = WAIT_FOR_START_STATE;
            update_led_event_mode(WAIT_FOR_START_EVENT);
            break;
        case INIT_SUPABASE_CONN_STATE:
            if (eNextState != currentFsmState)
                ESP_LOGI(TAG, ">>> Starting INIT_SUPABASE_CONN_STATE <<<");
            currentFsmState = INIT_SUPABASE_CONN_STATE;
            ret_code = spb_init_config();
            if (ret_code != ESP_OK) {
                ESP_LOGE(TAG, "Failed to config supabase https client!");
                eNextState = WAIT_FOR_START_STATE;
                break;
            }
            update_led_event_mode(SUCCESS_EVENT);
            ret_code = spb_start_connection();
            if (ret_code != ESP_OK) {
                ESP_LOGE(TAG, "Failed to start supabase https client data send!");
                eNextState = WAIT_FOR_START_STATE;
                break;
            }
            update_led_event_mode(SUCCESS_EVENT);
            eNextState = INIT_SAC_DM_ROUTINE_STATE;
            break;
        case INIT_SAC_DM_ROUTINE_STATE:
            if (eNextState != currentFsmState)
                ESP_LOGI(TAG, ">>> Starting INIT_SAC_DM_ROUTINE_STATE <<<");
            currentFsmState = INIT_SAC_DM_ROUTINE_STATE;
            ret_code = init_sacdm_routine_periodic_timer();
            if (ret_code != ESP_OK)
                eNextState = WAIT_FOR_START_STATE;
            eNextState = SAC_DM_DATA_COLLECTING_STATE;
            break;
        case SAC_DM_DATA_COLLECTING_STATE:
            if (eNextState != currentFsmState)
                ESP_LOGI(TAG, ">>> Starting SAC_DM_DATA_COLLECTING_STATE <<<");
            currentFsmState = SAC_DM_DATA_COLLECTING_STATE;
            if (get_sacdm_data_state() == READY_DATA) {
                eNextState = SEND_DATA_TO_SUPABASE_STATE;
            }
            break;
        case SEND_DATA_TO_SUPABASE_STATE:
            if (eNextState != currentFsmState)
                ESP_LOGI(TAG, ">>> Starting SEND_DATA_TO_SUPABASE_STATE <<<");
            currentFsmState = SEND_DATA_TO_SUPABASE_STATE;
            ret_code = spb_write_sacdm_data(create_sacdm_payload_body());
            set_sacdm_data_state(NOT_READY_DATA);
            eNextState = SAC_DM_DATA_COLLECTING_STATE;
            break;
        case EXIT_SUPABASE_CONN_STATE:
            if (eNextState != currentFsmState)
                ESP_LOGI(TAG, ">>> Starting EXIT_SUPABASE_CONN_STATE <<<");
            currentFsmState = EXIT_SUPABASE_CONN_STATE;
            ret_code = spb_close_connection();
            if (ret_code != ESP_OK) {
                ESP_LOGE(TAG, "Failed to close client connection with supabase!");
                eNextState = IDLE_STATE;
                break;
            }
            ret_code = sacdm_reset();
            if (ret_code != ESP_OK) {
                eNextState = IDLE_STATE;
                break;
            }
            update_led_event_mode(SUCCESS_EVENT);
            eNextState = WAIT_FOR_START_STATE;
            break;
        case IDLE_STATE:
            if (eNextState != currentFsmState)
                ESP_LOGI(TAG, ">>> Starting IDLE_STATE <<<");
            currentFsmState = IDLE_STATE;
            update_led_event_mode(IDLE_LED_STATE);
            break;
        default:
            ESP_LOGI(TAG, "Starting default state. Wrong event sent!");
            eNextState = IDLE_STATE;
            break;
        }
        vTaskDelay(5);
    }
}
