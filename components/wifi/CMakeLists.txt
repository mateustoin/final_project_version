idf_component_register(SRCS "connect.c" "connect_sta.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES nvs_flash esp_netif esp_wifi)
target_compile_options(${COMPONENT_LIB} PRIVATE  -Wno-format)