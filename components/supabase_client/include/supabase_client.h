#pragma once

#include "esp_http_client.h"
#include "esp_log.h"

typedef struct {
    char *table_url;
    char *api_key;
} supabase_config;

esp_err_t on_client_data(esp_http_client_event_t *evt);
void fetch_data(void);

void spb_open(supabase_config *config_info);
void spb_init(void);
void spb_close(void);
void spb_read(void);
void spb_write(void *data);
// void spb_change_config(void *config_data);
