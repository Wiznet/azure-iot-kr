# WizFi360 MQTT AT Command를 이용하여 Auzre IoT Hub에 연동



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
> ※ 본 문서는 [**체험 계정**][Link-Azure-Account-Free]으로 진행합니다.
>
> Azure Portal을 사용하여 IoT Hub 만들기 등 앞선 일련의 과정에 대하여 [Azure Cloud 서비스 소개][Link-Azure_Cloud_Introduction]를 참조하시기 바랍니다.
>
> * [[MS] Azure Portal을 사용하여 IoT Hub 만들기][Link-Create_IoT_Hub_Through_Azure_Portal]
> * [Azure Portal을 사용하여 Blob Storage 만들기][Link-Create_Blob_Storage_Through_Azure_Portal]
> * [Azure Portal을 사용하여 Stream Analytics 만들기][Link-Create_Stream_Analytics_Through_Azure_Portal]
> * [Azure Portal을 사용하여 Stream Analytics 작업 입 · 출력 구성 및 변환 Query 정의][Link-Configure_Stream_Analytics_Job_Input_Output_And_Define_The_Transformation_Query_Through_Azure_Portal]



<a name="Introduction"></a>
## 소개

**Microsoft Azure Service**에 **WizFi360**을 **연동**하여, Data를 Cloud로 전송하고, Monitoring을 할 수 있습니다.

Data 통신은 다음과 같은 구조로 이루어집니다.

![][Link-Data_Communication_Structure]

**MQTT AT Command**를 이용하여, IoT Hub Service 연결 및 Data 송신을 합니다.

IoT Hub로 송신이 된 Data는 Stream Analytics를 통하여 Data 저장소 Blob Storage로 저장이 됩니다.

본 문서는 WizFi360 MQTT AT Command 이용하여 Microsoft Azure Service 연결 방법에 대한 Guide를 제공합니다.



<a name="Prerequisites"></a>
## Step 1: 필수 구성 요소

본 문서를 따라하기에 전에 다음 항목이 준비되어야 합니다.

### Hadrware
 - Desktop or Laptop Computer
 - [WizFi360-EVB-Shield][Link-Wizfi360-Evb-Shield]
 - Micro 5 Pin USB Cable

 ![][Link-Required_Item]

### Software
 - Preferred Serial Terminal (TeraTerm, Hercules, etc . . .)



<a name="Prepare_Device"></a>
## Step 2: Device 준비

### 1. Hardware 준비

WizFi360-EVB-Shield는 **Standalone Mode**로 사용되어 집니다. 따라서 WizFi360-EVB-Shield의 DIP Switch를 다음과 같이 설정이 필요합니다.

> * SW1 : On
> * SW2 : Off
> * SW3 : Off

![][Link-Set_Wizfi360_Evb_Shield_Dip_Sw]

### 2. Device 연결

Hardware 설정 후, Micro 5 Pin USB Cable을 이용하여 WizFi360-EVB-Shield를 Desktop 혹은 Laptop Computer와 연결을 합니다.

**장치 관리자**에서 WizFi360-EVB-Shield와 연결된 **COM Port**를 확인 할 수 있습니다.

![][Link-Device-Management]

> 장치 관리자에서 COM Port를 확인 할 수 없는 경우, 다음 Link에서 Driver를 Downlonad하여 설치하시기 바랍니다.
>
> * [CP210x USB to UART Bridge VCP Drivers][Link-Cp210x_Usb_To_Uart_Bridge_Vcp_Drivers]



<a name="Example"></a>
## Step 3: 동작 예제

**WizFi360**의 조금 더 상세한 **사용 방법**은 [**Quick Start Guide**][Link-Wizfi360-Quick_Start_Guide], **AT Command**는 [**AT Instruction Set**][Link-Wizfi360-At_Instruction_Set]을 참고 바랍니다.

### 1. Mode 설정

|  Command | Response |
|:--------|:--------|
| AT+CWMODE_CUR=1 | OK |

### 2. DHCP 설정

| Command | Response |
|:--------|:--------|
| AT+CWDHCP_CUR=1,1 | OK |

### 3. AP 접속

| Command | Response |
|:--------|:--------|
| AT+CWJAP_CUR="**ssid**","**password**" <br><br> Example : <br> AT+CWJAP_CUR="**wiznet**","**0123456789**" | WIFI CONNECTED <br> WIFI GOT IP <br><br> OK |

### 4. MQTT 연결 설정

