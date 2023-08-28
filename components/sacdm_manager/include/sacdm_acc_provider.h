#pragma once

#include <stdio.h>
#include "mpu6886.h"

typedef struct {
    int16_t accX;
    int16_t accY;
    int16_t accZ;
} raw_acc_val;

void sacdm_acc_provider_init(void);
void sacdm_acc_provider_read(raw_acc_val* acc_val);
