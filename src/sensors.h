#ifndef _SENSORS_H
#define _SENSORS_H

#include "Arduino.h"
#include "globaldef.h"
#include "utils.h"
#include <Wire.h>
#include <SPI.h>

class Sensors
{
public:
  /**
   * Configura algumas portas do dispositivo
   * @param void sem parametros
   * @return uint8_t SUCESSO(1)
   */
  uint8_t configuraPortas(void);

  /**
   * Obtem os dados de todos os sensores
   * @param void sem parametros
   * @return uint8_t SUCESSO(1)
   */
  uint8_t obtemSensores(void);

private:
  
};

#endif
