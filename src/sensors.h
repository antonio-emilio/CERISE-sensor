#ifndef _SENSORS_H
#define _SENSORS_H

#include "Arduino.h"
#include "globaldef.h"
#include "utils.h"
#include <map>
#include <BH1750.h>
#include <Wire.h>
#include "Adafruit_BME680.h"
#include <SensirionI2CScd4x.h>
#include <SensirionI2CSgp41.h>
#include "Adafruit_SGP30.h"
#include <GP2Y1010sensor.h>
#include <AGS10.h>
#include <SPI.h>

#define BATERIA_ADDR 0x36
#define RTC_ADDR 0x68
#define BME_ADDR 0x76
#define SGP30_ADDR 0x58
#define AGS10_ADDR 0x1A
#define SGP41_ADDR 0x59
#define SCD41_ADDR 0x62
#define BH1750_ADDR 0x23

// Number of last N raw voltage readings
#define NG2P 200

class Sensors
{
public:
  bool checkI2CDevice(uint8_t deviceAddress);

  void verifyBus(void);
  /**
   * Configura algumas portas do dispositivo
   * @param void sem parametros
   * @return uint8_t SUCESSO(1)
   */
  uint8_t configurePins(void);

  /**
   * Configura os sensores do dispositivo
   * @param void sem parametros
   * @return uint8_t SUCESSO(1)
   */
  uint8_t configureSensors(void);

  /**
   * Configura o sensor BME 280 do dispositivo
   * @param void sem parametros
   * @return uint8_t SUCESSO(1)
   */
  uint8_t configureBME680(void);

  /**
   * Obtem os valores do MHZ
   * @param void sem parametros
   * @return sem retorno
   */
  float getCO2(void);

  /**
   * Obtem os valores do DHT
   * @param void sem parametros
   * @return sem retorno
   */
  float getTemperature(void);

  float getPressure(void);

  float getGas(void);

  /**
   * Obtem os valores do DHT
   * @param void sem parametros
   * @return sem retorno
   */
  float getHumidity(void);

  float getRuido(void);
  float getLux(void);

  /**
   * Calibra o sensor MHZ14A
   * @param void sem parametros
   * @return sem retorno
   */
  void configureSCD41(void);

  void configureSGP41(void);

  float getNOX(void);

  float getVOC(void);

  void configureAGS10(void);

  float getTVOCAGS10(void);

  void configureSGP30(void);

  float getECO2SGP30(void);
  float getTVOCSGP30(void);
  float getH2SGP30(void);
  float getEthanolSGP30(void);
  float getTemperatureSCD41(void);
  float getHumiditySCD41(void);
  bool isButtonPressed();
  int getBatteryLevel();
  void setupG2M();
  float readG2M();
  // Arduino pin numbers.
  const int sharpLEDPin = 14; // Arduino digital pin 7 connect to sensor LED.
  const int sharpVoPin = 10;  // Arduino analog pin 5 connect to sensor Vo.
  // Set the typical output voltage in Volts when there is zero dust.
  float voc = 0.6;
  // Use the typical sensitivity in units of V per 100ug/m3.
  const float KG2P = 0.5;
  uint8_t isVapeDetected(void);

  const byte cmdCal[9] = {0xFF, 0x01, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78};

  std::map<std::string, float> getSensors(void);

  std::map<std::string, float> verifyValues(std::map<std::string, float> saida);

  Adafruit_BME680 bme; // use I2C interface

  SensirionI2CScd4x scd4x;

  SensirionI2CSgp41 sgp41;

  AGS10 ags10 = AGS10();

  Adafruit_SGP30 sgp;

  GP2Y1010sensor GP2;

  // time in seconds needed for NOx conditioning
  uint16_t conditioning_s = 10;

private:
};

#endif
