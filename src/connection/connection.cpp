#include "connection.h"

static Utils utils;

uint8_t Connection::setupConn(void)
{
  utils.sendMessage("[CONNECTION] Connection type not defined.", SERIAL_DEBUG, SEM_TOPICO);
  return ERRO;
}

uint8_t Connection::verifyConn(void)
{
  utils.sendMessage("[CONNECTION] Connection type not defined.", SERIAL_DEBUG, SEM_TOPICO);
  return ERRO;
}

uint8_t Connection::startConn(void)
{
  utils.sendMessage("[CONNECTION] Connection type not defined.", SERIAL_DEBUG, SEM_TOPICO);
  return ERRO;
}

uint8_t Connection::stopConn(void)
{
  utils.sendMessage("[CONNECTION] Connection type not defined.", SERIAL_DEBUG, SEM_TOPICO);
  return ERRO;
}

uint8_t Connection::restartConn(void)
{
  utils.sendMessage("[CONNECTION] Connection type not defined.", SERIAL_DEBUG, SEM_TOPICO);
  return ERRO;
}

uint8_t Connection::getConnType(void)
{
  utils.sendMessage("[CONNECTION] Connection type not defined.", SERIAL_DEBUG, SEM_TOPICO);
  return ERRO;
}

uint8_t Connection::getConnStatus(void)
{
  utils.sendMessage("[CONNECTION] Connection type not defined.", SERIAL_DEBUG, SEM_TOPICO);
  return ERRO;
}

uint8_t Connection::getConnStrength(void)
{
  utils.sendMessage("[CONNECTION] Connection type not defined.", SERIAL_DEBUG, SEM_TOPICO);
  return ERRO;
}

String Connection::getIP(void)
{
  utils.sendMessage("[CONNECTION] Connection type not defined.", SERIAL_DEBUG, SEM_TOPICO);
  return "";
}

String Connection::getSSID(void)
{
  utils.sendMessage("[CONNECTION] Connection type not defined.", SERIAL_DEBUG, SEM_TOPICO);
  return "";
}

String Connection::getPass(void)
{
  utils.sendMessage("[CONNECTION] Connection type not defined.", SERIAL_DEBUG, SEM_TOPICO);
  return "";
}

uint8_t Connection::sendData(String data, String url)
{
  utils.sendMessage("[CONNECTION] Connection type not defined.", SERIAL_DEBUG, SEM_TOPICO);
  return ERRO;
}

uint8_t Connection::resetCredentials(void)
{
  utils.sendMessage("[CONNECTION] Connection type not defined.", SERIAL_DEBUG, SEM_TOPICO);
  return ERRO;
}
