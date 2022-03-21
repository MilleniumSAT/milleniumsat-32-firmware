#include "wifi.h"
#include <HTTPClient.h>
#include <WiFiManager.h>
#include "sensores.h"
#include <esp32fota.h>
static Utils utils;
WiFiClient client;
WiFiManager wifiManager;

char* ota_cert= \
  "-----BEGIN CERTIFICATE-----\n"  
  "MIIDxTCCAq2gAwIBAgIQAqxcJmoLQJuPC3nyrkYldzANBgkqhkiG9w0BAQUFADBsMQswCQYDVQQG\n"  
  "EwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3d3cuZGlnaWNlcnQuY29tMSsw\n"  
  "KQYDVQQDEyJEaWdpQ2VydCBIaWdoIEFzc3VyYW5jZSBFViBSb290IENBMB4XDTA2MTExMDAwMDAw\n"  
  "MFoXDTMxMTExMDAwMDAwMFowbDELMAkGA1UEBhMCVVMxFTATBgNVBAoTDERpZ2lDZXJ0IEluYzEZ\n"  
  "MBcGA1UECxMQd3d3LmRpZ2ljZXJ0LmNvbTErMCkGA1UEAxMiRGlnaUNlcnQgSGlnaCBBc3N1cmFu\n"  
  "Y2UgRVYgUm9vdCBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMbM5XPm+9S75S0t\n"  
  "Mqbf5YE/yc0lSbZxKsPVlDRnogocsF9ppkCxxLeyj9CYpKlBWTrT3JTWPNt0OKRKzE0lgvdKpVMS\n"  
  "OO7zSW1xkX5jtqumX8OkhPhPYlG++MXs2ziS4wblCJEMxChBVfvLWokVfnHoNb9Ncgk9vjo4UFt3\n"  
  "MRuNs8ckRZqnrG0AFFoEt7oT61EKmEFBIk5lYYeBQVCmeVyJ3hlKV9Uu5l0cUyx+mM0aBhakaHPQ\n"  
  "NAQTXKFx01p8VdteZOE3hzBWBOURtCmAEvF5OYiiAhF8J2a3iLd48soKqDirCmTCv2ZdlYTBoSUe\n"  
  "h10aUAsgEsxBu24LUTi4S8sCAwEAAaNjMGEwDgYDVR0PAQH/BAQDAgGGMA8GA1UdEwEB/wQFMAMB\n"  
  "Af8wHQYDVR0OBBYEFLE+w2kD+L9HAdSYJhoIAu9jZCvDMB8GA1UdIwQYMBaAFLE+w2kD+L9HAdSY\n"  
  "JhoIAu9jZCvDMA0GCSqGSIb3DQEBBQUAA4IBAQAcGgaX3NecnzyIZgYIVyHbIUf4KmeqvxgydkAQ\n"  
  "V8GK83rZEWWONfqe/EW1ntlMMUu4kehDLI6zeM7b41N5cdblIZQB2lWHmiRk9opmzN6cN82oNLFp\n"  
  "myPInngiK3BD41VHMWEZ71jFhS9OMPagMRYjyOfiZRYzy78aG6A9+MpeizGLYAiJLQwGXFK3xPkK\n"  
  "mNEVX58Svnw2Yzi9RKR/5CYrCsSXaQ3pjOLAEFe4yHYSkVXySGnYvCoCWw9E1CAx2/S6cCZdkGCe\n"  
  "vEsXCS+0yx5DaMkHJ8HSXPfqIbloEpw8nL+e/IBcm2PN7EeqJSdnoDfzAIJ9VNep+OkuE6N36B9K\n" 
  "-----END CERTIFICATE-----\n" ;

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
  secureEsp32FOTA secureEsp32FOTA("esp32-fota-https", V_FIRMWARE);

  uint8_t TIMETOUT_OTA = 180;
  utils.enviaMensagem("[UTILS] Verificando atualizações.", SERIAL_DEBUG, SEM_TOPICO);

  if (!verificaConexao())
  {
    utils.enviaMensagem("[UTILS] Sem conexão com a internet.", SERIAL_DEBUG, SEM_TOPICO);
    return ERRO;
  }

  secureEsp32FOTA._host="google.com"; //e.g. example.com
  secureEsp32FOTA._descriptionOfFirmwareURL = "/UFG-INF-OBSAT/ota/master/firmware.json";
  secureEsp32FOTA._certificate = ota_cert;
  secureEsp32FOTA.clientForOta = client2;

  bool shouldExecuteFirmwareUpdate = secureEsp32FOTA.execHTTPSCheck();
  if (shouldExecuteFirmwareUpdate)
  {
    utils.enviaMensagem("[UTILS] Atualização disponivel.", SERIAL_DEBUG, SEM_TOPICO);
    secureEsp32FOTA.executeOTA();
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