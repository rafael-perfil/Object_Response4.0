//=====================================================================
//===================== ESP-32 : OBJECT-RESPONSE=======================
//=====================================================================
//Inclusão de bibliotecas 
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiMulti.h>
#include <MD_MAX72xx.h>
#include <time.h>
#include <driver/rtc_io.h>
#include "ArduinoJson.h"


#define  CHAR_SPACING  1 // pixels entre os caracteres 
//---------------------------------------------------------------------
//definição das variáveis de time
unsigned long startMillis;  
unsigned long currentMillis;
const unsigned long period = 1800000; // 600000 = 10min, 1800000 = 30min, 3600000 = 1h
//---------------------------------------------------------------------
//Configurações do JSON
char json[1024] = {0};
StaticJsonDocument<1024> doc;
int j = 0;
const char* serverName[]{
"https://projeto1/integracaoJson",
"https://projeto2/integracaoJson",
"https://projeto3/integracaoJson",
"https://projeto4/integracaoJson",
"https://projeto5/integracaoJson",
"https://projeto6/integracaoJson",
"https://projeto7/integracaoJson",
"https://projeto8/integracaoJson",
"https://projeto9/integracaoJson",
"http://projeto10/integracaoJson",
"https://projeto11/integracaoJson",
"https://projeto12/integracaoJson",
"https://projeto13/integracaoJson",
"http://projeto/14/integracaoJson",
}; 
//---------------------------------------------------------------------
//Constantes para NTP/Relógio base/url: https://randomnerdtutorials.com/esp32-date-time-ntp-client-server-arduino/
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -3600*3; //Variável define o deslocamento em segundos entre seu fuso horário e GMT
const int daylightOffset_sec = 0; //Variável define o deslocamento em segundos para o horário de verão, geralmente é uma hora, que corresponde a 3600 segundos.
//---------------------------------------------------------------------
//Definindo botões
#define botao1 13
#define botao2 12
#define botao3 4
int intensidadeX=0;
//----------------------------------------------------------------------
//Ativar instruções de depuração para a saída serial
#define  DEBUG  1
#if  DEBUG
#define PRINT(s, x) { Serial.print(F(s)); Serial.print(x); }
#define PRINTS(x) Serial.print(F(x))
#define PRINTD(x) Serial.println(x)
#else
#define PRINT(s, x)
#define PRINTS(x)
#define PRINTD(x)
#endif
//----------------------------------------------------------------------
// ========== DIFINIÇÕES DOS PINOS PARA A BIBLIOTECA MD_MAX72xx.h ==========
//Pino 23 = DIN, Pino 18 = CLK, Pino 5 = CS. 4 = Quantidade de displays
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 20
#define CLK_PIN   18
#define DATA_PIN  23
#define CS_PIN    5

//-----------------------------------------------------------------------
// SPI hardware interface
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
//MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

//Sempre esperamos um pouco entre as atualizações do display
#define  DELAYTIME  25  //em milissegundos

//Definir modo de transição de texto, elemento pixel para esquerda.
#define MODO_TSL MD_MAX72XX::TSL
//-----------------------------------------------------------------------
//FUNÇÕES PARA CONSULTA JSON
void consultaJson(){
  Serial.println("teste");
  currentMillis = millis(); //time atual
  Serial.println(currentMillis);
     if (j==0 ||(currentMillis - startMillis) > period)
     {    startMillis = currentMillis;
          j=1;
          Serial.println(j);
          Serial.println(startMillis);
             //3 - iniciamos a URL alvo, pega a resposta e finaliza a conexão
             for (int s=0; s < 14; s++ )
             {
             HTTPClient http;
             http.begin(serverName[s]); //url api
             int httpCode = http.GET();
             String payload = "{}"; 
                  if (httpCode > 0) { //Maior que 0, tem resposta a ser lida
                  String payload = http.getString();
                  Serial.println(httpCode);
                  Serial.println(payload);
                  resultOfGet(payload);
                  Serial.println("teste4");
                  //message_to_telegram(payload);
                  http.end();
                  Serial.println("teste5");
                  }
                  else {
                  Serial.println("Falha na requisição");
                  }    
             
             vTaskDelay(pdMS_TO_TICKS(200));
              };
      };
};
//-----------------------------------------------------------------------
//Função para tratamento e exibição do arquivo Json
void resultOfGet(String msg){

    Serial.println("teste2");
  
    memset(json,0,sizeof(json));
    msg.toCharArray(json, 1024);
    deserializeJson(doc, json);
  
    for (int i=0 ; i < 7; i++ )
    {
    Serial.println("teste3");
    JsonObject integracao = doc[{i}];
        if (integracao == 0){
          break; };
    
    const char* integracao_empresa = integracao["empresa"];
    int integracao_empresaid = integracao["empresa_id"];
    const char* integracao_tipo = integracao["integracao"];
    int integracao_produtos = integracao["desatualizados"];
      
        if (integracao_produtos > 0){
        std::string msgIntegracao = "Empresa: "+ std::string(integracao_empresa)+", Integraçao: "+ std::string(integracao_tipo)+", Produtos desatualizados: "
        +std::to_string(integracao_produtos);
        Serial.println (msgIntegracao.c_str());
        message_to_telegram(msgIntegracao.c_str());
            std::string produtostemp = "prod. destz:" + std::to_string(integracao_produtos);
            const char* produtoConcatenado = produtostemp.c_str();
        //printText(8, 19, integracao_empresa);
        scrollText(integracao_empresa);
        delay(2000);  
        printText(8, 19, produtoConcatenado);
        delay(3500);
        }
    }

} 
//-----------------------------------------------------------------------
// ========== DEFINIÇÕES ANIMAÇÃO PACMAN ==========

