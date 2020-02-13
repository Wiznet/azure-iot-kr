# WizFi360 MQTT AT Command를 이용한 Auzre IoT Hub 연동 예제



## 목차

- [시작하기 전에](#Prerequisites)
- [소개](#Introduction)
- [Step 1: 필수 구성 요소](#Prerequisites)
- [Step 2: Device 준비](#Prepare_Device)
- [Step 3: 동작 예제](#Example)
- [Step 4: 동작 예제 결과](#Result)
- [더 보기](#Read_More)



<a name="Prerequisites"></a>
## 시작하기 전에

> [**Azure Portal**][Link-Azure-Portal]에 Login을 합니다. 계정이 없는 경우, 계정 생성 후에 Login을 진행합니다.
>
> ※ 본 문서에는 [**체험 계정**][Link-Azure-Account-Free]으로 진행합니다.
>
> Azure Portal을 사용하여 IoT Hub 만들기 등 앞선 일련의 과정에 대하여 [Azure Cloud 서비스 소개][Link-Azure_Cloud_Introduction]를 참조하시기 바랍니다.
>
> * [[MS] Azure Portal을 사용하여 IoT Hub 만들기][Link-Create_IoT_Hub_Through_Azure_Portal]
> * [Azure Portal을 사용하여 Blob Storage 만들기][Link-Create_Blob_Storage_Through_Azure_Portal]
> * [Azure Portal을 사용하여 Stream Analytics 만들기][Link-Create_Stream_Analytics_Through_Azure_Portal]
> * [Azure Portal을 사용하여 Stream Analytics 작업 입 · 출력 구성 및 변환 Query 정의][Link-Configure_Stream_Analytics_Job_Input_Output_And_Define_The_Transformation_Query_Through_Azure_Portal]
> * [WizFi360 MQTT AT Command를 이용하여 Azure IoT Hub에 연동][Link-Standalone_Mqtt_Atcmd_Wizfi360]



<a name="Introduction"></a>
## 소개

**Microsoft Azure Service**에 **WizFi360**을 **연동**하여, Data를 Cloud로 전송하고, Monitoring을 할 수 있습니다.

Data 통신은 다음과 같은 구조로 이루어집니다.

![][Link-Data_Communication_Structure]

**MQTT AT Command**를 이용하여, IoT Hub Service 연결 및 Data 송신을 합니다.

IoT Hub로 송신이 된 Data는 Stream Analytics를 통하여 Data 저장소 Blob Storage로 저장이 됩니다.

본 문서는 [Arduino][Link-Arduino] 기반 WizFi360 MQTT AT Command 이용한 Microsoft Azure Service 연동 예제에 대하여 Guide를 제공합니다.



<a name="Prerequisites"></a>
## Step 1: 필수 구성 요소

본 문서를 따라하기 전에 다음 항목이 준비되어야 합니다.

### Hadrware
 - Desktop or Laptop Computer
 - [WizFi360-EVB-Shield][Link-Wizfi360-Evb-Shield]
 - [Arduino Mega 2560][Link-Arduino_Mega_2560]
 - USB Type-B Cable

 ![][Link-Required_Item]

### Software
 - [Arduino IDE][Link-Arduino_Ide]
 - Preferred Serial Terminal (TeraTerm, Hercules, etc . . .)



<a name="Prepare_Device"></a>
## Step 2: Device 준비

### 1. Hardware 준비

WizFi360-EVB-Shield는 Arduino Mega 2560과 결합을 하여 사용되어 집니다. 따라서 WizFi360-EVB-Shield의 DIP Switch 설정 및 Jumper Cable 연결이 다음과 같이 필요합니다.

> * SW1 : Off
> * SW2 : Off
> * SW3 : On
> * WizFi360-EVB-Shield : D8 - Arduino Mega 2560 : 18
> * WizFi360-EVB-Shield : D2 - Arduino Mega 2560 : 19
> * WizFi360-EVB-Shield : SDA - Arduino Mega 2560 : 14

![][Link-Set_Wizfi360_Evb_Shield_Dip_Sw]

### 2. Device 연결

Hardware 설정 후, USB Type-B Cable을 이용하여 Arduino Mega 2560을 Desktop 혹은 Laptop Computer와 연결을 합니다.

**장치 관리자**에서 Arduino Mega 2560과 연결된 **COM Port**를 확인 할 수 있습니다.

![][Link-Device-Management]

> Arduino IDE를 정상적으로 설치를 하였다면 위와 같이 장치 관리자에서 COM 포트를 확인할 수 있습니다.
>
> 장치 관리자에서 COM Port를 확인 할 수 없는 경우, 다음 Link의 설명에 따라 설정 바랍니다.
>
> * [Manually install Drivers on Windows][Link-Manually_Install_Drivers_On_Windows]



<a name="Example"></a>
## Step 3: 동작 예제

### 1. 예제 Download 및 실행

**예제 Download**를 한 후, .ino 확장자를 선택하여 **Project 실행**합니다.

> 예제는 다음 경로에 위치하고 있는 Project를 참고 바랍니다.
>
> * **samples/Wi-Fi/Arduino_Mqtt_Atcmd_Wizfi360**

### 2. Parameter 값 수정

Azure IoT Hub에 연결을 하기 위하여, 다음의 Parameter를 수정합니다.

```cpp
/* WiFi info */
char ssid[] = "xxxxxxxxxx";
char password[] = "xxxxxxxxxx";

/* MQTT info */
char hub_name[] = "xxxxxxxxxx";
char host_name[] = "xxxxxxxxxx";
char device_id[] = "xxxxxxxxxx";
char sas_token[] = "xxxxxxxxxx";
int alive_time = xx;    // range : 30 ~ 300
```

![][Link-Execute_Project_Through_Arduino_Ide_1]

> **SAS Token 생성**은 다음을 참고 바랍니다.
>
> * [SAS Token 생성하기][Link-Create-Sas-Token]

### 3. Project Compile 및 Upload

사용하는 Board, COM Port 선택 후, Compile 및 Upload를 수행합니다.

> Note :
>
> **Stream Analytics 실행** 중이어야 Blob Storage로 Data가 전달됩니다.

![][Link-Execute_Project_Through_Arduino_Ide_2]

![][Link-Execute_Project_Through_Arduino_Ide_3]

![][Link-Execute_Project_Through_Arduino_Ide_4]



<a name="Result"></a>
## Step 4: 동작 예제 결과

Terminal Program으로 WizFi360에서 Azure IoT Hub로 전송하는 Data, Blob Storage에서 WizFi360으로부터 수신한 Data를 확인 할 수 있습니다.

![][Link-Result_Execute_Project_Through_Arduino_Ide_1]

![][Link-Result_Execute_Project_Through_Arduino_Ide_2]



<a name="Read_More"></a>
## 더 보기

- [WizFi360 MQTT AT Command를 이용하여 Auzre IoT Hub에 연동][Link-Standalone_Mqtt_Atcmd_Wizfi360]



[Link-Azure-Portal]: https://portal.azure.com/
[Link-Azure-Account-Free]: https://azure.microsoft.com/ko-kr/free/
[Link-Azure_Cloud_Introduction]: https://github.com/Wiznet/azure-iot-kr/tree/master/docs/Azure_Cloud
[Link-Create_IoT_Hub_Through_Azure_Portal]: https://docs.microsoft.com/ko-kr/azure/iot-hub/iot-hub-create-through-portal
[Link-Create_Blob_Storage_Through_Azure_Portal]: https://github.com/Wiznet/azure-iot-kr/blob/master/docs/Azure_Cloud/create_blob_storage_through_azure_portal.md
[Link-Create_Stream_Analytics_Through_Azure_Portal]: https://github.com/Wiznet/azure-iot-kr/blob/master/docs/Azure_Cloud/create_stream_analytics_through_azure_portal.md
[Link-Configure_Stream_Analytics_Job_Input_Output_And_Define_The_Transformation_Query_Through_Azure_Portal]: https://github.com/Wiznet/azure-iot-kr/blob/master/docs/Azure_Cloud/configure_stream_analytics_job_input_output_and_define_the_transformation_query_through_azure_portal.md
[Link-Data_Communication_Structure]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/mqtt_data_communication_structure.png
[Link-Arduino]: https://www.arduino.cc/
[Link-Wizfi360-Evb-Shield]: https://wizwiki.net/wiki/doku.php/products:wizfi360:start
[Link-Arduino_Mega_2560]: https://store.arduino.cc/usa/mega-2560-r3
[Link-Required_Item]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/mqtt_atcmd_wizfi360_required_item_3.png
[Link-Arduino_Ide]: https://www.arduino.cc/en/main/software
[Link-Set_Wizfi360_Evb_Shield_Dip_Sw]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/set_wizfi360_evb_shield_dip_sw_3.png
[Link-Device-Management]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/device_management_3.png
[Link-Manually_Install_Drivers_On_Windows]: https://www.arduino.cc/en/Guide/DriverInstallation
[Link-Execute_Project_Through_Arduino_Ide_1]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/execute_project_through_arduino_ide_1.png
[Link-Create-Sas-Token]: https://github.com/Wiznet/azure-iot-kr/tree/master/docs/Azure_Cloud/create_sas_token.md
[Link-Execute_Project_Through_Arduino_Ide_2]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/execute_project_through_arduino_ide_2.png
[Link-Execute_Project_Through_Arduino_Ide_3]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/execute_project_through_arduino_ide_3.png
[Link-Execute_Project_Through_Arduino_Ide_4]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/execute_project_through_arduino_ide_4.png
[Link-Result_Execute_Project_Through_Arduino_Ide_1]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/result_execute_project_through_arduino_ide_1.png
[Link-Result_Execute_Project_Through_Arduino_Ide_2]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/result_execute_project_through_arduino_ide_2.png
[Link-Standalone_Mqtt_Atcmd_Wizfi360]: https://github.com/Wiznet/azure-iot-kr/blob/master/docs/IoT_device/Connectivities/Wi-Fi/standalone_mqtt_atcmd_wizfi360.md