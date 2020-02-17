// ----------------------------------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------------------------------
#include <at_cmd_parser.h>
#include <dht.h>
#include <simple_timer.h>

// ----------------------------------------------------------------------------------------------------
// Define
// ----------------------------------------------------------------------------------------------------
/* Default setting */
#define ON                              1
#define OFF                             0
#define ENABLE                          1
#define DISABLE                         0       
#define RET_OK                          1
#define RET_NOK                         -1

/* Pin configuraiton */
#define WIZFI360_TX                     8
#define WIZFI360_RX                     2
#define WIZFI360_RESET                  9
#define WIZFI360_SENSOR_CDS             A0
#define WIZFI360_SENSOR_DHT             14

/* Serial setting */
#define WIZFI360_DEFAULT_BAUD_RATE      115200

/* Parser setting */
#define WIZFI360_PARSER_DELIMITER       "\r\n"

/* Debug message setting */
#define DEBUG_ENABLE                    1
#define DEBUG_DISABLE                   0

#define WIZFI360_PARSER_DEBUG           DEBUG_DISABLE

/* Timeout setting */
#define WIZFI360_DEFAULT_TIMEOUT        1000

/* Timer setting */
#define WIZFI360_DEFAULT_TICK_PERIOD    1000

/* Sensor */
#define DHT_TYPE                        DHT11

/* Wi-Fi */
#define STATION                         1
#define SOFTAP                          2
#define STATION_SOFTAP                  3

#define WIFI_MODE                       STATION

/* MQTT */
#define MQTT_PORT                       8883

// ----------------------------------------------------------------------------------------------------
// Variables
// ----------------------------------------------------------------------------------------------------
ATCmdParser _parser = ATCmdParser(&Serial1);
SimpleTimer timer;
DHT _dht(WIZFI360_SENSOR_DHT, DHT_TYPE);

int time_cnt;

/* Sensor info */
struct sensor
{
    int ill;    // illuminance
    int cel;    // celsius
    float fah;  // fahrenheit
    int hum;    // humidity
};

sensor sensor_info;

/* Wi-Fi info */
char ssid[] = "wiznet";
char password[] = "0123456789";

/* MQTT info */
int alive_time = 60;    // range : 30 ~ 300

/* Azure info */
char hub_name[] = "MyWizFi360IoTHub";
char host_name[] = "MyWizFi360IoTHub.azure-devices.net";
char device_id[] = "MyWizFi360IoTDevice";
char sas_token[] = "SharedAccessSignature sr=MyWizFi360IoTHub.azure-devices.net%2Fdevices%2FMyWizFi360IoTDevice&sig=t3R9nDS7ezMGBdb%2FNd5ktb3xQx5jx4NC02n325vRA6c%3D&se=1611895717";

// ----------------------------------------------------------------------------------------------------
// Functions
// ----------------------------------------------------------------------------------------------------
/* Serial */
void serialPcInit(void);
void serialDeviceInit(void);
void serialAtParserInit(const char* delimiter, bool debug_en);

/* Device */
void deviceInit_WizFi360(void);
void deviceReset_WizFi360(void);
void deviceIsReady_WizFi360(void);

/* Timer */
void deviceSetTimer_WizFi360(void);
void deviceStartTimer_WizFi360(void);

/* Sensor */
int deviceGetIllVal_WizFi360(void);
void deviceSetDht_WizFi360(void);

/* Wi-Fi */
int8_t deviceSetWifiMode_WizFi360(int mode);
int8_t deviceSetDhcp_WizFi360(int mode, int en_dhcp);
int8_t deviceConnAP_WizFi360(int mode, char* ssid, char* password);

/* MQTT */
int8_t deviceSetMqttConnConf_WizFi360(char* host_name, char* device_id, char* sas_token, int alive_time);
int8_t deviceSetTopic_WizFi360(char* device_id);
int8_t deviceConnBroker_WizFi360(int en_auth, char* broker_ip, int broker_port);
int8_t devicePublishMsg_WizFi360(sensor* sensor_info);

