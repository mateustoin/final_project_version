#pragma once

#include <stdio.h>

#include "sacdm_acc_provider.h"

typedef enum {
    NOT_READY_DATA,
    READY_DATA
} sacdm_data_state_t;

esp_err_t sacdm_reset(void);
char *create_sacdm_payload_body(void);

esp_err_t init_sacdm_routine_periodic_timer();
esp_err_t stop_sacdm_routine_periodic_timer();
void sacdm_periodic_calculate();

void set_sacdm_data_state(sacdm_data_state_t dState);
sacdm_data_state_t get_sacdm_data_state();