#define ANIMATION_DELAY 75  // milliseconds
#define MAX_FRAMES      4   // números de animações por frame

//variáveis globais para PACMAN
const uint8_t pacman[MAX_FRAMES][18] =  // ghost pursued by a pacman
{
  { 0xfe, 0x73, 0xfb, 0x7f, 0xf3, 0x7b, 0xfe, 0x00, 0x00, 0x00, 0x3c, 0x7e, 0x7e, 0xff, 0xe7, 0xc3, 0x81, 0x00 },
  { 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe, 0x00, 0x00, 0x00, 0x3c, 0x7e, 0xff, 0xff, 0xe7, 0xe7, 0x42, 0x00 },
  { 0xfe, 0x73, 0xfb, 0x7f, 0xf3, 0x7b, 0xfe, 0x00, 0x00, 0x00, 0x3c, 0x7e, 0xff, 0xff, 0xff, 0xe7, 0x66, 0x24 },
  { 0xfe, 0x7b, 0xf3, 0x7f, 0xf3, 0x7b, 0xfe, 0x00, 0x00, 0x00, 0x3c, 0x7e, 0xff, 0xff, 0xff, 0xff, 0x7e, 0x3c },
};
const uint8_t DATA_WIDTH = (sizeof(pacman[0])/sizeof(pacman[0][0]));

uint32_t prevTimeAnim = 0;  // Lembre-se do valor de millis() para animação
int16_t idx;                // display index (column)
uint8_t frame;              // frame atual da animação
uint8_t deltaFrame;         // Animação do frame para o próximo frame
// ========== FUNÇÃO MODO PACMAN ==========
void ModePacmanLoop(){
     for (int pacNum = 0; pacNum <= 25900; pacNum++){
   modePacman();
   Serial.println (pacNum);
    };
  };
void modePacman()
{ 
  static boolean bInit = true;  // inicialização da animação

  // Is it time to animate?
  if (millis()-prevTimeAnim < ANIMATION_DELAY)
    return;
  prevTimeAnim = millis();      // starting point for next time

  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);

  // inicialização
  if (bInit)
  { 
    mx.clear();
    idx = -DATA_WIDTH;
    frame = 0;
    deltaFrame = 1;
    bInit = false;

    // Lay out the dots
    for (uint8_t i=0; i<MAX_DEVICES; i++)
    { 
      
      mx.setPoint(3, (i*COL_SIZE) + 3, true);
      mx.setPoint(4, (i*COL_SIZE) + 3, true);
      mx.setPoint(3, (i*COL_SIZE) + 4, true);
      mx.setPoint(4, (i*COL_SIZE) + 4, true);
    }
  }

  // now run the animation
  PRINT("\nINV I:", idx);
  PRINT(" frame ", frame);
  
  // clear old graphic
  for (uint8_t i=0; i<DATA_WIDTH; i++){

    mx.setColumn(idx-DATA_WIDTH+i, 0);}
  // move reference column and draw new graphic
  idx++;
  for (uint8_t i=0; i<DATA_WIDTH; i++)
    mx.setColumn(idx-DATA_WIDTH+i, pacman[frame][i]);

  // advance the animation frame
  frame += deltaFrame;
  if (frame == 0 || frame == MAX_FRAMES-1)
    deltaFrame = -deltaFrame;

  // check if we are completed and set initialise for next time around
  bInit = (idx == mx.getColumnCount()+DATA_WIDTH);

  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);

  return;
};
//-----------------------------------------------------------------------
WiFiMulti wifiMulti;
//-----------------------------------------------------------------------
//Definições para comunicação com API
String apiKey = "preencher";              //Adicione o número do Token que o bot enviou para você no WhatsApp messenger
String phone_number = "preencher"; //Adicione seu número de telefone registrado no aplicativo WhatsApp (mesmo número que o bot lhe envia na url)
String telegram_grupo = "preencher"; //Adicione o token do grupo do telegram.
String url;                            //url String será usado para armazenar o URL final gerado

