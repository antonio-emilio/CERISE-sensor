#ifndef _GLOBALDEF_H
#define _GLOBALDEF_H

#include "globalvar.h"
#define V_FIRMWARE 25

// #define SMARTAIR
//#define SMARTGATEWAY
//#define VAPE_DETECTOR
//#define SMARTBAIT
#define ACTUATOR

#ifdef SMARTAIR
#define DEVICE_NAME "SmartAir "
#define WIFI_ENABLE
// #define ESPNOW_ENABLE
#define MQTT_ENABLE
#elif defined SMARTGATEWAY
#define DEVICE_NAME "SmartGateway "
#define WIFI_ENABLE
#define ESPNOW_ROOT_ENABLE
#elif defined VAPE_DETECTOR
#define DEVICE_NAME "VapeDetector "
#define WIFI_ENABLE
#elif defined SMARTBAIT
#define DEVICE_NAME "SmartBait "
#define LOW_POWER_ENABLE
#define WIFI_ENABLE
//#define ESPNOW_ENABLE
#elif defined ACTUATOR
#define DEVICE_NAME "Actuator "
#define WIFI_ENABLE
#define MQTT_ENABLE
#endif

//#define LOW_POWER_ENABLE
//#define ETH_ENABLE
//#define GPRS_ENABLE
//#define LORA_ENABLE
//#define MQTT_ENABLE
//#define WIFI_ENABLE
//#define ESPNOW_ENABLE
//#define ESPNOW_ROOT_ENABLE
//#define IRCONTROL_ENABLE
//#define CONFIG_SMARTBAIT
//#define ACTUATOR
//#define USEPMSENSOR
//#define USERUIDO

/*Definição de pinos*/
#define RXD2 16
#define TXD2 17
#define BLUE_LED 15
#define GREEN_LED 2
#define BOTAO 5
#define RED_LED 4
#define PIR_1 4
#define PIR_2 15
#define PIR_3 2
#define RUIDO_PIN 10
#define OUTLET_PIN 17
#define LAMP_PIN 4
#define BOOT_PIN 0

#define VALOR_MINIMO_TENSAO 0.00
#define VALOR_MAXIMO_TENSAO 5.00
#define VALOR_MINIMO_DB 37.00
#define VALOR_MAXIMO_DB 82.00
#define sampleWindow 50

/*Definições gerais*/
#define uS_TO_S_FACTOR 1000000
#define TIME_TO_SLEEP 20
#define NUMERO_MAXIMO_PACOTES 200
#define WARMUP_SECONDS 180

#define MAX_TENTATIVAS_WIFI 10
#define MAX_TENTATIVA_LEITURA_SENSOR 5
#define MAX_TENTATIVA_ENVIO 5

/*STATEMACHINE*/
#define AGUARDA_TEMPO 70
#define AMOSTRAS_POR_DIA 720
#define STATE_0 0
#define STATE_1 1
#define STATE_2 2
#define STATE_3 3
#define STATE_4 4
#define STATE_5 5
#define STATE_6 6
#define STATE_7 7
#define MALFUNCIONAMENTO_SENSOR 98
#define INVALID_STATE 99

/*STATUS*/
#define SUCESSO 1
#define ERRO 0
#define FLAG_ATIVADA 1
#define FLAG_DESATIVADA 0
#define VALOR_ZERADO 0

/*TIPOS*/
#define TIPO_STRING "STRING"

/* MQTT */
#define TOPICO_LAMPADA "CAE/S101/control/lamp"
#define TOPICO_LAMPADAS2 "CAE/S101/control/lamp/s2"



/*DEBUG*/
#define SERIAL_DEBUG 1
#define MQTT_DEBUG 2
#define SOCKET_DEBUG 3
#define POST_DEBUG 4
#define ESPNOW_DEBUG 5
#define LORA_DEBUG 6
#define SEM_TOPICO "SEM_TOPICO"
#define PORTA_SOCKET 1883
#define SERVIDOR_ALERTA "http://alert.floats.com.br:2154/send-message"
#define TEMPO_DELAY_WDT 8

/*LED_STATE*/
#define LENDO_SENSORES 1
#define MONTANDO_JSON 10
#define ENVIANDO 20
#define HIBERNANDO 100

#define ON_IDLE 14
#define WORKING 2
#define WIFI 3

/*SERIAL*/
#define VEL_SERIAL_DEBUG 115200
#define VEL_SERIAL_2 9600

/*SENSORES*/
#define VALOR_MAXIMO_CO2 10000
#define VALOR_MINIMO_CO2 400
#define DELAY_LEITURA_CO2 30
#define DELAY_LEITURA_SENSOR 1000

/*NVS*/
#define ID_SMARTAIR "id"
#define ESP_NOW_INIT "espnow"
#define ESP_NOW_ROOT "root"
#define LOW_POWER "lowpower"
#define ETHERNET "ethernet"
#define LORA "lora"
#define PACOTES_PARA_ENVIAR "pacote"
#define PACOTES_SALVOS "pacotesalvo"
#define PHONENUMBER "phone"
#define NRO_ALERT "nroalert"

/*INDICADORES*/
#define ID_AUSENTE 0
#define NUMERO_MAXIMO_TENTATIVAS 10
#define TAMANHO_MAXIMO_PACOTE 240

/*MQTT*/
#define PREFIX_MQTT "CAE/S101/"

/*ALERTS SMARTBAIT*/
#define LIMIT_ALERTS 4

/*TASKS*/
#define TASK_SERIAL "vLowSerial"
#define TASK_LED "vLowLED"
#define TASK_GENERICA "vLowTasks"
#define TASK_HEALTHCHECK "vLowHealthCheck"
#define TASK_BAIXA_PRIORIDADE 2048
#define TASK_ALTA_PRIORIDADE 8196

#define PACOTE_SENSOR 1
#define PACOTE_HEALTHCHECK 2

#define TOPICO "topic/smartair/comandos/"


#ifdef VAPE_DETECTOR 
#define SERVIDOR "https://pkg-receiver.floats.com.br/api/newData"
#elif defined SMARTGATEWAY
#define SERVIDOR "https://pkg-receiver.floats.com.br/api/newData"
#elif defined SMARTAIR
#define SERVIDOR "https://pkg-receiver.floats.com.br/api/newData"
#elif defined SMARTBAIT
#define SERVIDOR "https://mice-package-receive.vercel.app/api/newData"
#elif defined ACTUATOR
#define SERVIDOR "https://pkg-receiver.floats.com.br/api/newData"
#endif

#endif