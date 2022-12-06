# Object Response 4.0

# A idaia -
Depois de contato com pequenos projetos de IoT, surgiu a ideia de projetar um dispositivo display para monitoramento de Sites e leitura de arquivos .JSON por API, pensei que seria interresante um dispositivo que enviasse requisições de status para determinados sites afim de verificar a sua disponibilidade. 

# Funcionamento 

Após ligar, o ESP32 realiza a busca pelas redes wi-fi configuradas, se conectando á primeira encontrada, e consulta um servidor de horário via NTP

#A Contrução
Construido com um microcontrolador ESP32, 20 matrizes LED 8x8, botões de controle e um powerbank.
O Display é separado em 3 zonas, a primeira zona é constituida com 12 matrizes para exibição de data e textos, segunda zona formada por 4 matrizes para hora, tericeira zona formada por 4 matrizes, led individual correspondente a resposta da requisição do status do site, On/Off, ligado ou desligado.

