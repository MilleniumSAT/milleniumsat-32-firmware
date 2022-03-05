#include "sensors.h"

static Utils utils;

uint8_t Sensors::obtemSensores()
{

  utils.enviaMensagem("[SENSORS] Iniciando leitura dos sensores.", SERIAL_DEBUG, SEM_TOPICO);


  utils.enviaMensagem("[SENSORS] Leitura dos sensores finalizada.", SERIAL_DEBUG, SEM_TOPICO);

  return SUCESSO;
}

uint8_t Sensors::configuraPortas()
{
  pinMode(ledPino, OUTPUT);
  Wire.begin();

  return SUCESSO;
}