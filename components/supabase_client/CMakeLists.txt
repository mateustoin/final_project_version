idf_component_register(SRCS "supabase_client.c"
                    INCLUDE_DIRS "include"
                    REQUIRES communication_interface
                    PRIV_REQUIRES esp_http_client nvs_flash esp_netif esp-tls
                    EMBED_TXTFILES "prod_ca_2021.crt")
target_compile_options(${COMPONENT_LIB} PRIVATE  -Wno-format)