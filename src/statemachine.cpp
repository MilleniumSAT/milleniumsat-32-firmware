#include "statemachine.h"

void StateMachine::inicializaMilleniumSAT()
{
  utils.iniciaComunicacaoSerial();
  utils.enviaMensagem("[UTILS] Iniciando MilleniumSAT.", SERIAL_DEBUG, SEM_TOPICO);
  utils.obtemValoresMemoria();
  iniciaProcessos();
  wifiMilleniumSAT.iniciaConexao();
  utils.enviaMensagem("[UTILS] MilleniumSAT iniciado com sucesso.", SERIAL_DEBUG, SEM_TOPICO);
}

void StateMachine::iniciaProcessos()
{
  utils.enviaMensagem("[UTILS] Iniciando processos necessarios.", SERIAL_DEBUG, SEM_TOPICO);
  sensores.configuraPortas();
  task.configuraTasks();
  utils.enviaMensagem("[UTILS] Processos necessarios iniciados.", SERIAL_DEBUG, SEM_TOPICO);
}

void StateMachine::processaEstadoMaquina()
{
  

  switch (state_s1)
  {

  case STATE_0:
    utils.LED_STATE = OBTENDO_SENSORES;
    utils.enviaMensagem(sensores.obtemJSON(), POST_DEBUG, SEM_TOPICO);

    r = SUCESSO;

    (r == SUCESSO) ? (state_s1 = STATE_1) : (state_s1 = STATE_0);
    break;

  case STATE_1:
    utils.executaVerificacoes();
    r = SUCESSO;
    (r == SUCESSO) ? (state_s1 = STATE_2) : (state_s1 = STATE_1);
    break;

  case STATE_2:
    utils.LED_STATE = HIBERNANDO;
    utils.aguardaProximoEnvio();
    r = SUCESSO;
    (r == SUCESSO) ? (state_s1 = STATE_0) : (state_s1 = STATE_2);
    break;

  default:
    break;
  }
}