| Command | Response |
|:--------|:--------|
| AT+MQTTSET="**iot_hub_host_name**/**device_id**/?api-version=2018-06-30","**sas_token**","**device_id**",60<br><br> Example : <br> AT+MQTTSET="**MyWizFi360IoTHub.azure-devices.net**/**MyWizFi360IoTDevice**/?api-version=2018-06-30","**SharedAccessSignature sr=MyWizFi360IoTHub.azure-devices.net%2Fdevices%2FMyWizFi360IoTDevice&sig=t3R9nDS7ezMGBdb%2FNd5ktb3xQx5jx4NC02n325vRA6c%3D&se=1611895717**","**MyWizFi360IoTDevice**",60 | OK |

> **SAS Token 생성**은 다음을 참고 바랍니다.
>
> * [Device Explorer를 사용하여 SAS Token 생성하기][Link-Create_Sas_Token_Through_Device_Explorer]
> * [Azure IoT Explorer를 사용하여 SAS Token 생성하기][Link-Create_Sas_Token_Through_Azure_Iot_Explorer]

### 5. Topic 설정

| Command | Response |
|:--------|:--------|
| AT+MQTTTOPIC="devices/**device_id**/messages/events/",devices/**device_id**/messages/devicebound/# <br><br> Example : <br> AT+MQTTTOPIC="devices/**MyWizFi360IoTDevice**/messages/events/","devices/**MyWizFi360IoTDevice**/messages/devicebound/#" | OK |

### 6. Broker 연결

| Command | Response |
|:--------|:--------|
| AT+MQTTCON=1,"**iot_hub_host_name**",8883 <br><br> Example : <br> AT+MQTTCON=1,"**MyWizFi360IoTHub.azure-devices.net**",8883 | CONNECT <br><br> OK |

### 7. Publish Message

> Note :
>
> Data를 Publish 할 때, [**Stream Analytics 실행**](#Run_Stream_Analytics) 중이어야 Blob Storage로 Data가 전달됩니다.

| Command | Response |
|:--------|:--------|
| AT+MQTTPUB="publish_data" <br><br> Example : <br> AT+MQTTPUB="{"deviceId":"MyWizFi360IoTDevice","temperature":21.97,"humidity":43.58}"| OK |

> Note :
>
> Publish Data는 어떤 형태라도 가능하지만, Azure Guide의 Stream Analytics에서 작업 입력 구성 설정 중,
>
> Event Serialization 형식이 기본 JSON 형태이므로 맞춰주어야 합니다.

![][Link-Standalone_Mqtt_Atcmd_Wizfi360_Set_Up_And_Run_Wizfi360_1]



<a name="Result"></a>
## Step 4: 동작 예제 결과

<a name="Run_Stream_Analytics"></a>
### 1. Stream Analytics 작업 시작

Stream Analytics에서 **개요** > **시작** > **현재** > **시작**을 선택합니다.

![][Link-Start_The_Stream_Analytics_Job_And_Check_The_Output_1]

![][Link-Start_The_Stream_Analytics_Job_And_Check_The_Output_2]

![][Link-Start_The_Stream_Analytics_Job_And_Check_The_Output_3]

### 2. 출력 확인

1) Blob Storage에서 **개요** > **컨테이너**를 선택합니다.

![][Link-Start_The_Stream_Analytics_Job_And_Check_The_Output_4]

2) 목록에서 Container를 선택하여 엽니다.

![][Link-Start_The_Stream_Analytics_Job_And_Check_The_Output_5]

3) 목록에서 Blob을 선택하여 엽니다.

![][Link-Start_The_Stream_Analytics_Job_And_Check_The_Output_6]

4) **편집**을 눌러 WizFi360으로부터 수신한 Data를 확인합니다.

![][Link-Start_The_Stream_Analytics_Job_And_Check_The_Output_7]

![][Link-Start_The_Stream_Analytics_Job_And_Check_The_Output_8]



<a name="Read_More"></a>
## 더 보기

- [Azure Portal을 사용하여 Stream Analytics 작업 입 · 출력 구성 및 변환 Query 정의][Link-Configure_Stream_Analytics_Job_Input_Output_And_Define_The_Transformation_Query_Through_Azure_Portal]
- WizFi360 MQTT AT Command를 이용한 Auzre IoT Hub 연동 예제
	- [Mbed Example][Link-Mbed_Mqtt_Atcmd_Wizfi360]
	- [Arduino Example][Link-Arduino_Mqtt_Atcmd_Wizfi360]



