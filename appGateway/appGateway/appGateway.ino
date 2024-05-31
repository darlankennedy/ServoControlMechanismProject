
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
#define transmissionBand 915E6  //define a frequencia media de transmissão: 868E6, 915E6
#define KEY 0xF3                // Chave para receber pacote

#define ssid "Darlan"  //difine a rede do wifi
#define password "Dko115609@"  //define a senha de acesso

#define RelayPin 12  //// define pino 12 para uso do botão
//iniciar servidor na porta 98
AsyncWebServer server(80);


String packSize = "--";
String packet ;
float currentTemp;

/* Variaveis globais */
long informacao_a_ser_enviada = 0;
int ledPin = 13;  // LED conectado ao pino digital 13
int inPin = 7;    // botão conectado ao pino digital 7
int val = 0;      // variável para guardar o valor lido

// HTML web page
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>ESP32 DHT Server</h2>
  <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="dht-labels">Temperature</span> 
    <span id="temperature">%TEMPERATURE%</span>
    <sup class="units">&deg;C</sup>
  </p>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 10000 ) ;

</script>
</html>)rawliteral";


// Replaces placeholder with button section in your web page
String processor(const String& var){
  //Serial.println(var);
  if(var == "TEMPERATURE"){
    return packet;
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

void cbk(int packetSize);

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void sendPacket(int currentTemp);
void onReceive(int packetSize);

void setupWifi() {
  delay(100);
  WiFi.disconnect();
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
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", packet.c_str());
  });
  server.onNotFound(notFound);
  server.begin();
}

void setup() {
  Heltec.begin(displayState, loRaState, serialState, powerAmplifier, transmissionBand);
  Serial.begin(115200);
  setupWifi();
  
  LoRa.receive();
  Serial.print(packet);  //Imprime no monitor serial a temperatura
  delay(1000);
  handlePageclient();

}


void loop() {

  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    cbk(packetSize);
    digitalWrite(LED, HIGH);  // Liga o LED
    delay(500);               // Espera 500 milissegundos
    digitalWrite(LED, LOW);   // Desliiga o LED
    delay(500);               // Espera 500 milissegundos
    Serial.print("Recebendo a temperatura: ");
    Serial.print(packet);  //Imprime no monitor serial a temperatura
    Serial.println("°C");
  }
  delay(10);
}

void sendPacket(int currentTemp) {
  LoRa.beginPacket();
  LoRa.print(currentTemp);
  LoRa.endPacket();
}
void cbk(int packetSize) {
  packet = "";
  packSize = String(packetSize);
   for (int i = 0; i < packetSize; i++) {
    packet += (char) LoRa.read(); //Atribui um caractere por vez a váriavel packet 
  }
}