//-----------------------------------------------------------------------
// 3 FUNÇÕES PARA API WHATSAPP E TELEGRAM. FUNÇÃO DE CONCATENAÇÃO PARA URL DA API; FUNÇÃO PARA REQUEST

// ========== API TELEGRAM ==========
void message_to_telegram(String message)//função de definição do usuário para enviar mensagem para o aplicativo WhatsApp  
{ 
  url = "https://api.callmebot.com/telegram/group.php?apikey=" + telegram_grupo + "&text=" + urlencode(message);
  
  postData();//chamando postData para executar o URL gerado acima uma vez para que você receba uma mensagem.
  };

// ========== API WHATSPP ==========
/*void message_to_whatsapp(String message)//função de definição do usuário para enviar mensagem para o aplicativo WhatsApp  
{
  url = "https://api.callmebot.com/whatsapp.php?phone=" + phone_number + "&text=" + urlencode(message) + "&apikey=" + apiKey;
  
  postData();//chamando postData para executar o URL gerado acima uma vez para que você receba uma mensagem.
  };*/

void postData()     //userDefine function used to call api(POST data)
{
  int httpCode;     // variável usada para obter o código http de resposta após chamar a api
  HTTPClient http;  // Declare object of class HTTPClient
  http.begin(url);  // inicie o objeto HTTPClient com url gerado
  httpCode = http.POST(url); // Post URL com esta função e ela armazenará o status http
  if (httpCode == 200)      // Verifique se o código http de resposta é 200
  {
    Serial.println("Sent ok."); // imprimir mensagem enviada ok mensagem
  }
  else                      //se o código HTTP de resposta não for 200, significa que há algum erro.
  {
    Serial.println("Error."); // imprimir mensagem de erro.
  }
  http.end();          // Depois de chamar a API, encerre o objeto cliente HTTP.
}

