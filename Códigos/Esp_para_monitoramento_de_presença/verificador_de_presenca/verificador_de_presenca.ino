#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
//#include <ArduinoOTA.h>
#include <espnow.h>

bool sensorStatus = false;
int sensorPresenca = D1;
int speakerPin = D2;
//int buzzer = D2;  // Tem que ser pino analógico
int hall = D4;
int botao_trocar = D5;
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


#define NOTE_B1 247
#define NOTE_C2 262
#define NOTE_CS2 277
#define NOTE_D2 294
#define NOTE_DS2 311
#define NOTE_E2 330
#define NOTE_F2 349
#define NOTE_FS2 370
#define NOTE_G2 392
#define NOTE_GS2 415
#define NOTE_A2 440
#define NOTE_AS2 466
#define NOTE_B2 494
#define NOTE_C3 523
#define NOTE_CS3 554
#define NOTE_D3 587
#define NOTE_DS3 622
#define NOTE_E3 659
#define NOTE_F3 698
#define NOTE_FS3 740
#define NOTE_G3 784
#define NOTE_GS3 831
#define NOTE_A3 880
#define NOTE_AS3 932
#define NOTE_B3 988
#define NOTE_C4 1047
#define NOTE_CS4 1109
#define NOTE_D4 1175
#define NOTE_DS4 1245
#define NOTE_E4 1319
#define NOTE_F4 1397
#define NOTE_FS4 1480
#define NOTE_G4 1568
#define NOTE_GS4 1661
#define NOTE_A4 1760
#define NOTE_AS4 1865
#define NOTE_B4 1976
#define NOTE_C5 2093
#define NOTE_CS5 2217
#define NOTE_D5 2349
#define NOTE_DS5 2489
#define NOTE_E5 2637
#define NOTE_F5 2794
#define NOTE_FS5 2960
#define NOTE_G5 3136
#define NOTE_GS5 3322
#define NOTE_A5 3520
#define NOTE_AS5 3729
#define NOTE_B5 3951
#define NOTE_C6 4186

// Notas da música "Jingle Bells" em duas oitavas acima
int melody[] = {
  NOTE_E5, NOTE_E5, NOTE_E5,                                              // Jingle
  NOTE_E5, NOTE_E5, NOTE_E5,                                              // Bells
  NOTE_E5, NOTE_G5, NOTE_C5, NOTE_D5, NOTE_E5,                            // Jingle all the way
  NOTE_F5, NOTE_F5, NOTE_F5, NOTE_F5, NOTE_F5, NOTE_E5, NOTE_E5,          // Oh what fun it is to ride
  NOTE_E5, NOTE_E5, NOTE_E5, NOTE_D5, NOTE_D5, NOTE_E5, NOTE_D5, NOTE_G5  // in a one horse open sleigh
};

// Durações das notas ajustadas para tocar ainda mais devagar
int noteDurations[] = {
  18, 18, 9,                     // Jingle
  18, 18, 9,                     // Bells
  18, 18, 18, 18, 9,             // Jingle all the way
  18, 18, 18, 18, 18, 18, 18,    // Oh what fun it is to ride
  18, 18, 18, 18, 18, 18, 18, 9  // in a one horse open sleigh
};

unsigned long previousMillis = 0;
int noteIndex = 0;
int noteDuration = 0;
unsigned long noteStartTime = 0;
bool notePlaying = false;


void setup() {
  pinMode(sensorPresenca, INPUT);
  pinMode(speakerPin, OUTPUT);
  /*
  pinMode(buzzer, OUTPUT);
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

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);

  // Registrar peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  esp_now_send(broadcastAddress, (uint8_t*)&myData, sizeof(myData));
}

/*
void verifica_presenca() {
  Serial.println(estadoAr);
  // Lê os estados dos sensores
  sensorStatus = digitalRead(sensorPresenca);
  Serial.println(sensorStatus);
  // Verifica se o sensor detectou ou não presença
  //if (millis() - tempo_mensagem > 1000) {
  //  if (sensorStatus == LOW) {

  //  } else {

  //  }
  //  tempo_mensagem = millis();
  //  Serial.println(estadoAr);
  //}
  if (sensorStatus == false) {
    Serial.println("Movimento não detectado" + sensorStatus);
    // Se não detectou presença, começa a contar um tempo de 15 minutos para desligar o ar condicionado
    if (millis() - tempo > 120000) {  //1800000
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
  }else{
    Serial.println("Movimento detectado");
    if(estadoAr == false){
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
    } else if(millis() - tempo_ligarar > 600000) {
      
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
*/


void playTone(int frequency, int duration) {
  int period = 1000000 / frequency;
  int pulse = period / 2;
  for (long i = 0; i < duration * 100L; i += period) {
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(pulse);
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(pulse);
  }
}


void verifica_hall() {
  //Serial.println(digitalRead(hall));
  /*
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
  */
  if (digitalRead(hall) == 1) {
    Serial.println(tempo_aberta);
    Serial.println("O tempo está: " + tempo_aberta);
    if (millis() - tempo_aberta > 300000) {
      unsigned long currentMillis = millis();

      // Verifique se a nota está tocando
      if (notePlaying && currentMillis - noteStartTime >= noteDuration) {
        // Desliga o tom quando o tempo da nota acabar
        digitalWrite(speakerPin, LOW);
        notePlaying = false;
      }

      // Iniciar a próxima nota se necessário
      if (!notePlaying && noteIndex < sizeof(melody) / sizeof(melody[0])) {
        int noteFrequency = melody[noteIndex];
        noteDuration = 3500 / noteDurations[noteIndex];  // Duração maior para tocar mais devagar
        playTone(noteFrequency, noteDuration);           // Tocar a nota
        noteStartTime = currentMillis;
        notePlaying = true;
        noteIndex++;
      }

      // Repetir a música após terminar
      if (noteIndex >= sizeof(melody) / sizeof(melody[0]) && !notePlaying) {
        noteIndex = 0;
      }
    }
  } else{
    tempo_aberta = millis();
  }
}


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
  //verifica_presenca();
  verifica_hall();
  //}
}
