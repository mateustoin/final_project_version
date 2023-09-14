idf_component_register(
    SRCS "mpu6886.c"
    INCLUDE_DIRS "."
    REQUIRES i2c_manager
)
target_compile_options(${COMPONENT_LIB} PRIVATE  -Wno-format)