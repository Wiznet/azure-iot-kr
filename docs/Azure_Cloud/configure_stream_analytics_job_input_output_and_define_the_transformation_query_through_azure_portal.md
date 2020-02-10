#  Azure Portal을 사용하여 Stream Analytics 작업 입 · 출력 구성 및 변환 Query 정의



## 목차

- [시작하기 전에](#Prerequisites)
- [소개](#Introduction)
- [Step1 : 작업 입력 구성](#Step-1-Configure_Job_Input)
- [Step2 : 작업 출력 구성](#Step-2-Configure_Job_Output)
- [Step3 : 변환 Query 정의](#Step-3-Define_The_Transformation_Query)
- [더 보기](#Read_More)



<a name="Prerequisites"></a>
## 시작하기 전에

> [**Azure Portal**][Link-Azure-Portal]에 Login을 합니다. 계정이 없는 경우, 계정 생성 후에 Login을 진행합니다.
>
> ※ 본 문서에는 [**체험 계정**][Link-Azure-Account-Free]으로 진행합니다.



<a name="Introduction"></a>
## 소개

본 문서는 Azure Portal을 사용하여 **작업 입 · 출력 구성 및 변환 Query 정의**하는 방법을 설명합니다.



<a name="Step-1-Configure_Job_Input"></a>
## Step1 : 작업 입력 구성

### 1. Stream Analytics에서 작업 **작업 토폴로지** > **입력** > **스트림 입력 추가**를 선택합니다.

![][Link-Configure_Job_Input_2]

### 2. **IoT Hub**를 선택합니다.

![][Link-Configure_Job_Input_3]

### 3. **입력 별칭**을 입력하고 나머지 항목을 다음 그림과 같이 입력 및 설정합니다.

![][Link-Configure_Job_Input_5]

### 4. **저장**을 눌러 작업 입력을 구성합니다.

![][Link-Configure_Job_Input_6]



<a name="Step-2-Configure_Job_Output"></a>
## Step2 : 작업 출력 구성

### 1. Stream Analytics에서 작업 **작업 토폴로지** > **출력** > **추가**를 선택합니다.

![][Link-Configure_Job_Output_2]

### 2. **Blob Storage/Data Lake Storage Gen2**를 선택합니다.

![][Link-Configure_Job_Output_3]

### 3. **출력 별칭**을 입력하고 나머지 항목을 다음 그림과 같이 입력 및 설정합니다.

![][Link-Configure_Job_Output_5]

### 4. **저장**을 눌러 작업 출력을 구성합니다.

![][Link-Configure_Job_Output_6]



<a name="Step-3-Define_The_Transformation_Query"></a>
## Step3 : 변환 Query 정의

### 1. Stream Analytics에서 작업 **작업 토폴로지** > **쿼리**를 선택합니다.

![][Link-Define_The_Transformation_Query_2]

### 2. **Query**를 다음 그림과 같이 **수정 및 작성**합니다.

![][Link-Define_The_Transformation_Query_3]

### 3. **쿼리 저장**을 눌러 저장합니다.

![][Link-Define_The_Transformation_Query_4]



<a name="Read_More"></a>
## 더 보기

- [Azure Portal을 사용하여 Stream Analytics 만들기][Link-Create_Stream_Analytics_Through_Azure_Portal]
- [WizFi360 MQTT AT Command를 이용하여 Auzre IoT Hub에 연동][Link-Standalone_Mqtt_Atcmd_Wizfi360]
- [WizFi360 Azure AT Command를 이용하여 Auzre IoT Hub에 연동][Link-Standalone_Azure_Atcmd_Wizfi360]



[Link-Azure-Portal]: https://portal.azure.com/
[Link-Azure-Account-Free]: https://azure.microsoft.com/ko-kr/free/
[Link-Configure_Job_Input_2]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/configure_job_input_2.png
[Link-Configure_Job_Input_3]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/configure_job_input_3.png
[Link-Configure_Job_Input_5]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/configure_job_input_5.png
[Link-Configure_Job_Input_6]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/configure_job_input_6.png
[Link-Configure_Job_Output_2]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/configure_job_output_2.png
[Link-Configure_Job_Output_3]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/configure_job_output_3.png
[Link-Configure_Job_Output_5]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/configure_job_output_5.png
[Link-Configure_Job_Output_6]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/configure_job_output_6.png
[Link-Define_The_Transformation_Query_2]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/define_the_transformation_query_2.png
[Link-Define_The_Transformation_Query_3]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/define_the_transformation_query_3.png
[Link-Define_The_Transformation_Query_4]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/define_the_transformation_query_4.png
[Link-Create_Stream_Analytics_Through_Azure_Portal]: https://github.com/Wiznet/azure-iot-kr/blob/master/docs/Azure_Cloud/create_stream_analytics_through_azure_portal.md
[Link-Standalone_Mqtt_Atcmd_Wizfi360]: https://github.com/Wiznet/azure-iot-kr/blob/master/docs/IoT_device/Connectivities/Wi-Fi/standalone_mqtt_atcmd_wizfi360.md
[Link-Standalone_Azure_Atcmd_Wizfi360]: https://github.com/Wiznet/azure-iot-kr/blob/master/docs/IoT_device/Connectivities/Wi-Fi/standalone_azure_atcmd_wizfi360.md