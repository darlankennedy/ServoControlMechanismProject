#include <heltec.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <AsyncTCP.h>
#include <SPI.h>
#include "HT_SSD1306Wire.h"
#include <ESPAsyncWebServer.h>

/// Definições do rádio LoRa
#define displayState false  // Define se o display estará ou não ativo
#define loRaState true      // Define se o rádio estará ou não ativo
#define serialState true    // Define se o rádio estará ou não ativo

#define powerAmplifier true     // Define se o amplificador de potência PABBOOST estará ou não ativo
#define transmissionBand 915E6  // Define a frequência média de transmissão: 868E6, 915E6

#define KEY 0xF3  // Chave para receber pacote

#define ssid "motog2"          // Define a rede do WiFi
#define password "115609123"  // Define a senha de acesso

#define RelayPin 12  // Define pino 12 para uso do botão

// Iniciar servidor na porta 80
AsyncWebServer server(80);
static SSD1306Wire display(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED);  // addr, freq, i2c group, resolution, rst

String packSize = "--";
String packet;
float currentTemp;

/* Variáveis globais */
long informacao_a_ser_enviada = 0;
int ledPin = 13;  // LED conectado ao pino digital 13
int inPin = 7;    // Botão conectado ao pino digital 7
int val = 0;      // Variável para guardar o valor lido

bool relayState = false;

unsigned long previousMillisDisplay = 0;
const long intervalDisplay = 1000;

unsigned long previousMillisGetTemp = 0;
const long intervalDisplayGetTemp = 1000;



const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
      font-family: Arial;
      display: inline-block;
      text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    body { max-width: 600px; margin: 0px auto; padding-bottom: 25px; }
    .switch { position: relative; display: inline-block; width: 120px; height: 68px; }
    .switch input { display: none; }
    .slider { position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 34px; }
    .slider:before { position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; transition: .4s; border-radius: 68px; }
    input:checked+.slider { background-color: #2196F3; }
    input:checked+.slider:before { transform: translateX(52px); }
    .relay-label { font-size: 1.5rem; margin-left: 10px; vertical-align: middle; }
    .relay-container { display: flex; align-items: center; justify-content: center; }
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
  <p class="relay-container">
    <label class="switch">
      <input type="checkbox" onchange="toggleRelay(this)">
      <span class="slider"></span>
    </label>
    <span class="relay-label">RELE</span>
  </p>
  <script>
    function toggleRelay(element) {
      var state = element.checked ? "on" : "off";
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", "/relay?state=" + state, true);
      xhttp.send();
    }

    setInterval(function () {
      var xhttp = new XMLHttpRequest();
        if (this.readyState == 4 && this.status == 200) {
      xhttp.onreadystatechange = function() {
          document.getElementById("temperature").innerHTML = this.responseText;
        }
      };
      xhttp.open("GET", "/temperature", true);
      xhttp.send();
    }, 10000);
  </script>
</body>
</html>)rawliteral";


// Substitui placeholder com a seção de botão na sua página web
String processor(const String &var) {
  if (var == "TEMPERATURE") {
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
void sendControlMessage(bool state);

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void sendPacket(int currentTemp);
void onReceive(int packetSize);
void startOLED();
void handleRelayRequest(AsyncWebServerRequest *request);
void sendPacketRele(const char *message);

void setupWifi() {
  delay(100);
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.setAutoConnect(true);

  WiFi.begin(ssid, password);

  delay(100);

  byte count = 0;

  while (WiFi.status() != WL_CONNECTED && count < 10) {
    count++;
    delay(500);
    Serial.println("Conectando WiFi...");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Conectado...");
    Serial.println("Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("Esp Mac Address");
    Serial.println(WiFi.subnetMask());
    Serial.println("Gateway IP:");
    Serial.println(WiFi.gatewayIP());
    Serial.println("DNS IP:");
    Serial.println(WiFi.dnsIP());
  } else {
    Serial.println("Falha...");
  }
}

void handlePageClient() {
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html, processor);
});

  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", packet.c_str());
  });
  server.on("/relay", HTTP_GET, handleRelayRequest);
  server.onNotFound(notFound);
  server.begin();
}

void handleRelayRequest(AsyncWebServerRequest *request) {
  String state = request->getParam("state")->value();

  if (state == "on") {
    sendControlMessage(true);
  } else {
    sendControlMessage(false);
  }

  request->send(200, "text/plain", "OK");
}

void setup() {
  Heltec.begin(displayState, loRaState, serialState, powerAmplifier, transmissionBand);
  Serial.begin(115200);
  pinMode(RelayPin, OUTPUT);
  digitalWrite(RelayPin, HIGH);  // Desliga o relé por padrão
  setupWifi();
  startOLED();
  delay(1000);
  handlePageClient();
  LoRa.receive();
  LoRa.setSyncWord(KEY);
}

void loop() {

  unsigned long currentMillis = millis();

  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    cbk(packetSize);
    Serial.print("Recebendo a temperatura: ");
    Serial.print(packet);  // Imprime no monitor serial a temperatura
    Serial.println("°C");
  }

  if (currentMillis - previousMillisDisplay >= intervalDisplay) {
    previousMillisDisplay = currentMillis;

    display.clear();
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(64, 15, "IP: " + WiFi.localIP().toString());
    display.drawString(64, 35, "Temperatura: " + packet + " C");
    display.display();
  }
}

// Envio de mensagem de controle para o cliente
void sendControlMessage(bool state) {
  String message = state ? "C:ON" : "C:OFF";
  LoRa.beginPacket();
  LoRa.print(message);
  LoRa.endPacket();
}

void cbk(int packetSize) {
  String packetResult = "";
  packSize = String(packetSize);

  for (int i = 0; i < packetSize; i++) {
    packetResult += (char)LoRa.read();  // Atribui um caractere por vez à variável packet
  }
  if (packetResult.startsWith("T:")) {
    String tempString = packetResult.substring(2);
    float temperature = tempString.toFloat();
    packet = tempString;
  }

  Serial.print("' with RSSI ");
  Serial.println(LoRa.packetRssi());
}

void startOLED() {
  if (!display.init()) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;) {  // Não prossiga, loop para sempre
      delay(1);
    }
  }
  display.setFont(ArialMT_Plain_10);
  display.clear();
  display.drawString(33, 5, "Iniciado");
  display.drawString(10, 30, "com Sucesso!");
  display.drawString(10, 45, "IP: " + WiFi.localIP().toString());
  display.display();
  delay(1000);
}
