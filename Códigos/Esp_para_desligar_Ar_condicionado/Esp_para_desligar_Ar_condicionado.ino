//#include <ESP8266WiFi.h>
//#include <IRremoteESP8266.h>
//#include <IRsend.h>
//#include <espnow.h>
//#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
//#include <ArduinoOTA.h>
#include <WiFi.h>
#include <esp_now.h>
#include <Arduino.h>
#include <IRremote.hpp>

//Definindo o pino do emissor infravermelho
int emissorverm = 2;
//int interruptor = 2;
bool botao_trocar = true;

IRsend irsend(emissorverm);

uint16_t Desligar[] = { 4650, 4250, 600, 1500, 600, 450, 600, 1500, 600, 1500,
                          650, 400, 600, 450, 600, 1500, 650, 400, 600, 450, 600,
                          1500, 650, 400, 600, 450, 600, 1500, 650, 1450, 650, 400,
                          650, 1450, 650, 400, 650, 1450, 650, 1500, 600, 1500, 600,
                          1500, 600, 450, 600, 1500, 600, 1500, 600, 1500, 600, 450,
                          600, 450, 600, 450, 600, 450, 600, 1500, 600, 450, 600, 450,
                          600, 1500, 600, 1500, 600, 1500, 600, 450, 600, 450, 600, 450,
                          600, 450, 600, 450, 600, 450, 600, 450, 600, 450, 600, 1500,
                          600, 1500, 600, 1500, 600, 1500, 600, 1550, 550 };

uint16_t Ligar[] = { 4500, 4450,
                        600, 1600, 550, 550, 550, 1600, 550, 1650,
                        600, 500, 550, 550, 550, 1600, 600, 500,
                        550, 550, 550, 1650, 550, 550, 550, 550,
                        550, 1600, 550, 1650, 600, 500, 550, 1650,
                        550, 550, 550, 1600, 600, 500, 550, 1650,
                        550, 1650, 550, 1600, 600, 1600, 550, 1650,
                        550, 1600, 600, 500, 550, 1650, 550, 550,
                        550, 550, 550, 550, 550, 500, 600, 500,
                        550, 550, 550, 550, 550, 550, 550, 550,
                        550, 550, 550, 550, 550, 550, 550, 500,
                        600, 1600, 550, 1650, 550, 1650, 600, 1550, 600, 1600, 550, 1650, 550, 1600, 600, 1600, 550 };


typedef struct struct_message {
  char a[32];
  int b;
  float c;
  String d;
  bool e;
} struct_message;

struct_message myData;


void OnDataRecv(uint8_t* mac, uint8_t* incomingData, uint8_t len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.println(myData.d);
  Serial.println("Código recebido");
  if (myData.d == "Desligar") {
    Serial.println("Desligando ar condiconado");
    //Código para desligar o ar condicionado
    //irsend.sendRaw(Desligar, sizeof(Desligar) / sizeof(Desligar[0]), 38);
    IrSender.sendRaw(Desligar, sizeof(Desligar) / sizeof(Desligar[0]), 38);
  } else if (myData.d == "Ligar") {
    Serial.println("Ligando ar condicionado");
    //Código para ligar o ar condicionado
    //irsend.sendRaw(Ligar, sizeof(Ligar) / sizeof(Ligar[0]), 38);
    IrSender.sendRaw(Ligar, sizeof(Ligar) / sizeof(Ligar[0]), 38);
  }
}

void setup() {
  //pinMode(emissorverm, OUTPUT);
  //pinMode(interruptor, INPUT);


  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));


  Serial.begin(9600);
  IrSender.begin(2, ENABLE_LED_FEEDBACK, USE_DEFAULT_FEEDBACK_LED_PIN);
  Serial.println("Passou do setup");
}

void loop() {
  /*
#ifndef STASSID
#define STASSID "Infravermelho"
#define STAPSK "Eletronica123"
#endif

    const char* ssid = STASSID;
    const char* password = STAPSK;
    WiFi.softAP("Infravermelho", "Eletronica123");
    // Atribuindo um endereço de IP fixo na conexão com o webserver
    // Futuramente substituir por um DNS
    IPAddress local_ip(192, 168, 1, 1);
    IPAddress gateway(192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);
    WiFi.softAPConfig(local_ip, gateway, subnet);

    ArduinoOTA.onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
      } else {  // U_FS
        type = "filesystem";
      }
      Serial.println("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() {
      Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) {
        Serial.println("Auth Failed");
      } else if (error == OTA_BEGIN_ERROR) {
        Serial.println("Begin Failed");
      } else if (error == OTA_CONNECT_ERROR) {
        Serial.println("Connect Failed");
      } else if (error == OTA_RECEIVE_ERROR) {
        Serial.println("Receive Failed");
      } else if (error == OTA_END_ERROR) {
        Serial.println("End Failed");
      }
    });
    ArduinoOTA.begin();
    Serial.println("Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    ArduinoOTA.handle();
    Serial.println("Modo de passar código");
*/
}