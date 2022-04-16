#include <Arduino.h>

#include "_config.h"
#include "_types.h"

#include <Wire.h>

#include "BMx280I2C.h"
#include "uFire_SHT20.h"
#include "Adafruit_SHT31.h"
#include "Adafruit_CCS811.h"
#include "MPU9250.h"
#include "BMX-055.h"
#include <TinyGPS++.h>
#include <HardwareSerial.h>

#include "PION_System.h"
#include "PION_Sensors.h"

// Global Values
float Sensors::pressure = 0;
float Sensors::temperature = 0;
float Sensors::humidity = 0;
float Sensors::BMPtemp = 0;
float Sensors::CO2Level = 0;
float Sensors::luminosity = 0;
float Sensors::accel[3];
float Sensors::gps[3];
float Sensors::gyro[3];
float Sensors::mag[3];

// Sensor Objects
uFire_SHT20 sht20;
Adafruit_CCS811 ccs;
Adafruit_SHT31 sht31 = Adafruit_SHT31();
BMx280I2C bmp(BMP280_ADDRESS);
MPU9250 IMU(Wire, MPU9250_ADDRESS);
BMX055 bmx(Wire, BMX055_ACCEL_DEFAULT_ADDRESS, BMX055_GYRO_DEFAULT_ADDRESS, BMX055_MAG_DEFAULT_ADDRESS);
static const uint32_t GPSBaud = 9600;
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

bool sht20NotFound = false;
bool mpu9050NotFound = false;

void BaroTask(void *pvParameters);
void TempHumTask(void *pvParameters);
void CO2Task(void *pvParameters);
void AnalogTask(void *pvParameters);
void IMUTask(void *pvParameters);
void GPSTask(void *pvParameters);

void configureIMU();

InitStatus_t Sensors::init()
{

  InitStatus_t sensorStatus = INIT_OK;

  LOG("Sensors:init");
  ENDL;

  if (xSemaphoreTake(System::xI2C_semaphore, TIME_TO_SEMAPHORE) == pdTRUE)
  {

    Wire.begin();

    //create a BMx280I2C object using the I2C interface with I2C Address 0x76
    if (!bmp.begin())
    {
      LOG("Falha no sensor de pressão BMP280!");
      ENDL;
      sensorStatus = (InitStatus_t)((uint8_t)sensorStatus | (uint8_t)(1 << ((uint8_t)(ERROR_BMP280 - 1))));
      // return ERROR_BMP280;
    }
    else
    {
      LOG("BPM280::init");
      ENDL;
      //reset sensor to default parameters.
      bmp.resetToDefaults();
      //rewrite better settings
      bmp.writeOversamplingPressure(BMx280MI::OSRS_P_x16);
      bmp.writeOversamplingTemperature(BMx280MI::OSRS_T_x16);
    }

    LOG("SHT::init");
    ENDL;

    if (!sht20.begin())
    { // Set to 0x45 for alternate i2c addr
      sht20NotFound = true;
    }

    if (sht20NotFound)
    {
      if (!sht31.begin(0x44))
      { // Set to 0x45 for alternate i2c addr
        LOG("Falha no sensor de Temperatura e Umidade SHT");
        ENDL;
        sensorStatus = (InitStatus_t)((uint8_t)sensorStatus | (uint8_t)(1 << ((uint8_t)(ERROR_SHT20 - 1))));
      }
    }

    LOG("CCS811::Init");
    ENDL;

    if (ccs.begin())
    {

      while (!ccs.available())
      {
        vTaskDelay(10);
      }
      if (ccs.readData())
      {
        LOG(ccs.geteCO2());
        ENDL;
        if (ccs.geteCO2() < 400)
        {
          LOG("Falha no sensor de CO2!");
          ENDL;
          sensorStatus = (InitStatus_t)((uint8_t)sensorStatus | (uint8_t)(1 << ((uint8_t)(ERROR_CCS811 - 1))));
        }
      }
    }
    else
    {
      LOG("Falha no sensor de CO2!");
      ENDL;
      sensorStatus = (InitStatus_t)((uint8_t)sensorStatus | (uint8_t)(1 << ((uint8_t)(ERROR_CCS811 - 1))));
    }

    LOG("IMU::Init");
    ENDL;

    int status = IMU.begin();

    if (status < 0)
    {
      mpu9050NotFound = true;
    }
    else
    {
      configureIMU();
    }

    if (mpu9050NotFound)
    {
      status = bmx.begin();
      if (status != 1)
      {
        LOG("Falha na IMU, código: ");
        LOG(status);
        ENDL;
        sensorStatus = (InitStatus_t)((uint8_t)sensorStatus | (uint8_t)(1 << ((uint8_t)(ERROR_MPU9250 - 1))));
      }
    }

    if (sensorStatus != 0)
    {
      xSemaphoreGive(System::xI2C_semaphore);
      return sensorStatus;
    }
    xSemaphoreGive(System::xI2C_semaphore);
  }
  else
  {
    LOG("[D]Sensor init Semaphore");
    ENDL;
  }

  xTaskCreatePinnedToCore(BaroTask, "BaroTask", 2048, NULL, 2, NULL, ARDUINO_RUNNING_CORE);
  xTaskCreatePinnedToCore(GPSTask, "GPSTask", 2048, NULL, 2, NULL, ARDUINO_RUNNING_CORE);
  xTaskCreatePinnedToCore(TempHumTask, "TempHumTask", 2048, NULL, 2, NULL, ARDUINO_RUNNING_CORE);
  xTaskCreatePinnedToCore(CO2Task, "CO2Task", 2048, NULL, 2, NULL, ARDUINO_RUNNING_CORE);
  xTaskCreatePinnedToCore(IMUTask, "IMUTask", 4096, NULL, 2, NULL, ARDUINO_RUNNING_CORE);
  xTaskCreatePinnedToCore(AnalogTask, "AnalogTask", 1024, NULL, 1, NULL, ARDUINO_RUNNING_CORE);

  return sensorStatus;
}

