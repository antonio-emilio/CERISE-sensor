#include "mesh.h"
#include "utils.h"
#include <esp_wifi.h>

int fMeshAtivada = FLAG_DESATIVADA;
int isBootFinished = FLAG_DESATIVADA;
int fMeshMasterAtivada = FLAG_DESATIVADA;
bool packageAvailable = FLAG_DESATIVADA;
bool shouldReboot = FLAG_DESATIVADA;

uint8_t slaveAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
Utils utils;
data_struct_espnow data_espnow;

void OnRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  char *buff = (char *)incomingData;
  String message = String(buff);
  Serial.println("[ESP_NOW] Received message: " + message);

  String savedPackets = NVS.getString(PACOTES_SALVOS);
  NVS.setString(PACOTES_SALVOS, savedPackets + message + ";");
  packageAvailable = true;
}

void configureESPNOWrecv()
{
  Serial.println("[ESP_NOW] Configuring ESP-NOW to receive data.");
  WiFi.mode(WIFI_AP_STA);
  delay(1000);
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("[ESP_NOW] There was an error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnRecv);

  bool apStarted = WiFi.softAP(String(String(DEVICE_NAME) + "(" + String(smartAir_id) + ")").c_str(), "7412369852");
  Serial.println("[ESP_NOW] ESP-NOW configured successfully.");
}

void OnSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  if (status == ESP_NOW_SEND_SUCCESS)
  {
    Serial.println("[ESP_NOW] Message sent successfully");
  }
  else
  {
    Serial.println("[ESP_NOW] Failed to send message");
  }
}

constexpr char WIFI_SSID[] = "SmartGateway";

int32_t getWiFiChannel(const char *ssid)
{
  Serial.println("[ESP_NOW] Getting WiFi channel.");
  if (int32_t n = WiFi.scanNetworks())
  {
    for (uint8_t i = 0; i < n; i++)
    {
      if (strstr(WiFi.SSID(i).c_str(), ssid))
      {
        Serial.println("[ESP_NOW] Found network: " + String(ssid) + " on channel " + String(WiFi.channel(i)));
        return WiFi.channel(i);
      }
    }

    Serial.println("[ESP_NOW] SmartGateway SSID not found. Using default channel 1.");
    shouldReboot = true;
  }
  return 1;
}

void configureESPNOWsend()
{
  Serial.println("[ESP_NOW] Configuring ESP-NOW to send data.");

  WiFi.mode(WIFI_STA);

  int32_t channel = getWiFiChannel(WIFI_SSID);
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

  Serial.println("[ESP_NOW] Channel: " + String(channel));

  if (esp_now_init() != ESP_OK)
  {
    Serial.println("[ESP_NOW] There was an error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnSent);

  esp_now_peer_info_t slaveInfo = {};
  memcpy(slaveInfo.peer_addr, slaveAddress, 6);
  slaveInfo.channel = 0;
  slaveInfo.encrypt = false;

  if (esp_now_add_peer(&slaveInfo) != ESP_OK)
  {
    Serial.println("There was an error registering the slave");
    return;
  }
  Serial.println("[ESP_NOW] ESP-NOW configured to send data.");
}

boolean espnow_habilitado()
{
#ifdef ESPNOW_ENABLE
  return true;
#else
  return false;
#endif
}

boolean espnow_root()
{
#ifdef SMARTGATEWAY
  return true;
#else
  return false;
#endif
}

boolean espnow_node()
{
#ifdef ESPNOW_ENABLE
  return true;
#else
  return false;
#endif
}

uint8_t sendData(String content)
{
  Serial.println("[ESP_NOW] Sending data: " + content);

  strncpy((char *)data_espnow.message, content.c_str(), TAMANHO_MAXIMO_PACOTE - 1);
  data_espnow.message[TAMANHO_MAXIMO_PACOTE - 1] = '\0';
  esp_err_t result = esp_now_send(slaveAddress, (uint8_t *)&data_espnow, sizeof(data_espnow));

  if (result == ESP_OK)
  {
    Serial.println("[ESP_NOW] Sent successfully");
    return SUCESSO;
  }
  else
  {
    Serial.println("[ESP_NOW] Failed to send");
    return ERRO;
  }
}