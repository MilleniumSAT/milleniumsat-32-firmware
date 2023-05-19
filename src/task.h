#ifndef _TASKS_H
#define _TASKS_H

#include "Arduino.h"
#include "globaldef.h"
#include "utils.h"
#include <i2cdetect.h>
#include <esp_task_wdt.h>
#include "wifi.h"
#include "PION_System.h"

class Tasks
{
public:
    String input;
    /**
     * Cria todas as tasks necessarias
     * @param void sem parametro
     * @return uint8_t SUCESSO(1)
     */
    uint8_t configuraTasks(void);

private:
};

#endif