void GPSTask(void *pvParameters)
{

  (void)pvParameters;
  SerialGPS.begin(9600, SERIAL_8N1, 16, 17);

  vTaskDelay(2000);
  for (;;)
  {

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
      Sensors::gps[0] = gps.location.lat();
      Sensors::gps[1] = gps.location.lng();
      Sensors::gps[2] = gps.altitude.meters();
      nextSerialTaskTs = millis() + TASK_SERIAL_RATE;
    }

    vTaskDelay(100);
  }
}

void BaroTask(void *pvParameters)
{

  (void)pvParameters;

  vTaskDelay(2000);
  for (;;)
  {
    if (xSemaphoreTake(System::xI2C_semaphore, TIME_TO_SEMAPHORE) == pdTRUE)
    {
      if (!bmp.measure())
      {
        LOG("could not start measurement");
        ENDL;
        continue;
      }
      //wait for the measurement to finish
      if (bmp.hasValue())
      {
        Sensors::pressure = (float)bmp.getPressure64();
        Sensors::BMPtemp = bmp.getTemperature();
      }
      xSemaphoreGive(System::xI2C_semaphore);
    }
    else
    {
      LOG("[D]Baro Semaphore");
      ENDL;
    }
    vTaskDelay(100);
  }
}

void TempHumTask(void *pvParameters)
{
  (void)pvParameters;
  vTaskDelay(2000);

  for (;;)
  {

    if (xSemaphoreTake(System::xI2C_semaphore, TIME_TO_SEMAPHORE) == pdTRUE)
    {
      if (sht20NotFound)
      {
        Sensors::temperature = sht31.readTemperature() - TEMPERATURE_CORRECTION;
        Sensors::humidity = sht31.readHumidity();
      }
      else
      {
        sht20.measure_all();
        Sensors::temperature = sht20.tempC - TEMPERATURE_CORRECTION;
        Sensors::humidity = sht20.RH;
      }
      xSemaphoreGive(System::xI2C_semaphore);
    }
    else
    {
      LOG("[D]Temp Semaphore");
      ENDL;
    }
    vTaskDelay(2000);
  }
}

