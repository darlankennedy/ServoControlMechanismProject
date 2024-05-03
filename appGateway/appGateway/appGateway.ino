
#include <heltec.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <AsyncTCP.h>
#include <SPI.h>
#include <ESPAsyncWebServer.h>

///definições do radio lora
#define displayState false  //define se display estará ou nao ativo
#define loRaState true      //define se o rádio estará ou nao ativo
#define serialState true    //define se o rádio estará ou nao ativo

#define powerAmplifier true     //define se o amplificador de potencia PABBOOST estará ou nao ativo
#define transmissionBand 433E6  //define a frequencia media de transmissão: 868E6, 915E6
#define KEY 0xF3                // Chave para receber pacote

#define ssid "Darlan"          //difine a rede do wifi
#define password "Dko115609@"  //define a senha de acesso

#define RelayPin 12 //// define pino 12 para uso do botão
//iniciar servidor na porta 98
AsyncWebServer server(80);



unsigned int counter = 0;  // Contador de pacotes
float temp = 0;            // Temperatura
float umid = 0;            // Umidade

/* Variaveis globais */
long informacao_a_ser_enviada = 0;
int ledPin = 13;  // LED conectado ao pino digital 13
int inPin = 7;    // botão conectado ao pino digital 7
int val = 0;      // variável para guardar o valor lido

// HTML web page
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
  <head>
    <title>ESP Pushbutton Web Server</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      body { font-family: Arial; text-align: center; margin:0px auto; padding-top: 30px;}
      .button {
        padding: 10px 20px;
        font-size: 24px;
        text-align: center;
        outline: none;
        color: #fff;
        background-color: #2f4468;
        border: none;
        border-radius: 5px;
        box-shadow: 0 6px #999;
        cursor: pointer;
        -webkit-touch-callout: none;
        -webkit-user-select: none;
        -khtml-user-select: none;
        -moz-user-select: none;
        -ms-user-select: none;
        user-select: none;
        -webkit-tap-highlight-color: rgba(0,0,0,0);
      }  
      .button:hover {background-color: #1f2e45}
      .button:active {
        background-color: #1f2e45;
        box-shadow: 0 4px #666;
        transform: translateY(2px);
      }
    </style>
  </head>
  <body>
    <h1>ESP Pushbutton Web Server</h1>
    <button class="button" onmousedown="toggleCheckbox('on');" ontouchstart="toggleCheckbox('on');" onmouseup="toggleCheckbox('off');" ontouchend="toggleCheckbox('off');">LED PUSHBUTTON</button>
   <script>
   function toggleCheckbox(x) {
     var xhr = new XMLHttpRequest();
     xhr.open("GET", "/" + x, true);
     xhr.send();
   }
  </script>
  </body>
</html>)rawliteral";

// Replaces placeholder with button section in your web page
String processor(const String &var) {
  //Serial.println(var);
  if (var == "BUTTONPLACEHOLDER") {
    String buttons = "";
    buttons += "<h4>Output - GPIO 2</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"2\" " + outputState(2) + "><span class=\"slider\"></span></label>";
    return buttons;
  }
  return String();
}

String outputState(int output) {
  if (digitalRead(output)) {
    return "checked";
  } else {
    return "";
  }
}

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void sendPacket(int currentTemp);
void onReceive(int packetSize);

void setupWifi() {
  WiFi.disconnect(true);

  delay(100);

  // IPAddress ip(192, 168, 31, 115);
  // IPAddress gateway(192, 168, 31, 1);
  // IPAddress subnet(255, 255, 0, 0);
  // IPAddress primaryDns(8,8,8,8);
  // IPAddress secondaryDns(8,8,4,4);

  WiFi.mode(WIFI_STA);
  WiFi.setAutoConnect(true);

  // if(!WiFi.config(ip, gateway, subnet,primaryDns,secondaryDns)){
  //   Serial.println("STA Failed to configure");
  // }

  WiFi.begin(ssid, password);

  delay(100);

  byte count = 0;

  while (WiFi.status() != WL_CONNECTED && count < 10) {
    count++;
    delay(500);
    Serial.println("Connectando Wifi...");
  }

  if (WiFi.status() == WL_CONNECTED) {

    Serial.println("Conectado...");
    Serial.println("Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("Esp Mac Address");
    Serial.println(WiFi.subnetMask());
    Serial.println("Gateway Ip:");
    Serial.println(WiFi.gatewayIP());
    Serial.println(WiFi.dnsIP());

  } else {
    Serial.println("Falha...");
  }
}

void handlePageclient() {


  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
  });

  // Receive an HTTP GET request
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println(1);
    sendPacket(1);
    request->send(200, "text/plain", "ok");
  });

  // Receive an HTTP GET request
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println(0);
    sendPacket(0);
    request->send(200, "text/plain", "ok");
  });
  server.onNotFound(notFound);
  server.begin();
}

void setup() {
  pinMode(LED, OUTPUT);  //inicializa o LED
  Heltec.begin(displayState, loRaState, serialState, powerAmplifier, transmissionBand);
  Serial.begin(115200);
  pinMode(RelayPin, OUTPUT);
  LoRa.setSyncWord(KEY);
  setupWifi();
  delay(1000);
}


void loop() {
  handlePageclient();
  int botaoPressionado = digitalRead(RelayPin);  // Lê o estado do botão
  sendPacket(botaoPressionado);
  counter++;
  digitalWrite(LED, HIGH);
  delay(400);
  digitalWrite(LED, LOW);
  delay(400);
}

void sendPacket(int currentTemp) {
  LoRa.beginPacket();
  LoRa.print(currentTemp);
  LoRa.endPacket();
}