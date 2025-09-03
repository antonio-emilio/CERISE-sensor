
#include "sensors.h"
#include <BH1750.h>

BH1750 lightMeter;
static Utils utils;
float VALOR_TEMPERATURA_SENSOR;
float VALOR_UMIDADE_SENSOR;
float VALOR_CO2_SENSOR;
bool hasBateria;
bool hasRTC;
bool hasBME;
bool hasSGP30;
bool hasAGS10;
bool hasSGP41;
bool hasSCD41;
bool hasBH1750;
unsigned int sample;

std::map<std::string, float> Sensors::getSensors()
{
  std::map<std::string, float> saida;
  utils.sendMessage("[SENSORS] Starting sensor readings.", SERIAL_DEBUG, SEM_TOPICO);
  if (hasSCD41)
  {
    vTaskDelay(pdMS_TO_TICKS(5000));
    saida["CO2"] = getCO2();
    Serial.println("[SENSORS] CO2 level: " + String(saida["CO2"]) + " ppm");
    if (!hasBME)
    {
      saida["UMIDADE"] = getHumiditySCD41();
      Serial.println("[SENSORS] Humidity level: " + String(saida["UMIDADE"]) + " %");
      saida["TEMPERATURA"] = getTemperatureSCD41();
      Serial.println("[SENSORS] Temperature level: " + String(saida["TEMPERATURA"]) + " °C");
    }
  }
  else
  {
    saida["CO2"] = 0;
  }
  vTaskDelay(pdMS_TO_TICKS(1000));
  if (hasBME)
  {
    saida["TEMPERATURA"] = getTemperature();
    Serial.println("[SENSORS] Temperature level: " + String(saida["TEMPERATURA"]) + " °C");
    saida["UMIDADE"] = getHumidity();
    Serial.println("[SENSORS] Humidity level: " + String(saida["UMIDADE"]) + " %");
    saida["PRESSAO"] = getPressure();
    Serial.println("[SENSORS] Pressure level: " + String(saida["PRESSAO"]) + " hPa");
    saida["GAS"] = getGas();
    Serial.println("[SENSORS] Gas level: " + String(saida["GAS"]) + " KOhms");
  }
  else if (!hasSCD41)
  {
    saida["TEMPERATURA"] = 0;
    saida["UMIDADE"] = 0;
    saida["PRESSAO"] = 0;
    saida["GAS"] = 0;
  }
  vTaskDelay(pdMS_TO_TICKS(1000));
  if (hasSGP30)
  {
    saida["TVOC"] = getTVOCSGP30();
    Serial.println("[SENSORS] TVOC level: " + String(saida["TVOC"]) + " ppb");
    if (!hasSCD41)
    {
      saida["CO2"] = getECO2SGP30();
      Serial.println("[SENSORS] eCO2 level: " + String(saida["CO2"]) + " ppm");
    }
    else
    {
      saida["eCO2"] = getECO2SGP30();
      Serial.println("[SENSORS] eCO2 level: " + String(saida["eCO2"]) + " ppm");
    }
    saida["H2"] = getH2SGP30();
    Serial.println("[SENSORS] H2 level: " + String(saida["H2"]) + " ppm");
    saida["ETANOL"] = getEthanolSGP30();
    Serial.println("[SENSORS] Ethanol level: " + String(saida["ETANOL"]) + " ppm");
  }
  else
  {
    saida["TVOC"] = 0;
    saida["eCO2"] = 0;
    saida["H2"] = 0;
    saida["ETANOL"] = 0;
  }
  vTaskDelay(pdMS_TO_TICKS(1000));
  if (hasAGS10)
  {
    saida["TVOC"] = getTVOCAGS10();
    Serial.println("[SENSORS] AGS10 TVOC level: " + String(saida["TVOC"]) + " ppb");
  }

  vTaskDelay(pdMS_TO_TICKS(1000));
  if (hasSGP41)
  {
    saida["NOX"] = getNOX();
    Serial.println("[SENSORS] NOX level: " + String(saida["NOX"]) + " ppb");
    saida["VOC"] = getVOC();
    Serial.println("[SENSORS] VOC level: " + String(saida["VOC"]) + " ppb");
  }
  else
  {
    saida["NOX"] = 0;
    saida["VOC"] = 0;
  }

#ifdef USEPMSENSOR
  setupG2M();
  saida["GAS"] = readG2M();
  Serial.println("[SENSORS] Vape level: " + String(saida["GAS"]));
  vTaskDelay(pdMS_TO_TICKS(2000));
  Wire.begin(21, 14);
#endif

#ifdef USERUIDO
  saida["RUIDO"] = getRuido();
  Serial.println("[SENSORS] Ruido level: " + String(saida["RUIDO"]) + " dB");
#endif

  if (hasSGP41)
  {
    saida["LUX"] = getLux();
    Serial.println("[SENSORS] Lux level: " + String(saida["LUX"]) + " lux");
  }
  else
  {
    saida["LUX"] = 0;
  }
  vTaskDelay(pdMS_TO_TICKS(1000));
  saida["SUCESSO"] = true;

  utils.sendMessage("[SENSORS] Sensor readings completed.", SERIAL_DEBUG, SEM_TOPICO);

  return saida;
}

