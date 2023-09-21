#pragma once

#include <stdio.h>

typedef enum {
    INIT_WIFI_STATE,
    INIT_MPU6886_STATE,
    INIT_GPIO_STATE,
    WAIT_FOR_START_STATE,
    INIT_SUPABASE_CONN_STATE,
    INIT_SAC_DM_ROUTINE_STATE,
    SAC_DM_DATA_COLLECTING_STATE,
    SEND_DATA_TO_SUPABASE_STATE,
    EXIT_SUPABASE_CONN_STATE,
    IDLE_STATE
} project_states_t;

//Different state of ATM machine
typedef enum {
    Idle_State,
    Card_Inserted_State,
    Pin_Eentered_State,
    Option_Selected_State,
    Amount_Entered_State,
} eSystemState;

//Different type events
typedef enum {
    Card_Insert_Event,
    Pin_Enter_Event,
    Option_Selection_Event,
    Amount_Enter_Event,
    Amount_Dispatch_Event
} eSystemEvent;

void run_project_fsm();

void fsm_set_current_state(project_states_t newState);
void fsm_set_next_state(project_states_t newState);
project_states_t fsm_get_current_state();
eSystemEvent ReadEvent();
