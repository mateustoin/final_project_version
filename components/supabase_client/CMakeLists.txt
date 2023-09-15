idf_component_register(SRCS "supabase_client.c"
                    INCLUDE_DIRS "include"
                    REQUIRES communication_interface
                    PRIV_REQUIRES esp_http_client nvs_flash esp_netif esp-tls)
target_compile_options(${COMPONENT_LIB} PRIVATE  -Wno-format)