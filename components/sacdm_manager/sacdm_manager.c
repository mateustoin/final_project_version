#include "sacdm_manager.h"

// FreeRTOS libs
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <esp_log.h>

// Tratamento de JSON
#include "cJSON.h"

/*
    ************************************
    Definição de variáveis para o SAC-DM
    ************************************
*/
#define SAMPLE_SIZE 500

static float threshold = 1.20;
static int value = 0, readings = 0, peaks_x = 0, peaks_y = 0, peaks_z = 0;
static double rho_x, rho_y, rho_z;

static int16_t signals_x[3] = {0, 0, 0};
static int16_t signals_y[3] = {0, 0, 0};
static int16_t signals_z[3] = {0, 0, 0};

static raw_acc_val raw_val = {
    .accX = 0,
    .accY = 0,
    .accZ = 0
};

TaskHandle_t *sacdm_notification_emitter;

char *create_sacdm_payload_body(void)
{
    cJSON *value = cJSON_CreateObject();
    char sac_values[50] = "";
    sprintf(sac_values, "rX:%f,rY:%f,rZ:%f", rho_x, rho_y, rho_z);
    cJSON_AddStringToObject(value, "value", sac_values);
    char *payload_body = cJSON_Print(value);
    // ESP_LOGI("payload", "%s", payload_body);
    return payload_body;
}

void sacdm_init(TaskHandle_t *notify_handler)
{
    sacdm_notification_emitter = notify_handler;
    sacdm_acc_provider_init();
}

void sacdm_reset(void)
{
    value = 0; 
    readings = 0; 
    peaks_x = 0; 
    peaks_y = 0; 
    peaks_z = 0;
    rho_x = 0.0;
    rho_y = 0.0;
    rho_z = 0.0;
}

void sacdm_calculate()
{
    sacdm_acc_provider_read(&raw_val);

    signals_x[0] = signals_x[1];
    signals_x[1] = signals_x[2];
    signals_x[2] = raw_val.accX;

    signals_y[0] = signals_y[1];
    signals_y[1] = signals_y[2];
    signals_y[2] = raw_val.accY;

    signals_z[0] = signals_z[1];
    signals_z[1] = signals_z[2];
    signals_z[2] = raw_val.accZ;

    readings++;

    if (readings > 2) {
        if ((float)signals_x[1] > (float)signals_x[0]*threshold && (float)signals_x[1] > (float)signals_x[2]*threshold) peaks_x++;
        if ((float)signals_y[1] > (float)signals_y[0]*threshold && (float)signals_y[1] > (float)signals_y[2]*threshold) peaks_y++;
        if ((float)signals_z[1] > (float)signals_z[0]*threshold && (float)signals_z[1] > (float)signals_z[2]*threshold) peaks_z++;
    }

    if (readings == SAMPLE_SIZE) {
        rho_x = (float)peaks_x / (float)SAMPLE_SIZE;
        rho_y = (float)peaks_y / (float)SAMPLE_SIZE;
        rho_z = (float)peaks_z / (float)SAMPLE_SIZE;
        ESP_LOGI("sacdm_manager", "rho_x: %f, rho_y: %f, rho_z: %f", rho_x, rho_y, rho_z);
        xTaskNotifyGive(*sacdm_notification_emitter);
        readings = 1;
        peaks_x = 0;
        peaks_y = 0;
        peaks_z = 0;
    }
}