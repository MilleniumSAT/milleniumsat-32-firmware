#ifndef _STATEMACHINE_H
#define _STATEMACHINE_H

#include "Arduino.h"
#include "globaldef.h"
#include "utils.h"
#include "wifi.h"
#include "sensores.h"
#include "task.h"
class StateMachine
{
public:
  uint8_t state_s1;
  uint8_t state_prev_s1;
  uint8_t pin_s1;
  uint8_t val_s1;
  uint8_t r;

  /**
   * Inicializa o airpure chamando todas as funções necessárias
   * @param void sem parametros
   * @return void sem retorno
   */
  void inicializaMilleniumSAT(void);

  /**
   * Processa os estados do MilleniumSAT
   * @param void sem parametros
   * @return void sem retorno
   */
  void processaEstadoMaquina(void);

  /**
   * Inicia processos que são necessários na inicialização
   * @param void sem parametros
   * @return void sem retorno
   */
  void iniciaProcessos(void);

private:
  WifiMilleniumSAT wifiMilleniumSAT;
  Sensores sensores;
  Utils utils;
  Tasks task;
};

#endif
