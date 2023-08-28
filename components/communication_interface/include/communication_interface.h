#pragma once

// includes

#ifdef __cplusplus
extern "C" {
#endif

extern void open_communication(void *config_info);    // Inicialização de todas as configurações do lado do ESP
extern void close_communication(void);
extern void read_data(void);
extern void write_data(void *data);
extern void change_config(void *config_data);


#ifdef __cplusplus
}
#endif