#pragma once

#include <stdio.h>

#include "sacdm_acc_provider.h"

typedef enum {
    NOT_READY_DATA,
    READY_DATA
} sacdm_data_state_t;

void sacdm_init(TaskHandle_t *notify_handler);
void sacdm_reset(void);
void sacdm_calculate();
char *create_sacdm_payload_body(void);

void init_sacdm_routine_periodic_timer();
void stop_sacdm_routine_periodic_timer();
void sacdm_periodic_calculate();

void set_sacdm_data_state(sacdm_data_state_t dState);
sacdm_data_state_t get_sacdm_data_state();