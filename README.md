
# SensorHub Firmware

Firmware embarcado para dispositivos de monitoramento ambiental e automação, desenvolvido para rodar em microcontroladores ESP32. Este projeto permite a análise da qualidade do ar, controle de atuadores e comunicação em rede, sendo altamente modular e adaptável para diferentes aplicações (Sensor, Actuator).

## Características Principais

- **Monitoramento da Qualidade do Ar:**
  - Leitura de sensores ambientais como temperatura, umidade, pressão, CO2, TVOC, NOx, luminosidade, ruído, poeira, entre outros.
  - Suporte a múltiplos sensores (BME680, SGP30, SGP41, SCD41, AGS10, BH1750, GP2Y1010, etc).
- **Automação e Controle:**
  - Controle de atuadores (relés, lâmpadas, tomadas, etc) via comandos MQTT.
  - Suporte a diferentes modos de operação: Sensor (monitoramento) e Actuator (atuadores).
- **Comunicação e Conectividade:**
  - Comunicação via Wi-Fi, MQTT, e suporte a outros protocolos (LoRa, Ethernet, GPRS, ESP-NOW).
  - Envio de dados para servidores remotos e integração com plataformas IoT.
- **Atualização Remota (FOTA):**
  - Suporte a atualização remota de firmware via HTTPS.
- **Gerenciamento de Energia:**
  - Modos de baixo consumo para aplicações alimentadas por bateria.
- **Arquitetura Modular:**
  - Implementação baseada em máquina de estados, facilitando a manutenção e expansão.
  - Código organizado em módulos para sensores, comunicação, tarefas e utilidades.

## Funcionalidades

- Inicialização automática dos sensores e configuração dos pinos.
- Leitura periódica dos dados ambientais e envio para o servidor.
- Recebimento e execução de comandos remotos via MQTT.
- Detecção de falhas em sensores e tentativas automáticas de reconexão.
- Suporte a diferentes perfis de dispositivo via diretivas de compilação (`Sensor`, `ACTUATOR`).
- Interface para atualização de firmware Over-The-Air (OTA/FOTA).

## Estrutura do Projeto

- `src/` - Código-fonte principal (máquinas de estado, sensores, comunicação, utilitários)
- `lib/` - Bibliotecas de terceiros utilizadas
- `include/` - Arquivos de inclusão e definições globais
- `test/` - Testes e exemplos

## Como Rodar o Projeto Localmente

1. **Pré-requisitos:**
   - Visual Studio Code instalado
   - Extensão PlatformIO instalada no VSCode

2. **Compilação:**
   - Abra o projeto no VSCode
   - No menu lateral do PlatformIO, selecione a opção `Build` para compilar o firmware
   - A extensão irá baixar todas as dependências e compilar o código automaticamente

3. **Upload para o Dispositivo:**
   - Conecte o ESP32 ao computador via USB
   - Selecione a opção `Upload and Monitor` para gravar o firmware e abrir o monitor serial

Pronto! O dispositivo estará funcionando conforme o perfil selecionado no código.

## Perfis de Dispositivo

O firmware pode ser compilado para diferentes perfis, alterando as diretivas no arquivo `globaldef.h`:

- `SENSOR` - Monitoramento ambiental
- `ACTUATOR` - Controle de atuadores

Basta descomentar a linha correspondente ao perfil desejado e compilar o projeto.
