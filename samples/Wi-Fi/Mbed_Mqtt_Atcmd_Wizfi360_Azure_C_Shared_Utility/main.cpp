/* Mbed Example Program
 * Copyright (c) 2020 WIZnet Co.,Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// ----------------------------------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------------------------------
#include "mbed.h"

#include "Dht11.h"
#include "sastoken.h"
#include "crt_abstractions.h"

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
#define MBED_CONF_WIZFI360_TX           D8
#define MBED_CONF_WIZFI360_RX           D2
#define MBED_CONF_WIZFI360_RESET        D9
#define MBED_CONF_WIZFI360_SENSOR_CDS   A0
#define MBED_CONF_WIZFI360_SENSOR_DHT   D14

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

/* Time setting */
#define NTP_BEGIN_TIME_YEAR             1970
#define NTP_BEGIN_TIME_MONTH            1
#define NTP_BEGIN_TIME_DAY              1

/* Wi-Fi */
#define STATION                         1
#define SOFTAP                          2
#define STATION_SOFTAP                  3

#define WIFI_MODE                       STATION

/* MQTT */
#define MQTT_PORT                       8883

/* Azure */
#define AZURE_SCOPE                     "%s.azure-devices.net/devices/%s"

// ----------------------------------------------------------------------------------------------------
// Variables
// ----------------------------------------------------------------------------------------------------
Serial pc(USBTX, USBRX);

UARTSerial *_serial;
ATCmdParser *_parser;

DigitalOut _RESET_WIZFI360(MBED_CONF_WIZFI360_RESET);
AnalogIn _cdsVal(MBED_CONF_WIZFI360_SENSOR_CDS);
Dht11 _dhtVal(MBED_CONF_WIZFI360_SENSOR_DHT);

/* Time info  */
struct ntp_time
{
    int year;
    char month[3];
    int day;
    int hour;
    int minute;
    int second;
    char day_of_the_week[3];
};

int ntp_diff_day;
int ntp_raw_time;   // second

/* Sensor info */
struct sensor
{
    int ill;    // illuminance
    int cel;    // celsius
    float fah;  // fahrenheit
    int hum;    // humidity
};

/* Wi-Fi info */
char ssid[] = "Austin_2.4G";
char password[] = "01029474191";

/* MQTT info */
int alive_time = 60;    // range : 30 ~ 300

/* Azure info */
char hub_name[] = "MyWizFi360IoTHub";
char host_name[] = "MyWizFi360IoTHub.azure-devices.net";
char device_id[] = "MyWizFi360IoTDevice";
char device_key[] = "iUiK7N/mwgeVChq4aParKLxvrEL6Wgi2rJqOfJKzZb8=";
char sas_token[256];
int expiry_time = 60 * 60 * 24 * 365;

// ----------------------------------------------------------------------------------------------------
// Functions
// ----------------------------------------------------------------------------------------------------
/* Util */
void delay(int ms);
int convertDay(int year, int month, int day);
int differenceDay(int cur_year, int cur_month, int cur_day, int pre_year, int pre_month, int pre_day);
int convertMonth(char *month);
int calcNtpRawTime(int day);
int8_t deviceSetNtpTime_WizFi360(int en_sntp, int timezone);
int8_t deviceGetNtpTime_WizFi360(ntp_time *ntp_time_info);

/* Serial */
void serialPcInit(void);
void serialDeviceInit(PinName tx, PinName rx, int baudrate);
void serialAtParserInit(const char *delimiter, bool debug_en);

/* Device */
void deviceInit_WizFi360(void);
void deviceReset_WizFi360(void);
void deviceIsReady_WizFi360(void);

/* Sensor */
int deviceGetIllVal_WizFi360(void);
int deviceGetCelVal_WizFi360(void);
float deviceGetFahVal_WizFi360(void);
int deviceGetHumVal_WizFi360(void);

/* Wi-Fi */
int8_t deviceSetWifiMode_WizFi360(int mode);
int8_t deviceSetDhcp_WizFi360(int mode, int en_dhcp);
int8_t deviceConnAP_WizFi360(int mode, char *ssid, char *password);

/* MQTT */
int8_t deviceSetMqttConnConf_WizFi360(char *host_name, char *device_id, char *sas_token, int alive_time);
int8_t deviceSetTopic_WizFi360(char *device_id);
int8_t deviceConnBroker_WizFi360(int en_auth, char *broker_ip, int broker_port);
int8_t devicePublishMsg_WizFi360(sensor *sensor_info);

/* Azure */
int8_t deviceCreateSasToken_WizFi360(char *hub_name, char *device_id, char *device_key, int expiry_time, int raw_time);

