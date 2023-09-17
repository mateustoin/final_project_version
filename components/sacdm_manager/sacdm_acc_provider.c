#include "sacdm_acc_provider.h"
#include <esp_log.h>
#include "hal/i2c_types.h"
#include "driver/i2c.h"

static const char* TAG = "sacdm_acc_provider";
static i2c_port_t port = I2C_NUM_0;
static raw_acc_val raw;

void sacdm_acc_provider_init(void)
{
    ESP_LOGI(TAG, "MPU6886 initialization...");
    mpu6886_init(&port);
    raw.accX = 0;
    raw.accY = 0;
    raw.accZ = 0;
}

esp_err_t sacdm_mpu6886_init(void)
{
    ESP_LOGI(TAG, "MPU6886 initialization...");
    raw.accX = 0;
    raw.accY = 0;
    raw.accZ = 0;
    return mpu6886_init(&port);
}

void sacdm_acc_provider_read(raw_acc_val* acc_val)
{
    // Atualiza dados da MPU em cada variável de aceleração
    esp_err_t err = mpu6886_adc_accel_get( &raw.accX, &raw.accY, &raw.accZ );

    // Checa se houve algum erro na coleta dos dados
    if (err == ESP_OK){
        acc_val->accX = raw.accX;
        acc_val->accY = raw.accY;
        acc_val->accZ = raw.accZ;
    }else{
        ESP_LOGE(TAG, "Couldn't get accelerometer values");
    }
}