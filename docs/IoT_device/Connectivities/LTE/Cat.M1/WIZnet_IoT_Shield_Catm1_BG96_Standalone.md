# WIZnet IoT Shield(WIoT-QC01)과 PC를 이용하여 Azure IoT Hub에 연결



## 목차

- [시작하기 전에](#Prerequisites)
- [소개](#Introduction)
- [Step 1: 디바이스 준비](#Prepare_Device)
- [Step 2: AT 명령어](#Step-2-ATCommand)




<a name="Prerequisites"></a>
## 시작하기 전에
>[**Azure Portal**][Link-Azure-Portal]에 Login 을 합니다. 계정이 없는 경우, 계정 생성 후에 Login을 진행합니다.

>Azure Portal을 사용하여 IoT Hub 만들기 등 앞선 일련의 과정에 대한 내용은 [**Azure Cloud 소개**][Link-Azure_Cloud_Introduction]를 참조하시기 바랍니다. 


<a name="Introduction"></a>
## 소개

**Microsoft Azure Service**에 **SKT LTE Cat.M1**을 **연동**하여, Data를 Cloud로 전송하고, Monitoring을 할 수 있습니다.

Data 통신은 다음과 같은 구조로 이루어집니다.

![][Link-Data_Communication_Structure] 

**MQTT AT Command**를 이용하여, IoT Hub Service 연결 및 Data 송신을 합니다.

IoT Hub로 송신이 된 Data는 Stream Analytics를 통하여 Data 저장소 Blob Storage로 저장이 됩니다.

본 문서는 SKT LTE Cat.M1(Quectel BG96) MQTT AT Command 이용하여 Microsoft Azure Service 연결 방법에 대한 Guide를 제공합니다.

PS. MQTT AT Command를 지원하지 않는 경우, TCP/IP 통신을 이용하여 MQTT 메시지를 구현하시면 됩니다. 



<a name="Prepare_Device"></a>
## Step 1: 디바이스 준비

### 1. 하드웨어 준비

- [**WIZnet IoT Shield**]("https://github.com/Wiznet/wiznet-iot-shield-kr")
- Cat.M1 Module [**WIoT-QC01,Quectel BG96**]("https://github.com/Wiznet/wiznet-iot-shield-kr/blob/master/docs/imgs/hw/wiot-qc01_top.png")
- **Micro USB Cable**
- **PC**
- **WIZnet IoT Shield Jumper 설정 (StandAlone Mode)**


![Jumper Setting](https://github.com/Wiznet/azure-iot-kr/blob/master/images/Jumper_Setting_StandAlone.png?raw=true) 

### 2. 소프트웨어 준비

- **시리얼 터미널 프로그램** (Hercules, Teraterm, TokentoShell etc)
- [**CP210x USB to UART Bridge VCP Drivers**]("https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers") (PC와 WIZnet IoT Shield 연결 후 장치 관리자에서 COM Port 확인 불가인 경우 설치)


### 3. 환경 설정


![Test Environment Setting](https://github.com/Wiznet/azure-iot-kr/blob/master/images/Test_Environment_StandAlone.png?raw=true)  


<a name="Step-2-ATCommand"></a>
## Step 2: AT 명령어


### 1.MQTT 소켓 오픈

**AT+QMTOPEN** -> **AT+QMTCONN**


**AT Command:** AT+QMTOPEN

**Syntax:**

| Type | Syntax | Response | Example |
|:--------|:--------|:--------|:--------|
| Test | AT+QMTOPEN=? | +QMTOPEN: (0 ~ 5),"host_name",(1 ~ 65536)<br><br>OK | - |
| Read | AT+QMTOPEN? | +QMTOPEN: (tcpconnectID),"(host_name)",(port)<br>OK | AT+QMTOPEN?<br>+QMTOPEN: 0,"iot_hub_host_name",8883<br>OK |
| Write | AT+QMTOPEN=(tcpconnectID),(host_name),(port) | OK<br>+QMTOPEN: (tcpconnectID),(result)<br>+CME ERROR: <err> | AT+QMTOPEN=0,"MyCatm1IoTHub.azure-devices.net",8883<br>OK<br>+QMTOPEN: 0,0 |


**Defined values:**

| Parameter |  Description |
|:--------|:--------|
| (tcpconnectID) | MQTT socket identifier. The range is 0-5. |
| (host_name) | The address of the server. It could be an IP address or a domain name. |
| (port) | The port of the server. The range is 1-65535. |
| (result) | Result of the command execution |


### 2.MQTT 연결

**AT Command:** AT+QMTCONN

**Syntax:**

| Type | Syntax | Response | Example
|:--------|:--------|:--------|:--------|
| Test | AT+QMTCONN=? | +QMTCONN: (list of supported (tcpconnectID)s),"(clientID)"[,"(username)"[,"(password)"]] | - |
| Read | AT+QMTCONN? | [+QMTCONN: (tcpconnectID),(state)]<br>OK | - |
| Write | AT+QMTCONN=(tcpconnectID),"(clientID)"[,"(username)"[,"(password)"]] | OK<br>+QMTCONN: (tcpconnectID),(result)[,(ret_code)]<br>If there is an error related to ME functionality, response:<br>+CME ERROR: (err)|  AT+QMTCONN=0,"iot_hub_host_name/device_id/?api-version=2018-06-30","MyCatm1IoTDevice","SAS Token"<br>OK<br>+QMTCONN: 0,0,0 |

> **SAS Token 생성**은 다음을 참고 바랍니다.
>
> * [Device Explorer를 사용하여 SAS Token 생성하기][Link-Create_Sas_Token_Through_Device_Explorer]
>
> * [Azure IoT Explorer를 사용하여 SAS Token 생성하기][Link-Create_Sas_Token_Through_Azure_Iot_Explorer]

**Defined values:**

| Parameter |  Description |
|:--------|:--------|
| (tcpconnectID) | MQTT socket identifier. The range is 0-5. |
| (clientID) | The client identifier string. |
| (username) | User name of the client. It can be used for authentication |
| (password) | Password corresponding to the user name of the client. It can be used for authentication |


### 3.MQTT 데이터 Publish 

AT+QMTPUB 명령은 MQTT Broker에게 데이터를 전송할 때 사용됩니다. 토픽 이름으로 구분되어지는 메시지를 전송하며, 해당 토픽을 Subscribe하는 MQTT 클라이언트에게 이를 전송합니다.

**AT Command:** AT+QMTPUB

**Syntax:**

| Type | Syntax | Response | Example
|:--------|:--------|:--------|:--------|
| Test | AT+QMTPUB=? | +QMTPUB: (0-5), (msgid),(0-2),(0,1),"topic",(1-1548)<br>OK | -|
| Write | AT+QMTPUB=(tcpconnectID),(msgID),(qos),(retain),"(topic)"<br> 명령어 이후에 전송할 메시지 입력이 끝나면 Ctrl + Z를 입력합니다.| OK<br>+QMTPUB: (tcpconnectID),(msgID),(result),(value)<br>+CME ERROR: (err) | AT+QMTPUB=0,0,0,0,""<br> > {"deviceId":"MyCatm1IoTDevice","temperature":21.97,"humidity":43.58}<br>OK |



**Defined values:**

| Parameter | Description |
|:--------|:--------|
| (tcpconnectID) | MQTT socket identifier. |
| (msgID) | Message identifier of packet. | 
| (qos) | The QoS level at which the client wants to publish the messages.| 
| (retain) | Whether or not the server will retain the message after it has been delivered to the current subscribers.| 
| (topic) | Topic that needs to be published| 
| (msg) | Message to be published| 
| (result) | Result of the command execution| 
| (value) | If (result) is 1, it means the times of packet retransmission. If (result) is 0 or 2, it will not be presented.| 
| (pkt_timeout) | Timeout of the packet delivery. The range is 1-60. The default value is 5. | 
| (retry_times) | Retry times when packet delivery times out. The range is 0-10. The default valueis 3.| 


[Link-Data_Communication_Structure]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/Azure%20connection%20Flow.png?raw=true
[Link-Azure-Portal]: https://portal.azure.com/
[Link-Create_Sas_Token_Through_Device_Explorer]: https://github.com/Wiznet/azure-iot-kr/tree/master/docs/Azure_Cloud/create_sas_token_through_device_explorer.md
[Link-Create_Sas_Token_Through_Azure_Iot_Explorer]: https://github.com/Wiznet/azure-iot-kr/tree/master/docs/Azure_Cloud/create_sas_token_through_azure_iot_explorer.md
