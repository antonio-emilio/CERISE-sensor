#include "utils.h"
#include <ArduinoJson.h>
#include "driver/adc.h"
#include <esp_bt.h>
#include <esp32fota.h>
#include "MAX17048.h"
#include "mesh.h"
#include "time.h"
#include "statemachine/statemachine.h"
#include "ircontrol.h"

StaticJsonDocument<350> pacoteEnvio;
esp32FOTA esp32FOTA("esp32s3-fota-http", V_FIRMWARE, false, true);
WiFiClientSecure client2;
MAX17048 pwr_mgmt;
Tasks task;
Sensors sensors;
uint8_t smartAir_id;
uint8_t esp_now_ini;
uint8_t low_power_enable;
uint8_t ethernet_enable;
uint8_t lora_enable;
uint8_t nAlerts;
String phone;
uint8_t esp_now_root;
uint8_t LED_STATE;
Irctrl irctrl;

const char *ntpServer = "pool.ntp.org";

void Utils::restart()
{
  ESP.restart();
}

unsigned long Utils::getTime()
{
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("[UTILS] Failed to get current time");
    return (0);
  }
  time(&now);
  return now;
}

float getVoltage()
{
  if (hasBateria)
  {
    pwr_mgmt.attatch(Wire);
    vTaskDelay(pdMS_TO_TICKS(1000));

    Serial.print("VCELL ADC : ");
    Serial.println(pwr_mgmt.adc());
    Serial.print("VCELL V   : ");
    Serial.println(pwr_mgmt.voltage());
    Serial.print("VCELL SOC : ");
    Serial.print(pwr_mgmt.percent());
    Serial.println(" \%");
    Serial.print("VCELL SOC : ");
    Serial.print(pwr_mgmt.accuratePercent());
    Serial.println(" \%");
    Serial.println();
    return pwr_mgmt.voltage();
  }
  else
  {
    return 0;
  }
}

uint8_t Utils::getNVSvalues()
{
  NVS.begin();
  sendMessage("[UTILS] Retrieving saved variables from memory.", SERIAL_DEBUG, SEM_TOPICO);

  smartAir_id = NVS.getString(ID_SMARTAIR).toInt();

  if (smartAir_id == ID_AUSENTE)
  {
    Serial.println("[UTILS] ID not found. Enter the 'setid' command to set the ID.");
    LED_STATE = ERRO;
    while (1)
    {
      Serial.println("[UTILS] ID not found. Enter the 'setid' command to set the ID.");
      vTaskDelay(pdMS_TO_TICKS(1000));
      smartAir_id = NVS.getString(ID_SMARTAIR).toInt();
      if (smartAir_id != ID_AUSENTE)
      {
        break;
      }
    }
    return ERRO;
  }

#ifdef ESPNOW_ENABLE
  esp_now_ini = FLAG_ATIVADA;
#else
  esp_now_ini = FLAG_DESATIVADA;
#endif

  if (esp_now_ini)
  {
    sendMessage("[UTILS] ESPNOW connection enabled.", SERIAL_DEBUG, SEM_TOPICO);
    esp_now_ini = FLAG_ATIVADA;
  }
  else
  {
    sendMessage("[UTILS] ESPNOW connection disabled.", SERIAL_DEBUG, SEM_TOPICO);
    esp_now_ini = FLAG_DESATIVADA;
  }

#ifdef ESPNOW_ROOT_ENABLE
  esp_now_root = FLAG_ATIVADA;
#else
  esp_now_root = FLAG_DESATIVADA;
#endif

  if (esp_now_root)
  {
    sendMessage("[UTILS] ESPNOW - ROOT connection enabled.", SERIAL_DEBUG, SEM_TOPICO);
    esp_now_root = FLAG_ATIVADA;
  }
  else
  {
    sendMessage("[UTILS] ESPNOW - ROOT connection disabled.", SERIAL_DEBUG, SEM_TOPICO);
    esp_now_root = FLAG_DESATIVADA;
  }

  low_power_enable = NVS.getInt(LOW_POWER);
  if (low_power_enable)
  {
    sendMessage("[UTILS] Low Power enabled.", SERIAL_DEBUG, SEM_TOPICO);
    low_power_enable = FLAG_ATIVADA;
  }
  else
  {
    sendMessage("[UTILS] Low Power disabled.", SERIAL_DEBUG, SEM_TOPICO);
    low_power_enable = FLAG_DESATIVADA;
  }

  ethernet_enable = NVS.getInt(ETHERNET);
  if (ethernet_enable)
  {
    sendMessage("[UTILS] Ethernet enabled.", SERIAL_DEBUG, SEM_TOPICO);
    ethernet_enable = FLAG_ATIVADA;
  }
  else
  {
    sendMessage("[UTILS] Ethernet disabled.", SERIAL_DEBUG, SEM_TOPICO);
    ethernet_enable = FLAG_DESATIVADA;
  }

  lora_enable = NVS.getInt(LORA);
  if (lora_enable)
  {
    sendMessage("[UTILS] Lora enabled.", SERIAL_DEBUG, SEM_TOPICO);
    lora_enable = FLAG_ATIVADA;
  }
  else
  {
    sendMessage("[UTILS] Lora disabled.", SERIAL_DEBUG, SEM_TOPICO);
    lora_enable = FLAG_DESATIVADA;
  }

#ifdef VAPE_DETECTOR
  phone = NVS.getString(PHONENUMBER);
  if (phone.length() > 0)
  {
    sendMessage("[UTILS] Phone number found: " + phone, SERIAL_DEBUG, SEM_TOPICO);
  }
  else
  {
    sendMessage("[UTILS] Phone number not found.", SERIAL_DEBUG, SEM_TOPICO);
  }
#endif

#ifdef SMARTBAIT
  String alertas = NVS.getString(NRO_ALERT);
  Serial.println("[UTILS] Number of alerts at startup: " + alertas);
  nAlerts = alertas.toInt();
#endif

  sendMessage("[UTILS] " + String(DEVICE_NAME) + "ID: " + String(smartAir_id), SERIAL_DEBUG, SEM_TOPICO);
  sendMessage("[UTILS] Memory values obtained successfully.", SERIAL_DEBUG, SEM_TOPICO);
  return SUCESSO;
}

