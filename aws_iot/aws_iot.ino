//Nodemcu D7 ----> PB6
//Nodemcu D8 ----> PA10

#include <SoftwareSerial.h>
#include <Arduino.h>
#include <String>
#include <ArduinoJson.h>
#include <ESP8266WiFiMulti.h>

//AWS
#include "sha256.h"
#include "Utils.h"

//WEBSockets
#include <Hash.h>
#include <WebSocketsClient.h>
#include <PubSubClient.h>

#define MQTT_MAX_PACKET_SIZE 256

//AWS MQTT Websocket 
#include "Client.h"
#include "AWSWebSocketClient.h"
#include "CircularByteBuffer.h"

extern "C" {
  #include "user_interface.h"
}

//Gateway parameters
#define GATEWAY_ID 3

//AWS IOT config, change these:
char wifi_ssid[]       = "xxxxxxxxxx";
char wifi_password[]   = "xxxxxxxxxxxxxxxxxxxx";
char aws_endpoint[]    = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
char aws_key[]         = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
char aws_secret[]      = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
char aws_region[]      = "xxxxxxxxxxxxxxxxxx";
const char* aws_topic  = "xxxxxxxxxxxxxx";   //viso/node/displacement  $aws/things/esp_example/shadow/update
int port = 443;


//MQTT config
const int maxMQTTpackageSize = 512;
const int maxMQTTMessageHandlers = 1;

ESP8266WiFiMulti WiFiMulti;
SoftwareSerial swSer(13, 15, false, 256);   //STM32 ile haberleşmek için softwareserial kullanıyoruz
AWSWebSocketClient awsWSclient(1000);
PubSubClient client(awsWSclient);

//# of connections
long connection = 0;

//generate random mqtt clientID
char* generateClientID () {
  char* cID = new char[23]();
  for (int i=0; i<22; i+=1)
    cID[i]=(char)random(1, 256);
  return cID;
}

//count messages arrived
int arrivedcount = 0;

/* define json document */
StaticJsonDocument<256> doc;    //awsden publish edeceğimiz veriyi okumak için öncelikle json oluşturuyoruz

/*
char* data[256];
String hold = "";
*/
//callback to handle mqtt messages

void callback(char* topic, byte* payload, unsigned int length) {
  
  deserializeJson(doc, payload);
  int gatewayID = doc["gatewayID"];
  int sensorID = doc["sensorID"];
  int sleepTime = doc["sleepTime"];
  int timee = doc["timee"];
  int configType = doc["configType"];
  uint8_t configBuffer[64];
  uint32_t value = 0xa455a95f;

  
  if(configType!=0){
    if(gatewayID == GATEWAY_ID){
      Serial.println("Config Received\n");
      Serial.print("gatewayID: ");
      Serial.println(gatewayID);
      Serial.print("sensorID: ");
      Serial.println(sensorID);
      Serial.print("sleepTime: ");
      Serial.println(sleepTime);
      Serial.print("timee: ");
      Serial.println(timee);
      Serial.print("configType: ");
      Serial.println(configType);
  
      configBuffer[0]=(uint8_t)(sensorID>>24);          //msb kullandığımız için  64 bytelık dizimize 4 bytelık
      configBuffer[1]=(uint8_t)(sensorID>>16);
      configBuffer[2]=(uint8_t)(sensorID>>8);
      configBuffer[3]=(uint8_t)(sensorID);
  
      configBuffer[4]=(uint8_t)(sleepTime>>24);
      configBuffer[5]=(uint8_t)(sleepTime>>16);
      configBuffer[6]=(uint8_t)(sleepTime>>8);
      configBuffer[7]=(uint8_t)(sleepTime);
  
      configBuffer[8]=(uint8_t)(timee>>24);
      configBuffer[9]=(uint8_t)(timee>>16);
      configBuffer[10]=(uint8_t)(timee>>8);
      configBuffer[11]=(uint8_t)(timee);
  
      configBuffer[56]=(uint8_t)(configType>>24);
      configBuffer[57]=(uint8_t)(configType>>16);
      configBuffer[58]=(uint8_t)(configType>>8);
      configBuffer[59]=(uint8_t)(configType);
  
      configBuffer[60]=(uint8_t)(value>>24);
      configBuffer[61]=(uint8_t)(value>>16);
      configBuffer[62]=(uint8_t)(value>>8);
      configBuffer[63]=(uint8_t)(value);
      
      int i =0;
      for(i=0;i<64;i++){
        swSer.write(configBuffer[i]);
        delay(50);
      }
    }
  }
}


