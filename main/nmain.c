#include <stdio.h>
#include <string.h>

// Bibliotecas do projeto
#include "project_fsm.h"
#include "gpio_button.h"

void app_main(void)
{
    init_gpio_button_config();
    runFsm();
}
