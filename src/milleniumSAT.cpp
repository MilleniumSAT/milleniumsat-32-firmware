#include "Arduino.h"
#include "statemachine.h"

StateMachine stateMachine;

/**
 * Faz as configurações iniciais do sistema
 * @param void sem parametros
 * @return void sem retorno
 */
void setup(void)
{
  stateMachine.inicializaMilleniumSAT();
}

/**
 * Laço principal do sistema
 * @param void sem parametros
 * @return void sem retorno
 */
void loop(void)
{
  stateMachine.processaEstadoMaquina();
}
