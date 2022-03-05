#ifndef _WIFIMILLENIUMSAT_H
#define _WIFIMILLENIUMSAT_H

#include "Arduino.h"
#include "utils.h"
#include <WiFi.h>


class WifiMilleniumSAT
{
public:
  String SSID_MILLENIUMSAT = "MilleniumSAT";
  String SENHA_MILLENIUMSAT = "12345678";

  /**
   * Inicia a comunicao WiFi
   * @param void sem parametro 
   * @return void sem retorno
   */
  void iniciaConexao(void);

  /**
   * Verifica a conexao WiFi
   * @param void sem parametro 
   * @return uint8_t SUCESSO(1) caso a conexao esteja ok e ERRO(0) caso contrario
   */
  uint8_t verificaConexao(void);

  /**
   * Reconecta ao WiFi
   * @param void sem parametro 
   * @return void sem retorno
   */
  void reconectaConexao(void);

  /**
   * Envia informacao via SOCKET
   * @param String conteudo - conteudo a ser enviado 
   * @param int porta - porta a ser enviada
   * @param String url - url a ser enviada 
   * @return uint8_t SUCESSO(1) caso o envio ocorra corretamente e ERRO(0) caso contrario
   */
  uint8_t enviaSocket(String conteudo, int porta, String url, bool forcaEnvio);

  /**
   * Reseta a credenciais do WiFi
   * @param void - sem parametro
   * @return void - sem retorno
   */
  void reiniciaCredenciaisWifi(void);

private:
};

#endif
