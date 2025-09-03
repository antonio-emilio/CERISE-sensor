#ifndef _GLOBALVAR_H
#define _GLOBALVAR_H

extern float VALOR_TEMPERATURA_SENSOR;
extern float VALOR_UMIDADE_SENSOR;
extern uint16_t VALOR_LUMINOSIDADE_SENSOR;
extern float VALOR_ECO2_SENSOR;
extern float VALOR_TVOC_SENSOR;
extern float VALOR_CO2_SENSOR;
extern float VALOR_DB_SENSOR;
extern float VALOR_C0;
extern float VALOR_NO2;
extern float VALOR_NH3;
extern float VALOR_C3H8;
extern float VALOR_C4H10;
extern float VALOR_CH4;
extern float VALOR_H2;
extern float VALOR_C2H5OH;
extern unsigned int sample;
extern uint8_t smartAir_id;
extern uint8_t nAlerts;
extern uint8_t esp_now_ini;
extern uint8_t low_power_enable;
extern uint8_t ethernet_enable;
extern uint8_t lora_enable;
extern uint8_t esp_now_root;
extern uint8_t LED_STATE;
extern uint8_t qntdLeituras;
extern String phone;
extern uint8_t errCount;
extern bool hasBateria;
extern bool hasRTC;
extern bool hasBME;
extern bool hasSGP30;
extern bool hasAGS10;
extern bool hasSGP41;
extern bool hasSCD41;
extern bool hasBH1750;
extern String dataBoot;
extern bool packageAvailable;
extern bool shouldReboot;
extern bool isLampOn;

#endif