[Link-Azure-Portal]: https://portal.azure.com/
[Link-Azure-Account-Free]: https://azure.microsoft.com/ko-kr/free/
[Link-Data_Communication_Structure]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/mqtt_data_communication_structure.png
[Link-Azure_Cloud_Introduction]: https://github.com/Wiznet/azure-iot-kr/tree/master/docs/Azure_Cloud
[Link-Create_IoT_Hub_Through_Azure_Portal]: https://docs.microsoft.com/ko-kr/azure/iot-hub/iot-hub-create-through-portal
[Link-Create_Blob_Storage_Through_Azure_Portal]: https://github.com/Wiznet/azure-iot-kr/blob/master/docs/Azure_Cloud/create_blob_storage_through_azure_portal.md
[Link-Create_Stream_Analytics_Through_Azure_Portal]: https://github.com/Wiznet/azure-iot-kr/blob/master/docs/Azure_Cloud/create_stream_analytics_through_azure_portal.md
[Link-Configure_Stream_Analytics_Job_Input_Output_And_Define_The_Transformation_Query_Through_Azure_Portal]: https://github.com/Wiznet/azure-iot-kr/blob/master/docs/Azure_Cloud/configure_stream_analytics_job_input_output_and_define_the_transformation_query_through_azure_portal.md
[Link-Wizfi360-Evb-Shield]: https://wizwiki.net/wiki/doku.php/products:wizfi360:start
[Link-Required_Item]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/mqtt_atcmd_wizfi360_required_item_1.png
[Link-Set_Wizfi360_Evb_Shield_Dip_Sw]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/set_wizfi360_evb_shield_dip_sw_1.png
[Link-Device-Management]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/device_management_1.png
[Link-Cp210x_Usb_To_Uart_Bridge_Vcp_Drivers]: https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers
[Link-Wizfi360-Quick_Start_Guide]: http://wizwiki.net/wiki/lib/exe/fetch.php/products:wizfi360:wizfi360ds:wizfi360qs_v113k.pdf
[Link-Wizfi360-At_Instruction_Set]: http://wizwiki.net/wiki/lib/exe/fetch.php/products:wizfi360:wizfi360ds:wizfi360_atset_v107.2k.pdf
[Link-Create_Sas_Token_Through_Device_Explorer]: https://github.com/Wiznet/azure-iot-kr/tree/master/docs/Azure_Cloud/create_sas_token_through_device_explorer.md
[Link-Create_Sas_Token_Through_Azure_Iot_Explorer]: https://github.com/Wiznet/azure-iot-kr/tree/master/docs/Azure_Cloud/create_sas_token_through_azure_iot_explorer.md
[Link-Standalone_Mqtt_Atcmd_Wizfi360_Set_Up_And_Run_Wizfi360_1]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/standalone_mqtt_atcmd_wizfi360_set_up_and_run_wizfi360_1.png
[Link-Start_The_Stream_Analytics_Job_And_Check_The_Output_1]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/start_the_stream_analytics_job_and_check_the_output_1.png
[Link-Start_The_Stream_Analytics_Job_And_Check_The_Output_2]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/start_the_stream_analytics_job_and_check_the_output_2.png
[Link-Start_The_Stream_Analytics_Job_And_Check_The_Output_3]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/start_the_stream_analytics_job_and_check_the_output_3.png
[Link-Start_The_Stream_Analytics_Job_And_Check_The_Output_4]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/start_the_stream_analytics_job_and_check_the_output_4.png
[Link-Start_The_Stream_Analytics_Job_And_Check_The_Output_5]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/start_the_stream_analytics_job_and_check_the_output_5.png
[Link-Start_The_Stream_Analytics_Job_And_Check_The_Output_6]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/start_the_stream_analytics_job_and_check_the_output_6.png
[Link-Start_The_Stream_Analytics_Job_And_Check_The_Output_7]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/start_the_stream_analytics_job_and_check_the_output_7.png
[Link-Start_The_Stream_Analytics_Job_And_Check_The_Output_8]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/start_the_stream_analytics_job_and_check_the_output_8.png
[Link-Mbed_Mqtt_Atcmd_Wizfi360]: https://github.com/Wiznet/azure-iot-kr/blob/master/docs/IoT_device/Connectivities/Wi-Fi/mbed_mqtt_atcmd_wizfi360.md
[Link-Arduino_Mqtt_Atcmd_Wizfi360]: https://github.com/Wiznet/azure-iot-kr/blob/master/docs/IoT_device/Connectivities/Wi-Fi/arduino_mqtt_atcmd_wizfi360.md