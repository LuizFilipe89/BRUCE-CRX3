# Pendencias do DevKit Custom

Correcoes identificadas durante a revisao e deixadas para uma proxima solicitacao.

## GPIOs do ESP32 classico

- Substituir `TFT_BL=46`: GPIO 46 nao existe no ESP32 DevKit V1. Como o backlight esta ligado a 3V3, o codigo deve aceitar ausencia de pino sem chamar `pinMode()` ou `digitalWrite()`.
- Revisar `TX=43` e `RX=44` em `boards/devkit-custom/pins_arduino.h`; esses GPIOs tambem nao pertencem ao ESP32 classico.

## Configuracao persistida

- Criar migracao/versionamento para configuracoes do LittleFS, pois valores antigos podem sobrescrever defaults novos de rotacao, pinos e RF.
- Documentar quando e necessario apagar completamente a flash.

## RF e CC1101

- Restaurar um limite configuravel para o jammer; atualmente ele nao possui corte automatico pratico.
- Validar em hardware o GDO0 compartilhado por TX e RX em scan, replay, RAW, spectrum e jammer.
- Avaliar o consumo de stack/memoria do buffer RMT ampliado de 64 para 256 simbolos.
- Confirmar que GPIO15 nao causa interferencia no boot e fica em estado seguro durante reset.

## Recursos da placa

- Ocultar menus de perifericos inexistentes no DevKit Custom.
- Garantir que `-1` seja tratado corretamente para GPS, BadUSB, NRF24, SD e backlight.
- Criar uma tabela unica e revisada das ligacoes do display, botoes e CC1101.

## Manutencao

- Adicionar compilacao de `DevKit-Custom` ao CI.
- Separar alteracoes da placa das alteracoes gerais do Bruce para facilitar merges do upstream.
- Normalizar textos do repositorio para UTF-8 e corrigir mojibake restante.