//connects to websocket layer and mqtt layer
bool connect () {

    if (client.connected()) {    
        client.disconnect ();
    }  
    //delay is not necessary... it just help us to get a "trustful" heap space value
    delay (1000);
    Serial.print (millis ());
    Serial.print (" - conn: ");
    Serial.print (++connection);
    Serial.print (" - (");
    Serial.print (ESP.getFreeHeap ());

    //creating random client id
    char* clientID = generateClientID ();
    client.setServer(aws_endpoint, port);
    if (client.connect(clientID)) {
      Serial.println("connected");     
      return true;
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      return false;
    }    
}



//subscribe to a mqtt topic
void subscribe () {
    client.setCallback(callback);
    client.subscribe(aws_topic);
   //subscript to a topic
    Serial.println("MQTT subscribed");
}


/*Software Reset function */
void sendReset() {
  Serial.println("Sending Reset command");
  Serial.println(1/0);
}


/*send a message to a mqtt topic*/
void sendmessageData (char **parsedMessage) {
    StaticJsonDocument<255> JSONdata;
    JSONdata["gatewayID"]=GATEWAY_ID;
    JSONdata["sensorID"]=parsedMessage[0];
    JSONdata["SensorTime"]=parsedMessage[1];
    JSONdata["BatteryVoltage"]=parsedMessage[2];
    JSONdata["Displacement"]=parsedMessage[3];
    JSONdata["Temperature"]=parsedMessage[4];
    JSONdata["AngX"]=parsedMessage[5];
    JSONdata["AngY"]=parsedMessage[6];
    JSONdata["AngZ"]=parsedMessage[7];
    //send a message   
    char buf[255];
    serializeJson(JSONdata, buf);
    //Serial.println(buf);
    client.publish(aws_topic,buf);
}

void setup() {
    wifi_set_sleep_type(NONE_SLEEP_T);
    Serial.begin (9600);
    swSer.begin(9600);

    delay (2000);
    //Serial.setDebugOutput(1);

    //fill with ssid and wifi password
    WiFiMulti.addAP(wifi_ssid, wifi_password);
    Serial.println ("connecting to wifi");
    while(WiFiMulti.run() != WL_CONNECTED) {
        delay(100);
        Serial.print (".");
    }
    Serial.println ("\nconnected");
    //fill AWS parameters    
    awsWSclient.setAWSRegion(aws_region);
    awsWSclient.setAWSDomain(aws_endpoint);
    awsWSclient.setAWSKeyID(aws_key);
    awsWSclient.setAWSSecretKey(aws_secret);
    awsWSclient.setUseSSL(true);
    if (connect ()){
      subscribe ();
    }
}

#define MAX_MESSAGE_LENGTH 256
int j=0;
char messageData[MAX_MESSAGE_LENGTH];
bool messageReceiving=false;

void loop() {
  //keep the mqtt up and running
  if(swSer.available() > 0) {
    char readVal = swSer.read();
    
    if(messageReceiving){
      if(readVal=='\n'){
      char tmpData[j+1];
      strcpy(tmpData,messageData);
      tmpData[j]=';';
      Serial.println(tmpData);
      int k;
      char *parsedMessage[8];
      parsedMessage[0]=strtok(tmpData,":;");
      for(k=1;k<8;k++){
        parsedMessage[k]=strtok(NULL,":;");
      }
      sendmessageData(parsedMessage);
      j=0;
      messageReceiving=false;
    }else{
      messageData[j]=readVal;
      j++;
    }
    }
    if(readVal=='*'){
      messageReceiving=true;
    }
    //Serial.write(readVal);
  }

  if (awsWSclient.connected ()) {   
     client.loop ();
  } else {
    //reset when reconnect
    void sendReset();
  }
  
}
