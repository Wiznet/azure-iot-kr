# NUCLEO-L476RG + WizFi360 Azure AT Command를 이용하여 Azure IoT Hub에 연결



## 목차
- [시작하기 전에](#What_To_Do)
- [소개](#Learning_Content)
- [디바이스 준비](#Device_Prep)
- [AT 명령어](#Required_Item)
- [동작 예제](#Example)
- [동작 예제 결과](#Step-5-Read_Data_From_IoT_Hub)
- [다음 단계](#Next)



<a name="What_To_Do"></a>
## 시작하기 전에

### Hardware Requirement
-   [NUCLEO-L476RG][Link-Nucleo-L476rg]
-   Desktop or laptop computer
-   USB 케이블
-   WizFi360-EVB-Shield

### Software Requirement

- 	MS Azure Account (Azure 구독이 아직 없는 경우 체험 무료[계정]을 만듭니다.)
-   Preferred Serial Terminal (TeraTerm, YAT, etc.)
-   [Azure IoT Explorer]
-   MBED Studio / MBED Online Compiler

<a name="Learning_Content"></a>
## 소개
Microsoft Azure 는 Microsoft 의 클라우드 컴퓨팅 서비스입니다.
Microsoft Azure 의 서비스에 [WizFi360] 을 연동하여 데이터를 클라우드로 전송하고, 모니터링 할 수 있습니다.

본 문서에서는 NUCLEO-L476G + WizFi360 이용하여 MS Azure Services에 연결 방법에 대한 가이드를 제공합니다.
이 프로세스는 다음 단계로 구성됩니다.
- Azure IoT Hub 준비
- IoT 디바이스 등록
- Azure IoT와 연결 및 데이터 통신

Azure IoT Hub 준비와 IoT 디바이스 등록 과정 대해 [Azure Cloud 소개] 참조하시기 바랍니다.

WiFi모듈 테스트를 위해 [WizFi360-EVB-Shield] Evaluation 보드를 사용되었습니다.

![](/images/mqtt_atcmd_wizfi360_required_item_1.png)

<a name="Device_Prep"></a>
## 디바이스 준비

### 하드웨어 설정

WizFi360-EVB-Shield는 NUCLEO-L476RG와 결합을 하여 사용되어 집니다. 따라서 WizFi360-EVB-Shield의 DIP Switch 및 UART Jumper Pin을 다음과 같이 설정이 필요합니다.

> * SW1 : Off
> * SW2 : Off
> * SW3 : On
> * D2 : UART Tx
> * D8 : UART Rx

![](/images/mqtt_atcmd_wizfi360_required_item_2.png)



### 디바이스 연결
Hardware 설정 후, Mini USB Cable을 이용하여 NUCLEO-L476RG를 Desktop 혹은 Laptop Computer와 연결을 합니다.

**장치 관리자**에서 NUCLEO-L476RG와 연결된 **COM Port**를 확인 할 수 있습니다.

![][Link-Device-Management]

> 장치 관리자에서 COM Port를 확인 할 수 없는 경우, 다음 Link에서 Driver를 Downlonad하여 설치하시기 바랍니다.
>
> * [ST-LINK, ST-LINK/V2, ST-LINK/V2-1 USB driver][Link-St_Link_St_Link_V2_St_Link_V2_1_Usb_Driver]

<a name="Required_Item"></a>
## AT 명령어


### 1. Set current WiFi mode (not saved in flash)
**AT Command:** AT+CWMODE_CUR
Syntax:

| Type | Command | Response |
|:--------|:--------|:--------|
| Query | AT+CWMODE_CUR? | +CWMODE:&lt;mode&gt; <br> OK |
| Set| AT+CWMODE_CUR=&lt;mode&gt; | OK |

Defined values:

| Mode | Value |
|:--------|:--------|
| 1 | Station mode|
| 2 | SoftAP mode (factory default) |
| 3 | Station+SoftAP mode|

### 2. Enable DHCP
**AT Command:** AT+CWDHCP_CUR
Syntax:

| Type | Command | Response |
|:--------|:--------|:--------|
| Query | AT+CWDHCP_CUR? | +CWDHCP_CUR:&lt;para&gt; <br> OK |
| Set | AT+CWMODE_CUR=&lt;para&gt;,&lt;en&gt; | OK |

Defined values:

| Parameter | Value |
|:--------|:--------|
| 0 |  SoftAP DHCP 와 Station DHCP 를 disable 한다.|
| 1 | SoftAP DHCP 는 enable 하고 Station DHCP 는 disable 한다. |
| 2 | 2: SoftAP DHCP 는 disable 하고 Station DHCP 는 enable 한다. |
| 3 | SoftAP DHCP 와 Station DHCP 를 enable 한다. (factory default)|

### 3. List available APs
**AT Command:** AT+CWLAP
Syntax:

| Type | Command | Response |
|:--------|:--------|:--------|
| Query | AT+CWLAP | +CWLAP:([&lt;ecn&gt;,&lt;ssid&gt;,&lt;rssi&gt;,&lt;mac&gt;,&lt;channel&gt;,&lt;wps&gt;]) |


Defined values:

| Parameter | Value |
|:--------|:--------|
| &lt;ecn&gt;| 0: Open <br> 1: WEP <br> 2: WPA_PSK<br>3: WPA2_PSK<br>4:WPA_WPA2_PSK |
| &lt;ssid&gt; | string parameter. AP의 ssid |
| &lt;rssi&gt; | signal strength |
| &lt;mac&gt; |  string parameter. AP의 mac|
| &lt;wps&gt; | 0: WPS는 disable된다 <br> 1: WPS는 enable된다 |

### 4. Connect to AP
**AT Command:** AT+CWJAP_CUR
Syntax:

| Type | Command | Response |
|:--------|:--------|:--------|
| Set | AT+CWJAP_CUR=&lt;ssid&gt;,&lt;pwd&gt;,[&lt;bssid&gt;] | +CWJAP_CUR:&lt;ssid&gt;,&lt;bssid&gt;,&lt;channel&gt;,&lt;rssi&gt; <br> OK|

Defined values:

| Parameter | Value |
|:--------|:--------|
| &lt;ssid&gt; | string parameter. Target AP의 ssid. MAX: 32 bytes |
| &lt;pwd&gt; | string parameter. Target AP의 password. MAX: 64-byte ASCII |
| &lt;bssid&gt; | string parameter, target AP 의 MAC address, 같은 SSID 를 가진 여러 개의 AP 들이 있을 때 사용된다. |

### 5. Azure IoT Hub configuration set

**AT Command:** AT+AZSET

Syntax:

| Type | Command | Response |
|:--------|:--------|:--------|
| Set | AT+AZSET=&lt;iothub_name&gt;,&lt;device_id&gt;,&lt;device_key&gt; | OK |

Defined values:

| Parameter | Value |
|:--------|:--------|
| &lt;hub ID&gt; | string parameter. IoT Hub의 ID |
| &lt;device ID&gt; | string parameter. IoT Device의 ID |
| &lt;key&gt; | string parameter, IoT Device의 Key |

### 6. Set MQTT Topic

**AT Command:** AT+MQTTTOPIC

Syntax:

| Type | Command | Response |
|:--------|:--------|:--------|
| Set | AT+MQTTTOPIC=&lt;publish topic&gt;,&lt;subscribe topic&gt;,&lt;subscribe topic2&gt;,&lt;subscribe topic3&gt; | OK |

Defined values:

| Parameter | Value |
|:--------|:--------|
| &lt;publish topic&gt; | string parameter, WizFi360 이 publish 하는 topic |
| &lt;subscribe topic&gt; |  string parameter, WizFi360 이 subscribe 하는 topic|
| &lt;subscribe topic2&gt; | string parameter, WizFi360 이 subscribe 하는 topic |
| &lt;subscribe topic3&gt; | string parameter, WizFi360 이 subscribe 하는 topic |

> Note:
- 이 command 는 broker 에 연결하기전에 설정되어야 합니다.
- &lt;subscribe topic2&gt; 와 &lt;subscribe topic3&gt;는 Firmware v1.0.5.0 이후 version 부터 사용가능 합니다.

### 7. Connect to Azure

**AT Command:** AT+AZCON

Syntax:

| Type | Command | Response |
|:--------|:--------|:--------|
| Set | AT+AZCON | CONNECT <br> OK |
> Note:
• 이 command 를 전송하기전에 AT+AZSET command 와 AT+MQTTTOPIC command 를 설정합니다.
• Connect 이후 AT+MQTTPUB command 를 통해 Azure Sever 에 데이터를 전송합니다.
• 자세한 내용은 https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-mqtt-support 를 참조하세요.

### 8. Publish a message

**AT Command:** AT+MQTTPUB

Syntax:

| Type | Command | Response |
|:--------|:--------|:--------|
| Set | AT+MQTTPUB=&lt;message&gt;| OK |

> Note:
• 이 command 는 MQTT 가 연결되어 있을 때 사용됩니다.
• Publish 한 data 의 topic 은 AT+MQTTTOPIC command 에 의해 결정되며, 사용자는 broker 에 연결하기전에 topic 을 설정합니다.

<a name="Example"></a>
## 동작 예제

### Mbed 예제 코드 다운로드
**예제 Download**를 한 후, **File** > **Open Workspace**을 선택하여 **Project 실행**합니다.

> 예제는 다음 경로에 위치하고 있는 Project를 참고 바랍니다.
>
> **samples/Wi-Fi/Mbed_Azure_Atcmd_Wizfi360**

![][Link-Execute_Project_Through_Mbed_Studio_1]

Online Compiler 경우 다음 링크에서 Compiler에 import을 할 수 있습니다.
> https://os.mbed.com/users/vikshin/code/Mbed-Azure-Atcmd-WizFi360/
![](/images/Mbed_Azure_online_compiler.JPG)

### Modify parameters

Azure IoT Hub 연결 위한 WiFi ssid, WiFi password, Hub ID, Device ID, Device Key 변경하여 테스트 해볼 수 있습니다.
````cpp
/* WiFi info */
char ssid[] = "XXXXXXXXXXXXXXXXXXXXXXXX";
char password[] = "XXXXXXXXXXXXXXXXXXXXXXXXXX";

/* Azure info */
char hub_name[] = "XXXXXXXXXXXXXXXXXXXXXXXXXX";
char device_id[] = "XXXXXXXXXXXXXXXXXXXXXXXXXX";
char device_primary_key[] = "XXXXXXXXXXXXXXXXXXXXXXXXXX=";
````


**Run Program**을 눌러 Project Build 및 Run을 합니다.

![][Link-Execute_Project_Through_Mbed_Studio_3]

업로드를 완료한 후, 시리얼 모니터를 이용하여 정상적으로 Nucleo 보드에 업로드 되었는지 확인할 수 있습니다.
![](/images/Mbed_Azure_atcmd_serial_monitor.JPG)


<a name="Step-5-Read_Data_From_IoT_Hub"></a>
### 5. 동작 예제 결과

1. IoT Explorer 에서 Telemetry Section안에 "Start" 버튼을 누릅니다.
> MQTTPUB 명령을 통해 메시지를 보내기 전에 "Start" 버튼을 눌러야 합니다.
2. MQTTPUB command으로 수신한 데이터를 확인 할 수 있습니다.

![](/images/Mbed_Azure_atcmd_iot_explorer.JPG)
<a name="Next"></a>
## 더 보기

[WizFi360 Azure AT Command를 이용하여 Azure IoT Hub에 연결]


[계정]: https://azure.microsoft.com/ko-kr/free/
[Azure Portal]: https://portal.azure.com/
[WizFi360]: https://wizwiki.net/wiki/doku.php/products:wizfi360:start
[WizFi360-EVB-Shield]: https://wizwiki.net/wiki/doku.php/products:wizfi360:board:wizfi360-evb:start
[Azure IoT Explorer]: https://catalog.azureiotsolutions.com/docs?title=Azure/azure-iot-device-ecosystem/manage_iot_hub
[Silicon Labs CP210x USB to UART Driver]: https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers
[Communicate with your IoT hub using the MQTT protocol: Using the MQTT protocol directly (as a device)]: https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-mqtt-support
[Azure Cloud 소개]: https://github.com/Wiznet/azure-iot-kr/tree/master/docs/Azure_Cloud
[Link-Nucleo-L476rg]:https://os.mbed.com/platforms/ST-Nucleo-L476RG/
[Link-Device-Management]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/device_management_2.png
[Link-Execute_Project_Through_Mbed_Studio_1]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/execute_project_through_mbed_studio_1.png
[Link-Execute_Project_Through_Mbed_Studio_3]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/execute_project_through_mbed_studio_3.png
[WizFi360 Azure AT Command를 이용하여 Azure IoT Hub에 연결]: https://github.com/Wiznet/azure-iot-kr/blob/master/docs/IoT_device/Connectivities/Wi-Fi/standalone_azure_atcmd_wizfi360.md
