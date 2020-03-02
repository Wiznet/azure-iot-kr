# WizFi360 MQTT AT Command를 이용한 Azure IoT Hub 연동 예제



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
> * [WizFi360 MQTT AT Command를 이용하여 Azure IoT Hub에 연동][Link-Standalone_Mqtt_Atcmd_Wizfi360]



<a name="Introduction"></a>
## 소개

**Microsoft Azure Service**에 **WizFi360**을 **연동**하여, Data를 Cloud로 전송하고, Monitoring을 할 수 있습니다.

Data 통신은 다음과 같은 구조로 이루어집니다.

![][Link-Data_Communication_Structure]

**MQTT AT Command**를 이용하여, IoT Hub Service 연결 및 Data 송신을 합니다.

IoT Hub로 송신이 된 Data는 Stream Analytics를 통하여 Data 저장소 Blob Storage로 저장이 됩니다.

본 문서는 [Mbed][Link-Mbed] 기반 WizFi360 MQTT AT Command 이용한 Microsoft Azure Service 연동 예제에 대하여 Guide를 제공합니다.



<a name="Prerequisites"></a>
## Step 1: 필수 구성 요소

본 문서를 따라하기 전에 다음 항목이 준비되어야 합니다.

### Hadrware
 - Desktop or Laptop Computer
 - [WizFi360-EVB-Shield][Link-Wizfi360-Evb-Shield]
 - [NUCLEO-L476RG][Link-Nucleo-L476rg]
 - Mini USB Cable

 ![][Link-Required_Item]

### Software
 - [Mbed Studio][Link-Mbed_Studio]
 - Preferred Serial Terminal (TeraTerm, Hercules, etc . . .)



<a name="Prepare_Device"></a>
## Step 2: Device 준비

### 1. Hardware 준비

WizFi360-EVB-Shield는 NUCLEO-L476RG와 결합을 하여 사용되어 집니다. 따라서 WizFi360-EVB-Shield의 DIP Switch 및 UART Jumper Pin을 다음과 같이 설정이 필요합니다.

> * SW1 : Off
> * SW2 : Off
> * SW3 : On
> * D2 : UART Tx
> * D8 : UART Rx

![][Link-Set_Wizfi360_Evb_Shield_Dip_Sw]

### 2. Device 연결

Hardware 설정 후, Mini USB Cable을 이용하여 NUCLEO-L476RG를 Desktop 혹은 Laptop Computer와 연결을 합니다.

**장치 관리자**에서 NUCLEO-L476RG와 연결된 **COM Port**를 확인 할 수 있습니다.

![][Link-Device-Management]

> 장치 관리자에서 COM Port를 확인 할 수 없는 경우, 다음 Link에서 Driver를 Downlonad하여 설치하시기 바랍니다.
>
> * [ST-LINK, ST-LINK/V2, ST-LINK/V2-1 USB driver][Link-St_Link_St_Link_V2_St_Link_V2_1_Usb_Driver]



<a name="Example"></a>
## Step 3: 동작 예제

### 1. 예제 Download 및 실행

**예제 Download**를 한 후, **File** > **Open Workspace**을 선택하여 **Project 실행**합니다.

> 예제는 다음 경로에 위치하고 있는 Project를 참고 바랍니다.
>
> * **samples/Wi-Fi/Mbed_Mqtt_Atcmd_Wizfi360**
>
> * **samples/Wi-Fi/Mbed_Mqtt_Atcmd_Wizfi360_Azure_C_Shared_Utility**
>
> **Mbed_Mqtt_Atcmd_Wizfi360** 경우에는 Device Explorer 혹은 Azure IoT Explorer와 같은 Tool을 이용하여 **별도의 SAS Token 생성 과정이 필요**합니다.
>
> **Mbed_Mqtt_Atcmd_Wizfi360_Azure_C_Shared_Utility** 경우에는 Azure IoT Common Library인 azure_c_shared_utility Library를 Porting을 한 Project이므로 **별도의 SAS Token 생성 과정이 필요 없으며, Project 내부에서 SAS Token 생성 과정을 처리**합니다.

![][Link-Execute_Project_Through_Mbed_Studio_1]

### 2. Parameter 값 수정

Azure IoT Hub에 연결을 하기 위하여, 다음의 Parameter를 수정합니다.

```cpp
/* Wi-Fi info */
char ssid[] = "xxxxxxxxxx";
char password[] = "xxxxxxxxxx";

/* MQTT info */
int alive_time = xx;    // range : 30 ~ 300

/* Azure info */
char hub_name[] = "xxxxxxxxxx";
char host_name[] = "xxxxxxxxxx";
char device_id[] = "xxxxxxxxxx";
char device_key[] = "xxxxxxxxxx";
char sas_token[] = "xxxxxxxxxx";
```

![][Link-Execute_Project_Through_Mbed_Studio_2]

