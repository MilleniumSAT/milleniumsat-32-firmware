#include "task.h"

WifiMilleniumSAT wifiMilleniumSAT;
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

      if (task.input.equals("wifi"))
        utils.salvaConteudoPersonalizadoNVS("wifi", TIPO_STRING);

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
  
}

uint8_t Tasks::configuraTasks()
{
  utils.enviaMensagem("[TASKS] Configurando tasks", SERIAL_DEBUG, SEM_TOPICO);
  xTaskCreate(vLowSerial, "vLowSerial", 2048, NULL, 0, &task_low_serial);
  xTaskCreate(vLowTask, "vLowTasks", 2048, NULL, 5, &task_low);
  utils.enviaMensagem("[TASKS] Tasks configuradas", SERIAL_DEBUG, SEM_TOPICO);

  return SUCESSO;
}
