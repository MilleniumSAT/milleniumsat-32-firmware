#include "wifi.h"
#include <HTTPClient.h>
#include <WiFiManager.h>
static Utils utils;
WiFiClient client;
WiFiManager wifiManager;

void WifiMilleniumSAT::iniciaConexao()
{

  utils.LED_STATE = WIFI;
  WiFi.setAutoConnect(true);
  wifiManager.setTimeout(80);
  wifiManager.setBreakAfterConfig(true);
  wifiManager.setConfigPortalTimeout(80);

  if (!wifiManager.autoConnect("MilleniumSAT", "12345678"))
  {
    Serial.println("[ERRO] Falhou para se conectar... Reiniciando.");
    utils.reiniciaMilleniumSAT();
  }

}

uint8_t WifiMilleniumSAT::requisicaoPOST(String json)
{

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("[WIFI] Realizando envio de pacote para o servidor.");

    HTTPClient http;

    String parametros = String(SERVIDOR) + "?json=" + json;
    http.begin(parametros);

    http.addHeader("Content-Type", "application/json");

    Serial.println(parametros);
    int httpResponseCode = http.GET();

    if (httpResponseCode == 200)
    {
      Serial.println("[WIFI] Envio realizado com sucesso.");
      http.end();
      return SUCESSO;
    }

    Serial.println("[ERRO] Falha ao enviar pacote.");
    return ERRO;
  }
  else
  {
    Serial.println("[ERRO] Falhou ao enviar json com conteudo.");
    return ERRO;
  }
}

uint8_t WifiMilleniumSAT::verificaConexao()
{
  uint8_t TIMEOUT_CONEXAO_WIFI = 30;
  while (WiFi.status() != WL_CONNECTED && TIMEOUT_CONEXAO_WIFI)
  {
    reconectaConexao();
    TIMEOUT_CONEXAO_WIFI--;
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    utils.enviaMensagem("[WIFI] Conexao com o MilleniumSAT estabelecida.", SERIAL_DEBUG, SEM_TOPICO);
    return SUCESSO;
  }
  else
  {
    utils.enviaMensagem("[WIFI] Falha ao se conectar com o WiFi.", SERIAL_DEBUG, SEM_TOPICO);
    return ERRO;
  }
}

void WifiMilleniumSAT::reconectaConexao()
{
  utils.enviaMensagem("[WIFI] Tentando reconectar a rede WiFi.", SERIAL_DEBUG, SEM_TOPICO);
  WiFi.disconnect();
  utils.aguardaTempo(1);
  WiFi.reconnect();
}

uint8_t WifiMilleniumSAT::enviaSocket(String conteudo, int porta, String url, bool forcaEnvio)
{

  if (client.connect(url.c_str(), porta))
  {
    client.print(conteudo);
    client.stop();
    utils.enviaMensagem("[WIFI] Mensagem enviada via Socket.", SERIAL_DEBUG, SEM_TOPICO);
    return SUCESSO;
  }
  else
  {
    utils.enviaMensagem("[WIFI] Falha ao enviar mensagem via Socket.", SERIAL_DEBUG, SEM_TOPICO);
    return ERRO;
  }
}

void WifiMilleniumSAT::reiniciaCredenciaisWifi()
{
  utils.enviaMensagem("[TASKS] Reiniciando credenciais Wifi", SERIAL_DEBUG, SEM_TOPICO);
  utils.enviaMensagem("[TASKS] Credenciais Wifi reiniciadas", SERIAL_DEBUG, SEM_TOPICO);
}