void Utils::configureSerial()
{
  Serial.begin(VEL_SERIAL_DEBUG);
  vTaskDelay(pdMS_TO_TICKS(1000));
}

String Utils::createAlertMessage()
{
  Serial.println("[UTILS] Creating alert message.");
  String alerta = "";
  alerta = "Olá! \n🚨 Detectamos um possível uso de cigarro eletrônico no ambiente. \n";
  alerta += " ID do dispositivo: " + String(smartAir_id) + "\n";
  alerta += " TVOC: " + String(sensors.getTVOCAGS10()) + " ppb\n";
  alerta += " Sugerimos que você verifique o ambiente e tome as medidas necessárias. \n";
  alerta += " Atenciosamente, \n Equipe VapeDetect.";

  pacoteEnvio["message"] = alerta;
  pacoteEnvio["phoneNumber"] = "120363222023133723"; // phone;
  pacoteEnvio["token"] = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9";

  String conteudoEnvio = "";
  serializeJson(pacoteEnvio, conteudoEnvio);
  Serial.println(conteudoEnvio);

  return conteudoEnvio;
}

uint8_t Utils::sendMessage(String conteudo, uint8_t whereTo, String url = SERVIDOR)
{
  switch (whereTo)
  {
  case SERIAL_DEBUG:
    Serial.println(conteudo);
    return SUCESSO;
    break;

  case SOCKET_DEBUG:
    return SUCESSO;
    break;

  case POST_DEBUG:
    return connection->sendData(conteudo, url);
    break;

  case ESPNOW_DEBUG:
    return sendData(conteudo);
    break;

  case LORA_DEBUG:
    return connection->sendData(conteudo, SEM_TOPICO);
    break;
  }
}

float Utils::threatFloat(float valor)
{
  valor = !isnan(valor) ? valor : 0;
  return valor;
}

