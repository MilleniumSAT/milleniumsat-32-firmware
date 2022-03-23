#include "wifi.h"
#include <HTTPClient.h>
#include <WiFiManager.h>
#include "sensores.h"
#include <esp32fota.h>
static Utils utils;
WiFiClient client;
WiFiManager wifiManager;

char *ota_cert =
    "-----BEGIN CERTIFICATE-----"
    "MIIHMDCCBhigAwIBAgIQAkk+B/qeN1otu8YdlEMPzzANBgkqhkiG9w0BAQsFADBw\n"\
    "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"\
    "d3cuZGlnaWNlcnQuY29tMS8wLQYDVQQDEyZEaWdpQ2VydCBTSEEyIEhpZ2ggQXNz\n"\
    "dXJhbmNlIFNlcnZlciBDQTAeFw0yMDA1MDYwMDAwMDBaFw0yMjA0MTQxMjAwMDBa\n"\
    "MGoxCzAJBgNVBAYTAlVTMRMwEQYDVQQIEwpDYWxpZm9ybmlhMRYwFAYDVQQHEw1T\n"\
    "YW4gRnJhbmNpc2NvMRUwEwYDVQQKEwxHaXRIdWIsIEluYy4xFzAVBgNVBAMTDnd3\n"\
    "dy5naXRodWIuY29tMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAsj49\n"\
    "6jJ99veEXO7WdxGQZ7idtCnDcjZqQeDiy6057SwXj9yDUVnqhwo/yII8+y6Jpk3g\n"\
    "75LpPpYNjiOwYp/JkpWbpBAd1FWlvXJo/eZS+TwuIYb7JSc2H3NDDKt2VV5SSKQd\n"\
    "XOkDNqq7BisOFp2/TYwCMZboLufwRR5fKxL0nTKIOCwpnH8k//UdWpvTgIixDGLY\n"\
    "QCwHt0fYEo49jFeDaKD4WMBPq6Tx1iKWBhw3HVc/OyvI3yjRAx4Anf/DCSt9YTW6\n"\
    "f/ND4O/fOowcfW5T7zii1Kw0yw+ulBrE/xe6taVhL+QR0MXNkQV2iHNN85swidwM\n"\
    "tcdGI8g3fYL48bSRywIDAQABo4IDyjCCA8YwHwYDVR0jBBgwFoAUUWj/kK8CB3U8\n"\
    "zNllZGKiErhZcjswHQYDVR0OBBYEFIygCmlH3IkysE3GEUViXxovlk46MHsGA1Ud\n"\
    "EQR0MHKCDnd3dy5naXRodWIuY29tggwqLmdpdGh1Yi5jb22CCmdpdGh1Yi5jb22C\n"\
    "CyouZ2l0aHViLmlvgglnaXRodWIuaW+CFyouZ2l0aHVidXNlcmNvbnRlbnQuY29t\n"\
    "ghVnaXRodWJ1c2VyY29udGVudC5jb20wDgYDVR0PAQH/BAQDAgWgMB0GA1UdJQQW\n"\
    "MBQGCCsGAQUFBwMBBggrBgEFBQcDAjB1BgNVHR8EbjBsMDSgMqAwhi5odHRwOi8v\n"\
    "Y3JsMy5kaWdpY2VydC5jb20vc2hhMi1oYS1zZXJ2ZXItZzYuY3JsMDSgMqAwhi5o\n"\
    "dHRwOi8vY3JsNC5kaWdpY2VydC5jb20vc2hhMi1oYS1zZXJ2ZXItZzYuY3JsMEwG\n"\
    "A1UdIARFMEMwNwYJYIZIAYb9bAEBMCowKAYIKwYBBQUHAgEWHGh0dHBzOi8vd3d3\n"\
    "LmRpZ2ljZXJ0LmNvbS9DUFMwCAYGZ4EMAQICMIGDBggrBgEFBQcBAQR3MHUwJAYI\n"\
    "KwYBBQUHMAGGGGh0dHA6Ly9vY3NwLmRpZ2ljZXJ0LmNvbTBNBggrBgEFBQcwAoZB\n"\
    "aHR0cDovL2NhY2VydHMuZGlnaWNlcnQuY29tL0RpZ2lDZXJ0U0hBMkhpZ2hBc3N1\n"\
    "cmFuY2VTZXJ2ZXJDQS5jcnQwDAYDVR0TAQH/BAIwADCCAX0GCisGAQQB1nkCBAIE\n"\
    "ggFtBIIBaQFnAHYARqVV63X6kSAwtaKJafTzfREsQXS+/Um4havy/HD+bUcAAAFx\n"\
    "6y8fFgAABAMARzBFAiEA59y6w9oaoAoM2fvFq6KofYWRh0xRm4VEEaMHBtsBYUgC\n"\
    "IBZxJhjA7SGWUlo57YslG8u6clHngDNvoTNVw1HQtTr3AHUAIkVFB1lVJFaWP6Ev\n"\
    "8fdthuAjJmOtwEt/XcaDXG7iDwIAAAFx6y8evwAABAMARjBEAiBmEjiioTbc1//h\n"\
    "CInYIX6O8hph5oLRVGCTxrTBfSRT2wIgZz7x3ZNIKQkWPKOFaaW3AxcB0DzhFsD6\n"\
    "gxhkbl1p0AgAdgBRo7D1/QF5nFZtuDd4jwykeswbJ8v3nohCmg3+1IsF5QAAAXHr\n"\
    "Lx8JAAAEAwBHMEUCIBQ/6El+TCCtWuop7IderN0+byn5sDreTu+Xz3GiY8cLAiEA\n"\
    "7S83HxFFdQhQqpjjbWbIVBA88Nn/riaf5Jb8h3oJV8cwDQYJKoZIhvcNAQELBQAD\n"\
    "ggEBAADzu/I/4dMPwG4QzMFHZmgQFlnc/xqXtaNLqONIzXPznBQmHQi481xKgAR4\n"\
    "jZOTTknlwOLBXnDXvV6rJQZXut3pxHSvVJk2kvuyDO3RC0uudd81AXIUsd6Pnjt2\n"\
    "D6Xd/ypUAoMkyE+8euYESEFk4HlnrpXtN7OSTGVYZQk0aJrDINslXdmUL9E6AQiI\n"\
    "YaRIpRMRdj4stG6CkPJpfSauWa19kReZ6hTQR5f89L6x50us7GuWlmH6EmVFIbhf\n"\
    "9EO02QA3CcU7bE1iLWMHmKcU6ythmgsvNRU5TikxvF77JFv7n1/y8GLrprmKpB6Q\n"\
    "Df4PA8S9ROX9Rzgwe3KTIM6qeKU=\n"\
    "-----END CERTIFICATE-----\n";

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

