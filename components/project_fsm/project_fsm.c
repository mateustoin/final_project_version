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
eSystemEvent currentEvt;

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
            ESP_LOGI(TAG, "Entering Idle_State");
            if(Card_Insert_Event == eNewEvent)
            {
                eNextState = InsertCardHandler();
            }
            setSystemEvent(Pin_Enter_Event);
        break;
        case Card_Inserted_State:
            ESP_LOGI(TAG, "Entering Card_Inserted_State");
            if(Pin_Enter_Event == eNewEvent)
            {
                eNextState = EnterPinHandler();
            }
            setSystemEvent(Option_Selection_Event);
        break;
        case Pin_Eentered_State:
            ESP_LOGI(TAG, "Entering Pin_Eentered_State");
            if(Option_Selection_Event == eNewEvent)
            {
                eNextState = OptionSelectionHandler();
            }
            setSystemEvent(Amount_Enter_Event);
        break;
        case Option_Selected_State:
            ESP_LOGI(TAG, "Entering Option_Selected_State");
            if(Amount_Enter_Event == eNewEvent)
            {
                eNextState = EnterAmountHandler();
            }
            setSystemEvent(Amount_Dispatch_Event);
        break;
        case Amount_Entered_State:
            ESP_LOGI(TAG, "Entering Amount_Entered_State");
            if(Amount_Dispatch_Event == eNewEvent)
            {
                eNextState = AmountDispatchHandler();
            }
            setSystemEvent(Card_Insert_Event);
        break;
        default:
            ESP_LOGI(TAG, "Entering default state");
            break;
        }
        vTaskDelay(100);
    }
}

void runProjectFsm()
{
    project_states_t eNextState = INIT_WIFI_STATE;
    // eSystemEvent eNewEvent;
    // setSystemEvent(Card_Insert_Event);
    while(true) {
        esp_err_t ret_code;
        //Read system Events
        // eNewEvent = ReadEvent();
        switch(eNextState) {
        case INIT_WIFI_STATE:
            ESP_LOGI(TAG, "Entering INIT_WIFI_STATE");
            ret_code = wifi_config_sta();
            if (ret_code != ESP_OK) {
                ESP_LOGE(TAG, "Failed to init WiFi!");
                eNextState = IDLE_STATE;
                break;
            }
            eNextState = INIT_MPU6886_STATE;
        break;
        case INIT_MPU6886_STATE:
            ESP_LOGI(TAG, "Entering INIT_MPU6886_STATE");
            eNextState = INIT_GPIO_STATE;
        break;
        case INIT_GPIO_STATE:
            ESP_LOGI(TAG, "Entering INIT_GPIO_STATE");
            init_gpio_button_config();
            eNextState = WAIT_FOR_START_STATE;
        break;
        case WAIT_FOR_START_STATE:
            ESP_LOGI(TAG, "Entering WAIT_FOR_START_STATE");
        break;
        case INIT_SUPABASE_CONN_STATE:
            ESP_LOGI(TAG, "Entering INIT_SUPABASE_CONN_STATE");
        break;
        case INIT_SAC_DM_ROUTINE_STATE:
            ESP_LOGI(TAG, "Entering INIT_SAC_DM_ROUTINE_STATE");
            break;
        case SAC_DM_DATA_COLLECTING_STATE:
            ESP_LOGI(TAG, "Entering SAC_DM_DATA_COLLECTING_STATE");
            break;
        case SEND_DATA_TO_SUPABASE_STATE:
            ESP_LOGI(TAG, "Entering SEND_DATA_TO_SUPABASE_STATE");
            break;
        case EXIT_SUPABASE_CONN_STATE:
            ESP_LOGI(TAG, "Entering EXIT_SUPABASE_CONN_STATE");
            break;
        case IDLE_STATE:
            ESP_LOGI(TAG, "Entering IDLE_STATE");
            break;
        default:
            ESP_LOGI(TAG, "Entering default state");
            break;
        }
        vTaskDelay(100);
    }
}
