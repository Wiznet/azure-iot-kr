#include "DHT.h"
#include "at_cmd_parser.h"

#define RET_OK        1
#define RET_NOK      -1
#define RESP_OK       "OK\r\n"

// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6, 7); // RX, TX
#endif

// DHT Sensor settings
#define DHTPIN 14
#define DHTTYPE DHT11

//Information from DHT Sensor
struct sensor
{
  int t; // Temperature in Celsius
  float f; // Temperature in Fahrenheit
  int h; // Humidity
  float hi; // Heat Index
};
//WiFi credentials
char ssid[] = "XXXXXXXXXXXXX";    // your network SSID (name)
char pass[] = "XXXXXXXXXXXXX";        // your network password

//Azure Hub & Device credentials
char IotHubname[] = "XXXXXXXXXXXXXXXXX";
char DeviceId[] = "XXXXXXXXXXXXXX";
char DevicePrimaryKey[] = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"; 


ATCmdParser m_parser = ATCmdParser(&Serial1);
DHT dht(DHTPIN, DHTTYPE);

sensor DhtSensor;

void setup() {

  initModule();
  
  m_parser.set_timeout(1000);
  m_parser.set_delimiter("\r\n");
  
  PrepareModule();
  
  WifiConnect(ssid, pass);
    
  dht.begin();
  
  SetAzureConf(IotHubname,DeviceId,DevicePrimaryKey);
  
  SetMqttTopic(DeviceId);

  ConnectToAzure();
  
}

void loop() {
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  DhtSensor.h = dht.readHumidity();
  // Read temperature as Celsius
  DhtSensor.t = dht.readTemperature();
  // Read temperature as Fahrenheit
  DhtSensor.f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(DhtSensor.h) || isnan(DhtSensor.t) || isnan(DhtSensor.f)) {
    Serial.println("Failed to read from DHT sensor! switching to simulation..");
    DhtSensor.h = random(10,30);
    DhtSensor.t = random(10,30);
    DhtSensor.f = random(10,30);
    MqttPub(DeviceId,&DhtSensor);
    return;
  }

  // Compute heat index
  // Must send in temp in Fahrenheit!
  DhtSensor.hi = dht.computeHeatIndex(DhtSensor.f,DhtSensor.h);

   MqttPub(DeviceId,&DhtSensor);   
}

void initModule(){
    
  // initialize serial for debugging
  Serial.begin(115200);
  // initialize serial for ESP module
  Serial1.begin(115200);
  // initialize ESP module
  Serial.println("Initializing module...");
  
  if (m_parser.send(F("AT")) && m_parser.recv(F(RESP_OK)))
    {
      Serial.println("Module is available");
      return RET_OK;
    }
}

void WifiConnect(char *_ssid, char *_pass){
    Serial.println("Connecting to Wi-Fi...");
    if (m_parser.send(("AT+CWJAP_CUR=\"%s\",\"%s\""),_ssid,_pass) )
    {
      while(1){
        if (m_parser.recv("WIFI CONNECTED") && m_parser.recv("WIFI GOT IP") && m_parser.recv(F(RESP_OK))){
          Serial.println("Connected to WiFi");
          break;  
        }      
      }
    }
}

int8_t PrepareModule()
{
    bool echo = false, Mode = false, Dhcp = false, MqttDis = false;
    if(m_parser.send(F("ATE0")) && m_parser.recv(F(RESP_OK))){
      echo = true;
    }
    if(m_parser.send(F("AT+CWMODE_CUR=1")) && m_parser.recv(F(RESP_OK))){
      Mode = true;
    }
    if(m_parser.send(F("AT+CWDHCP_CUR=1,1")) && m_parser.recv(F(RESP_OK))){
      Dhcp = true;
    }
    if(m_parser.send(F("AT+MQTTDIS")) && m_parser.recv(F(RESP_OK))){
      MqttDis = true;
    }  
    if (echo&&Mode&&Dhcp&&MqttDis) {
      return RET_OK;
    } else {
      return RET_NOK;
    }
}

int8_t SetAzureConf(char *_IoTHub, char *_DeviceId, char *_SasToken)
{
    bool ConfRes = false;
     char buf[100];
     if(m_parser.send("AT+AZSET=\"%s\",\"%s\",\"%s\"",_IoTHub,_DeviceId,_SasToken) && m_parser.recv(F(RESP_OK))){
      ConfRes = true;
      Serial.println("Azure setting successfully");
    }
    if (ConfRes) return RET_OK;
}

int8_t SetMqttTopic(char *_DeviceId)
{
    bool SetMT = false;
    char buf[100];
    if(m_parser.send("AT+MQTTTOPIC=\"devices/%s/messages/events/\",\"devices/%s/messages/devicebound/#\"",_DeviceId,_DeviceId) && m_parser.recv(F(RESP_OK))){
      SetMT = true;
      Serial.println("SetTopic successfully");
    }
    if (SetMT) return RET_OK;
}

int8_t ConnectToAzure()
{
     char resp_str[100];
     char * search_pt;
     uint8_t i = 0;
     bool AzCon = false;
     
     m_parser.send(F("AT+AZCON"));
     while (1){
        if (m_parser.recv("CONNECT") && m_parser.recv(RESP_OK)) {
          AzCon = true;
          Serial.println("AzureCon success");
          break;
        }
     }
     if (AzCon) return RET_OK;
}

int8_t MqttPub(char *_DeviceId, sensor *_sensor_info)
{
     bool done = false;
     char buf[40];
     if(m_parser.send("AT+MQTTPUB=\"{\"deviceId\":\"%s\",\"temperature\":%d, \"humidity\":%d}",_DeviceId,_sensor_info->t,_sensor_info->h) && m_parser.recv(F(RESP_OK))){
        done = true;
        sprintf((char *)buf, "Temperature: %d    Humidity: %d", _sensor_info->t,_sensor_info->h);
        Serial.println(buf);
      } 
      if (done) return RET_OK;
      
}

