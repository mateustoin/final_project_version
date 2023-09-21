idf_component_register(SRCS "project_fsm.c"
                    INCLUDE_DIRS "include"
                    PRIV_REQUIRES gpio_manager wifi sacdm_manager supabase_client esp_timer)
target_compile_options(${COMPONENT_LIB} PRIVATE  -Wno-format)