#include "connectionWifi.h"
#include <ESP_WiFiManager.h>

static Utils utils;
ESP_WiFiManager wifiManager;

uint8_t ConnectionWiFi::setupConn(void)
{
  if (espnow_node())
  {
    utils.sendMessage("[WIFI] Ignoring WiFi ConnectionWiFi because the device is a mesh node.", SERIAL_DEBUG, SEM_TOPICO);
    return SUCESSO;
  }
  else if (ethernet_enable)
  {
    utils.sendMessage("[WIFI] Ignoring WiFi ConnectionWiFi because the device is configured to use Ethernet.", SERIAL_DEBUG, SEM_TOPICO);
    return SUCESSO;
  }
  else if (lora_enable)
  {
    utils.sendMessage("[WIFI] Ignoring WiFi ConnectionWiFi because the device is configured to use LoRa.", SERIAL_DEBUG, SEM_TOPICO);
    return SUCESSO;
  }

  String ssid = NVS.getString("SSID_SALVO") != "" ? NVS.getString("SSID_SALVO") : "Antonielli";
  String senha = NVS.getString("PASS_SALVO") != "" ? NVS.getString("PASS_SALVO") : "antonielli@123antonio";

  int tentativas = 0;

  utils.sendMessage("[WIFI] Trying to connect to WiFi: " + String(ssid) + " password: " + String(senha), SERIAL_DEBUG, SEM_TOPICO);

  if (WiFi.status() != WL_CONNECTED)
  {
    LED_STATE = WIFI;
    WiFi.mode(WIFI_AP_STA);
    delay(1000);
    WiFi.begin(ssid.c_str(), senha.c_str());

    while (WiFi.status() != WL_CONNECTED && tentativas++ < MAX_TENTATIVAS_WIFI)
    {
      vTaskDelay(pdMS_TO_TICKS(2000));
      utils.sendMessage("[WIFI] Trying to connect to WiFi: (" + String(tentativas) + "/" + String(MAX_TENTATIVAS_WIFI) + ")", SERIAL_DEBUG, SEM_TOPICO);
    }
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    utils.sendMessage("[WIFI] WiFi ConnectionWiFi established. Signal: " + String(WiFi.RSSI()) + " dBm", SERIAL_DEBUG, SEM_TOPICO);
    utils.sendMessage("[WIFI] WiFi channel: " + String(WiFi.channel()), SERIAL_DEBUG, SEM_TOPICO);
    return SUCESSO;
  }

  LED_STATE = WIFI;
  utils.sendMessage("[WIFI] Starting AP for WiFi configuration...", SERIAL_DEBUG, SEM_TOPICO);

#ifdef VAPE_DETECTOR
  ESP_WMParameter custom_phone("<label for='phone'>Numero de telefone:</label><br><input type='text' id='phone' name='phone' placeholder='Numero de telefone (Ex: 556299999999)'>");

  wifiManager.addParameter(&custom_phone);
#endif

  WiFi.setAutoConnect(true);
  wifiManager.setTimeout(80);
  wifiManager.setBreakAfterConfig(true);

  if (!wifiManager.autoConnect((SSID_SMARTAIR + " (" + String(smartAir_id) + ")").c_str(), SENHA_SMARTAIR.c_str()))
  {

    utils.sendMessage("[ERRO] Failed to connect to SSID: " + wifiManager.WiFi_SSID(), SERIAL_DEBUG, SEM_TOPICO);
    NVS.setString("SSID_SALVO", wifiManager.WiFi_SSID());
    NVS.setString("PASS_SALVO", wifiManager.WiFi_Pass());
    vTaskDelay(pdMS_TO_TICKS(3000));
    return ERRO;
  }

  utils.sendMessage("[WIFI] Saving WiFi credentials...", SERIAL_DEBUG, SEM_TOPICO);
  NVS.setString("SSID_SALVO", wifiManager.WiFi_SSID());
  NVS.setString("PASS_SALVO", wifiManager.WiFi_Pass());

  LED_STATE = WORKING;

  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(RED_LED, LOW);

  return SUCESSO;
}

