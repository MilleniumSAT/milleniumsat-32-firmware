#include "task.h"

WifiMilleniumSAT wifiAirpure;
static Utils utils;
static Tasks task;
TaskHandle_t task_low_serial;
TaskHandle_t task_low_led;
TaskHandle_t task_low;

/**
 * Task para lidar com tarefas paralelas
 * @param pvParameters parametro interno da task
 * @return void sem retorno
 */
void vLowTask(void *pvParameters)
{
  while (true)
  {

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

/**
 * Task para comunicação com a serial
 * @param pvParameters parametro interno da task
 * @return void sem retorno
 */
void vLowSerial(void *pvParameters)
{
  while (true)
  {
    if (Serial.available())
    {
      task.input = Serial.readStringUntil('\x0D');
      utils.enviaMensagem("[TASKS] Comando digitado: " + task.input, SERIAL_DEBUG, SEM_TOPICO);

      if (task.input.equals("setid"))
        utils.salvaConteudoPersonalizadoNVS("id", TIPO_STRING);

      if (task.input.equals("i2cdetect"))
        i2cdetect();

      if (task.input.equals("help"))
        utils.mostraMenu();
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

/**
 * Task para o controle do led
 * @param pvParameters parametro interno da task
 * @return void sem retorno
 */
void vLowLED(void *pvParameters)
{
  const int freq = 5000;
  const int ledChannel = 0;
  const int resolution = 8;
  uint8_t LED_CYCLES = 255;

  esp_task_wdt_init(TEMPO_DELAY_WDT, true);
  esp_task_wdt_add(NULL);
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(ledPino, ledChannel);

  while (true)
  {
    esp_task_wdt_reset();

    while (LED_CYCLES--)
    {
      ledcWrite(ledChannel, LED_CYCLES);
      esp_task_wdt_reset();
      delay(utils.LED_STATE);
    }

    while (LED_CYCLES < 255)
    {
      ledcWrite(ledChannel, LED_CYCLES);
      esp_task_wdt_reset();
      delay(utils.LED_STATE);
    }
  }
}

uint8_t Tasks::configuraTasks()
{
  utils.enviaMensagem("[TASKS] Configurando tasks", SERIAL_DEBUG, SEM_TOPICO);
  xTaskCreate(vLowSerial, "vLowSerial", 2048, NULL, 0, &task_low_serial);
  xTaskCreate(vLowLED, "vLowLED", 2048, NULL, 0, &task_low_led);
  xTaskCreate(vLowTask, "vLowTasks", 2048, NULL, 5, &task_low);
  utils.enviaMensagem("[TASKS] Tasks configuradas", SERIAL_DEBUG, SEM_TOPICO);

  return SUCESSO;
}
