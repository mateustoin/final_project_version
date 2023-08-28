#pragma once

#include <stdio.h>

#include "sacdm_acc_provider.h"

void sacdm_init(TaskHandle_t *notify_handler);
void sacdm_reset(void);
void sacdm_calculate();
char *create_sacdm_payload_body(void);
