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

eSystemState AmountDispatchHandler()
{
    return Idle_State;
}

eSystemState EnterAmountHandler()
{
    return Amount_Entered_State;
}

eSystemState OptionSelectionHandler()
{
    return Option_Selected_State;
}

eSystemState EnterPinHandler()
{
    return Pin_Eentered_State;
}

eSystemState InsertCardHandler()
{
    return Card_Inserted_State;
}

void setSystemEvent(eSystemEvent evt)
{
    currentEvt = evt;
}

eSystemEvent ReadEvent()
{
    return currentEvt;
}

void runFsm()
{
    eSystemState eNextState = Idle_State;
    eSystemEvent eNewEvent;
    setSystemEvent(Card_Insert_Event);
    while(true) {
        //Read system Events
        eNewEvent = ReadEvent();
        switch(eNextState) {
        case Idle_State:
            ESP_LOGI(TAG, "Starting Idle_State");
            if(Card_Insert_Event == eNewEvent)
            {
                eNextState = InsertCardHandler();
            }
            setSystemEvent(Pin_Enter_Event);
        break;
        case Card_Inserted_State:
            ESP_LOGI(TAG, "Starting Card_Inserted_State");
            if(Pin_Enter_Event == eNewEvent)
            {
                eNextState = EnterPinHandler();
            }
            setSystemEvent(Option_Selection_Event);
        break;
        case Pin_Eentered_State:
            ESP_LOGI(TAG, "Starting Pin_Eentered_State");
            if(Option_Selection_Event == eNewEvent)
            {
                eNextState = OptionSelectionHandler();
            }
            setSystemEvent(Amount_Enter_Event);
        break;
        case Option_Selected_State:
            ESP_LOGI(TAG, "Starting Option_Selected_State");
            if(Amount_Enter_Event == eNewEvent)
            {
                eNextState = EnterAmountHandler();
            }
            setSystemEvent(Amount_Dispatch_Event);
        break;
        case Amount_Entered_State:
            ESP_LOGI(TAG, "Starting Amount_Entered_State");
            if(Amount_Dispatch_Event == eNewEvent)
            {
                eNextState = AmountDispatchHandler();
            }
            setSystemEvent(Card_Insert_Event);
        break;
        default:
            ESP_LOGI(TAG, "Starting default state");
            break;
        }
        vTaskDelay(100);
    }
}

void runProjectFsm()
{
    eNextState = INIT_GPIO_STATE;
    
    while(true) {
        esp_err_t ret_code;
        //Read system Events
        // eNewEvent = ReadEvent();
        switch(eNextState) {
        case INIT_GPIO_STATE:
            ESP_LOGI(TAG, "Starting INIT_GPIO_STATE");
            currentFsmState = INIT_GPIO_STATE;
            ret_code = init_gpio_main_button_config();
            if (ret_code != ESP_OK) {
                ESP_LOGE(TAG, "Failed to configure Main Button GPIO!");
                eNextState = IDLE_STATE;
                break;
            }
            eNextState = INIT_WIFI_STATE;
            break;
        case INIT_WIFI_STATE:
            ESP_LOGI(TAG, "Starting INIT_WIFI_STATE");
            currentFsmState = INIT_GPIO_STATE;
            ret_code = wifi_config_sta();
            if (ret_code != ESP_OK) {
                ESP_LOGE(TAG, "Failed to init WiFi!");
                eNextState = IDLE_STATE;
                break;
            }
            eNextState = INIT_MPU6886_STATE;
            break;
        case INIT_MPU6886_STATE:
            ESP_LOGI(TAG, "Starting INIT_MPU6886_STATE");
            currentFsmState = INIT_MPU6886_STATE;
            // Code...
            eNextState = WAIT_FOR_START_STATE;
            break;
        case WAIT_FOR_START_STATE:
            ESP_LOGI(TAG, "Starting WAIT_FOR_START_STATE");
            currentFsmState = WAIT_FOR_START_STATE;
            // Code...
            break;
        case INIT_SUPABASE_CONN_STATE:
            ESP_LOGI(TAG, "Starting INIT_SUPABASE_CONN_STATE");
            currentFsmState = INIT_SUPABASE_CONN_STATE;
            // Code...
            eNextState = INIT_SAC_DM_ROUTINE_STATE;
            break;
        case INIT_SAC_DM_ROUTINE_STATE:
            ESP_LOGI(TAG, "Starting INIT_SAC_DM_ROUTINE_STATE");
            currentFsmState = INIT_SAC_DM_ROUTINE_STATE;
            // Code...
            eNextState = SAC_DM_DATA_COLLECTING_STATE;
            break;
        case SAC_DM_DATA_COLLECTING_STATE:
            ESP_LOGI(TAG, "Starting SAC_DM_DATA_COLLECTING_STATE");
            currentFsmState = SAC_DM_DATA_COLLECTING_STATE;
            // Code...
            eNextState = SEND_DATA_TO_SUPABASE_STATE;
            break;
        case SEND_DATA_TO_SUPABASE_STATE:
            ESP_LOGI(TAG, "Starting SEND_DATA_TO_SUPABASE_STATE");
            currentFsmState = SEND_DATA_TO_SUPABASE_STATE;
            // Code...
            eNextState = EXIT_SUPABASE_CONN_STATE;
            break;
        case EXIT_SUPABASE_CONN_STATE:
            ESP_LOGI(TAG, "Starting EXIT_SUPABASE_CONN_STATE");
            currentFsmState = EXIT_SUPABASE_CONN_STATE;
            // Code...
            eNextState = IDLE_STATE;
            break;
        case IDLE_STATE:
            ESP_LOGI(TAG, "Starting IDLE_STATE");
            currentFsmState = IDLE_STATE;
            // Code...
            break;
        default:
            ESP_LOGI(TAG, "Starting default state");
            break;
        }
        vTaskDelay(100);
    }
}