void Utils::configureNTP()
{
  if (espnow_node())
  {
    return;
  }

  sendMessage("[UTILS] Starting NTP configuration.", SERIAL_DEBUG, SEM_TOPICO);
  configTime(0, 0, ntpServer);
  sendMessage("[UTILS] NTP configuration completed successfully.", SERIAL_DEBUG, SEM_TOPICO);
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

void Utils::await(int times)
{
  while (times--)
  {
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

float Utils::mapTo(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

uint8_t Utils::verifyUpdates()
{

  uint8_t TIMETOUT_OTA = 180;
  sendMessage("[UTILS] Checking for updates.", SERIAL_DEBUG, SEM_TOPICO);

  if (!connection->verifyConn())
  {
    sendMessage("[UTILS] No internet connection.", SERIAL_DEBUG, SEM_TOPICO);
    return ERRO;
  }

  esp32FOTA.checkURL = "https://raw.githubusercontent.com/floatSolutions/dmhfu/main/firmware.json";

  bool shouldExecuteFirmwareUpdate = esp32FOTA.execHTTPcheck();
  if (shouldExecuteFirmwareUpdate)
  {
    sendMessage("[UTILS] Update available.", SERIAL_DEBUG, SEM_TOPICO);
    esp32FOTA.execOTA();
    while (TIMETOUT_OTA--)
      await(1);
  }
  else
  {
    sendMessage("[UTILS] No update available.", SERIAL_DEBUG, SEM_TOPICO);
  }

  return SUCESSO;
}

void Utils::sendHealthCheck()
{
  StaticJsonDocument<450> pacote;

  sendMessage("[TASK] Getting values for healthcheck.", SERIAL_DEBUG, SEM_TOPICO);

  String ssid = NVS.getString("SSID_SALVO");
  String senha = NVS.getString("PASS_SALVO");
  String id = NVS.getString(ID_SMARTAIR);
  String ip = connection->getIP();
  float intensidade_wifi = connection->getConnStrength();

#ifdef VAPE_DETECTOR
  pacote["ID"] = id;
  pacote["UPTIME"] = millis() / 1000;
  pacote["tipo"] = String(PACOTE_HEALTHCHECK);
  pacote["VBAT"] = (analogRead(35) * 0.0011);
  pacote["FIRMWARE"] = V_FIRMWARE;
  pacote["UPCOUNT"] = qntdLeituras;
  pacote["WIFI_STRENGTH"] = intensidade_wifi;
  pacote["SSID"] = ssid;
  pacote["PASS"] = senha;
  pacote["ERR"] = errCount;
  pacote["BOOT"] = dataBoot;
  pacote["IP"] = ip;
#elif defined SMARTAIR
  pacote["ID"] = id;
  pacote["UPTIME"] = millis() / 1000;
  pacote["tipo"] = String(PACOTE_HEALTHCHECK);
  pacote["VBAT"] = (analogRead(35) * 0.0011);
  pacote["FIRMWARE"] = V_FIRMWARE;
  pacote["UPCOUNT"] = qntdLeituras;
  pacote["WIFI_STRENGTH"] = intensidade_wifi;
  pacote["SSID"] = ssid;
  pacote["PASS"] = senha;
  pacote["ERR"] = errCount;
  pacote["BOOT"] = dataBoot;
  pacote["IP"] = ip;
#else
  pacote["id_trap"] = id;
  pacote["uptime"] = String(millis() / 1000);
  pacote["tipo"] = String(PACOTE_HEALTHCHECK);
  pacote["vbat"] = (analogRead(34) * 0.001241);
  pacote["vfirmware"] = V_FIRMWARE;
  pacote["wifi_strength"] = intensidade_wifi;
  pacote["ssid"] = ssid;
  pacote["password"] = senha;
  pacote["err_count"] = errCount;
  pacote["boot"] = dataBoot;
#endif
  String conteudoEnvio = "";

  serializeJson(pacote, conteudoEnvio);

  sendMessage("[TASK] Json: " + conteudoEnvio, SERIAL_DEBUG, SEM_TOPICO);

  sendMessage(conteudoEnvio, POST_DEBUG, SERVIDOR);
}

String Utils::createPackage(std::map<std::string, float> leituras)
{
#ifdef SMARTAIR
  pacoteEnvio["tipo"] = PACOTE_SENSOR;
  pacoteEnvio["temperatura"] = leituras["TEMPERATURA"];
  pacoteEnvio["umidade"] = leituras["UMIDADE"];
  pacoteEnvio["co2"] = leituras["CO2"];
  pacoteEnvio["pressao"] = leituras["PRESSAO"];
  pacoteEnvio["gas"] = leituras["GAS"];
  pacoteEnvio["nox"] = leituras["NOX"];
  pacoteEnvio["voc"] = leituras["VOC"];
  pacoteEnvio["etanol"] = leituras["ETANOL"];
  pacoteEnvio["h2"] = leituras["H2"];
  pacoteEnvio["lux"] = 0;
  pacoteEnvio["v_firmware"] = V_FIRMWARE;
  pacoteEnvio["eco2"] = leituras["ECO2"];
  pacoteEnvio["tvoc"] = leituras["TVOC"];
  pacoteEnvio["db"] = leituras["RUIDO"];
  pacoteEnvio["uptime"] = millis() / 1000;
  pacoteEnvio["voltage"] = getVoltage();
  pacoteEnvio["wifi_strength"] = connection->getConnStrength();
  pacoteEnvio["idDispositivo"] = smartAir_id;
#elif VAPE_DETECTOR
  pacoteEnvio["tipo"] = PACOTE_SENSOR;
  pacoteEnvio["temperatura"] = leituras["TEMPERATURA"];
  pacoteEnvio["umidade"] = leituras["UMIDADE"];
  pacoteEnvio["co2"] = leituras["CO2"];
  pacoteEnvio["pressao"] = leituras["PRESSAO"];
  pacoteEnvio["gas"] = leituras["GAS"];
  pacoteEnvio["nox"] = leituras["NOX"];
  pacoteEnvio["voc"] = leituras["VOC"];
  pacoteEnvio["etanol"] = leituras["ETANOL"];
  pacoteEnvio["h2"] = leituras["H2"];
  pacoteEnvio["lux"] = 0;
  pacoteEnvio["v_firmware"] = V_FIRMWARE;
  pacoteEnvio["eco2"] = leituras["ECO2"];
  pacoteEnvio["tvoc"] = leituras["TVOC"];
  pacoteEnvio["db"] = leituras["RUIDO"];
  pacoteEnvio["uptime"] = millis() / 1000;
  pacoteEnvio["voltage"] = getVoltage();
  pacoteEnvio["wifi_strength"] = connection->getConnStrength();
  pacoteEnvio["idDispositivo"] = smartAir_id;
#elif SMARTBAIT
  pacoteEnvio["reason"] = "Detecção";
  pacoteEnvio["note"] = "Detecção de movimento";
  pacoteEnvio["id_trap"] = smartAir_id;
  pacoteEnvio["acknowledged"] = false;
#endif

  String conteudoEnvio = "";
  serializeJson(pacoteEnvio, conteudoEnvio);
  Serial.println(conteudoEnvio);
  return conteudoEnvio;
}

void Utils::verifyGeneralState(uint8_t qntdLeituras)
{
  verifyUpdates();

  if (qntdLeituras > AMOSTRAS_POR_DIA)
  {
    sendMessage("[UTILS] Daily sample limit reached.", SERIAL_DEBUG, SEM_TOPICO);
    restart();
  }

  sendMessage("[UTILS] Current sample: " + String(qntdLeituras), SERIAL_DEBUG, SEM_TOPICO);
}

void Utils::waitNextSendTime()
{
  bool ledState = 1;

  sendMessage("[UTILS] Waiting for next send time.", SERIAL_DEBUG, SEM_TOPICO);

  LED_STATE = ON_IDLE;

  for (int i = 0; i < TIME_TO_SLEEP; i++)
  {
    vTaskDelay(pdMS_TO_TICKS(1000));
    sendMessage("[STATEMACHINE] (" + String(i) + "/" + String(TIME_TO_SLEEP) + ") seconds - Waiting for the next send.", SERIAL_DEBUG, SEM_TOPICO);
  }

  vTaskDelay(pdMS_TO_TICKS(3000));

  sendMessage("[UTILS] Time finished.", SERIAL_DEBUG, SEM_TOPICO);
}

uint8_t Utils::cleanPackages()
{
  sendMessage("[UTILS] Cleaning saved packages.", SERIAL_DEBUG, SEM_TOPICO);
  NVS.setString(PACOTES_SALVOS, "");
  vTaskDelay(pdMS_TO_TICKS(1000));
}

uint8_t Utils::sendNVSpackages()
{

  String pacotesSemEnvio = NVS.getString(PACOTES_SALVOS);

  int count = 0;
  for (int i = 0; i < pacotesSemEnvio.length(); i++)
  {
    if (pacotesSemEnvio[i] == ';')
      count++;
  }

  if (pacotesSemEnvio.indexOf(";") != -1)
  {
    sendMessage("[UTILS] Saved packages found. Number of packages: " + String(count), SERIAL_DEBUG, SEM_TOPICO);
    String pacote = "";

    for (int i = 0; i < count; i++)
    {
      pacote = getValue(pacotesSemEnvio, ';', i);
      pacote.replace(";", "");
      sendMessage("[UTILS] Sending package " + String(i) + ": " + pacote, SERIAL_DEBUG, SEM_TOPICO);
      if (connection->sendData(pacote, SERVIDOR) == SUCESSO)
      {
        sendMessage("[UTILS] Package sent successfully.", SERIAL_DEBUG, SEM_TOPICO);
      }
      else
      {
        sendMessage("[UTILS] Error sending package " + String(i) + ".", SERIAL_DEBUG, SEM_TOPICO);
        continue;
      }

      delay(100);
    }

    NVS.setString(PACOTES_SALVOS, "");
  }
  else
  {
    sendMessage("[UTILS] No saved packages.", SERIAL_DEBUG, SEM_TOPICO);
    return SUCESSO;
  }

  return SUCESSO;
}

void Utils::storePackageNVS(String pacote)
{
  sendMessage("[UTILS] Saving content (" + pacote + ") to NVS.", SERIAL_DEBUG, SEM_TOPICO);

  String conteudo = NVS.getString(PACOTES_SALVOS);
  conteudo = conteudo + pacote + ";";
  saveContentParameterNVS(PACOTES_SALVOS, conteudo, TIPO_STRING);
  vTaskDelay(pdMS_TO_TICKS(1000));

  sendMessage("[UTILS] Package saved successfully.", SERIAL_DEBUG, SEM_TOPICO);
}

void Utils::saveContentNVS(String chave, String tipo)
{
  Serial.print("[TASK] Enter the content: ");
  task.input = "";
  while (task.input.equals(""))
    task.input = Serial.readStringUntil('\x0D');

  if (tipo.equals(TIPO_STRING))
  {
    NVS.setString(chave.c_str(), task.input);
    Serial.println("[TASK] Value set: " + task.input);
  }
  else
  {
    NVS.setString(chave.c_str(), task.input);
    Serial.println("[TASK] Value set: " + task.input.toInt());
  }
}

void Utils::saveContentParameterNVS(String chave, String conteudo, String tipo)
{

  if (tipo.equals(TIPO_STRING))
  {
    NVS.setString(chave.c_str(), conteudo);
    Serial.println("[TASK] Value set: " + conteudo);
  }
  else
  {
    NVS.setString(chave.c_str(), conteudo);
    Serial.println("[TASK] Value set: " + conteudo.toInt());
  }
}

void Utils::showMenu()
{
  Serial.println(
      "~~~~~~~~~~~~~~~SmartAir - POWERED BY UFG~~~~~~~~~~~~~~~");
  Serial.println("List of available commands:");
  Serial.println("'setid': Set the SmartAir ID.");
  Serial.println("'i2cdetect': Show I2C devices.");
  Serial.println("'resetwifi': Reset WiFi credentials.");
  Serial.println(
      "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
}

void Utils::registraRecebimentoPacote(String pacote)
{
  if (ponteiroPacote < NUMERO_MAXIMO_PACOTES)
  {
    pacotes += pacote + ',';
    ponteiroPacote++;
  }
  else
  {
    saveContentParameterNVS("pacote", pacote, TIPO_STRING);
  }
}

void Utils::hibernate()
{
  sendMessage("[UTILS] Entering hibernation mode.", SERIAL_DEBUG, SEM_TOPICO);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
  esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL, ESP_PD_OPTION_OFF);
  adc_power_off();
  WiFi.disconnect(true); // Disconnect from the network
  WiFi.mode(WIFI_OFF);   // Switch WiFi off
  btStop();
  esp_bt_controller_disable();

#ifndef SMARTBAIT
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
#else
  uint64_t t_sec = 86400;
  uint64_t sleeptime = UINT64_C(t_sec * 1000000);
  if (nAlerts < LIMIT_ALERTS)
  {
    sendMessage("[UTILS] Device will be hibernating till next detection.", SERIAL_DEBUG, SEM_TOPICO);
  }
  else
  {
    sendMessage("[UTILS] Device will be hibernating for 86400 seconds.", SERIAL_DEBUG, SEM_TOPICO);
  }
  esp_sleep_enable_timer_wakeup(sleeptime);
#endif
  esp_deep_sleep_start();
}