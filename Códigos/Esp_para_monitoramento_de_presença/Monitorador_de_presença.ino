//#include <ESP8266WiFi.h>
//#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <WiFi.h>
//#include <ArduinoOTA.h>
#include <esp_now.h>

int sensorPresenca = 13;
//int buzzer = D2;  // Tem que ser pino analógico
//int hall = D4;
//int botao_trocar = D5;
long tempo = 0;
long tempo_ligar = 0;
long tempo_ligarar = 0;
long tempo_aberta = 0;
long tempo_mensagem = 0;
bool estadoAr = false;
bool ativar_buzzer = false;

uint8_t broadcastAddress[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

typedef struct struct_message {
  char a[32];
  int b;
  float c;
  String d;
  bool e;
} struct_message;

struct_message myData;

esp_now_peer_info_t peerInfo;

unsigned long lastTime = 0;
unsigned long timerDelay = 2000;

// Callback quando os dados são enviados
void OnDataSent(uint8_t* mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0) {
    Serial.println("Delivery success");
  } else {
    Serial.println("Delivery fail");
  }
}

void setup() {
  pinMode(sensorPresenca, INPUT);
  //pinMode(buzzer, OUTPUT);
  /*
  pinMode(botao_trocar, INPUT_PULLUP);
  pinMode(hall, INPUT_PULLUP);
  */
  Serial.begin(115200);
  WiFi.softAP("Verificador de preseça", "Eletronica123");

  // Inicializar Wi-Fi no modo STA antes de iniciar ESP-NOW
  WiFi.mode(WIFI_STA);

  // Inicializar ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}


void verifica_presenca() {
  Serial.println(estadoAr);
  // Lê os estados dos sensores

  Serial.println(digitalRead(sensorPresenca));
  // Verifica se o sensor detectou ou não presença
  //if (millis() - tempo_mensagem > 1000) {
  //  if (sensorStatus == LOW) {

  //  } else {

  //  }
  //  tempo_mensagem = millis();
  //  Serial.println(estadoAr);
  //}
  if (digitalRead(sensorPresenca) == false) {
    Serial.println("Movimento não detectado" + digitalRead(sensorPresenca));
    // Se não detectou presença, começa a contar um tempo de 15 minutos para desligar o ar condicionado
    if (millis() - tempo > 10000 && estadoAr == true) {  //1800000
      // 1800000
      // Se o tempo sem detecção de presença foi atingido, envia uma mensagem pelo protocolo ESP-NOW para o outro ESP
      tempo = millis();
      Serial.println("Desligar o ar condicionado");
      myData.d = "Desligar";
      Serial.println("Código enviado");

      // Enviando mensagem via ESP-NOW
      esp_now_send(broadcastAddress, (uint8_t*)&myData, sizeof(myData));

      estadoAr = false;
    }
  } else {
    Serial.println("Movimento detectado");
    if (estadoAr == false) {
      tempo = millis();
      //if (estadoAr == "Desligado") {
      // Se detectou presença, iguala o contador ao millis para "zerar" o contador
      // E envia uma mensagem para o outro ESP para ligar o ar condicionado
      Serial.println("Ligando ar condicionado");
      myData.d = "Ligar";
      esp_now_send(broadcastAddress, (uint8_t*)&myData, sizeof(myData));
      Serial.println("Código enviado");
      tempo_ligarar = millis();

      estadoAr = true;
    } else {
      if (millis() - tempo_ligarar > 10000) {

        tempo = millis();
        // Se detectou presença, iguala o contador ao millis para "zerar" o contador
        // E envia uma mensagem para o outro ESP para ligar o ar condicionado
        Serial.println("Ligando ar condicionado");
        myData.d = "Ligar";
        esp_now_send(broadcastAddress, (uint8_t*)&myData, sizeof(myData));
        Serial.println("Código enviado");
        tempo_ligarar = millis();
      }
    }
  }
}


/*
void verifica_hall() {
  //Serial.println(digitalRead(hall));
  if (digitalRead(hall) == 1) {
    if (millis() - tempo_aberta > 10000) {
      if (millis() - tempo_ligar < 500) {
        tone(buzzer, 1500);
        //Serial.println("buzzer ligado");
      } else {
        noTone(buzzer);
        //Serial.println("buzzer desligado");
      }
      if (millis() - tempo_ligar > 1000) {
        tempo_ligar = millis();
      }
    }
  } else {
    noTone(buzzer);
    tempo_aberta = millis();
  }
}
*/

void loop() {
  // Serial.println(digitalRead(botao_trocar));
  /*
  if (digitalRead(botao_trocar) == 0) {
    // Parte de enviar o código por WiFi
    noTone(buzzer);
#ifndef STASSID
#define STASSID "PortaEletronica"
#define STAPSK "Eletronica123"
#endif

    const char* ssid = STASSID;
    const char* password = STAPSK;

    WiFi.softAP("Verificador de preseça", "Eletronica123");
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

  } else {  // else do espNow
  */
  //WiFi.disconnect();
  //WiFi.mode(WIFI_OFF);
  //WiFi.mode(WIFI_STA);
  verifica_presenca();
  //verifica_hall();
  //}
}
