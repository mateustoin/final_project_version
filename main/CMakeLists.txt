idf_component_register(SRCS "nmain.c"
                    INCLUDE_DIRS "."
                    REQUIRES project_fsm gpio_manager)
target_compile_options(${COMPONENT_LIB} PRIVATE  -Wno-format)