#include "sensores.h"
#include <ArduinoJson.h>
#include "MadgwickAHRS.h"
#include "time.h"

StaticJsonDocument<500> pacote;
const float sensorRate = 100;
uint32_t sequencial = 0;
static Utils utils;
Madgwick filter;
const char* ntpServer = "pool.ntp.org";

String Sensores::obtemJSON()
{
  float xAcc, yAcc, zAcc, xGyro, yGyro, zGyro, roll, pitch, heading, battery, tmp, umd, press, co2, lum, mgx, mgy, mgz, lat, lon, alt;

  utils.enviaMensagem("[SENSORS] Iniciando leitura dos sensores.", SERIAL_DEBUG, SEM_TOPICO);
  xAcc = cubeSat.getAccelerometer(0);
  delay(200);
  yAcc = cubeSat.getAccelerometer(1);
  delay(200);
  zAcc = cubeSat.getAccelerometer(2);
  delay(200);
  xGyro = cubeSat.getGyroscope(0);
  delay(200);
  yGyro = cubeSat.getGyroscope(1);
  delay(200);
  zGyro = cubeSat.getGyroscope(2);
  delay(200);
  battery = cubeSat.getBattery();
  delay(200);
  tmp = cubeSat.getTemperature();
  delay(200);
  umd = cubeSat.getHumidity();
  delay(200);
  press = cubeSat.getPressure();
  delay(200);
  co2 = cubeSat.getCO2Level();
  delay(200);
  lum = cubeSat.getLuminosity();
  delay(200);
  mgx = cubeSat.getMagnetometer(0);
  delay(200);
  mgy = cubeSat.getMagnetometer(1);
  delay(200);
  mgz = cubeSat.getMagnetometer(2);
  delay(200);
  lat = cubeSat.getGps(0);
  delay(200);
  lon = cubeSat.getGps(1);
  delay(200);
  alt = cubeSat.getGps(2);

  delay(200);
  utils.enviaMensagem("[SENSORS] Sensores lidos com sucesso.", SERIAL_DEBUG, SEM_TOPICO);

  filter.updateIMU(xGyro, yGyro, zGyro, xAcc, yAcc, zAcc);
  roll = filter.getRoll();
  pitch = filter.getPitch();
  heading = filter.getYaw();

  utils.enviaMensagem("[SENSORS] Montando JSON de envio.", SERIAL_DEBUG, SEM_TOPICO);

  configTime(0, 0, ntpServer);

  pacote["equipe"] = MILLENIUMSAT_ID;
  pacote["bateria"] = battery;
  pacote["temperatura"] = tmp;
  pacote["pressao"] = press;
  pacote["giroscopio"][0] = xGyro;
  pacote["giroscopio"][1] = yGyro;
  pacote["giroscopio"][2] = zGyro;
  pacote["acelerometro"][0] = xAcc;
  pacote["acelerometro"][1] = yAcc;
  pacote["acelerometro"][2] = zAcc;
  pacote["payload"]["tmstmp"] = getTime();
  pacote["payload"]["georeferencia"][0] = lat;
  pacote["payload"]["georeferencia"][1] = lon;
  pacote["payload"]["georeferencia"][2] = alt;
  pacote["payload"]["magnetometro"][0] = mgx;
  pacote["payload"]["magnetometro"][1] = mgy;
  pacote["payload"]["magnetometro"][2] = mgz;
  pacote["payload"]["orientacao"][0] = pitch;
  pacote["payload"]["orientacao"][1] = roll;
  pacote["payload"]["orientacao"][2] = heading;

  sequencial++;

  utils.enviaMensagem("[SENSORS] JSON de envio montado com sucesso.", SERIAL_DEBUG, SEM_TOPICO);

  utils.enviaMensagem("[SENSORS] Iniciando persistencia do pacote.", SERIAL_DEBUG, SEM_TOPICO);
  cubeSat.logOnSDFile();
  utils.enviaMensagem("[SENSORS] Pacote persistido com sucesso.", SERIAL_DEBUG, SEM_TOPICO);

  String conteudoEnvio = "";
  serializeJson(pacote, conteudoEnvio);
  Serial.println("[UTILS] Pacote: " + conteudoEnvio);

  return conteudoEnvio;
}

unsigned long Sensores::getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}

uint8_t Sensores::configuraPortas()
{
  utils.enviaMensagem("[SENSORS] Iniciando configuracao dos sensores.", SERIAL_DEBUG, SEM_TOPICO);
  cubeSat.initNoNetwork();
  cubeSat.createLogOnSD();

  delay(2000);
  utils.enviaMensagem("[SENSORS] Sensores configurados com sucesso.", SERIAL_DEBUG, SEM_TOPICO);
  filter.begin(sensorRate);
  return SUCESSO;
}
