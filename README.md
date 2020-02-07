# WIZnet Azure IoT Documents and Samples
이 Repository는 IoT 장치를 Cat.M1, WiFi, Ethernet을 이용하여 Azure IoT Hub에 연결하고 Azure의 다른 서비스와 연동하기 위해 작성되었습니다.
([docs.microsoft.com](https://docs.microsoft.com/ko-kr/) 사이트를 참고하여 작성되었습니다.)

## Contents
-  [Folder Structure](#Folder)
-  [사전준비](#사전준비)
-  [Cat.M1 통신을 이용하여 Azure IoT Hub에 연결하기](#Cat)
-  [WiFi 통신을 이용하여 Azure IoT Hub에 연결하기](#WiFi)
-  [Azure Sphere Gardian(WiFi, Ethernet)을 이용하여 Azure IoT Hub에 연결하기](#Azure_Sphere_Gardian)
-  [Azure Cloud](#Azure_Cloud)
-  [Support](#Support)


<a name="Folder"></a>
## Folder Structure

### /docs/IoT_device/Connectivity
Cat.M1, WiFi, Ethernet을 이용한 Azure IoT Hub를 활용시 참고할 수 있는 문서가 위치합니다.
* LTE/Cat.M1 : Cat.M1을 이용하여 Azure IoT Hub에 연결시 참고할 수 있는 문서
* Wi-Fi : WizFi360을 이용하여 Azure IoT Hub에 연결시 참고할 수 있는 문서
* Wi-Fi/Gateway : WizFi630을 이용하여 Azure IoT Hub에 연결시 참고할 수 있는 문서
* Ethernet : WIZ-ASG-200을 이용하여 Azure IoT Hub에 연결시 참고할 수 있는 문서

### /docs/Azure_Cloud
IoT Hub를 이용한 Azure Cloud를 활용시 참고할 수 있는 문서가 위치합니다.


### /samples
Cat.M1, WiFi, Ethernet, Gateway를 이용하여 Azure IoT Hub에 하는 예제 코드가 위치합니다. 
* LTE : MCU, 라즈베리파이, PC를 이용한 예제 코드
* Wi-Fi: WizFi360등 을 이용한 예제 코드
* Ethernet: WIZ-ASG-200을 이용한 예제 코드
* Gateway : WizFi630을 이용한 예제 코드

## Key features
:heavy_check_mark: 준비 완료  :heavy_minus_sign: 준비중

<a name="사전준비"></a>
## 사전준비
|         Doc        |                            Description                           |
|:------------------:|:------------------|
| :heavy_check_mark: | [Azure IoT Hub 만들기](https://docs.microsoft.com/ko-kr/azure/iot-hub/iot-hub-create-through-portal)


<a name="Cat"></a>
## Cat.M1 통신을 이용하여 Azure IoT Hub에 연결하기
|         Doc        |                            Description                           |
|:------------------:|:------------------|
| :heavy_check_mark: | [MCU(STM32)와 Cat.M1(BG96)모듈을 이용하여 Azure IoT Hub에 연결하기](https://github.com/Wiznet/azure-iot-kr/blob/master/docs/IoT_device/Connectivities/LTE/Cat.M1/nucleo_stm32l496_azure_st_sdk_bg96.md)
| :heavy_minus_sign: | [라즈베리파이와 Cat.M1(BG96)모듈을 이용하여 Azure IoT Hub에 연결하기](https://github.com/Wiznet/azure-iot-kr/blob/master/docs/IoT_device/Connectivities/LTE/Cat.M1/raspberrypi_azure_c_sdk.md)
| :heavy_minus_sign: | [Cat.M1(BG96)모듈의 AT커맨드로 Azure IoT Hub에 연결하기](https://github.com/Wiznet/azure-iot-kr/blob/master/docs/IoT_device/Connectivities/LTE/Cat.M1/WIZnet_IoT_Shield_Catm1_BG96_Standalone.md)

<a name="WiFi"></a>
## WiFi 통신을 이용하여 Azure IoT Hub에 연결하기
|         Doc        |                            Description                           |
|:------------------:|:------------------|
| :heavy_minus_sign: | [WiFi(WizFi630S)모듈의 리눅스에서 Azure IoT Hub에 연결하기](https://github.com/Wiznet/azure-iot-kr/blob/master/docs/IoT_device/Connectivities/Wi-Fi/Gateway/wizfi630s_azure_c_sdk.md)
| :heavy_check_mark: | [WiFi(WizFi360)모듈의 MQTT AT커맨드로 Azure IoT Hub에 연결하기](https://github.com/Wiznet/azure-iot-kr/blob/master/docs/IoT_device/Connectivities/Wi-Fi/standalone_mqtt_atcmd_wizfi360.md)
| :heavy_check_mark: | [WiFi(WizFi360)모듈의 Azure AT커맨드로 Azure IoT Hub에 연결하기](https://github.com/Wiznet/azure-iot-kr/blob/master/docs/IoT_device/Connectivities/Wi-Fi/standalone_azure_atcmd_wizfi360.md)

<a name="Azure_Sphere_Gardian"></a>
## Azure Sphere Gardian(WiFi, Ethernet)을 이용하여 Azure IoT Hub에 연결하기
|         Doc        |                            Description                           |
|:------------------:|:------------------|
| :heavy_minus_sign: | [Azure Sphere Gardia(WIZ-AZG-200)을 이용하여 이더넷으로 Azure IoT Hub에 연결하기](https://github.com/Wiznet/azure-iot-kr/blob/master/docs/IoT_device/Connectivities/Ethernet/wiz-asg-200_azure_sphere_sdk.md)

<a name="Azure_Cloud"></a>
## Azure Cloud
|         Doc        |                            Description                           |
|:------------------:|:------------------|
| :heavy_check_mark: | [Azure Cloud 소개](https://github.com/Wiznet/azure-iot-kr/blob/master/docs/Azure_Cloud/README.md)
| :heavy_check_mark: | [ST AZURE Dashboard 가이드](https://github.com/Wiznet/azure-iot-kr/blob/master/docs/Azure_Cloud/st_azure_dashboard.md)
| :heavy_check_mark: | [Azure Portal을 사용하여 Blob Storage 만들기](https://github.com/Wiznet/azure-iot-kr/blob/master/docs/Azure_Cloud/create_blob_storage_through_azure_portal.md)
| :heavy_check_mark: | [SAS Token 생성하기](https://github.com/Wiznet/azure-iot-kr/blob/master/docs/Azure_Cloud/create_sas_token.md)
| :heavy_check_mark: | [Azure Portal을 사용하여 Blob Storage 만들기](https://github.com/Wiznet/azure-iot-kr/blob/master/docs/Azure_Cloud/create_blob_storage_through_azure_portal.md)
| :heavy_check_mark: | [작업 입 · 출력 구성 및 변환 Query 정의](https://github.com/Wiznet/azure-iot-kr/blob/master/docs/Azure_Cloud/configure_job_input_output_and_define_the_transformation_query.md)

<a name="Support"></a>
## Support

[![WIZnet Developer Forum][forum]](https://forum.wiznet.io/c/korean-forum/oshw/)

**[WIZnet Developer Forum](https://forum.wiznet.io/c/korean-forum/oshw/)** 에서 전세계의 WIZnet 기술 전문가들에게 질문하고 의견을 전달할 수 있습니다.<br>지금 방문하세요!


[forum]: https://github.com/Wiznet/wiznet-iot-shield-mbed-kr/blob/master/docs/imgs/forum.jpg