// ----------------------------------------------------------------------------------------------------
// Main
// ----------------------------------------------------------------------------------------------------
int main()
{
    ntp_time ntp_time_info;
    sensor sensor_info;

    /* Set serial */
    serialPcInit();

    /* Set device */
    deviceInit_WizFi360();
    deviceReset_WizFi360();
    deviceIsReady_WizFi360();

    /* Set Wi-Fi */
    deviceSetWifiMode_WizFi360(WIFI_MODE);
    deviceSetDhcp_WizFi360(WIFI_MODE, ENABLE);
    deviceConnAP_WizFi360(WIFI_MODE, ssid, password);

    /* Set NTP time */
    deviceSetNtpTime_WizFi360(ENABLE, 8);
    deviceGetNtpTime_WizFi360(&ntp_time_info);
    ntp_diff_day = differenceDay(ntp_time_info.year, convertMonth(ntp_time_info.month), ntp_time_info.day, NTP_BEGIN_TIME_YEAR, NTP_BEGIN_TIME_MONTH, NTP_BEGIN_TIME_DAY);
    ntp_raw_time = calcNtpRawTime(ntp_diff_day);

    /* Create SAS token */
    deviceCreateSasToken_WizFi360(hub_name, device_id, device_key, expiry_time, ntp_raw_time);

    /* Set MQTT */
    deviceSetMqttConnConf_WizFi360(host_name, device_id, sas_token, alive_time);
    deviceSetTopic_WizFi360(device_id);
    deviceConnBroker_WizFi360(ENABLE, host_name, MQTT_PORT);

    while(true)
    {
        /* Get sensor info */
        sensor_info.ill = deviceGetIllVal_WizFi360();
        sensor_info.cel = deviceGetCelVal_WizFi360();
        sensor_info.fah = deviceGetFahVal_WizFi360();
        sensor_info.hum = deviceGetHumVal_WizFi360();

        /* Publish message */
        devicePublishMsg_WizFi360(&sensor_info);

        delay(3000);
    }
}

// ----------------------------------------------------------------------------------------------------
// Functions : Util
// ----------------------------------------------------------------------------------------------------
void delay(int ms)
{
    wait_us(ms * 1000);
} 

