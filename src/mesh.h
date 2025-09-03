#ifndef _MESH_H
#define _MESH_H

#include "utils.h"
#include "globaldef.h"
#include <esp_now.h>

typedef struct data_struct_espnow
{
   uint8_t message[225];
};

void OnRecv(const uint8_t *mac, const uint8_t *incomingData, int len);

void configureESPNOWrecv();

void OnSent(const uint8_t *mac_addr, esp_now_send_status_t status);

void configureESPNOWsend();

uint8_t sendData(String conteudo);

boolean espnow_habilitado();

boolean espnow_root();

boolean espnow_node();

#endif