#include "sensores.h"
#include "PION_System.h"
#include <ArduinoJson.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include "MadgwickAHRS.h"

StaticJsonDocument<500> pacote;
const float sensorRate = 104.00;
static Utils utils;
static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;
System cubeSat;
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);
Madgwick filter;

String Sensores::obtemSensores()
{

  utils.enviaMensagem("[SENSORS] Iniciando configuracao e leitura dos sensores.", SERIAL_DEBUG, SEM_TOPICO);
  cubeSat.initNoNetwork();
  filter.begin(sensorRate);
  float xAcc, yAcc, zAcc, xGyro, yGyro, zGyro, roll, pitch, heading;
  xAcc = cubeSat.getAccelerometer(0);
  yAcc = cubeSat.getAccelerometer(1);
  zAcc = cubeSat.getAccelerometer(2);
  xGyro = cubeSat.getGyroscope(0);
  yGyro = cubeSat.getGyroscope(1);
  zGyro = cubeSat.getGyroscope(2);
  filter.updateIMU(xGyro, yGyro, zGyro, xAcc, yAcc, zAcc);
  roll = filter.getRoll();
  pitch = filter.getPitch();
  heading = filter.getYaw();
  delay(1000);

  pacote["bt"] = cubeSat.getBattery();
  pacote["tmp"] = cubeSat.getTemperature();
  pacote["umd"] = cubeSat.getHumidity();
  pacote["press"] = cubeSat.getPressure();
  pacote["co2"] = cubeSat.getCO2Level();
  pacote["lum"] = cubeSat.getLuminosity();
  pacote["accx"] = xAcc;
  pacote["accy"] = yAcc;
  pacote["accz"] = zAcc;
  pacote["grx"] = xGyro;
  pacote["gry"] = yGyro;
  pacote["grz"] = zGyro;
  pacote["mgx"] = cubeSat.getMagnetometer(0);
  pacote["mgy"] = cubeSat.getMagnetometer(1);
  pacote["mgz"] = cubeSat.getMagnetometer(2);
  pacote["roll"] = roll;
  pacote["pitch"] = pitch;
  pacote["heading"] = heading;
  pacote["v"] = V_FIRMWARE;

  ss.begin(GPSBaud);
  while (ss.available() > 0)
  {
    gps.encode(ss.read());
    if (gps.location.isUpdated())
    {
      pacote["lat"] = gps.location.lat();
      pacote["long"] = gps.location.lng();
    }
  }

  String conteudoEnvio = "";
  serializeJson(pacote, conteudoEnvio);

  Serial.println("[UTILS] Pacote: " + conteudoEnvio);

  utils.enviaMensagem("[SENSORS] Leitura dos sensores finalizada.", SERIAL_DEBUG, SEM_TOPICO);

  return conteudoEnvio;
}

uint8_t Sensores::configuraPortas()
{

  return SUCESSO;
}