void setup() 
{
    /* Set serial */
    serialPcInit();

    /* Set device */
    deviceInit_WizFi360();
    deviceReset_WizFi360();
    deviceIsReady_WizFi360();

    /* Set timer */
    deviceSetTimer_WizFi360();

    /* Set sensor */
    deviceSetDht_WizFi360();

    /* Set Wi-Fi */
    deviceSetWifiMode_WizFi360(WIFI_MODE);
    deviceSetDhcp_WizFi360(WIFI_MODE, ENABLE);
    deviceConnAP_WizFi360(WIFI_MODE, ssid, password);

    /* Set MQTT */
    deviceSetMqttConnConf_WizFi360(host_name, device_id, sas_token, alive_time);
    deviceSetTopic_WizFi360(device_id);
    deviceConnBroker_WizFi360(ENABLE, host_name, MQTT_PORT);
}

void loop() 
{
    /* Get sensor info */
    sensor_info.ill = deviceGetIllVal_WizFi360();
    sensor_info.cel = _dht.readTemperature();
    sensor_info.fah = _dht.readTemperature(true);
    sensor_info.hum = _dht.readHumidity();

    /* Publish message */
    devicePublishMsg_WizFi360(&sensor_info);

    delay(3000);
}

// ----------------------------------------------------------------------------------------------------
// Functions : Serial
// ----------------------------------------------------------------------------------------------------
void serialPcInit(void)
{
    Serial.begin(115200);
}

void serialDeviceInit(void)
{
    Serial1.begin(WIZFI360_DEFAULT_BAUD_RATE);
}

void serialAtParserInit(const char* delimiter, bool debug_en)
{
    _parser.debug_on(debug_en);
    _parser.set_delimiter(delimiter);
    _parser.set_timeout(WIZFI360_DEFAULT_TIMEOUT);
}

// ----------------------------------------------------------------------------------------------------
// Functions : Device
// ----------------------------------------------------------------------------------------------------
void deviceInit_WizFi360(void)
{
    serialDeviceInit();
    serialAtParserInit(WIZFI360_PARSER_DELIMITER, WIZFI360_PARSER_DEBUG);
}

void deviceReset_WizFi360(void)
{
    pinMode(WIZFI360_RESET, OUTPUT);

    digitalWrite(WIZFI360_RESET, HIGH);
    delay(300);

    digitalWrite(WIZFI360_RESET, LOW);
    delay(400);

    digitalWrite(WIZFI360_RESET, HIGH);
    delay(1000);
}

void deviceIsReady_WizFi360(void)
{
    while (1)
    {
        if (_parser.recv("ready"))
        {
            Serial.println("WizFi360 is available");

            return;
        }
        else if (_parser.send("AT") && _parser.recv("OK"))
        {
            Serial.println("WizFi360 is already available");

            return;
        }
        else
        {
            Serial.println("WizFi360 is not available");

            return;
        }
    }
}

// ----------------------------------------------------------------------------------------------------
// Functions : Timer
// ----------------------------------------------------------------------------------------------------
void deviceSetTimer_WizFi360(void)
{
    timer.setInterval(WIZFI360_DEFAULT_TICK_PERIOD, deviceStartTimer_WizFi360);
}

void deviceStartTimer_WizFi360(void)
{
    time_cnt++;
}

// ----------------------------------------------------------------------------------------------------
// Functions : Sensor
// ----------------------------------------------------------------------------------------------------
/* Illuminance */
int deviceGetIllVal_WizFi360(void)
{
    int val = analogRead(WIZFI360_SENSOR_CDS);

    return val;
}

/* DHT */
void deviceSetDht_WizFi360(void)
{
    _dht.begin();
}

// ----------------------------------------------------------------------------------------------------
// Functions : Wi-Fi
// ----------------------------------------------------------------------------------------------------
int8_t deviceSetWifiMode_WizFi360(int mode)
{
    int8_t ret = RET_NOK;
    char buf[64];

    if (_parser.send("AT+CWMODE_CUR=%d", mode) && _parser.recv("OK"))
    {
        sprintf((char*)buf, "Set Wi-Fi mode : %d", mode);
        Serial.println(buf);

        ret = RET_OK;
    }
    else
    {
        Serial.println("Set Wi-Fi mode : ERROR");
    }

    return ret;
}

