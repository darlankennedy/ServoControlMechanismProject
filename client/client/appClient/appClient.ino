#include "HT_SSD1306Wire.h"
#include "heltec.h"
#include <DHT.h>
///definições do radio lora
#define displayState false  //define se display estará ou nao ativo
#define loRaState true      //define se o rádio estará ou nao ativo
#define serialState true    //define se o rádio estará ou nao ativo
#define powerAmplifier true     //define se o amplificador de potencia PABBOOST estará ou nao ativo
#define transmissionBand 915E6  //define a frequencia media de transmissão: 868E6, 915E6
#define KEY 0xF3   // Chave para receber pacote
#define DHTPIN 13  // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22
#define PINRELE 17

static SSD1306Wire display(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED);  // addr , freq , i2c group , resolution , rst
DHT dht(DHTPIN, DHTTYPE);

float currentTemp;

unsigned long previousMillisTemp = 0;
unsigned long previousMillisSend = 0;
const long intervalTemp = 1000;
const long intervalSend = 1000;

String packSize = "--";
String packet;

bool stateRele = true;

void getTemp();
void sendPacket(float currentTemp);
void receivePacket();
void controlRele(bool state);
void setup() {

  pinMode(PINRELE, OUTPUT);

  Serial.begin(115200);
  Heltec.begin(displayState, loRaState, serialState, powerAmplifier, transmissionBand);
  dht.begin();
  startOLED();
  LoRa.receive();
  LoRa.setSyncWord(KEY);
  currentTemp = dht.readTemperature();
  digitalWrite(PINRELE, HIGH);
}

void loop() {

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillisTemp >= intervalTemp) {
    previousMillisTemp = currentMillis;
    getTemp();
  }

  if (currentMillis - previousMillisSend >= intervalSend) {
    previousMillisSend = currentMillis;
    sendPacket(currentTemp);
  }
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(30, 5, "Temperatura");
  display.drawString(33, 30, (String)currentTemp);
  display.drawString(78, 30, "°C");
  display.display();

  receivePacket();
}


void startOLED() {

  if (!display.init()) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }
  display.setFont(ArialMT_Plain_10);
  display.clear();
  display.drawString(33, 5, "Iniciado");
  display.drawString(10, 30, "com Sucesso!");
  display.display();
  delay(1000);
}

void sendPacket(float currentTemp) {
  LoRa.beginPacket();
  LoRa.print("T:");
  LoRa.print(currentTemp);
  LoRa.endPacket();
}

void getTemp() {
  float temperature = dht.readTemperature();

  if (isnan(temperature)) {
    Serial.println("Falha na leitura do sensor DHT!");
    return;
  }
  currentTemp = temperature;
  if (stateRele) {
    if (temperature > 4) {
      digitalWrite(PINRELE, LOW);  // Liga o relé 1
    } else {
      digitalWrite(PINRELE, HIGH);  // Desliga o relé 1
    }
  }
}

void controlRele(bool state) {
  if (state) {
    digitalWrite(PINRELE, LOW);  // Liga o relé
    Serial.println("Relé manualmente ligado");
  } else {
    digitalWrite(PINRELE, HIGH);  // Desliga o relé
    Serial.println("Relé manualmente desligado");
  }
}

void receivePacket() {
  String LoRaData;
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String packetResult = "";
    while (LoRa.available()) {
      LoRaData = LoRa.readString();
    }

    Serial.print(LoRaData);
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());

    if (LoRaData == "C:OFF") {
      controlRele(false);
    } else if (LoRaData == "C:ON") {
      controlRele(true);
    }
  }
}