#include "sensores.h"
#include "PION_System.h"
#include <ArduinoJson.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

StaticJsonDocument<300> pacote;
static Utils utils;
static System cubeSat;
static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);

String Sensores::obtemSensores()
{

  utils.enviaMensagem("[SENSORS] Iniciando leitura dos sensores.", SERIAL_DEBUG, SEM_TOPICO);

  pacote["bt"] = cubeSat.getBattery();
  pacote["tmp"] = cubeSat.getTemperature();
  pacote["umd"] = cubeSat.getHumidity();
  pacote["press"] = cubeSat.getPressure();
  pacote["co2"] = cubeSat.getCO2Level();
  pacote["lum"] = cubeSat.getLuminosity();
  pacote["accx"] = cubeSat.getAccelerometer(0);
  pacote["accy"] = cubeSat.getAccelerometer(1);
  pacote["accz"] = cubeSat.getAccelerometer(2);
  pacote["grx"] = cubeSat.getGyroscope(0);
  pacote["gry"] = cubeSat.getGyroscope(1);
  pacote["grz"] = cubeSat.getGyroscope(2);
  pacote["mgx"] = cubeSat.getMagnetometer(0);
  pacote["mgy"] = cubeSat.getMagnetometer(1);
  pacote["mgz"] = cubeSat.getMagnetometer(2);
  pacote["v"] = V_FIRMWARE;
  
  ss.begin(GPSBaud);
  while (ss.available() > 0){
    gps.encode(ss.read());
    if (gps.location.isUpdated()){
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
  pinMode(ledPino, OUTPUT);
  Wire.begin();

  return SUCESSO;
}