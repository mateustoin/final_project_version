#pragma once

#include "esp_http_client.h"
#include "esp_log.h"

typedef struct {
    char *table_url;
    char *api_key;
} supabase_config;

esp_err_t spb_init_config(void);
esp_err_t spb_start_connection(void);
esp_err_t spb_close_connection(void);
esp_err_t spb_write_sacdm_data(void *data);