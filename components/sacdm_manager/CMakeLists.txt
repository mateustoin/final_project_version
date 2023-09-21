idf_component_register(SRCS "sacdm_acc_provider.c" "sacdm_manager.c"
                    INCLUDE_DIRS "include"
                    REQUIRES MPU6886-idf i2c_manager json esp_timer)
target_compile_options(${COMPONENT_LIB} PRIVATE  -Wno-format)