void Sensors::setupG2M()
{
  GP2.init(sharpLEDPin, sharpVoPin, KG2P, NG2P);
  Serial.println("[SENSORS] G2M sensor configured.");
}

float Sensors::readG2M()
{
  float mvolt;
  float density;
  GP2.read(&density, &mvolt);

  Serial.println("[SENSORS] G2M sensor reading: " + String(density) + " ug/m3");

  return density;
}

float Sensors::getLux()
{
  lightMeter.begin();
  int countTimes = 10;

  float lux = lightMeter.readLightLevel();
  while (countTimes--)
  {
    lux = lightMeter.readLightLevel();
    delay(100);
  }

  return lux;
}

std::map<std::string, float> Sensors::verifyValues(std::map<std::string, float> saida)
{
  if (saida["CO2"] == VALOR_ZERADO)
  {
    errCount++;
    utils.sendMessage("[SENSORS] Error reading CO2 sensor.", SERIAL_DEBUG, SEM_TOPICO);
    saida["SUCESSO"] = false;
  }

  if (saida["UMIDADE"] == VALOR_ZERADO)
  {
    errCount++;
    utils.sendMessage("[SENSORS] Error reading humidity sensor.", SERIAL_DEBUG, SEM_TOPICO);
    saida["SUCESSO"] = false;
  }

  if (saida["TEMPERATURA"] == VALOR_ZERADO)
  {
    errCount++;
    utils.sendMessage("[SENSORS] Error reading temperature sensor.", SERIAL_DEBUG, SEM_TOPICO);
    saida["SUCESSO"] = false;
  }

  return saida;
}

void Sensors::configureAGS10()
{
  utils.sendMessage("[SENSORS] Configuring TVOC sensor.", SERIAL_DEBUG, SEM_TOPICO);
  ags10.begin();
  vTaskDelay(pdMS_TO_TICKS(100));
  utils.sendMessage("[SENSORS] TVOC sensor configured.", SERIAL_DEBUG, SEM_TOPICO);
}

float Sensors::getTVOCAGS10()
{
  int tvoc = 0;
  tvoc = ags10.readTVOC();
  return tvoc;
}

float Sensors::getRuido()
{
  unsigned long startMillis;
  double voltsTotal = 0;
  uint8_t QUANTIDADE_LEITURAS_MAX = 20;

  while (QUANTIDADE_LEITURAS_MAX--)
  {
    startMillis = millis();
    int signalMax = 0;
    int signalMin = 1024;

    while (millis() - startMillis < sampleWindow)
    {
      int sample = analogRead(RUIDO_PIN);

      if (sample < signalMin)
      {
        signalMin = sample;
      }
      else if (sample > signalMax)
      {
        signalMax = sample;
      }
    }

    unsigned int peakToPeak = signalMax - signalMin;
    double volts = (peakToPeak * 5.0) / 1024.0;
    voltsTotal += volts;
  }

  voltsTotal /= 20;
  float VALOR_DB_SENSOR = utils.mapTo(voltsTotal, VALOR_MINIMO_TENSAO, VALOR_MAXIMO_TENSAO, VALOR_MINIMO_DB, VALOR_MAXIMO_DB);

  utils.sendMessage("[SENSORS] Nível de ruído aferido (" + String(VALOR_DB_SENSOR) + "dB).", SERIAL_DEBUG, SEM_TOPICO);
  return VALOR_DB_SENSOR;
}