String urlencode(String str)  // Função usada para codificar o URL
{
    String encodedString="";
    char c;
    char code0;
    char code1;
    char code2;
    for (int i =0; i < str.length(); i++){
      c=str.charAt(i);
      if (c == ' '){
        encodedString+= '+';
      } else if (isalnum(c)){
        encodedString+=c;
      } else{
        code1=(c & 0xf)+'0';
        if ((c & 0xf) >9){
            code1=(c & 0xf) - 10 + 'A';
        }
        c=(c>>4)&0xf;
        code0=c+'0';
        if (c > 9){
            code0=c - 10 + 'A';
        }
        code2='\0';
        encodedString+='%';
        encodedString+=code0;
        encodedString+=code1;
        //encodedString+=code2;
      }
      yield();
    }
    return encodedString;
}
//-----------------------------------------------------------------------
//VETOR QUE CONTÉM TODOS OS SITES/PROJETOS
const char* projeto[] { 
"http://google.com.br", 
"http://facebook.com.br"
"http://globo.com.br"
"http://instagram.com"
"http://twitter.com"
"http://wikipedia.org"
"http://mercadolivre.com.br"
"http://whatsapp.com"
"http://blogspot.com"
"http://amazon.com.br"
"http://reddit.com"
"http://yahoo.com"
"http://magazineluiza.com.br"
"http://olx.com.br"
"http://americanas.com.br"
"http://betano.com"
"http://shopee.com.br"
"http://aliexpress.com"
"http://tiktok.com"
"http://bing.com"
"http://microsoft.com"
"http://github.com"
};
//-----------------------------------------------------------------------
//FUNÇÃO PARA REQUISIÇÃO NTP
//-----------------------------------------------------------------------
void requestNTP(){
configTime(gmtOffset_sec, daylightOffset_sec, ntpServer); //Inicia e obtém as horas
  struct tm timeinfo; //uma estrutura de tempo (struct tm) chamado 'timeinfo' que contém todos os detalhes sobre a hora (min, seg, hora, etc…).
             /*O'tm' estrutura contém uma data e hora do calendário dividida em seus componentes
             tm_sec: segundos após o minuto;
             tm_min: minutos após a hora;
             tm_hour: horas desde a meia-noite;
             tm_mday: dia do mês;
             tm_mon : mês;
             tm_year: anos desde 1900;
             tm_wday: dias desde domingo;
             tm_yday: dias desde 1º de janeiro;
             tm_isdst: Sinalizador de horário de verão;*/
  if(!getLocalTime(&timeinfo)){
      Serial.println("Falha ao obter tempo.");    
      return;
    }
  /*  
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.print("Day of week: ");
  Serial.println(&timeinfo, "%A");
  Serial.print("Month: ");
  Serial.println(&timeinfo, "%B");
  Serial.print("Day of Month: ");
  Serial.println(&timeinfo, "%d");
  Serial.print("Year: ");
  Serial.println(&timeinfo, "%Y");
  Serial.print("Hour: ");
  Serial.println(&timeinfo, "%H");
  Serial.print("Hour (12 hour format): ");
  Serial.println(&timeinfo, "%I");
  Serial.print("Minute: ");
  Serial.println(&timeinfo, "%M");
  Serial.print("Second: ");
  Serial.println(&timeinfo, "%S");
  */
  //----data completa----
  char day[3];
  char month[3];
  char year[3];
  char timeWeekDay[10];
  strftime(day,3, "%d", &timeinfo);
  strftime(month,3, "%m", &timeinfo);
  strftime(year,3, "%y", &timeinfo);
  strftime(timeWeekDay,10, "%A", &timeinfo);
  const char* dia = day;
  const char* mes = month;
  const char* ano = year;
  const char* diaSemana = timeWeekDay;
  std::string datatemp = std::string(diaSemana)+"    "+ std::string(dia) + "." + std::string(mes) + "." + std::string(ano);
  const char* dataConcatenada = datatemp.c_str();
  printText(8, 19, dataConcatenada);
  //----horas---- 
  char timeHour[3];
  char timeMin[3];
  Serial.println("Váriaveis de tempo");
  strftime(timeHour,3, "%H", &timeinfo);
  //Serial.println(timeHour);
  strftime(timeMin,3, "%M", &timeinfo);
  //Serial.println(timeMin);
  const char* hrs = timeHour;
  const char* minute = timeMin;
  std::string horastemp = std::string(hrs) + " : " + std::string(minute);
  const char* horasConcatenada = horastemp.c_str();
  printText(4, 7, horasConcatenada);
  
}
//----------------------------------------------------------------------
//FUNÇÃO DE TEXTO STATIC PARA DISPLAY 2
//----------------------------------------------------------------------
void printText(uint8_t modStart, uint8_t modEnd, const char *pMsg)
// Print the text string to the LED matrix modules specified.
// Message area is padded with blank columns after printing.
{
  uint8_t   state = 0;
  uint8_t    curLen;
  uint16_t  showLen;
  uint8_t   cBuf[8];
  int16_t   col = ((modEnd + 1) * COL_SIZE) - 1;

  mx.control(modStart, modEnd, MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);

  do     // finite state machine to print the characters in the space available
  {
    switch(state)
    {
      case 0: // Load the next character from the font table
        // if we reached end of message, reset the message pointer
        if (*pMsg == '\0')
        {
          showLen = col - (modEnd * COL_SIZE);  // padding characters
          state = 2;
          break;
        }

        // retrieve the next character form the font file
        showLen = mx.getChar(*pMsg++, sizeof(cBuf)/sizeof(cBuf[0]), cBuf);
        curLen = 0;
        state++;
        // !! deliberately fall through to next state to start displaying

      case 1: // display the next part of the character
        mx.setColumn(col--, cBuf[curLen++]);

        // done with font character, now display the space between chars
        if (curLen == showLen)
        {
          showLen = CHAR_SPACING;
          state = 2;
        }
        break;

      case 2: // initialize state for displaying empty columns
        curLen = 0;
        state++;
        // fall through

      case 3: // display inter-character spacing or end of message padding (blank columns)
        mx.setColumn(col--, 0);
        curLen++;
        if (curLen == showLen)
          state = 0;
        break;

      default:
        col = -1;   // this definitely ends the do loop
    }
  } while (col >= (modStart * COL_SIZE));

  mx.control(modStart, modEnd, MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
}
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
//FUNÇÕES PARA LIMPAR MODULO LED(DISPLAY) INDIVIDUAL.
//-----------------------------------------------------------------------

void clearDisplay1(){
  uint8_t dev0 = 0;
  uint8_t dev3 = 3;
  mx.clear(dev0,dev3);}
void clearDisplay2(){
  uint8_t dev4 = 4;
  uint8_t dev7 = 7;
  mx.clear(dev4,dev7);}
void clearDisplay3e4e5(){
  uint8_t dev8 = 8;
  uint8_t dev19 = 19;
  mx.clear(dev8,dev19);}

//-----------------------------------------------------------------------
//FUNÇÃO PARA CONEXÃO COM WIFI
//-----------------------------------------------------------------------

void iniciaWifi()
{   
    funcaobotao();
    wifiMulti.addAP("preencher", "preencher"); //Usuario e senha do wifi 1
    wifiMulti.addAP("preencher", "preencher"); //Usuario e senha do wifi 2   
    wifiMulti.addAP("preencher", "preencher"); /Usuario e senha do wifi 3
  
     if(WiFi.status() != WL_CONNECTED) {
        PRINTD("Connecting Wifi...");
        printText(8, 19, "  Conectando WiFi...");
        //scrollText("Conectando Wifi..."); 
        wifiMulti.run();
        PRINTS(".");
        delay(2000);
        clearDisplay3e4e5();
            voltaWifi:
            if(WiFi.status() != WL_CONNECTED){
              printText(8, 19, "WiFi nao conectado.....");
              //printText(0, 7, "......................");
              //scrollText("WIFI NÃO CONECTADO.");
              delay(2000);
              funcaobotao();
              wifiMulti.run();
              funcaobotao();
              clearDisplay3e4e5();
              clearDisplay1();
              ModePacmanLoop();
              goto voltaWifi;
              }
    }
    if (WiFi.status() == WL_CONNECTED){
    printText(8, 19, "   WiFi Conectado!");
    //scrollText("Wifi conectado!");
    delay(2000);
    PRINTD("");     
    PRINTD("WiFi connected");
    PRINTD("IP address: ");
    PRINTD(WiFi.localIP());
    char ipTexto[20];
    sprintf(ipTexto, "IP: %03d:%03d:%03d:%03d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);
    scrollText(ipTexto);
    delay(1000);
    clearDisplay3e4e5();
    }
}
//-----------------------------------------------------------------------
//FUNÇÃO PRINCIPAL DE LOOP
//-----------------------------------------------------------------------

void loopProjeto(){

  voltaLoop:
  int i = 0; //22 - Quantidade de sites no vetor = 

  const uint16_t maxCol = 4 * ROW_SIZE;

  if ((wifiMulti.run() == WL_CONNECTED)) { //Check the current connection status
    HTTPClient http;
    consultaJson();
  for (uint16_t col=maxCol -1; col >= 0; col--)
  {
    for (uint8_t row=0; row < ROW_SIZE; row++)
    {     
      if(WiFi.status() != WL_CONNECTED){
         iniciaWifi();
         };
           
           requestNTP();
           funcaobotao();
           mx.setPoint(row,col,false);
           i++;
           int contadorRequest = 0;
           voltaRequest:
           contadorRequest++;
           http.begin(projeto[i]); //Especifique a URL
           int httpCode = http.GET(); //Fazendo a resquest
              if (httpCode == 200 || 302 == httpCode || -1 == httpCode) { //Checando o retorno do status     
                  PRINTS(projeto[i]);
                  PRINTS(": Status ");
                  PRINTD(httpCode);                    
                  mx.setPoint(row,col,true);
                  }
  
              else {
                  if(contadorRequest < 3 ){
                  delay(200);
                  goto voltaRequest;};
                  mx.setPoint(row,col,true);                              
                  PRINTS(projeto[i]);
                  PRINTS(": Status ");
                  PRINTD(httpCode);
                  std::string tmp = std::to_string(httpCode);
                  const char* httpStatus = tmp.c_str();
                  const char* statusTexto = "Status: ";
                  std::string result = std::string(statusTexto) + std::string(httpStatus);
                  const char* statusConcatenado = result.c_str();
                  std::string contador = std::to_string(contadorRequest);              
                  PRINTD("Error on HTTP request");
                  delay(35);                  
                  mx.setPoint(row,col,false); 
                  std::string appMsg = "Site: " + std::string(projeto[i]) + "\n" + std::string(statusConcatenado) + "\n" + "Qtd. request: " + std::string(contador);
                  message_to_telegram(appMsg.c_str());
                  //message_to_whatsapp(appMsg.c_str());
                  scrollText(projeto[i]);
                  delay(1000);
                  printText(8, 19,statusConcatenado);
                  //scrollText(statusConcatenado);
                  delay(3000);
                  clearDisplay3e4e5();
                  //scrollText(httpStatus);            
              }
              if(i > 22) //Quantidades de loops
              { 
                PRINTD("fim do loop");
                goto voltaLoop;
                http.end(); //Liberando recursos do http 
              }
    delay(DELAYTIME);
    }
   }
  }
}
//-----------------------------------------------------------------------
//Função em desenvolvimento
//-----------------------------------------------------------------------
/*void loopSemDisplay(){
  int i = 0; //208
    if ((wifiMulti.run() == WL_CONNECTED)) {
      HTTPClient http
      for (i = 208, i >= 0, i++){
        http.begin(projeto[i]);
        int httpCode = http.GET();
        
          if(httpCode == 200 || 302 == httpCode || -1 == httpCode){
              PRINTS(projeto[i]);
              PRINTS(": Status ");
              PRINTD(httpCode);
              }
           else{
              PRINTS(projeto[i]);
              PRINTS(": Status ");
              PRINTD(httpCode);
                   if(WiFi.status() != WL_CONNECTED){
                      iniciaWifi();
                   };
           };
        
      };        
    };
};*/
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
//FUNÇÃO TEXTO SCROLL DISPLAY 3, 4 e 5
//-----------------------------------------------------------------------
void scrollText(const char *p)
{ 
  int j = 0;
  uint8_t charWidth;
  uint8_t cBuf[8];  // isso deve ser bom para todas as fontes internas
  uint8_t startDev = 8;
  uint8_t endDev = 19;
  PRINTS("\nScrolling text");
  clearDisplay3e4e5();
  
  while (*p != '\0')
  { 
    funcaobotao();
    j++;
    charWidth = mx.getChar(*p++, sizeof(cBuf) / sizeof(cBuf[0]), cBuf);
    
    for (uint8_t i=0; i<=charWidth; i++)  // permitir espaço entre os caracteres
    {
      mx.transform(startDev, endDev, MODO_TSL);
      if (i < charWidth)
        mx.setColumn(64, cBuf[i]);
      delay(DELAYTIME);
    }
   if (j >= 28){
      delay(4000);
      j=0;
  }
}
}

//-----------------------------------------------------------------------
//FUNÇÃO PARA BOTÕES FISICOS 
//-----------------------------------------------------------------------

void funcaobotao(){
    if (digitalRead(botao1)==0){
      Serial.println(intensidadeX);
      mx.control(MD_MAX72XX::INTENSITY,intensidadeX );
      intensidadeX++;  
      delay(50);
        if (intensidadeX >= 15) intensidadeX = 0;
    }

    if (digitalRead(botao2)==0){
      clearDisplay1();
      clearDisplay2();
      printText(8, 19, "      MODO SLEEP");
      delay(5000);
      clearDisplay3e4e5();
      esp_deep_sleep_start();
      }
  /*if em desenvolvimento
    if (digitalRead(botao3)==0){

      scrollText("MODO DISPLAY OFF/ON");
      delay(4000);
      mx.control(8,MD_MAX72XX::SHUTDOWN,true);

      };*/
}

//-----------------------------------------------------------------------
//FUNÇÃO SETUP PRINCIPAL
//-----------------------------------------------------------------------
void setup() 
{ 
  startMillis = millis(); //inicia o time
  const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_ARRAY_SIZE(8) + 1024;
  DynamicJsonDocument doc(capacity);
  pinMode(botao1, INPUT_PULLUP);
  pinMode(botao2, INPUT_PULLUP);
  pinMode(botao3, INPUT_PULLUP);
  mx.begin();
  mx.control(MD_MAX72XX::INTENSITY,intensidadeX);  
#if  DEBUG
  Serial.begin(115200);
#endif
   printText(8, 19, "  OBJECT RESPONSE");
   //scrollText("OBJECT RESPONSE");
   delay(2000);
   clearDisplay3e4e5();

   iniciaWifi(); 
}
//-----------------------------------------------------------------------
void loop() 
{ 
  requestNTP();
  loopProjeto(); 
}
//-----------------------------------------------------------------------