uint8_t ConnectionWiFi::verifyConn(void)
{
  uint8_t TIMEOUT_CONEXAO_WIFI = 30;
  if (espnow_node() || ethernet_enable || lora_enable)
  {
    return SUCESSO;
  }

  while (WiFi.status() != WL_CONNECTED && TIMEOUT_CONEXAO_WIFI)
  {
    restartConn();
    TIMEOUT_CONEXAO_WIFI--;
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    return SUCESSO;
  }
  else
  {
    utils.sendMessage("[WIFI] Failed to connect to WiFi.", SERIAL_DEBUG, SEM_TOPICO);
    return ERRO;
  }
}

uint8_t ConnectionWiFi::stopConn(void)
{
  WiFi.disconnect();
}

uint8_t ConnectionWiFi::restartConn(void)
{
  utils.sendMessage("[WIFI] Trying to reconnect to WiFi network.", SERIAL_DEBUG, SEM_TOPICO);
  WiFi.disconnect();
  utils.await(1);
  WiFi.reconnect();

  vTaskDelay(pdMS_TO_TICKS(2000));

  if (WiFi.status() == WL_CONNECTED)
  {
    utils.sendMessage("[WIFI] WiFi reconnected.", SERIAL_DEBUG, SEM_TOPICO);
    return SUCESSO;
  }
  else
  {
    utils.sendMessage("[WIFI] Failed to reconnect to WiFi network.", SERIAL_DEBUG, SEM_TOPICO);
    return ERRO;
  }
}

uint8_t ConnectionWiFi::getConnType(void)
{
  utils.sendMessage("[ConnectionWiFi] ConnectionWiFi type not defined.", SERIAL_DEBUG, SEM_TOPICO);
  return ERRO;
}

uint8_t ConnectionWiFi::getConnStatus(void)
{
  utils.sendMessage("[ConnectionWiFi] ConnectionWiFi type not defined.", SERIAL_DEBUG, SEM_TOPICO);
  return ERRO;
}

uint8_t ConnectionWiFi::getConnStrength(void)
{
  return WiFi.RSSI();
}

String ConnectionWiFi::getIP(void)
{
  return WiFi.localIP().toString();
}

String ConnectionWiFi::getSSID(void)
{
  utils.sendMessage("[ConnectionWiFi] ConnectionWiFi type not defined.", SERIAL_DEBUG, SEM_TOPICO);
  return "";
}

String ConnectionWiFi::getPass(void)
{
  utils.sendMessage("[ConnectionWiFi] ConnectionWiFi type not defined.", SERIAL_DEBUG, SEM_TOPICO);
  return "";
}

uint8_t ConnectionWiFi::sendData(String data, String url)
{
  if (WiFi.status() == WL_CONNECTED)
  {
    utils.sendMessage("[WIFI] Sending package to the server.", SERIAL_DEBUG, SEM_TOPICO);

    HTTPClient http;

    String parameters = url;
    http.begin(parameters);

    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(data);

    if (httpResponseCode == 200)
    {
      utils.sendMessage("[WIFI] Package sent successfully.", SERIAL_DEBUG, SEM_TOPICO);
      http.end();
      return SUCESSO;
    }

    utils.sendMessage("[ERRO] Failed to send package.", SERIAL_DEBUG, SEM_TOPICO);
    return ERRO;
  }
  else
  {
    utils.sendMessage("[ERRO] Failed to send package with content.", SERIAL_DEBUG, SEM_TOPICO);
    return ERRO;
  }
}

uint8_t ConnectionWiFi::resetCredentials(void)
{
  utils.sendMessage("[ConnectionWiFi] Restarting WiFi credentials.", SERIAL_DEBUG, SEM_TOPICO);
  wifiManager.resetSettings();
  utils.sendMessage("[ConnectionWiFi] WiFi credentials restarted.", SERIAL_DEBUG, SEM_TOPICO);

  return SUCESSO;
}