void Sensors::configureSGP30()
{
  while (1)
  {
    if (!sgp.begin())
    {
      Serial.println("[SENSORS] Failed to initialize SGP30 sensor.");
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
    else
    {
      break;
    }
  }
  Serial.print("Found SGP30 serial #");
  Serial.print(sgp.serialnumber[0], HEX);
  Serial.print(sgp.serialnumber[1], HEX);
  Serial.println(sgp.serialnumber[2], HEX);
  vTaskDelay(pdMS_TO_TICKS(1000));
}

float Sensors::getEthanolSGP30()
{

  if (!sgp.IAQmeasureRaw())
  {
    Serial.println("Raw Measurement failed");
    return 0;
  }

  return sgp.rawEthanol;
}

float Sensors::getH2SGP30()
{

  if (!sgp.IAQmeasureRaw())
  {
    Serial.println("Raw Measurement failed");
    return 0;
  }

  return sgp.rawH2;
}

float Sensors::getECO2SGP30()
{
  if (!sgp.IAQmeasure())
  {
    Serial.println("Measurement failed");
  }

  return sgp.eCO2;
}

float Sensors::getTVOCSGP30()
{

  if (!sgp.IAQmeasure())
  {
    Serial.println("Measurement failed");
  }

  return sgp.TVOC;
}

void Sensors::configureSGP41()
{
  while (1)
  {
    utils.sendMessage("[SENSORS] Configuring NOX sensor.", SERIAL_DEBUG, SEM_TOPICO);

    uint16_t error;
    char errorMessage[256];
    vTaskDelay(pdMS_TO_TICKS(1000));
    sgp41.begin(Wire);

    uint16_t serialNumber[3];
    uint8_t serialNumberSize = 3;

    error = sgp41.getSerialNumber(serialNumber, serialNumberSize);

    if (error)
    {
      Serial.print("Error trying to execute getSerialNumber(): ");
      errorToString(error, errorMessage, 256);
      Serial.println(errorMessage);
    }
    else
    {
      Serial.print("SerialNumber:");
      Serial.print("0x");
      for (size_t i = 0; i < serialNumberSize; i++)
      {
        uint16_t value = serialNumber[i];
        Serial.print(value < 4096 ? "0" : "");
        Serial.print(value < 256 ? "0" : "");
        Serial.print(value < 16 ? "0" : "");
        Serial.print(value, HEX);
      }
      Serial.println();
      return;
    }
    vTaskDelay(pdMS_TO_TICKS(100));
    utils.sendMessage("[SENSORS] NOX sensor configured.", SERIAL_DEBUG, SEM_TOPICO);
  }
}

float Sensors::getNOX()
{
  while (1)
  {
    uint16_t error;
    char errorMessage[256];
    uint16_t defaultRh = 0x8000;
    uint16_t defaultT = 0x6666;
    uint16_t srawVoc = 0;
    uint16_t srawNox = 0;

    vTaskDelay(pdMS_TO_TICKS(1000));

    if (conditioning_s > 0)
    {
      error = sgp41.executeConditioning(defaultRh, defaultT, srawVoc);
      conditioning_s--;
    }
    else
    {
      error = sgp41.measureRawSignals(defaultRh, defaultT, srawVoc, srawNox);
    }

    if (error)
    {
      Serial.print("Error trying to execute measureRawSignals(): ");
      errorToString(error, errorMessage, 256);
      Serial.println(errorMessage);
    }
    else
    {
      if (srawNox > 0)
        return srawNox;
    }
  }

  return 0;
}

float Sensors::getVOC()
{

  uint16_t error;
  char errorMessage[256];
  uint16_t defaultRh = 0x8000;
  uint16_t defaultT = 0x6666;
  uint16_t srawVoc = 0;
  uint16_t srawNox = 0;

  vTaskDelay(pdMS_TO_TICKS(1000));

  if (conditioning_s > 0)
  {
    error = sgp41.executeConditioning(defaultRh, defaultT, srawVoc);
    conditioning_s--;
  }
  else
  {
    error = sgp41.measureRawSignals(defaultRh, defaultT, srawVoc, srawNox);
  }

  if (error)
  {
    Serial.print("Error trying to execute measureRawSignals(): ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
  }
  else
  {
    return srawVoc;
  }

  return 0;
}

float Sensors::getCO2()
{
  float CO2 = 0;
  uint16_t error;
  char errorMessage[256];

  uint16_t co2 = 0;
  float temperature = 0.0f;
  float humidity = 0.0f;
  bool isDataReady = false;
  error = scd4x.getDataReadyFlag(isDataReady);
  if (error)
  {
    Serial.print("[SENSORS] Error trying to execute getDataReadyFlag(): ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
  }
  if (!isDataReady)
  {
    Serial.println("[SENSORS] No new data available");
  }
  error = scd4x.readMeasurement(co2, temperature, humidity);
  if (error)
  {
    Serial.print("[SENSORS] Error trying to execute readMeasurement(): ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
  }
  else
  {
    return co2;
  }

  return 0;
}

float Sensors::getHumiditySCD41()
{
  float CO2 = 0;
  uint16_t error;
  char errorMessage[256];
  vTaskDelay(pdMS_TO_TICKS(5000));

  uint16_t co2 = 0;
  float temperature = 0.0f;
  float humidity = 0.0f;
  bool isDataReady = false;
  error = scd4x.getDataReadyFlag(isDataReady);
  if (error)
  {
    Serial.print("[SENSORS] Error trying to execute getDataReadyFlag(): ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
  }
  if (!isDataReady)
  {
    Serial.println("[SENSORS] No new data available");
  }
  error = scd4x.readMeasurement(co2, temperature, humidity);
  if (error)
  {
    Serial.print("[SENSORS] Error trying to execute readMeasurement(): ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
  }
  else
  {
    return humidity;
  }

  return 0;
}

float Sensors::getTemperatureSCD41()
{
  float CO2 = 0;
  uint16_t error;
  char errorMessage[256];
  vTaskDelay(pdMS_TO_TICKS(5000));

  uint16_t co2 = 0;
  float temperature = 0.0f;
  float humidity = 0.0f;
  bool isDataReady = false;
  error = scd4x.getDataReadyFlag(isDataReady);
  if (error)
  {
    Serial.print("[SENSORS] Error trying to execute getDataReadyFlag(): ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
  }
  if (!isDataReady)
  {
    Serial.println("[SENSORS] No new data available");
  }
  error = scd4x.readMeasurement(co2, temperature, humidity);
  if (error)
  {
    Serial.print("[SENSORS] Error trying to execute readMeasurement(): ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
  }
  else
  {
    return temperature;
  }

  return 0;
}

void printUint16Hex(uint16_t value)
{
  Serial.print(value < 4096 ? "0" : "");
  Serial.print(value < 256 ? "0" : "");
  Serial.print(value < 16 ? "0" : "");
  Serial.print(value, HEX);
}

void printSerialNumber(uint16_t serial0, uint16_t serial1, uint16_t serial2)
{
  Serial.print("Serial: 0x");
  printUint16Hex(serial0);
  printUint16Hex(serial1);
  printUint16Hex(serial2);
  Serial.println();
}

void Sensors::configureSCD41()
{
  utils.sendMessage("[SENSORS] Configuring SCD41 sensor.", SERIAL_DEBUG, SEM_TOPICO);
  uint16_t error;
  char errorMessage[256];

  scd4x.begin(Wire);

  vTaskDelay(pdMS_TO_TICKS(1000));

  error = scd4x.stopPeriodicMeasurement();
  if (error)
  {
    Serial.print("[SENSORS] Error trying to execute stopPeriodicMeasurement(): ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
  }

  uint16_t serial0;
  uint16_t serial1;
  uint16_t serial2;
  error = scd4x.getSerialNumber(serial0, serial1, serial2);
  if (error)
  {
    Serial.print("[SENSORS] Error trying to execute getSerialNumber(): ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
  }
  else
  {
    printSerialNumber(serial0, serial1, serial2);
  }

  vTaskDelay(pdMS_TO_TICKS(1000));
  error = scd4x.startPeriodicMeasurement();
  if (error)
  {
    Serial.print("[SENSORS] Error trying to execute startPeriodicMeasurement(): ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
  }

  vTaskDelay(pdMS_TO_TICKS(1000));

  utils.sendMessage("[SENSORS] CO2 sensor configured.", SERIAL_DEBUG, SEM_TOPICO);
}

float Sensors::getTemperature()
{
  if (!bme.performReading())
  {
    Serial.println("[SENSORS] Failed to retrieve temperature.");
  }

  float temperature = bme.temperature;

  return temperature;
}

float Sensors::getHumidity()
{
  if (!bme.performReading())
  {
    Serial.println("[SENSORS] Failed to retrieve humidity.");
  }

  float humidity = bme.humidity;

  return humidity;
}

float Sensors::getPressure()
{
  if (!bme.performReading())
  {
    Serial.println("[SENSORS] Failed to retrieve pressure.");
  }

  float pressure = bme.pressure / 100.0;

  return pressure;
}

float Sensors::getGas()
{
  if (!bme.performReading())
  {
    Serial.println("[SENSORS] Failed to retrieve gas reading.");
  }

  float gas = bme.gas_resistance / 1000.0;

  return gas;
}

uint8_t Sensors::configurePins()
{
  utils.sendMessage("[SENSORS] Initializing IO configuration.", SERIAL_DEBUG, SEM_TOPICO);

#ifdef SMARTBAIT
  pinMode(PIR_1, INPUT);
  pinMode(PIR_2, INPUT);
  pinMode(PIR_3, INPUT);
#endif

#ifdef MQTT_ENABLE
  pinMode(OUTLET_PIN, OUTPUT);
  pinMode(LAMP_PIN, OUTPUT);
  pinMode(RUIDO_PIN, INPUT);
  pinMode(BOOT_PIN, INPUT_PULLUP);
  digitalWrite(OUTLET_PIN, HIGH);
  digitalWrite(LAMP_PIN, LOW);
#endif

  utils.sendMessage("[SENSORS] IO configured successfully.", SERIAL_DEBUG, SEM_TOPICO);
  return SUCESSO;
}

uint8_t Sensors::configureSensors()
{
  utils.sendMessage("[SENSORS] Initializing sensor configuration.", SERIAL_DEBUG, SEM_TOPICO);

  Wire.begin(21, 14);
  vTaskDelay(pdMS_TO_TICKS(2000));

  verifyBus();
  utils.sendMessage("[SENSORS] Sensors configured successfully.", SERIAL_DEBUG, SEM_TOPICO);
  return SUCESSO;
}

bool Sensors::checkI2CDevice(uint8_t deviceAddress)
{
  Wire.beginTransmission(deviceAddress);
  uint8_t error = Wire.endTransmission();

  if (error == 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

void Sensors::verifyBus()
{

  Serial.println("[SENSORS] Verifying sensors...");

  hasBateria = checkI2CDevice(BATERIA_ADDR);
  vTaskDelay(pdMS_TO_TICKS(100));
  hasRTC = checkI2CDevice(RTC_ADDR);
  vTaskDelay(pdMS_TO_TICKS(100));
  hasBME = checkI2CDevice(BME_ADDR);
  vTaskDelay(pdMS_TO_TICKS(100));
  hasSGP30 = checkI2CDevice(SGP30_ADDR);
  vTaskDelay(pdMS_TO_TICKS(100));
  hasAGS10 = checkI2CDevice(AGS10_ADDR);
  vTaskDelay(pdMS_TO_TICKS(100));
  hasSGP41 = checkI2CDevice(SGP41_ADDR);
  vTaskDelay(pdMS_TO_TICKS(100));
  hasSCD41 = checkI2CDevice(SCD41_ADDR);
  vTaskDelay(pdMS_TO_TICKS(100));
  hasBH1750 = checkI2CDevice(BH1750_ADDR);
  vTaskDelay(pdMS_TO_TICKS(100));

  if (hasBateria)
  {
    Serial.println(" - Battery (0x36) detected!");
  }

  if (hasRTC)
  {
    Serial.println(" - RTC (0x68) detected!");
  }

  if (hasBME)
  {
    Serial.println(" - BME 680 (0x76) detected! Initializing configuration...");
    configureBME680();
  }

  if (hasSGP30)
  {
    Serial.println(" - SGP30 (0x58) detected! Initializing configuration...");
    configureSGP30();
  }

  if (hasAGS10)
  {
    Serial.println(" - AGS10 (0x1A) detected! Initializing configuration...");
    configureAGS10();
  }

  if (hasSGP41)
  {
    Serial.println(" - SGP41 (0x59) detected! Initializing configuration...");
    configureSGP41();
  }

  if (hasSCD41)
  {
    Serial.println(" - SCD41 (0x62) detected! Initializing configuration...");
    configureSCD41();
  }

  if (hasBH1750)
  {
    Serial.println(" - BH1750 (0x23) detected! Initializing configuration...");
    lightMeter.begin();
  }
}

uint8_t Sensors::configureBME680()
{
  utils.sendMessage("[SENSORS] Configuring BME680 sensor.", SERIAL_DEBUG, SEM_TOPICO);
  if (!bme.begin(0x76))
  {
    Serial.println("[SENSORS] Failed to initialize BME680 sensor.");
    return ERRO;
  }
  vTaskDelay(pdMS_TO_TICKS(1000));
  utils.sendMessage("[SENSORS] BME680 sensor configured.", SERIAL_DEBUG, SEM_TOPICO);
  return SUCESSO;
}

int Sensors::getBatteryLevel()
{
  uint16_t value = 0;
  Wire.beginTransmission(BATERIA_ADDR);
  Wire.endTransmission(false);
  Wire.requestFrom(BATERIA_ADDR, 2);
  if (Wire.available() >= 2)
  {
    value = Wire.read();
    value |= Wire.read() << 8;
  }

  return value;
}

bool Sensors::isButtonPressed()
{
  if (digitalRead(BOTAO) == LOW)
  {
    return true;
  }
  else
  {
    return false;
  }
}

uint8_t Sensors::isVapeDetected()
{
  float referenceReading = 0.0f;
  uint8_t detectionCount = 0;

  if (hasAGS10)
  {
    referenceReading = getTVOCAGS10();
    Serial.println("[SENSORS] AGS10 TVOC reference level: " + String(referenceReading) + " ppb");
  }
  else
  {
    Serial.println("[SENSORS] No AGS10 sensor found!");
  }

  vTaskDelay(pdMS_TO_TICKS(1600));

  for (uint8_t i = 0; i < 20; i++)
  {
    float currentReading = 0.0f;

    if (hasAGS10)
    {
      currentReading = getTVOCAGS10();
      Serial.println("[SENSORS] AGS10 TVOC current level: " + String(currentReading) + " ppb");
    }
    else
    {
      Serial.println("[SENSORS] No AGS10 sensor found!");
    }

    if (currentReading >= referenceReading * 1.1f)
    {
      detectionCount++;
    }

    vTaskDelay(pdMS_TO_TICKS(1600));
  }

  if (detectionCount > 1)
  {
    Serial.println("[SENSORS] Vape detected!");
    return 1; 
  }
  else
  {
    Serial.println("[SENSORS] Vape not detected!");
    return 0; 
  }
}