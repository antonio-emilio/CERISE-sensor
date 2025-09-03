#include "ircontrol.h" // Define macros for input and output pin etc.

#ifdef IRCONTROL_ENABLE
#include <Arduino.h>

#if !defined(RAW_BUFFER_LENGTH)
#if RAMEND <= 0x4FF || RAMSIZE < 0x4FF
#define RAW_BUFFER_LENGTH 180 // 750 (600 if we have only 2k RAM) is the value for air condition remotes. Default is 112 if DECODE_MAGIQUEST is enabled, otherwise 100.
#elif RAMEND <= 0x8FF || RAMSIZE < 0x8FF
#define RAW_BUFFER_LENGTH 600 // 750 (600 if we have only 2k RAM) is the value for air condition remotes. Default is 112 if DECODE_MAGIQUEST is enabled, otherwise 100.
#else
#define RAW_BUFFER_LENGTH 750 // 750 (600 if we have only 2k RAM) is the value for air condition remotes. Default is 112 if DECODE_MAGIQUEST is enabled, otherwise 100.
#endif
#endif

#define MARK_EXCESS_MICROS 20

#include <IRremote.hpp>

//+=============================================================================
void Irctrl::setupIR(boolean send)
{
    Serial.println("[IR] Starting IR module setup");
    if (send)
    {
        Serial.println("[IR] Starting IR module in send mode");
        IrReceiver.begin(DISABLE_LED_FEEDBACK);
    }
    else
    {
        Serial.println("[IR] Starting IR module in receive mode");
        IrReceiver.begin(IR_RECEIVE_PIN, DISABLE_LED_FEEDBACK);
    }
    Serial.println("[IR] Ready to receive protocols:");
    printActiveIRProtocols(&Serial);
    Serial.println("on pin " STR(IR_RECEIVE_PIN));
}

String Irctrl::loopIR()
{
    uint32_t timeout = 600;
    while (timeout--)
    {
        Serial.println("[IR] Waiting for IR command...");
        if (IrReceiver.decode())
        {
            Serial.println("\n\n");
            IrReceiver.printIRResultShort(&Serial);
            if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_WAS_OVERFLOW)
            {
                Serial.println(F("Try to increase the \"RAW_BUFFER_LENGTH\" value of " STR(RAW_BUFFER_LENGTH) " in " __FILE__));
            }
            else
            {
                if (IrReceiver.decodedIRData.protocol == UNKNOWN)
                {
                    Serial.println(F("Received noise or an unknown (or not yet enabled) protocol"));
                }
                Serial.println();
                IrReceiver.printIRSendUsage(&Serial);
                Serial.println();
                Serial.println(F("Raw result in internal ticks (50 us) - with leading gap"));
                IrReceiver.printIRResultRawFormatted(&Serial, false); 
                Serial.println(F("Raw result in microseconds - with leading gap"));
                IrReceiver.printIRResultRawFormatted(&Serial, true); 
                Serial.println();                                   
                Serial.print(F("Result as internal 8bit ticks (50 us) array - compensated with MARK_EXCESS_MICROS="));
                Serial.println(MARK_EXCESS_MICROS);
                IrReceiver.compensateAndPrintIRResultAsCArray(&Serial, false); 
                Serial.print(F("Result as microseconds array - compensated with MARK_EXCESS_MICROS="));
                Serial.println(MARK_EXCESS_MICROS);
                IrReceiver.compensateAndPrintIRResultAsCArray(&Serial, true); 
                IrReceiver.printIRResultAsCVariables(&Serial);               

                IrReceiver.compensateAndPrintIRResultAsPronto(&Serial); 
            }
            IrReceiver.resume();

            String decodedRawDataHex = "";
            decodedRawDataHex = String(IrReceiver.decodedIRData.decodedRawData, HEX);

            return decodedRawDataHex;
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    return "ERROR";
}

String Irctrl::configureControl()
{
    Serial.println("[TASKS] Starting air conditioner control configuration.");
    vTaskDelay(pdMS_TO_TICKS(1000));
    Serial.println("[TASKS] Point the control at the IR sensor and press the power button.");
    String result = loopIR();
    if (result.equals("ERROR"))
    {
        Serial.println("[TASKS] Error configuring the air conditioner control.");
        return "ERROR";
    }
    else
    {
        NVS.setString("CTRL_PWR", result);
        Serial.println("[TASKS] Power control successfully configured (" + String(result) + ").");
    }

    Serial.println("[TASKS] Point the control at the IR sensor and press the temperature up button.");
    result = loopIR();
    if (result.equals("ERROR"))
    {
        Serial.println("[TASKS] Error configuring the air conditioner control.");
        return "ERROR";
    }
    else
    {
        NVS.setString("CTRL_UP", result);
        Serial.println("[TASKS] Temperature up control successfully configured (" + String(result) + ").");
    }

    Serial.println("[TASKS] Point the control at the IR sensor and press the temperature down button.");
    result = loopIR();
    if (result.equals("ERROR"))
    {
        Serial.println("[TASKS] Error configuring the air conditioner control.");
        return "ERROR";
    }
    else
    {
        NVS.setString("CTRL_DOWN", result);
        Serial.println("[TASKS] Temperature down control successfully configured (" + String(result) + ").");
    }
}

void Irctrl::sendCommand(String command)
{
    Serial.println("[IR] Sending command: " + command);
    uint32_t rawData = 0x7A85FC00;
    String address = command.substring(command.length() - 4, command.length());
    uint16_t addressInt = (uint16_t)strtol(address.c_str(), NULL, 16);

    String cmd = command.substring(command.length() - 6, command.length() - 4);
    uint8_t commandInt = (uint8_t)strtol(cmd.c_str(), NULL, 16);

    Serial.println("[IR] Address: " + address);
    Serial.println("[IR] Command: " + cmd);

    IrSender.sendNEC(addressInt, commandInt, 4);
}

#endif // IRCONTROL_ENABLE
