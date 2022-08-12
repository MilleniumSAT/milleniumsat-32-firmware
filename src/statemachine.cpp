#include "statemachine.h"

String pacoteParaEnvio = "";
unsigned long lastConnectionTime = 240000L;         //Tempo da última conexão.
const unsigned long postingInterval = 240000L; //Tempo de postagem, 4 mintuos.

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
  setCpuFrequencyMhz(80);
  utils.enviaMensagem("[UTILS] Processos necessarios iniciados.", SERIAL_DEBUG, SEM_TOPICO);
}

void StateMachine::processaEstadoMaquina()
{

  switch (state_s1)
  {

  case STATE_0:
    utils.LED_STATE = OBTENDO_SENSORES;
    pacoteParaEnvio = sensores.obtemJSON();

    if (millis() - lastConnectionTime > postingInterval)
    {
      utils.enviaMensagem("[UTILS] Timer atingiu seu tempo limite. Iniciando envio do pacote para o servidor.", SERIAL_DEBUG, SEM_TOPICO);
      lastConnectionTime = millis();
      r = SUCESSO;
    }
    else
    {
      utils.enviaMensagem("[UTILS] Timer ainda nao atingiu seu tempo limite. Aguardando 30 segundos.", SERIAL_DEBUG, SEM_TOPICO);
      r = AGUARDA;
      delay(30000);
    }

    (r == SUCESSO) ? (state_s1 = STATE_1) : (state_s1 = STATE_0);
    break;

  case STATE_1:
    utils.LED_STATE = OBTENDO_SENSORES;
    utils.enviaMensagem(pacoteParaEnvio, POST_DEBUG, SEM_TOPICO);

    r = SUCESSO;

    (r == SUCESSO) ? (state_s1 = STATE_2) : (state_s1 = STATE_0);
    break;

  case STATE_2:
    utils.executaVerificacoes();
    r = SUCESSO;
    (r == SUCESSO) ? (state_s1 = STATE_0) : (state_s1 = STATE_0);
    break;

  case STATE_3:
    utils.LED_STATE = HIBERNANDO;
    utils.aguardaProximoEnvio();
    r = SUCESSO;
    (r == SUCESSO) ? (state_s1 = STATE_0) : (state_s1 = STATE_2);
    break;

  default:
    break;
  }
}