
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <AutoOTA.h>
//#include <FileData.h>
//#include <LittleFS.h>


const char* ssid = "Dnlvrn";
const char* password =  "2155791975";

//=================================================ТОПИКИ
                  
const char* Tmg =  "MyDev/10a8c3a2/#" ;   //870690bb/set/mg";         //топик - свет в туалете //ID клиента ноутбук
const char* Tsupdata = "MyDev/10a8c3a2/17d35acf/set/supd";            // поиск обновлений ID клиент- мой телефон
const char* Tvers = "MyDev/10a8c3a2/ID/set/vers";                     // сюда шлем версию прошивы


//   MQTT  ============================================

const char* mqtt_server = "m4.wqtt.ru";
const int mqtt_port = 9478;
const char* mqtt_user = "u_5A3C2X";
const char* mqtt_password = "HilZPRjD";

WiFiClient espClient;
PubSubClient client(espClient);
//============================================================
#define led 2
String ver, notes;                      //при обновлении  версия и описание

unsigned long lastMsg;                  // время для отправки топиков
#define MSG_BUFFER_SIZE	(12)
char msg[MSG_BUFFER_SIZE];              //сообщение для отправки в топики

//int count =10;                        //просто счетчик
unsigned long was_ota;                  //засекаем время, что бы несколько раз не искать обнову

//=============================================================

AutoOTA ota("0.8.1", "Srvrn1/LipaSensorTualet");


void ota_chek(){
  if (ota.checkUpdate(&ver, &notes)) {
    Serial.print("пришло обновление: ");
    Serial.println(ver);
    Serial.println(notes);

    //client.publish(Tgvs, "Reset");
    ota.updateNow();
  }
  else Serial.println("нет обновы...");
  client.publish(Tvers, ota.version().c_str());
}

void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
//==============================================================================================
void callback(char* topic, byte* payload, int length) {          //обрабатываем входящие топики

  if(String(topic) == String(Tsupdata) && millis() - was_ota > 5000){       //топик обновы с моего ID то идем на GitHub искать обнову
    was_ota = millis();
    Serial.println("смотрим обнову");
    ota_chek();                     
  }

//=================================
  uint8_t bkv = strlen(topic);
  if(topic[bkv-2] == 'm' && topic[bkv-1] == 'g'){             //если топик /mg не важно с какого ID

    if ((char)payload[0] == '1') {                            //включаем свет в сортире
     digitalWrite(led, LOW); 
    } 
    else {
      digitalWrite(led, HIGH); 
    }
  }
  
}

void reconnect() {
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
 
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password )) {
 
      Serial.println("connected"); 

      client.publish(Tvers, ota.version().c_str());    //функция ".c_str()"" преобразует из STRING d const char

      client.subscribe(Tmg);                                   //подписка на топики
      //client.subscribe(Tsupdata);                              
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
 
    }
  }
}


void setup() {

  //LittleFS.begin();

  pinMode(led, OUTPUT);    
  digitalWrite(led, HIGH);

  Serial.begin(74880);
  Serial.println();
  Serial.println("gogo");
  Serial.print("Version ");
  Serial.println(ota.version());

  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
 
}

void loop() {
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

}