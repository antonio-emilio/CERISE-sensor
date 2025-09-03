
#include "connectionGprs.h"
#ifdef GPRS_ENABLE
#include "SoftwareSerial.h"
#include <TinyGsmClient.h>
#include <StreamDebugger.h>

const char apn[] = "timbrasil.br";
const char gprsUser[] = "tim";
const char gprsPass[] = "tim";

static Utils utils;
SoftwareSerial mySerial(16, 17);

#define SerialAT Serial1
#ifdef DEBUGGPRS
StreamDebugger debugger(SerialAT, Serial);
TinyGsm modem(debugger);
#else
TinyGsm modem(mySerial);
#endif

TinyGsmClient clientGSM(modem);

uint8_t ConnectionGPRS::setupConn(void)
{
  // mySerial.begin(4800);
  pinMode(16, INPUT_PULLUP);
  pinMode(17, OUTPUT);

  SerialAT.begin(115200, SERIAL_8N1, 16, 17, false);
  vTaskDelay(pdMS_TO_TICKS(1000));
  pinMode(19, OUTPUT);
  pinMode(3, INPUT);
  digitalWrite(19, HIGH);

  vTaskDelay(pdMS_TO_TICKS(100));

  digitalWrite(19, LOW);
  vTaskDelay(pdMS_TO_TICKS(1000));
  digitalWrite(19, HIGH);

  Serial.println("[SIM800C] Aguarde...");

  modem.restart();

  String modemInfo = modem.getModemInfo();
  Serial.println("[GPRS] Informacoes do modem: ");
  Serial.println(modemInfo);

  if (GSM_PIN && modem.getSimStatus() != 3)
  {
    modem.simUnlock(GSM_PIN);
  }
}

uint8_t ConnectionGPRS::startConn(void)
{
  Serial.println("[GPRS] Aguardando rede");
  if (!modem.waitForNetwork(240000L))
  {
    Serial.println(" [GPRS] Erro ao aguardar rede");
    return 0;
  }

  if (modem.isNetworkConnected())
  {
    Serial.println(" [GPRS] Rede encontrada com sucesso");
  }

  Serial.println(F("[GPRS] Conectando utilizando APN "));
  Serial.println(apn);
  Serial.println(modem.getLocalIP());
  if (!modem.gprsConnect(apn, gprsUser, gprsPass))
  {
    Serial.println("[GPRS] Erro ao conectar com APN");
    return 0;
  }
  else
  {
    Serial.println("[GPRS] Conexao realizada");
  }

  if (modem.isGprsConnected())
  {
    Serial.println("[GPRS] GPRS conectado com sucesso");
  }
  else
  {
    Serial.println("[GPRS] Houve algum erro ao tentar se conectar com o GPRS");
  }
  Serial.println("[GPRS] Tentando obter horario via NTP");
  modem.NTPServerSync("pool.ntp.org", 20);
  Serial.println("[GPRS] Horario obtido com sucesso via NTP");
  return SUCESSO;
}

uint8_t ConnectionGPRS::stopConn(void)
{
  modem.gprsDisconnect();
  Serial.println(("[GPRS] GPRS desconectado"));
}

uint8_t ConnectionGPRS::sendData(String data, String url)
{
  if (clientGSM.connect("pkg-receiver.floats.com.br", 80))
  {
    clientGSM.print(data);
    clientGSM.stop();
  }
  else
  {
    Serial.println("[GPRS] Nao foi possivel se conectar via GPRS");
  }
}

#endif