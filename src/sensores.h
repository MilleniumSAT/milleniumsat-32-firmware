#ifndef _SENSORES_H
#define _SENSORES_H

#include "Arduino.h"
#include "globaldef.h"
#include "PION_System.h"
#include "utils.h"
#include <Wire.h>
#include <SPI.h>

static System cubeSat;

class Sensores
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
   * @return String json com todos os sensores obtidos
   */
  String obtemJSON(void);

private:
  
};

#endif