int8_t deviceSetDhcp_WizFi360(int mode, int en_dhcp)
{
    int8_t ret = RET_NOK;
    char buf[32];
    int mode_dhcp;

    if (mode == STATION)
    {
        mode_dhcp = 1;
    }
    else if (mode == SOFTAP)
    {
        mode_dhcp = 0;
    }
    else if (mode == STATION_SOFTAP)
    {
        mode_dhcp = 2;
    }
    else
    {
        Serial.println("Set DHCP : ERROR");
    }

    if (_parser.send("AT+CWDHCP_CUR=%d,%d", mode_dhcp, en_dhcp) && _parser.recv("OK"))
    {
        sprintf((char *)buf, "Set DHCP : %d, %d", mode_dhcp, en_dhcp);
        Serial.println(buf);

        ret = RET_OK;
    }
    else
    {
        Serial.println("Set DHCP : ERROR");
    }

    return ret;
}

int8_t deviceConnAP_WizFi360(int mode, char* ssid, char* password)
{
    int8_t ret = RET_NOK;
    bool done = false;
    char buf[128];

    if ((mode == STATION) || (mode == STATION_SOFTAP))
    {
        _parser.send("AT+CWJAP_CUR=\"%s\",\"%s\"", ssid, password);

        time_cnt = 0;

        while (done != true)
        {
            timer.run();

            done = (_parser.recv("WIFI CONNECTED") && _parser.recv("WIFI GOT IP") && _parser.recv("OK"));

            if (time_cnt >= 20) // 20s
            {
                break;
            }
        }

        if (done)
        {
            sprintf((char *)buf, "Connect AP : %s, %s", ssid, password);
            Serial.println(buf);

            ret = RET_OK;
        }
        else
        {
            Serial.println("Connect AP : ERROR");
        }
    }
    else
    {
        Serial.println("Connect AP : ERROR");
    }

    return ret;
}

// ----------------------------------------------------------------------------------------------------
// Functions : MQTT
// ----------------------------------------------------------------------------------------------------
int8_t deviceSetMqttConnConf_WizFi360(char* host_name, char* device_id, char* sas_token, int alive_time)
{
    int8_t ret = RET_NOK;
    char buf[256];

    if (_parser.send("AT+MQTTSET=\"%s/%s/?api-version=2018-06-30\",\"%s\",\"%s\",%d", host_name, device_id, sas_token, device_id, alive_time) && _parser.recv("OK"))
    {
        sprintf((char *)buf, "Set MQTT connect configuration : %s, %s, %s, %d", host_name, device_id, sas_token, alive_time);
        Serial.println(buf);

        ret = RET_OK;
    }
    else
    {
        Serial.println("Set MQTT connect configuration : ERROR");
    }

    return ret;
}

int8_t deviceSetTopic_WizFi360(char* device_id)
{
    int8_t ret = RET_NOK;
    char buf[64];

    if (_parser.send("AT+MQTTTOPIC=\"devices/%s/messages/events/\",\"devices/%s/messages/devicebound/#\"", device_id, device_id) && _parser.recv("OK"))
    {
        sprintf((char *)buf, "Set topic : %s", device_id);
        Serial.println(buf);

        ret = RET_OK;
    }
    else
    {
        Serial.println("Set topic : ERROR");
    }

    return ret;
}

int8_t deviceConnBroker_WizFi360(int en_auth, char* broker_ip, int broker_port)
{
    int8_t ret = RET_NOK;
    bool done = false;
    char buf[128];
    
    _parser.send("AT+MQTTCON=%d,\"%s\",%d", en_auth, broker_ip, broker_port);

    time_cnt = 0;

    while (done != true)
    {
        timer.run();

        done = (_parser.recv("CONNECT") && _parser.recv("OK"));

        if (time_cnt >= 3)  // 3s
        {
            break;
        }
    }

    if (done)
    {
        sprintf((char *)buf, "Connect broker : %d, %s, %d", en_auth, broker_ip, broker_port);
        Serial.println(buf);

        ret = RET_OK;
    }
    else
    {
        Serial.println("Connect broker : ERROR");
    }

    return ret;
}

int8_t devicePublishMsg_WizFi360(sensor* sensor_info)
{
    int8_t ret = RET_NOK;
    char buf[64];

    if (_parser.send("AT+MQTTPUB=\"{\"deviceId\":\"MyWizFi360IoTDevice\",\"temperature\":%d,\"humidity\":%d}\"", sensor_info->cel, sensor_info->hum))
    {
        sprintf((char *)buf, "Publish message : %d, %d", sensor_info->cel, sensor_info->hum);
        Serial.println(buf);
    }

    return ret;
}
