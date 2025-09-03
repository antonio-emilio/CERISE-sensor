#ifndef _MQTT_H
#define _MQTT_H

#include "Arduino.h"
#include "globaldef.h"
#include "utils.h"
#include "WiFi.h"
#include <PubSubClient.h>

class MQTT
{
public:
  String USER_MQTT = "";
  String SENHA_MQTT = "";
  String CLIENTE_MQTT = "200.137.220.39";
  uint8_t PORTA_MQTT = 1883;
  String MQTT_USER;

  /**
   * Faz a configuração primária do MQTT
   * @param void sem parametros
   * @return uint8_t SUCESSO(1) caso consiga configurar ou FALHA(0) caso contrario
   */
  uint8_t startConn(void);

  /**
   * Verifica status de conexao do MQTT
   * @param void sem parametros
   * @return uint8_t SUCESSO(1) caso consiga reconectar ou ja esteja conectado ou FALHA(0) caso contrario
   */
  uint8_t verifyConn(void);

  /**
   * Reconecta ao MQTT
   * @param void sem parametros
   * @return uint8_t SUCESSO(1) caso consiga reconectar ou ja esteja conectado ou FALHA(0) caso contrario
   */
  uint8_t reconnectConn(void);

  /**
   * Envia mensagem para o broker MQTT
   * @param String topico - topico da mensagem
   * @param String mensagem - mensagem a ser enviada
   * @return uint8_t SUCESSO(1) caso consiga enviar a mensagem ou FALHA(0) caso contrario
   */
  uint8_t sendMessage(String topico, String mensagem);

  /**
   * Funcao que lida com os comandos passados via MQTT
   * @param conteudoGeral String - conteudo da mensagem de comando: comando/mensagem
   * @return Sem retorno
   */
  void threatCommands(String conteudoGeral);

  void mqttLoop(void);
};

#endif