uint8_t WifiMilleniumSAT::verificaAtualizacoes()
{
  WiFiClientSecure client2;
  esp32FOTA esp32FOTA("esp32-fota-http", V_FIRMWARE, false, true);

  uint8_t TIMETOUT_OTA = 180;
  utils.enviaMensagem("[UTILS] Verificando atualizações.", SERIAL_DEBUG, SEM_TOPICO);

  if (!verificaConexao())
  {
    utils.enviaMensagem("[UTILS] Sem conexão com a internet.", SERIAL_DEBUG, SEM_TOPICO);
    return ERRO;
  }

  esp32FOTA.checkURL = "https://raw.githubusercontent.com/UFG-INF-OBSAT/ota/main/firmware.json";

  bool shouldExecuteFirmwareUpdate = esp32FOTA.execHTTPcheck();
  if (shouldExecuteFirmwareUpdate)
  {
    utils.enviaMensagem("[UTILS] Atualização disponivel.", SERIAL_DEBUG, SEM_TOPICO);
    esp32FOTA.execOTA();
    while (TIMETOUT_OTA--)
      delay(1000);
  }
  else
  {
    utils.enviaMensagem("[UTILS] Nao ha atualização disponivel.", SERIAL_DEBUG, SEM_TOPICO);
  }

  return SUCESSO;
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