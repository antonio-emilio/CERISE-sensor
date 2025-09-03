#include "mqtt.h"
#include "ircontrol.h"
#include "mesh.h"

static Utils utils;
#ifdef IRCONTROL_ENABLE
static Irctrl ircontrol;
#endif
WiFiClient espClient;
PubSubClient mqttClient(espClient);

bool isLampOn = true;

void callback(char *topic, byte *message, unsigned int length)
{
    String messageTemp;
    MQTT mqtt;
    for (int i = 0; i < length; i++)
    {
        messageTemp += (char)message[i];
    }

    utils.sendMessage("[MQTT] Received message: " + messageTemp, SERIAL_DEBUG, SEM_TOPICO);

    mqtt.sendMessage((TOPICO + String(smartAir_id) + "confirmacao"), "[" + String(smartAir_id) + "]Message received - OK");
    mqtt.threatCommands(messageTemp);
}

uint8_t MQTT::startConn()
{
    if (espnow_node() || ethernet_enable || lora_enable)
    {
        return SUCESSO;
    }

    MQTT_USER = "smartair_" + String(ESP.getChipModel()) + "_" + String(smartAir_id);
    utils.sendMessage("[MQTT] Starting connection to MQTT broker (" + MQTT_USER + ")", SERIAL_DEBUG, SEM_TOPICO);
    uint8_t timeoutMQTT = 5;
    mqttClient.setServer(CLIENTE_MQTT.c_str(), 1883);
    mqttClient.setCallback(callback);

    while (!mqttClient.connected())
    {
        timeoutMQTT--;
        if (timeoutMQTT == 0)
        {
            utils.sendMessage("[MQTT] Error connecting to broker.", SERIAL_DEBUG, SEM_TOPICO);
            return ERRO;
        }

        if (mqttClient.connect(MQTT_USER.c_str()))
        {
            utils.sendMessage("[MQTT] Connected to broker.", SERIAL_DEBUG, SEM_TOPICO);
#ifdef ACTUATOR
            if (mqttClient.subscribe("CAE/S101/control/hvac"))
            {
                utils.sendMessage("[MQTT] Subscribed to topic: " + String(TOPICO) + String(smartAir_id), SERIAL_DEBUG, SEM_TOPICO);
            }
            else
            {
                utils.sendMessage("[MQTT] Error subscribing to topic.", SERIAL_DEBUG, SEM_TOPICO);
            }

            if (mqttClient.subscribe("CAE/S101/control/lamp"))
            {
                utils.sendMessage("[MQTT] Subscribed to topic: CAE/S101/control/lamp", SERIAL_DEBUG, SEM_TOPICO);
            }
            else
            {
                utils.sendMessage("[MQTT] Error subscribing to topic.", SERIAL_DEBUG, SEM_TOPICO);
            }

            if (mqttClient.subscribe("CAE/S101/control/outlet"))
            {
                utils.sendMessage("[MQTT] Subscribed to topic: CAE/S101/control/outlet", SERIAL_DEBUG, SEM_TOPICO);
            }
            else
            {
                utils.sendMessage("[MQTT] Error subscribing to topic.", SERIAL_DEBUG, SEM_TOPICO);
            }
#endif
        }
        else
        {
            utils.sendMessage("[MQTT] Waiting for connection to MQTT broker.", SERIAL_DEBUG, SEM_TOPICO);
            utils.await(1);
        }
    }

    utils.sendMessage("[MQTT] Connected to MQTT broker.", SERIAL_DEBUG, SEM_TOPICO);

    return SUCESSO;
}

void MQTT::mqttLoop()
{
    mqttClient.loop();
}

