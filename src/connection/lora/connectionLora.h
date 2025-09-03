
#ifndef _CONNECTION_LORA_H
#define _CONNECTION_LORA_H

#include "../connection.h"
#ifdef LORA_ENABLE
#include <Ra01S.h>

#define RF_FREQUENCY 915000000  // Hz  center frequency
#define TX_OUTPUT_POWER 22      // dBm tx output power
#define LORA_BANDWIDTH 4        // bandwidth
                                // 2: 31.25Khz
                                // 3: 62.5Khz
                                // 4: 125Khz
                                // 5: 250KHZ
                                // 6: 500Khz
#define LORA_SPREADING_FACTOR 7 // spreading factor [SF5..SF12]
#define LORA_CODINGRATE 1       // [1: 4/5,
                                //  2: 4/6,
                                //  3: 4/7,
                                //  4: 4/8]

#define LORA_PREAMBLE_LENGTH 8 // Same for Tx and Rx
#define LORA_PAYLOADLENGTH 0   // 0: Variable length packet (explicit header)
                               // 1..255  Fixed length packet (implicit header)

class ConnectionLora : public Connection
{
public:
  uint8_t setupConn(void) override;
  uint8_t sendData(String data, String url) override;
  uint8_t receiveData();

private:
  uint16_t minimo(uint16_t a, uint16_t b);
};

#endif
#endif