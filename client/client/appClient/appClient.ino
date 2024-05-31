#include "HT_SSD1306Wire.h"
#include "heltec.h"
#include <DHT.h>

///definições do radio lora
#define displayState false  //define se display estará ou nao ativo
#define loRaState true      //define se o rádio estará ou nao ativo
#define serialState true    //define se o rádio estará ou nao ativo

#define powerAmplifier true     //define se o amplificador de potencia PABBOOST estará ou nao ativo
#define transmissionBand 433E6  //define a frequencia media de transmissão: 868E6, 915E6

#define KEY 0xF3                // Chave para receber pacote
#define DHTPIN 13               // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22
#define PINRELE 17

static SSD1306Wire  display(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED); // addr , freq , i2c group , resolution , rst
DHT dht(DHTPIN, DHTTYPE);

float currentTemp;

bool stateRele;

void getTemp();
void sendPacket();

void setup() {

  pinMode(PINRELE, OUTPUT);

  Serial.begin(115200);
  Heltec.begin(displayState, loRaState, serialState, powerAmplifier, transmissionBand);
  dht.begin();
  startOLED();
  currentTemp = dht.readTemperature();
  delay(1000);
}

void loop() {
  getTemp();
  Serial.print("Temperatura: ");
  Serial.print(currentTemp);
  Serial.println("°C");

  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(30, 5, "Temperatura");
  display.drawString(33, 30, (String)currentTemp);
  display.drawString(78, 30, "°C");
  display.display();
  
  delay(1000);

}

void startOLED(){

  if(!display.init()) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.setFont(ArialMT_Plain_10);
  display.clear();
  display.drawString(33, 5, "Iniciado");
  display.drawString(10, 30, "com Sucesso!");
  display.display();
  delay(5000);


}

void sendPacket() {
  LoRa.beginPacket();
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
  ///Verifica se a temperatura está entre 2°C e 8°C
  if (temperature >= 20 && temperature <= 30) {
    digitalWrite(PINRELE, LOW);  // Liga o relé 1
    Serial.println("Relé 1 ligado");
  } else {
    digitalWrite(PINRELE, HIGH);   // Desliga o relé 1
    Serial.println("Relé 1 desligado");
  }
  delay(3000);
}