void MQTT::threatCommands(String content)
{
    uint8_t recognizedCommand = FLAG_DESATIVADA;
    if (content.equals("reiniciar"))
        ESP.restart();

    if (content.equals("power"))
    {
#ifdef IRCONTROL_ENABLE
        ircontrol.setupIR(true);
#endif
        recognizedCommand = SUCESSO;
        utils.sendMessage("[MQTT] Received power command.", SERIAL_DEBUG, SEM_TOPICO);
        String command = NVS.getString("CTRL_PWR");
#ifdef IRCONTROL_ENABLE
        ircontrol.sendCommand(command);
#endif
    }

    if (content.equals("up"))
    {
#ifdef IRCONTROL_ENABLE
        ircontrol.setupIR(true);
#endif
        recognizedCommand = SUCESSO;
        utils.sendMessage("[MQTT] Received up command.", SERIAL_DEBUG, SEM_TOPICO);
        String command = NVS.getString("CTRL_UP");
        utils.sendMessage(command, SERIAL_DEBUG, SEM_TOPICO);
#ifdef IRCONTROL_ENABLE
        ircontrol.sendCommand(command);
#endif
    }

    if (content.equals("down"))
    {
#ifdef IRCONTROL_ENABLE
        ircontrol.setupIR(true);
#endif
        recognizedCommand = SUCESSO;
        utils.sendMessage("[MQTT] Received down command.", SERIAL_DEBUG, SEM_TOPICO);
        String command = NVS.getString("CTRL_DOWN");
#ifdef IRCONTROL_ENABLE
        ircontrol.sendCommand(command);
#endif
    }

    if (content.equals("turn_lamp_on"))
    {
        Serial.println("[MQTT] Received turn_lamp_on command.");
        digitalWrite(LAMP_PIN, LOW);
    }

    if (content.equals("turn_lamp_off"))
    {
        Serial.println("[MQTT] Received turn_lamp_off command.");
        digitalWrite(LAMP_PIN, HIGH);
    }

    if (content.equals("turn_outlet_on"))
    {
        Serial.println("[MQTT] Received turn_outlet_on command.");
        isLampOn = true;
        digitalWrite(OUTLET_PIN, LOW);
    }

    if (content.equals("turn_outlet_off"))
    {
        Serial.println("[MQTT] Received turn_outlet_off command.");
        isLampOn = false;
        digitalWrite(OUTLET_PIN, HIGH);
    }
}

uint8_t MQTT::sendMessage(String topic, String message)
{
    if (verifyConn())
    {
        utils.sendMessage("[MQTT] Sending message: " + message + " to topic: " + topic, SERIAL_DEBUG, SEM_TOPICO);
        mqttClient.publish(topic.c_str(), message.c_str());
        return SUCESSO;
    }
    else
    {
        utils.sendMessage("[MQTT] Error sending message: " + message + " to topic: " + topic, SERIAL_DEBUG, SEM_TOPICO);
        return ERRO;
    }
}

uint8_t MQTT::verifyConn()
{
    if (espnow_node() || ethernet_enable || lora_enable)
    {
        utils.sendMessage("[MQTT] MQTT connection disabled.", SERIAL_DEBUG, SEM_TOPICO);
        return SUCESSO;
    }

    uint8_t TIMEOUT_CONEXAO_MQTT = 5;
    utils.sendMessage("[UTILS] Starting MQTT verification.", SERIAL_DEBUG, SEM_TOPICO);
    if (!mqttClient.connected())
    {
        utils.sendMessage("[MQTT] MQTT connection is not active. Reconnecting.", SERIAL_DEBUG, SEM_TOPICO);
        while (TIMEOUT_CONEXAO_MQTT-- && !reconnectConn())
            ;
        if (TIMEOUT_CONEXAO_MQTT == 0)
        {
            utils.sendMessage("[MQTT] Error reconnecting to broker - TIMEOUT.", SERIAL_DEBUG, SEM_TOPICO);
            return ERRO;
        }
        else
        {
            return SUCESSO;
        }
    }
    else
    {
        return SUCESSO;
    }
}

uint8_t MQTT::reconnectConn()
{
    utils.sendMessage("[UTILS] Starting MQTT reconnection.", SERIAL_DEBUG, SEM_TOPICO);

    if (!mqttClient.connected())
    {
        utils.sendMessage("[MQTT] Reconnecting to MQTT broker.", SERIAL_DEBUG, SEM_TOPICO);
        if (mqttClient.connect("id-1231561561877")) // TODO: CHANGE THIS ID
        {
            utils.sendMessage("[MQTT] Connected to MQTT broker.", SERIAL_DEBUG, SEM_TOPICO);
            return SUCESSO;
        }
        else
        {
            utils.sendMessage("[MQTT] Could not connect to MQTT broker.", SERIAL_DEBUG, SEM_TOPICO);
            return ERRO;
        }
    }
    else
    {
        return SUCESSO;
    }
}