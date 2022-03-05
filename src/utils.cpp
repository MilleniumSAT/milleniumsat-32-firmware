#include "utils.h"
#include <ArduinoJson.h>
StaticJsonDocument<300> pacoteEnvio;
secureEsp32FOTA secureEsp32FOTA("esp32-fota-https", V_FIRMWARE);
WiFiClientSecure client2;
WifiMilleniumSAT wifiMilleniumSAT;
Tasks task;
Sensors sensors;

uint8_t MILLENIUMSAT_ID;

void Utils::reiniciaMilleniumSAT()
{
  ESP.restart();
}

uint8_t Utils::obtemValoresMemoria()
{
  NVS.begin();
  MILLENIUMSAT_ID = NVS.getString("id").toInt();

  return SUCESSO;
}

void Utils::iniciaComunicacaoSerial()
{
  Serial.begin(VEL_SERIAL_DEBUG);
  Serial2.begin(VEL_SERIAL_2, SERIAL_8N1, RXD2, TXD2);
}

void Utils::enviaMensagem(String conteudo, uint8_t whereTo, String topico = SEM_TOPICO)
{
  switch (whereTo)
  {
  case SERIAL_DEBUG:
    Serial.println(conteudo);
    break;

  case SOCKET_DEBUG:
    wifiMilleniumSAT.enviaSocket(conteudo, PORTA_SOCKET, SERVIDOR_SOCKET, false);
    break;
  }
}

String Utils::getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++)
  {
    if (data.charAt(i) == separator || i == maxIndex)
    {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void Utils::aguardaTempo(int times)
{
  while (times--)
  {
    delay(1000);
  }
}

float Utils::geraMapaVariacao(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

uint8_t Utils::verificaAtualizacoes()
{

  uint8_t TIMETOUT_OTA = 180;
  enviaMensagem("[UTILS] Verificando atualizações.", SERIAL_DEBUG, SEM_TOPICO);

  if (!wifiMilleniumSAT.verificaConexao())
  {
    enviaMensagem("[UTILS] Sem conexão com a internet.", SERIAL_DEBUG, SEM_TOPICO);
    return ERRO;
  }

  secureEsp32FOTA._host = "raw.githubusercontent.com"; //e.g. example.com
  secureEsp32FOTA._descriptionOfFirmwareURL = "/MilleniumSAT/MilleniumSAT/main/Codigo/millenium/src/firmware.json";
  secureEsp32FOTA._certificate = test_root_ca;
  secureEsp32FOTA.clientForOta = client2;

  bool shouldExecuteFirmwareUpdate = secureEsp32FOTA.execHTTPSCheck();
  if (shouldExecuteFirmwareUpdate)
  {
    enviaMensagem("[UTILS] Atualização disponivel.", SERIAL_DEBUG, SEM_TOPICO);
    secureEsp32FOTA.executeOTA();
    while (TIMETOUT_OTA--)
      aguardaTempo(1);
  }
  else
  {
    enviaMensagem("[UTILS] Nao ha atualização disponivel.", SERIAL_DEBUG, SEM_TOPICO);
  }

  return SUCESSO;
}

String Utils::montaPacote()
{
  Serial.println("[TASKS] Montando pacote");

  pacoteEnvio["tmp"] = "tmp";


  String conteudoEnvio = "";
  serializeJson(pacoteEnvio, conteudoEnvio);

  Serial.println("[UTILS] Pacote: " + conteudoEnvio);
  return conteudoEnvio;
}

void Utils::executaVerificacoes()
{
  verificaAtualizacoes();
}

void Utils::aguardaProximoEnvio()
{
  enviaMensagem("[UTILS] Aguardando proximo envio.", SERIAL_DEBUG, SEM_TOPICO);

  esp_sleep_enable_timer_wakeup(60000000);
  esp_deep_sleep_start();

  enviaMensagem("[UTILS] Tempo finalizado.", SERIAL_DEBUG, SEM_TOPICO);
}

void Utils::salvaConteudoPersonalizadoNVS(String chave, String tipo)
{
  Serial.print("[TASK] Digite o conteudo: ");
  task.input = "";
  while (task.input.equals(""))
    task.input = Serial.readStringUntil('\x0D');

  if (tipo.equals(TIPO_STRING))
  {
    NVS.setString(chave.c_str(), task.input);
    Serial.println("[TASK] Valor definido: " + task.input);
    reiniciaMilleniumSAT();
  }
  else
  {
    NVS.setString(chave.c_str(), task.input);
    Serial.println("[TASK] Valor definido: " + task.input.toInt());
    reiniciaMilleniumSAT();
  }
}

void Utils::mostraMenu()
{
  Serial.println(
      "~~~~~~~~~~~~~~~MilleniumSAT - POWERED BY UFG~~~~~~~~~~~~~~~");
  Serial.println("Lista de comandos disponiveis:");
  Serial.println("'setid': Define o ID do MilleniumSAT.");
  Serial.println("'i2cdetect': Mostra os dispositivos I2C.");
  Serial.println("'resetwifi': Reseta as credenciais de conexao WiFi.");
  Serial.println(
      "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
}
