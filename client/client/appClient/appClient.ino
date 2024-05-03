#include "heltec.h"

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
char st; // varivel de recebimento

///essas variavies ainda não estão sendo ultilizadas
String rssi = "RSSI --"; // para teste
String packSize = "--"; // variavel do tamanho do pacote
String packet; /// pacote
bool receiveflag = false; /// flag para recebimento
// prototipo da função
void onReceive(int packetSize);

// iniciação do codigo
void setup() {
  pinMode(RelayPin,OUTPUT); //inicializa o LED
  Heltec.begin(displayState, loRaState, serialState, powerAmplifier, transmissionBand); /// inicia component esp32 lora 
  Serial.begin(115200); // defini canal do serial
  LoRa.setSyncWord(KEY); /// para recebimento do pocate lora 
  LoRa.receive(); /// escura o recebimento de pacote
  delay(1000);  
 
}

void loop() {
  int packetSize = LoRa.parsePacket(); /// pacote recebido
  if (packetSize) { //// verfica se recebeu algo 
    Serial.println("Pacote recebido");
    /* Recebe os dados conforme protocolo */   
    while (LoRa.available()) {  
      st = (char)LoRa.read();
    }
     Serial.println(st);
    if (st == '1')
    digitalWrite(RelayPin, HIGH);  /// liga o led
    delay(1000);
    digitalWrite(RelayPin, LOW); // desloga led
   /* Escreve RSSI de recepção e informação recebida */
    Serial.print("'com RSSI");
    Serial.println(LoRa.packetRssi());
  }
}


/// ainda ira se trabalhado , mas na teoria tambem se usa para receber 
void onReceive(int packetSize)//LoRa receiver interrupt service
{
    //if (packetSize == 0) return;

    Serial.println("esta recebendo os dados");
    packet = "";
    packSize = String(packetSize,DEC);

    while (LoRa.available())
    {
        packet += (char) LoRa.read();
    }

    Serial.println(packet);
    rssi = "RSSI: " + String(LoRa.packetRssi(), DEC);
    receiveflag = true;
}