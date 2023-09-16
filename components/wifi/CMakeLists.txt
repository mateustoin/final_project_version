idf_component_register(SRCS "connect.c" "connect_sta.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp_wifi nvs_flash esp_netif spi_flash)
target_compile_options(${COMPONENT_LIB} PRIVATE  -Wno-format)