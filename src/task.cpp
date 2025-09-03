#include "task.h"
#include "mqtt.h"
// #include <FastLED.h>
#include "mesh.h"
#include "ircontrol.h"
#include "statemachine/statemachine.h"

#define NUM_LEDS 1
#define DATA_PIN 45
#define CLOCK_PIN 13

// CRGB leds[NUM_LEDS];
MQTT mqtt;
#ifdef IRCONTROL_ENABLE
static Irctrl ircontrol;
#endif
static Utils utils;
static Tasks task;
static Sensors sensors;
TaskHandle_t task_low_serial;
TaskHandle_t task_low_led;
TaskHandle_t task_low;
TaskHandle_t task_healthcheck;

const int CHANNEL_BLUE = 0;
const int CHANNEL_RED = 1;
const int CHANNEL_GREEN = 2;
int buttonState;

/**
 * Task to handle parallel tasks
 * @param pvParameters internal parameter of the task
 * @return void no return
 */
void vLowTask(void *pvParameters)
{
  while (true)
  {
    if (WiFi.status() == WL_CONNECTED)
    {
#ifdef MQTT_ENABLE
      mqtt.mqttLoop();
      Serial.println("[TASKS] MQTT Loop.");

      // Verify boot pin status
      buttonState = digitalRead(BOOT_PIN); // Lê o estado do botão

      if (buttonState == LOW)
      {
        Serial.println("Botão pressionado");
        if (isLampOn){
          isLampOn = false;
          mqtt.sendMessage(TOPICO_LAMPADAS2, "turn_lamp_off");//{\"topic\": \"" + String(TOPICO_LAMPADA) + "\", \"message\": \"turn_lamp_on\"}");
          digitalWrite(LAMP_PIN, HIGH);
          vTaskDelay(pdMS_TO_TICKS(1500));
        } else {
          isLampOn = true;
          mqtt.sendMessage(TOPICO_LAMPADAS2, "turn_lamp_on");//{\"topic\": \"" + String(TOPICO_LAMPADA) + "\", \"message\": \"turn_lamp_off\"}");
          digitalWrite(LAMP_PIN, LOW);
          vTaskDelay(pdMS_TO_TICKS(1500));
        }
      }
      
#endif
    }
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

/**
 * Task for serial communication
 * @param pvParameters internal parameter of the task
 * @return void no return
 */
void vLowSerial(void *pvParameters)
{
  while (true)
  {
    if (Serial.available())
    {
      task.input = Serial.readStringUntil('\x0D');
      utils.sendMessage("[TASKS] Entered command: " + task.input, SERIAL_DEBUG, SEM_TOPICO);

      if (task.input.equals("setid"))
        utils.saveContentNVS("id", TIPO_STRING);

      if (task.input.equals("modonormal"))
        utils.saveContentParameterNVS("modoteste", "false", TIPO_STRING);

      if (task.input.equals("i2cdetect"))
        i2cdetect();

      if (task.input.equals("resetwifi"))
        connection->resetCredentials();

      if (task.input.equals("restart"))
        ESP.restart();

      if (task.input.equals("espnow"))
      {
        if (NVS.getInt(ESP_NOW_INIT) == 1)
        {
          NVS.setInt(ESP_NOW_INIT, 0);
        }
        else
        {
          NVS.setInt(ESP_NOW_INIT, 1);
        }
        ESP.restart();
      }

      if (task.input.equals("root"))
      {
        if (NVS.getInt(ESP_NOW_ROOT) == 1)
        {
          NVS.setInt(ESP_NOW_ROOT, 0);
        }
        else
        {
          NVS.setInt(ESP_NOW_ROOT, 1);
        }

        ESP.restart();
      }

      if (task.input.equals("lowpower"))
      {
        if (NVS.getInt(LOW_POWER) == 1)
        {
          NVS.setInt(LOW_POWER, 0);
        }
        else
        {
          NVS.setInt(LOW_POWER, 1);
        }

        ESP.restart();
      }

      if (task.input.equals("ethernet"))
      {
        if (NVS.getInt(ETHERNET) == 1)
        {
          NVS.setInt(ETHERNET, 0);
        }
        else
        {
          NVS.setInt(ETHERNET, 1);
        }

        ESP.restart();
      }

      if (task.input.equals("lora"))
      {
        if (NVS.getInt(LORA) == 1)
        {
          NVS.setInt(LORA, 0);
        }
        else
        {
          NVS.setInt(LORA, 1);
        }

        ESP.restart();
      }

      if (task.input.equals("controle"))
      {
#ifdef IRCONTROL_ENABLE
        ircontrol.setupIR(false);
        ircontrol.configureControl();
#endif
      }

      if (task.input.equals("teste"))
      {
#ifdef IRCONTROL_ENABLE
        utils.sendMessage("[MQTT] Sending power command.", SERIAL_DEBUG, SEM_TOPICO);
        String comando = NVS.getString("CTRL_PWR");
        ircontrol.setupIR(true);
        while (true)
        {
          ircontrol.sendCommand(comando);
          vTaskDelay(pdMS_TO_TICKS(1000));
        }
#endif
      }

      if (task.input.equals("wifidev"))
      {
        NVS.setString("SSID_SALVO", "Antonielli");
        vTaskDelay(pdMS_TO_TICKS(100));
        NVS.setString("PASS_SALVO", "antonielli@123antonio");
        vTaskDelay(pdMS_TO_TICKS(100));
        ESP.restart();
      }

      if (task.input.equals("setwifi"))
      {
        utils.sendMessage("[TASKS] Enter the SSID you want to set:", SERIAL_DEBUG, SEM_TOPICO);
        utils.saveContentNVS("SSID_SALVO", TIPO_STRING);
        vTaskDelay(pdMS_TO_TICKS(1000));
        utils.sendMessage("[TASKS] Enter the password you want to set:", SERIAL_DEBUG, SEM_TOPICO);
        utils.saveContentNVS("PASS_SALVO", TIPO_STRING);
        vTaskDelay(pdMS_TO_TICKS(1000));
        ESP.restart();
      }

      if (task.input.equals("help"))
        utils.showMenu();
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

/**
 * Task for healthcheck control
 * @param pvParameters internal parameter of the task
 * @return void no return
 */
void vLowHealthCheck(void *pvParameters)
{
  while (true)
  {
    vTaskDelay(pdMS_TO_TICKS(600000));
    if (connection->verifyConn())
    {
      utils.sendHealthCheck();
    }
  }
}

/**
 * Task for LED control
 * @param pvParameters internal parameter of the task
 * @return void no return
 */
void vLowLED(void *pvParameters)
{
  // esp_task_wdt_init(TEMPO_DELAY_WDT, true);
  // esp_task_wdt_add(NULL);
  // pinMode(2, OUTPUT);
  // FastLED.addLeds<SK6812, DATA_PIN, RGB>(leds, NUM_LEDS);

  // while (true)
  // {
  //   digitalWrite(2, HIGH);
  //   esp_task_wdt_reset();
  //   leds[0] = CRGB::Coral;
  //   FastLED.show();
  //   vTaskDelay(pdMS_TO_TICKS(100));

  //   esp_task_wdt_reset();
  //   leds[0] = CRGB::Black;
  //   FastLED.show();
  //   vTaskDelay(pdMS_TO_TICKS(100));

  //   esp_task_wdt_reset();
  //   leds[0] = CRGB::Coral;
  //   FastLED.show();
  //   vTaskDelay(pdMS_TO_TICKS(100));

  //   if (WiFi.status() == WL_CONNECTED)
  //   {

  //     for (uint64_t i = 0; i < 0x0000FF; i++)
  //     {
  //       leds[0] = i;
  //       FastLED.show();
  //       vTaskDelay(pdMS_TO_TICKS(10));
  //       esp_task_wdt_reset();
  //     }

  //     for (uint64_t i = 0x0000FF; i > 0; i--)
  //     {
  //       leds[0] = i;
  //       FastLED.show();
  //       vTaskDelay(pdMS_TO_TICKS(10));
  //       esp_task_wdt_reset();
  //     }
  //   }
  //   else
  //   {
  //     leds[0] = CRGB::Green;
  //     FastLED.show();
  //     vTaskDelay(pdMS_TO_TICKS(2700));
  //     esp_task_wdt_reset();
  //   }
  //   digitalWrite(2, LOW);
  // }
}
uint8_t Tasks::configureTasks()
{
  utils.sendMessage("[TASKS] Configuring tasks", SERIAL_DEBUG, SEM_TOPICO);

  xTaskCreate(vLowSerial, TASK_SERIAL, TASK_BAIXA_PRIORIDADE, NULL, 0, &task_low_serial);
  // xTaskCreate(vLowLED, TASK_LED, TASK_BAIXA_PRIORIDADE, NULL, 1, &task_low_led);
  xTaskCreate(vLowTask, TASK_GENERICA, TASK_ALTA_PRIORIDADE, NULL, 3, &task_low);

  // TODO: FIX
  //  if (!espnow_node())
  //  {
  //    Serial.println("[TASKS] Starting healthcheck.");
  //    xTaskCreate(vLowHealthCheck, TASK_HEALTHCHECK, TASK_ALTA_PRIORIDADE, NULL, 2, &task_healthcheck);
  //  } else {
  //    Serial.println("[TASKS] Node is a ESP-NOW node.");
  //  }

  utils.sendMessage("[TASKS] Tasks configured", SERIAL_DEBUG, SEM_TOPICO);
  return SUCESSO;
}
