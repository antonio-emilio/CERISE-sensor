
#include "connectionEth.h"
#ifdef ETH_ENABLE
static Utils utils;
char server[] = "pkg-receiver.floats.com.br";
char path[] = "/api/newData";
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
EthernetClient client;

void WiFiEvent(WiFiEvent_t event)
{
}

uint8_t ConnectionEth::setupConn(void)
{
  pinMode(15, OUTPUT);
  digitalWrite(15, HIGH);
  vTaskDelay(pdMS_TO_TICKS(1000));
  digitalWrite(15, LOW);
  vTaskDelay(pdMS_TO_TICKS(1000));
  digitalWrite(15, HIGH);
  pinMode(5, INPUT);
  pinMode(12, INPUT_PULLUP);

  Serial.println("Initializing Ethernet...");
  Ethernet.init(CS_GPIO);
  Serial.println("Ethernet Initialized");
  if (Ethernet.begin(mac))
  {
    Serial.println("DHCP OK!");
  }
  else
  {
    Serial.println("Failed to configure Ethernet using DHCP");
    if (Ethernet.hardwareStatus() == EthernetNoHardware)
    {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true)
      {
        vTaskDelay(pdMS_TO_TICKS(1));
      }
    }
    if (Ethernet.linkStatus() == LinkOFF)
    {
      Serial.println("Ethernet cable is not connected.");
    }

    IPAddress ip(MYIPADDR);
    IPAddress dns(MYDNS);
    IPAddress gw(MYGW);
    IPAddress sn(MYIPMASK);
    Ethernet.begin(mac, ip, dns, gw, sn);
    Serial.println("STATIC OK!");
  }
  vTaskDelay(pdMS_TO_TICKS(5000));

  Serial.print("Local IP : ");
  Serial.println(Ethernet.localIP());
  Serial.print("Subnet Mask : ");
  Serial.println(Ethernet.subnetMask());
  Serial.print("Gateway IP : ");
  Serial.println(Ethernet.gatewayIP());
  Serial.print("DNS Server : ");
  Serial.println(Ethernet.dnsServerIP());

  Serial.println("Ethernet Successfully Initialized");
  beginMicros = micros();
}

uint8_t ConnectionEth::sendData(String data, String url)
{
  if (client.connect(server, 80))
  {
    Serial.println("connected to server: " + String(server));
    client.print("POST ");
    client.print(path);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(strlen(data.c_str()));
    client.println("Connection: close");
    client.println();
    client.println(data);
  }
  else
  {
    Serial.println("connection failed");
  }

  while (client.connected())
  {
    if (client.available())
    {
      char c = client.read();
      Serial.write(c);
    }
  }

  client.stop();
}
#endif