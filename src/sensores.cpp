#include "sensores.h"
#include <ArduinoJson.h>
#include "MadgwickAHRS.h"
#include <TinyGPS++.h>
#include <HardwareSerial.h>

StaticJsonDocument<500> pacote;
const float sensorRate = 100;
static Utils utils;
static const uint32_t GPSBaud = 9600;
Madgwick filter;
TinyGPSPlus gps;
HardwareSerial SerialGPS(1);
struct GpsDataState_t
{
  double originLat = 0;
  double originLon = 0;
  double originAlt = 0;
  double distMax = 0;
  double dist = 0;
  double altMax = -999999;
  double altMin = 999999;
  double spdMax = 0;
  double prevDist = 0;
};
GpsDataState_t gpsState = {};

#define TASK_SERIAL_RATE 1000 // ms
uint32_t nextSerialTaskTs = 0;
uint32_t nextOledTaskTs = 0;

String Sensores::obtemJSON()
{
  float xAcc, yAcc, zAcc, xGyro, yGyro, zGyro, roll, pitch, heading, battery, tmp, umd, press, co2, lum, mgx, mgy, mgz;

  utils.enviaMensagem("[SENSORS] Iniciando configuracao dos sensores.", SERIAL_DEBUG, SEM_TOPICO);
  cubeSat.initNoNetwork();
  delay(2000);
  utils.enviaMensagem("[SENSORS] Sensores configurados com sucesso.", SERIAL_DEBUG, SEM_TOPICO);
  filter.begin(sensorRate);

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
  utils.enviaMensagem("[SENSORS] Sensores lidos com sucesso.", SERIAL_DEBUG, SEM_TOPICO);

  filter.updateIMU(xGyro, yGyro, zGyro, xAcc, yAcc, zAcc);
  roll = filter.getRoll();
  pitch = filter.getPitch();
  heading = filter.getYaw();

  //TX - 16  | RX - 17
  SerialGPS.begin(9600, SERIAL_8N1, 16, 17);
  long readValue;

  gpsState.originLat = (double)readValue / 1000000;

  gpsState.originLon = (double)readValue / 1000000;

  gpsState.originAlt = (double)readValue / 1000000;

  static int p0 = 0;
  int numeroLeituras = 100;
  float lat = 0;
  float lon = 0;
  float alt = 0;
  float spd = 0;
  while (1)
  {
    gpsState.originLat = gps.location.lat();
    gpsState.originLon = gps.location.lng();
    gpsState.originAlt = gps.altitude.meters();

    gpsState.distMax = 0;
    gpsState.altMax = -999999;
    gpsState.spdMax = 0;
    gpsState.altMin = 999999;

    while (SerialGPS.available() > 0)
    {
      gps.encode(SerialGPS.read());
    }

    if (gps.satellites.value() > 4)
    {
      gpsState.dist = TinyGPSPlus::distanceBetween(gps.location.lat(), gps.location.lng(), gpsState.originLat, gpsState.originLon);

      if (gpsState.dist > gpsState.distMax && abs(gpsState.prevDist - gpsState.dist) < 50)
      {
        gpsState.distMax = gpsState.dist;
      }
      gpsState.prevDist = gpsState.dist;

      if (gps.altitude.meters() > gpsState.altMax)
      {
        gpsState.altMax = gps.altitude.meters();
      }

      if (gps.speed.kmph() > gpsState.spdMax)
      {
        gpsState.spdMax = gps.speed.kmph();
      }

      if (gps.altitude.meters() < gpsState.altMin)
      {
        gpsState.altMin = gps.altitude.meters();
      }
    }

    if (nextSerialTaskTs < millis())
    {
      lat = gps.location.lat();
      lon = gps.location.lng();
      alt = gps.altitude.meters();
      spd = gps.speed.kmph();

      numeroLeituras--;
      if (!numeroLeituras)
      {
        Serial.println("[GPS] Timeout de leitura do GPS!");
        break;
      }

      nextSerialTaskTs = millis() + TASK_SERIAL_RATE;

      if (lat != 0)
      {
        Serial.println("[GPS] Leitura válida do GPS obtida com sucesso.");
        break;
      }
      else
      {
        Serial.println("[GPS] Aguardando uma leitura válida do GPS. (" + String(numeroLeituras) + "/100)");
      }
    }
  }

  pacote["lat"] = lat;
  pacote["lon"] = lon;
  pacote["alt"] = alt;
  pacote["spd"] = spd;

  utils.enviaMensagem("[SENSORS] Montando JSON de envio.", SERIAL_DEBUG, SEM_TOPICO);

  pacote["bt"] = battery;
  pacote["tmp"] = tmp;
  pacote["umd"] = umd;
  pacote["press"] = press;
  pacote["co2"] = co2;
  pacote["lum"] = lum;
  pacote["accx"] = xAcc;
  pacote["accy"] = yAcc;
  pacote["accz"] = zAcc;
  pacote["grx"] = xGyro;
  pacote["gry"] = yGyro;
  pacote["grz"] = zGyro;
  pacote["mgx"] = mgx;
  pacote["mgy"] = mgy;
  pacote["mgz"] = mgz;
  pacote["roll"] = roll;
  pacote["pitch"] = pitch;
  pacote["heading"] = heading;
  pacote["v"] = V_FIRMWARE;

  utils.enviaMensagem("[SENSORS] JSON de envio montado com sucesso.", SERIAL_DEBUG, SEM_TOPICO);

  String conteudoEnvio = "";
  serializeJson(pacote, conteudoEnvio);
  Serial.println("[UTILS] Pacote: " + conteudoEnvio);

  return conteudoEnvio;
}

uint8_t Sensores::configuraPortas()
{

  return SUCESSO;
}