void CO2Task(void *pvParameters)
{
  (void)pvParameters;
  vTaskDelay(2000);

  for (;;)
  {
    if (xSemaphoreTake(System::xI2C_semaphore, TIME_TO_SEMAPHORE) == pdTRUE)
    {
      if (ccs.available())
      {
        if (!ccs.readData())
        {
          Sensors::CO2Level = ccs.geteCO2();
        }
      }
      xSemaphoreGive(System::xI2C_semaphore);
    }
    else
    {
      LOG("[D]CO2 Semaphore");
      ENDL;
    }
    vTaskDelay(1000);
  }
}

void AnalogTask(void *pvParameters)
{
  (void)pvParameters;
  pinMode(BAT_SENSOR, INPUT);
  pinMode(LIGHT_SENSOR, INPUT);
  for (;;)
  {
    uint16_t uncorrectedLight = analogRead(LIGHT_SENSOR);
    Sensors::luminosity = map(uncorrectedLight, 0, 4000, 0, 100);
    vTaskDelay(100);
  }
}

void IMUTask(void *pvParameters)
{
  (void)pvParameters;
  vTaskDelay(2000);

  for (;;)
  {

    if (xSemaphoreTake(System::xI2C_semaphore, TIME_TO_SEMAPHORE) == pdTRUE)
    {
      if (mpu9050NotFound)
      {
        bmx.readAccel();
        Sensors::accel[0] = bmx.getAccelX_mss();
        Sensors::accel[1] = bmx.getAccelY_mss();
        Sensors::accel[2] = bmx.getAccelZ_mss();

        bmx.readGyro();
        Sensors::gyro[0] = bmx.getGyroX_degs();
        Sensors::gyro[1] = bmx.getGyroY_degs();
        Sensors::gyro[2] = bmx.getGyroZ_degs();

        bmx.readMag();
        Sensors::mag[0] = bmx.getMagX_uT();
        Sensors::mag[1] = bmx.getMagY_uT();
        Sensors::mag[2] = bmx.getMagZ_uT();
      }
      else
      {
        IMU.readSensor();
        Sensors::accel[0] = IMU.getAccelX_mss();
        Sensors::accel[1] = IMU.getAccelY_mss();
        Sensors::accel[2] = IMU.getAccelZ_mss();

        Sensors::gyro[0] = IMU.getGyroX_rads() * 57.2958f;
        Sensors::gyro[1] = IMU.getGyroY_rads() * 57.2958f;
        Sensors::gyro[2] = IMU.getGyroZ_rads() * 57.2958f;

        Sensors::mag[0] = IMU.getMagX_uT();
        Sensors::mag[1] = IMU.getMagY_uT();
        Sensors::mag[2] = IMU.getMagZ_uT();
      }
      xSemaphoreGive(System::xI2C_semaphore);
    }
    else
    {
      LOG("[D]IMU Semaphore");
      ENDL;
    }
    vTaskDelay(200);
  }
}

__attribute__((weak)) void configureIMU()
{
  // setting the accelerometer full scale range to +/-8G
  IMU.setAccelRange(MPU9250::ACCEL_RANGE_8G);
  // setting the gyroscope full scale range to +/-500 deg/s
  IMU.setGyroRange(MPU9250::GYRO_RANGE_1000DPS);
  // setting DLPF bandwidth to 20 Hz
  IMU.setDlpfBandwidth(MPU9250::DLPF_BANDWIDTH_92HZ);
  // setting SRD to 19 for a 50 Hz update rate
  IMU.setSrd(19);
}