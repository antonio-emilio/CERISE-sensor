
#include "connectionLora.h"
#ifdef LORA_ENABLE
static Utils utils;
SX126x lora(19, // Port-Pin Output: SPI select
            15, // Port-Pin Output: Reset
            20  // Port-Pin Input:  Busy
);

uint16_t ConnectionLora::minimo(uint16_t a, uint16_t b)
{
  return a < b ? a : b;
}

uint8_t ConnectionLora::setupConn(void)
{
  vTaskDelay(pdMS_TO_TICKS(1000));

  lora.DebugPrint(true);

  int16_t ret = lora.begin(RF_FREQUENCY,     // frequency in Hz
                           TX_OUTPUT_POWER); // tx power in dBm
  if (ret != ERR_NONE)
    while (1)
    {
      Serial.println("Error in init: ");
      delay(10);
    }

  lora.LoRaConfig(LORA_SPREADING_FACTOR,
                  LORA_BANDWIDTH,
                  LORA_CODINGRATE,
                  LORA_PREAMBLE_LENGTH,
                  LORA_PAYLOADLENGTH,
                  true,   // crcOn
                  false); // invertIrq

  return SUCESSO;
}

uint8_t ConnectionLora::sendData(String data, String url)
{
  const uint8_t packageSize = 240;
  uint16_t contentLength = data.length();
  uint16_t numPackages = (contentLength + packageSize - 1) / packageSize; // Calculate the number of packages

  Serial.println("contentLength: " + String(contentLength));

  for (uint16_t packageIndex = 0; packageIndex < numPackages; packageIndex++)
  {
    uint16_t startIndex = packageIndex * packageSize;
    uint16_t endIndex = minimo(startIndex + packageSize, contentLength);
    String packageData = data.substring(startIndex, endIndex);

    uint8_t txData[255];
    sprintf((char *)txData, "%s", packageData.c_str());
    uint8_t len = strlen((char *)txData);

    // Wait for transmission to complete
    if (lora.Send(txData, len, SX126x_TXMODE_SYNC))
    {
      Serial.println("Send success");
    }
    else
    {
      Serial.println("Send fail");
    }

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

uint8_t ConnectionLora::receiveData()
{
  uint8_t rxData[255];
  uint8_t rxLen = lora.Receive(rxData, 255);
  if (rxLen > 0)
  {
    Serial.print("Receive rxLen:");
    Serial.println(rxLen);
    for (int i = 0; i < rxLen; i++)
    {
      Serial.print(rxData[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
    for (int i = 0; i < rxLen; i++)
    {
      if (rxData[i] > 0x19 && rxData[i] < 0x7F)
      {
        char myChar = rxData[i];
        Serial.print(myChar);
      }
      else
      {
        Serial.print("?");
      }
    }
    Serial.println();

    int8_t rssi, snr;
    lora.GetPacketStatus(&rssi, &snr);
    Serial.print("rssi: ");
    Serial.print(rssi, DEC);
    Serial.println(" dBm");
    Serial.print("snr: ");
    Serial.print(snr, DEC);
    Serial.println(" dB");
  }
  vTaskDelay(pdMS_TO_TICKS(1));
}
#endif