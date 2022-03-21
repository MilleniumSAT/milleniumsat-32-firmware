#include "sensores.h"
#include <ArduinoJson.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include "MadgwickAHRS.h"

StaticJsonDocument<500> pacote;
const float sensorRate = 104.00;
static Utils utils;
static const int RXPin = 16, TXPin = 17;
static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);
Madgwick filter;

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
/*
  ss.begin(9600, SWSERIAL_8N1, 13, 12, false);
  if (!ss)
  {
    Serial.println("Invalid SoftwareSerial pin configuration, check config");
  }
  delay(1000);
  while (ss.available() > 0)
  {
    Serial.println(".");

    while (!gps.location.isValid())
    {
      gps.encode(ss.read());
      delay(100);
    }

    double latitude = (gps.location.lat());
    double longitude = (gps.location.lng());
    Serial.print("Latitude: ");
    Serial.println(latitude, 6);
    Serial.print("Longitude: ");
    Serial.println(longitude, 6);
  }

  if (gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
  }
*/
  String conteudoEnvio = "";
  serializeJson(pacote, conteudoEnvio);
  Serial.println("[UTILS] Pacote: " + conteudoEnvio);

  return conteudoEnvio;
}

uint8_t Sensores::configuraPortas()
{

  return SUCESSO;
}