> **SAS Token 생성**은 다음을 참고 바랍니다.
>
> * [Device Explorer를 사용하여 SAS Token 생성하기][Link-Create_Sas_Token_Through_Device_Explorer]
> * [Azure IoT Explorer를 사용하여 SAS Token 생성하기][Link-Create_Sas_Token_Through_Azure_Iot_Explorer]

### 3. Project Build 및 Run

**Run Program**을 눌러 Project Build 및 Run을 합니다.

> Note :
>
> **Stream Analytics 실행** 중이어야 Blob Storage로 Data가 전달됩니다.

![][Link-Execute_Project_Through_Mbed_Studio_3]



<a name="Result"></a>
## Step 4: 동작 예제 결과

Terminal Program으로 WizFi360에서 Azure IoT Hub로 전송하는 Data, Blob Storage에서 WizFi360으로부터 수신한 Data를 확인 할 수 있습니다.

![][Link-Result_Execute_Project_Through_Mbed_Studio_1]

![][Link-Result_Execute_Project_Through_Mbed_Studio_2]



<a name="Read_More"></a>
## 더 보기

- [WizFi360 MQTT AT Command를 이용하여 Azure IoT Hub에 연동][Link-Standalone_Mqtt_Atcmd_Wizfi360]



[Link-Azure-Portal]: https://portal.azure.com/
[Link-Azure-Account-Free]: https://azure.microsoft.com/ko-kr/free/
[Link-Azure_Cloud_Introduction]: https://github.com/Wiznet/azure-iot-kr/tree/master/docs/Azure_Cloud
[Link-Create_IoT_Hub_Through_Azure_Portal]: https://docs.microsoft.com/ko-kr/azure/iot-hub/iot-hub-create-through-portal
[Link-Create_Blob_Storage_Through_Azure_Portal]: https://github.com/Wiznet/azure-iot-kr/blob/master/docs/Azure_Cloud/create_blob_storage_through_azure_portal.md
[Link-Create_Stream_Analytics_Through_Azure_Portal]: https://github.com/Wiznet/azure-iot-kr/blob/master/docs/Azure_Cloud/create_stream_analytics_through_azure_portal.md
[Link-Configure_Stream_Analytics_Job_Input_Output_And_Define_The_Transformation_Query_Through_Azure_Portal]: https://github.com/Wiznet/azure-iot-kr/blob/master/docs/Azure_Cloud/configure_stream_analytics_job_input_output_and_define_the_transformation_query_through_azure_portal.md
[Link-Standalone_Mqtt_Atcmd_Wizfi360]: https://github.com/Wiznet/azure-iot-kr/blob/master/docs/IoT_device/Connectivities/Wi-Fi/standalone_mqtt_atcmd_wizfi360.md
[Link-Data_Communication_Structure]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/mqtt_data_communication_structure.png
[Link-Mbed]: https://www.mbed.com/en/
[Link-Wizfi360-Evb-Shield]: https://wizwiki.net/wiki/doku.php/products:wizfi360:start
[Link-Nucleo-L476rg]:https://os.mbed.com/platforms/ST-Nucleo-L476RG/
[Link-Required_Item]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/mqtt_atcmd_wizfi360_required_item_2.png
[Link-Mbed_Studio]: https://os.mbed.com/studio/
[Link-Set_Wizfi360_Evb_Shield_Dip_Sw]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/set_wizfi360_evb_shield_dip_sw_2.png
[Link-Device-Management]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/device_management_2.png
[Link-St_Link_St_Link_V2_St_Link_V2_1_Usb_Driver]: https://www.st.com/en/development-tools/stsw-link009.html
[Link-Execute_Project_Through_Mbed_Studio_1]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/execute_project_through_mbed_studio_1.png
[Link-Execute_Project_Through_Mbed_Studio_2]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/execute_project_through_mbed_studio_2.png
[Link-Create_Sas_Token_Through_Device_Explorer]: https://github.com/Wiznet/azure-iot-kr/tree/master/docs/Azure_Cloud/create_sas_token_through_device_explorer.md
[Link-Create_Sas_Token_Through_Azure_Iot_Explorer]: https://github.com/Wiznet/azure-iot-kr/tree/master/docs/Azure_Cloud/create_sas_token_through_azure_iot_explorer.md
[Link-Execute_Project_Through_Mbed_Studio_3]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/execute_project_through_mbed_studio_3.png
[Link-Result_Execute_Project_Through_Mbed_Studio_1]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/result_execute_project_through_mbed_studio_1.png
[Link-Result_Execute_Project_Through_Mbed_Studio_2]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/result_execute_project_through_mbed_studio_2.png
[Link-Standalone_Mqtt_Atcmd_Wizfi360]: https://github.com/Wiznet/azure-iot-kr/blob/master/docs/IoT_device/Connectivities/Wi-Fi/standalone_mqtt_atcmd_wizfi360.md