int convertDay(int year, int month, int day)
{
    int months[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int result;
    int i;

    result = (year - 1) * 365 + (year - 1) / 4 - (year - 1) / 100 + (year - 1) / 400;

    if ((!(year % 4) && (year % 100)) || !(year % 400))
    {
        months[1]++;
    }
        
    for (i = 0; i < month - 1; i++)
    {
        result += months[i];
    }
        
    result += day;

    return result;
}

int differenceDay(int cur_year, int cur_month, int cur_day, int pre_year, int pre_month, int pre_day)
{
    int result;

    result = convertDay(cur_year, cur_month, cur_day) - convertDay(pre_year, pre_month, pre_day);

    return result;
}

int convertMonth(char *month)
{
    int result;

    if(!(strcmp(month, "Jan")))
    {
        result = 1;
    }
    else if(!(strcmp(month, "Feb")))
    {
        result = 2;
    }
    else if(!(strcmp(month, "Mar")))
    {
        result = 3;
    }
    else if(!(strcmp(month, "Apr")))
    {
        result = 4;
    }
    else if(!(strcmp(month, "May")))
    {
        result = 5;
    }
    else if(!(strcmp(month, "Jun")))
    {
        result = 6;
    }
    else if(!(strcmp(month, "Jul")))
    {
        result = 7;
    }
    else if(!(strcmp(month, "Aug")))
    {
        result = 8;
    }
    else if(!(strcmp(month, "Sep")))
    {
        result = 9;
    }
    else if(!(strcmp(month, "Oct")))
    {
        result = 10;
    }
    else if(!(strcmp(month, "Nov")))
    {
        result = 11;
    }
    else if(!(strcmp(month, "Dec")))
    {
        result = 12;
    }

    return result;
}

int calcNtpRawTime(int day)
{
    int result;

    result = day * 60 * 60 * 24;

    return result;
}

int8_t deviceSetNtpTime_WizFi360(int en_sntp, int timezone)
{
    int8_t ret = RET_NOK;

    if(_parser->send("AT+CIPSNTPCFG=%d,%d", en_sntp, timezone) && _parser->recv("OK"))
    {
        ret = RET_OK;
    }

    return ret;    
}

int8_t deviceGetNtpTime_WizFi360(ntp_time *ntp_time_info)
{
    int8_t ret = RET_NOK;

    if(_parser->send("AT+CIPSNTPTIME?") && 
        _parser->recv("+CIPSNTPTIME:%s %s %d %d:%d:%d %d\r\n^\"]", 
            ntp_time_info->day_of_the_week, 
            ntp_time_info->month, 
            &ntp_time_info->day, 
            &ntp_time_info->hour, 
            &ntp_time_info->minute, 
            &ntp_time_info->second, 
            &ntp_time_info->year))
    {
        ret = RET_OK;
    }
    
    return ret;    
}

// ----------------------------------------------------------------------------------------------------
// Functions : Serial
// ----------------------------------------------------------------------------------------------------
void serialPcInit(void)
{
    pc.baud(115200);
    pc.format(8, Serial::None, 1);
}

void serialDeviceInit(PinName tx, PinName rx, int baudrate) 
{        
    _serial = new UARTSerial(tx, rx, baudrate);    
}

void serialAtParserInit(const char *delimiter, bool debug_en)
{
    _parser = new ATCmdParser(_serial);    
    _parser->debug_on(debug_en);
    _parser->set_delimiter(delimiter);    
    _parser->set_timeout(WIZFI360_DEFAULT_TIMEOUT);
}

// ----------------------------------------------------------------------------------------------------
// Functions : Device
// ----------------------------------------------------------------------------------------------------
void deviceInit_WizFi360(void)
{
    serialDeviceInit(MBED_CONF_WIZFI360_TX, MBED_CONF_WIZFI360_RX, WIZFI360_DEFAULT_BAUD_RATE);          
    serialAtParserInit(WIZFI360_PARSER_DELIMITER, WIZFI360_PARSER_DEBUG);
}

void deviceReset_WizFi360(void)
{
    _RESET_WIZFI360 = 1;
    delay(300);
    
    _RESET_WIZFI360 = 0;
    delay(400);
    
    _RESET_WIZFI360 = 1;    
    delay(1000);
}

void deviceIsReady_WizFi360(void)
{
    while(1) 
    {   
        if(_parser->recv("ready")) 
        {
            pc.printf("WizFi360 is available\r\n");

            return;
        }
        else if(_parser->send("AT") && _parser->recv("OK"))
        {
            pc.printf("WizFi360 is already available\r\n");

            return;
        }
        else
        {
            pc.printf("WizFi360 is not available\r\n");

            return; 
        }        
    }        
}

// ----------------------------------------------------------------------------------------------------
// Functions : Sensor
// ----------------------------------------------------------------------------------------------------
/* Illuminance */
int deviceGetIllVal_WizFi360(void)
{
    int val = _cdsVal.read_u16() / 100;

    return val;
}

/* Celsius */
int deviceGetCelVal_WizFi360(void)
{
    _dhtVal.read();

    int val = _dhtVal.getCelsius();
    
    return val;
}

/* Fahrenheit */
float deviceGetFahVal_WizFi360(void)
{
    _dhtVal.read();

    int val = _dhtVal.getFahrenheit();
    
    return val;
}

/* Humidity */
int deviceGetHumVal_WizFi360(void)
{
    _dhtVal.read();

    int val = _dhtVal.getHumidity();
    
    return val;
}

// ----------------------------------------------------------------------------------------------------
// Functions : Wi-Fi
// ----------------------------------------------------------------------------------------------------
int8_t deviceSetWifiMode_WizFi360(int mode)
{
    int8_t ret = RET_NOK;

    if(_parser->send("AT+CWMODE_CUR=%d", mode) && _parser->recv("OK"))
    {
        pc.printf("Set Wi-Fi mode : %d\r\n", mode);

        ret = RET_OK;
    }
    else
    {
        pc.printf("Set Wi-Fi mode : ERROR\r\n");
    }

    return ret;
}

int8_t deviceSetDhcp_WizFi360(int mode, int en_dhcp)
{
    int8_t ret = RET_NOK;
    int mode_dhcp;

    if(mode == STATION)
    {
        mode_dhcp = 1;
    }
    else if(mode == SOFTAP)
    {
        mode_dhcp = 0;
    }
    else if(mode == STATION_SOFTAP)
    {
        mode_dhcp = 2;
    }
    else
    {
        pc.printf("Set DHCP : ERROR\r\n");
    }

    if(_parser->send("AT+CWDHCP_CUR=%d,%d", mode_dhcp, en_dhcp) && _parser->recv("OK"))
    {
        pc.printf("Set DHCP : %d, %d\r\n", mode_dhcp, en_dhcp);

        ret = RET_OK;        
    }
    else
    {
        pc.printf("Set DHCP : ERROR\r\n");
    }

    return ret;  
}

int8_t deviceConnAP_WizFi360(int mode, char *ssid, char *password)
{
    int8_t ret = RET_NOK;
    bool done = false;
    Timer t;

    if((mode == STATION) || (mode == STATION_SOFTAP))
    {
        _parser->send("AT+CWJAP_CUR=\"%s\",\"%s\"", ssid ,password);

        t.start();

        while(done != true)
        {
            done = (_parser->recv("WIFI CONNECTED") && _parser->recv("WIFI GOT IP") &&  _parser->recv("OK"));

            if(t.read_ms() >= (WIZFI360_DEFAULT_TIMEOUT * 20))
            {
                t.stop();
                t.reset();

                break;
            }                
        }

        if(done)   
        {
            pc.printf("Connect AP : %s, %s\r\n", ssid, password);

            ret = RET_OK;
        }        
        else
        {
            pc.printf("Connect AP : ERROR\r\n");
        }         
    }
    else
    {
        pc.printf("Connect AP : ERROR\r\n");
    }
        
    return ret; 
}

// ----------------------------------------------------------------------------------------------------
// Functions : MQTT
// ----------------------------------------------------------------------------------------------------
int8_t deviceSetMqttConnConf_WizFi360(char *host_name, char *device_id, char *sas_token, int alive_time)
{
    int8_t ret = RET_NOK;

    if(_parser->send("AT+MQTTSET=\"%s/%s/?api-version=2018-06-30\",\"%s\",\"%s\",%d", host_name, device_id, sas_token, device_id, alive_time) && _parser->recv("OK"))
    {
        pc.printf("Set MQTT connect configuration : %s, %s, %s, %d\r\n", host_name, device_id, sas_token, alive_time);

        ret = RET_OK;
    }
    else
    {
        pc.printf("Set MQTT connect configuration : ERROR\r\n");
    }    

    return ret;
}

int8_t deviceSetTopic_WizFi360(char *device_id)
{
    int8_t ret = RET_NOK;

    if(_parser->send("AT+MQTTTOPIC=\"devices/%s/messages/events/\",\"devices/%s/messages/devicebound/#\"", device_id, device_id) && _parser->recv("OK"))
    {
        pc.printf("Set topic : %s\r\n", device_id);

        ret = RET_OK;
    }
    else
    {
        pc.printf("Set topic : ERROR\r\n");
    }    

    return ret;
}

int8_t deviceConnBroker_WizFi360(int en_auth, char *broker_ip, int broker_port)
{
    int8_t ret = RET_NOK;
    bool done = false;
    Timer t;

     _parser->send("AT+MQTTCON=%d,\"%s\",%d", en_auth, broker_ip, broker_port);

    t.start();
    
    while(done != true)
    {
        done = (_parser->recv("CONNECT") &&  _parser->recv("OK"));

        if(t.read_ms() >= (WIZFI360_DEFAULT_TIMEOUT * 3))
        {
            t.stop();
            t.reset();

            break;
        }                
    }

    if(done)   
    {
        pc.printf("Connect broker : %d, %s, %d\r\n", en_auth, broker_ip, broker_port);

        ret = RET_OK;
    }        
    else
    {
        pc.printf("Connect broker : ERROR\r\n");
    }     

    return ret;
}

int8_t devicePublishMsg_WizFi360(sensor *sensor_info)
{
    int8_t ret = RET_NOK;

    if(_parser->send("AT+MQTTPUB=\"{\"deviceId\":\"MyWizFi360IoTDevice\",\"temperature\":%d,\"humidity\":%d}\"", sensor_info->cel, sensor_info->hum))
    {
        pc.printf("Publish message : %d, %d\r\n", sensor_info->cel, sensor_info->hum);
    }
	
    return ret;
}

// ----------------------------------------------------------------------------------------------------
// Functions : Azure
// ----------------------------------------------------------------------------------------------------
int8_t deviceCreateSasToken_WizFi360(char *hub_name, char *device_id, char *device_key, int expiry_time, int raw_time)
{
    int8_t ret = RET_NOK;
    char azure_scope_tmp[128];
    char *strcpy_sas_token_tmp;
    STRING_HANDLE create_sas_token_tmp;

    memset(azure_scope_tmp, 0x00, sizeof(azure_scope_tmp));

    sprintf(azure_scope_tmp, AZURE_SCOPE, hub_name, device_id);
    create_sas_token_tmp = SASToken_CreateString(device_key, azure_scope_tmp, NULL, raw_time + expiry_time);
    
    if(create_sas_token_tmp == NULL)
    {
        STRING_delete(create_sas_token_tmp);

        pc.printf("Create SAS token : ERROR\r\n");

        return ret;
    }

    if(mallocAndStrcpy_s(&strcpy_sas_token_tmp, STRING_c_str(create_sas_token_tmp)) != 0)
    {
        free(strcpy_sas_token_tmp);

        pc.printf("Create SAS token : ERROR\r\n");

        return ret;   
    }

    strcpy(sas_token, strcpy_sas_token_tmp);

    STRING_delete(create_sas_token_tmp);
    free(strcpy_sas_token_tmp);

    pc.printf("Create SAS token : %s\r\n", sas_token);

    return ret;
}