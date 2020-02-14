# WIZnet Azure IoT Documents and Samples
이 Repository는 IoT 장치를 Cat.M1, WiFi, Ethernet을 이용하여 Azure IoT Hub에 연결하고 Azure의 다른 서비스와 연동하기 위해 작성되었습니다.
([docs.microsoft.com](https://docs.microsoft.com/ko-kr/) 사이트를 참고하여 작성되었습니다.)

## Contents
-  [Folder Structure](#Folder)
-  [공통준비사항](#사전준비)
-  [Cat.M1 통신을 이용하여 Azure IoT Hub에 연결하기](#Cat)
-  [WiFi 통신을 이용하여 Azure IoT Hub에 연결하기](#WiFi)
-  [Azure Sphere Guardian(WiFi, Ethernet)을 이용하여 Azure IoT Hub에 연결하기](#Azure_Sphere_Guardian)
-  [Azure IoT Hub와 연동되는 Azure Cloud 서비스 구현하기](#Azure_Cloud)
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
## 공통준비사항
|        Doc         | Description                                                                                           |
| :----------------: | :---------------------------------------------------------------------------------------------------- |
| :heavy_check_mark: | [[MS]Azure IoT Hub 만들기](https://docs.microsoft.com/ko-kr/azure/iot-hub/iot-hub-create-through-portal) |


<a name="Cat"></a>
## Cat.M1 통신을 이용하여 Azure IoT Hub에 연결하기
|        Doc         | Description                                                                                                                                                                                |
| :----------------: | :----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| :heavy_check_mark: | [MCU(STM32)와 Cat.M1(BG96)모듈을 이용하여 Azure IoT Hub에 연결하기](https://github.com/Wiznet/azure-iot-kr/blob/master/docs/IoT_device/Connectivities/LTE/Cat.M1/nucleo_stm32l496_azure_st_sdk_bg96.md) |
| :heavy_check_mark: | [라즈베리파이와 Cat.M1(BG96)모듈을 이용하여 Azure IoT Hub에 연결하기](https://github.com/Wiznet/azure-iot-kr/blob/master/docs/IoT_device/Connectivities/LTE/Cat.M1/raspberrypi_azure_c_sdk.md)                |
| :heavy_minus_sign: | [Cat.M1(BG96)모듈의 AT커맨드로 Azure IoT Hub에 연결하기](https://github.com/Wiznet/azure-iot-kr/blob/master/docs/IoT_device/Connectivities/LTE/Cat.M1/WIZnet_IoT_Shield_Catm1_BG96_Standalone.md)      |

<a name="WiFi"></a>
## WiFi 통신을 이용하여 Azure IoT Hub에 연결하기
|        Doc         | Description                                                                                                                                                                      |
| :----------------: | :------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| :heavy_check_mark: | [WiFi(WizFi630S)모듈의 리눅스에서 Azure IoT Hub에 연결하기](https://github.com/Wiznet/azure-iot-kr/blob/master/docs/IoT_device/Connectivities/Wi-Fi/Gateway/wizfi630s_azure_c_sdk.md)         |
| :heavy_check_mark: | [WiFi(WizFi360)모듈의 MQTT AT커맨드로 Azure IoT Hub에 연결하기](https://github.com/Wiznet/azure-iot-kr/blob/master/docs/IoT_device/Connectivities/Wi-Fi/standalone_mqtt_atcmd_wizfi360.md)   |
| :heavy_check_mark: | [WiFi(WizFi360)모듈의 Azure AT커맨드로 Azure IoT Hub에 연결하기](https://github.com/Wiznet/azure-iot-kr/blob/master/docs/IoT_device/Connectivities/Wi-Fi/standalone_azure_atcmd_wizfi360.md) |

<a name="Azure_Sphere_Guardian"></a>
## Azure Sphere Guardian(WiFi, Ethernet)을 이용하여 Azure IoT Hub에 연결하기
|        Doc         | Description                                                                                                                                                                                             |
| :----------------: | :------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| :heavy_check_mark: | [Azure Sphere Guardian(WIZ-ASG-200)을 이용하여 이더넷 or WiFi로 Azure IoT Hub에 연결하기](https://github.com/Wiznet/azure-iot-kr/blob/master/docs/IoT_device/Connectivities/Ethernet/wiz-asg-200_azure_sphere_sdk.md) |

<a name="Azure_Cloud"></a>
## Azure IoT Hub와 연동되는 Azure Cloud 서비스 구현하기
|        Doc         | Description                                                                                           |
| :----------------: | :---------------------------------------------------------------------------------------------------- |
| :heavy_check_mark: | [Azure Cloud 서비스 구현하기](https://github.com/Wiznet/azure-iot-kr/blob/master/docs/Azure_Cloud/README.md) |

<a name="Support"></a>
## Support

[![WIZnet Developer Forum][forum]](https://forum.wiznet.io/c/korean-forum/oshw/)

**[WIZnet Developer Forum](https://forum.wiznet.io/c/korean-forum/oshw/)** 에서 전세계의 WIZnet 기술 전문가들에게 질문하고 의견을 전달할 수 있습니다.<br>지금 방문하세요!


[forum]: https://github.com/Wiznet/wiznet-iot-shield-mbed-kr/blob/master/docs/imgs/forum.jpg

