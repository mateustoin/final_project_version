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
        eSystemEvent eNewEvent = ReadEvent();
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
