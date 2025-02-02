
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <AutoOTA.h>


const char* ssid = "Dnlvrn";
const char* password =  "2155791975";

//=================================================ТОПИКИ
                  
const char* Tmg =  "MyDev/10a8c3a2/#" ;   //870690bb/set/mg";         //топик - //ID клиента ноутбук
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
#define led 2            //D4
#define Rroz 4          //D2
//#define led_brite 14     //D5
//#define led_beck 12      //D6


String ver, notes;                      //при обновлении  версия и описание

//unsigned long lastMsg;                // время для отправки топиков
#define MSG_BUFFER_SIZE	(12)
char msg[MSG_BUFFER_SIZE];              //сообщение для отправки в топики

//int count =10;                        //просто счетчик
unsigned long was_ota;                  //засекаем время, что бы несколько раз не искать обнову

//=============================================================

AutoOTA ota("1.5", "Srvrn1/RitaRoom");


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
void callback(char* topic, byte* payload, int length) {          //обрабатываем входящие топики. length-кол-во символоа payload

  if(String(topic) == String(Tsupdata) && millis() - was_ota > 5000){       //топик обновы с моего ID то идем на GitHub искать обнову
    was_ota = millis();
    client.publish(Tvers, "888");
    Serial.println("смотрим обнову");
    ota_chek();                     
  }
//===================================
  /*Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();*/
//=================================
  uint8_t bkv = strlen(topic);     //узнаем сколько букв в топике

  /*Serial.print("--");
  Serial.print(topic[bkv-5]);
  Serial.print(topic[bkv-4]);
  Serial.print(topic[bkv-3]);
  Serial.print(topic[bkv-2]);
  Serial.print(topic[bkv-1]);
  Serial.println();*/

  if(topic[bkv-4] == 'R' && topic[bkv-3] == 'r' && (char)topic[bkv-2] == 'o' && (char)topic[bkv-1] == 'z'){    //если топик /Rroz не важно с какого ID
    if ((char)payload[0] == '1') {                            //включаем свет у Риты
     digitalWrite(Rroz, LOW); 
    } 
    else {
      digitalWrite(Rroz, HIGH); 
    }
  }

  else if(topic[bkv-5] == 'R' && topic[bkv-4] == 's' && topic[bkv-3] == 'v' && (char)topic[bkv-2] == 'e' && (char)topic[bkv-1] == 't'){
    if ((char)payload[0] == '1') {                            //включаем свет у Риты
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

  pinMode(led, OUTPUT);    
  digitalWrite(led, HIGH);
  pinMode(Rroz, OUTPUT);    
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