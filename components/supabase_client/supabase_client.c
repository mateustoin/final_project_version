#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <stdlib.h>
#include <ctype.h>

// inicio da copia
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_tls.h"
#if CONFIG_MBEDTLS_CERTIFICATE_BUNDLE
#include "esp_crt_bundle.h"
#endif

#if !CONFIG_IDF_TARGET_LINUX
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#endif
// fim da copia
#include "supabase_client.h"

#define MAX_HTTP_RECV_BUFFER 512
#define MAX_HTTP_OUTPUT_BUFFER 2048

static const char *TAG = "SUPABASE_CLIENT";

extern const char cert_pem_start[] asm("_binary_prod_ca_2021_crt_start");
extern const char cert_pem_end[]   asm("_binary_prod_ca_2021_crt_end");

static char auth[250] = "Bearer ";
static esp_http_client_handle_t client;

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    static char *output_buffer;  // Buffer to store response of http request from event handler
    static int output_len;       // Stores number of bytes read
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            /*
             *  Check for chunked encoding is added as the URL for chunked encoding used in this example returns binary data.
             *  However, event handler can also be used in case chunked encoding is used.
             */
            if (!esp_http_client_is_chunked_response(evt->client)) {
                // If user_data buffer is configured, copy the response into the buffer
                int copy_len = 0;
                if (evt->user_data) {
                    copy_len = MIN(evt->data_len, (MAX_HTTP_OUTPUT_BUFFER - output_len));
                    if (copy_len) {
                        memcpy(evt->user_data + output_len, evt->data, copy_len);
                    }
                } else {
                    const int buffer_len = esp_http_client_get_content_length(evt->client);
                    if (output_buffer == NULL) {
                        output_buffer = (char *) malloc(buffer_len);
                        output_len = 0;
                        if (output_buffer == NULL) {
                            ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
                            return ESP_FAIL;
                        }
                    }
                    copy_len = MIN(evt->data_len, (buffer_len - output_len));
                    if (copy_len) {
                        memcpy(output_buffer + output_len, evt->data, copy_len);
                    }
                }
                output_len += copy_len;
            }

            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            if (output_buffer != NULL) {
                // Response is accumulated in output_buffer. Uncomment the below line to print the accumulated response
                // ESP_LOG_BUFFER_HEX(TAG, output_buffer, output_len);
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            int mbedtls_err = 0;
            esp_err_t err = esp_tls_get_and_clear_last_error((esp_tls_error_handle_t)evt->data, &mbedtls_err, NULL);
            if (err != 0) {
                ESP_LOGI(TAG, "Last esp error code: 0x%x", err);
                ESP_LOGI(TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
            }
            if (output_buffer != NULL) {
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
        case HTTP_EVENT_REDIRECT:
            ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
            esp_http_client_set_header(evt->client, "From", "user@example.com");
            esp_http_client_set_header(evt->client, "Accept", "text/html");
            esp_http_client_set_redirection(evt->client);
            break;
    }
    return ESP_OK;
}

void spb_open(supabase_config *config_info)
{
    char local_response_buffer[MAX_HTTP_OUTPUT_BUFFER + 1] = {0};
    supabase_config *config = (supabase_config*)config_info;
    esp_http_client_config_t esp_http_client_config = {
        // .url = "https://nuopbiwoomjqqfgdasxh.supabase.co/rest/v1/Teste2",
        .url = config->table_url,
        .method = HTTP_METHOD_POST,
        .event_handler = _http_event_handler,
        .user_data = local_response_buffer,
        // .client_cert_pem = cert_pem_start,
        .buffer_size = 20000,              /*!< HTTP receive buffer size */
        .buffer_size_tx = 20000           /*!< HTTP transmit buffer size */
    };

    ESP_LOGI(TAG, "Inicializando Supabase (HTTP) Client");
    strcat(auth, config->api_key);
    client = esp_http_client_init(&esp_http_client_config);
    esp_http_client_set_url(client, "https://nuopbiwoomjqqfgdasxh.supabase.co/rest/v1/Teste2?");
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_header(client, "Prefer", "return=minimal");
    esp_http_client_set_header(client, "apikey", config->api_key);
    esp_http_client_set_header(client, "Authorization", auth);
}

void spb_init(void)
{
    ESP_LOGI(TAG, "Inicializando Supabase (HTTP) Client");
    // strcat(auth, config.api_key);
    // client = esp_http_client_init(&esp_http_client_config);
    // esp_http_client_set_header(client, "Content-Type", "application/json");
    // esp_http_client_set_header(client, "Prefer", "return=representation");
    // esp_http_client_set_header(client, "apikey", config.api_key);
    // esp_http_client_set_header(client, "Authorization", auth);
}

void spb_close(void)
{
    esp_http_client_cleanup(client);
}

void spb_read(void)
{
    ESP_LOGI(TAG, "Reading data from Supabase...");
}

void spb_write(void *data)
{
    char *payload_body = (char*) data;
    esp_http_client_set_post_field(client, payload_body, strlen(payload_body));
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Successfully POST sent. Status = %d, Content_length = %d",
                    esp_http_client_get_status_code(client),
                    esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE(TAG, "POST request failed: %s", esp_err_to_name(err));
    }
}

// esp_err_t on_client_data(esp_http_client_event_t *evt)
// {
//     switch(evt->event_id) {
//         // Quando chega dados
//         case HTTP_EVENT_ON_DATA:
//             // ESP_LOGI(TAG, "Length=%d", evt->data_len);
//             // printf("%.*s\n", evt->data_len, (char *)evt->data);
//             break;
//         default:
//             break;
//     }

//     return ESP_OK;
// }

void fetch_data(void)
{
    esp_http_client_config_t esp_http_client_config = {
        .url = "",
        .method = HTTP_METHOD_GET,
        .event_handler = on_client_data,
        .buffer_size = 15000,              /*!< HTTP receive buffer size */
        // .buffer_size_tx = 10000           /*!< HTTP transmit buffer size */
    };

    esp_http_client_handle_t client = esp_http_client_init(&esp_http_client_config);
    esp_http_client_set_header(client, "apikey", "");
    esp_http_client_set_header(client, "Authorization", "Bearer ");
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK){
        ESP_LOGI("HTTP_Client", "HTTP GET status = %d, content_length = %d",
                    esp_http_client_get_status_code(client),
                    esp_http_client_get_content_length(client));
    }else{
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
    // wifi_disconnect();
}
