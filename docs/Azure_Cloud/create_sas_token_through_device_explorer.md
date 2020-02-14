# Device Explorer를 사용하여 SAS Token 생성하기



## 목차

- [시작하기 전에](#Prerequisites)
- [소개](#Introduction)
- [SAS Token 생성하기](#Create_Sas_Token)



<a name="Prerequisites"></a>
## 시작하기 전에

> [**Azure Portal**][Link-Azure-Portal]에 Login을 합니다. 계정이 없는 경우, 계정 생성 후에 Login을 진행합니다.
>
> ※ 본 문서에는 [**체험 계정**][Link-Azure-Account-Free]으로 진행합니다.



<a name="Introduction"></a>
## 소개

본 문서는 Azure Portal과 [Device Eplorer][Link-Device_Explorer] 사용하여 **SAS Token 생성**하는 방법을 설명합니다.



<a name="Create_Sas_Token"></a>
## SAS Token 생성하기

### 1. IoT Hub에서 **설정** > **공유 액세스 정책**을 선택합니다.

![][Link-Create_Sas_Token_1]

### 2. **iothubowner**를 선택합니다.

![][Link-Create_Sas_Token_2]

### 3. 공유 엑세스 키에서 **연결 문자열 - 기본 키**를 복사합니다.

![][Link-Create_Sas_Token_3]

### 4. **Device Explorer**를 실행합니다.

### 5. **Configure** > **Connection Information** > **IoT Hub Connection String**에 복사한 **연결 문자열 - 기본 키**를 입력하고 Update를 눌러 정보를 갱신합니다.

![][Link-Create_Sas_Token_4]

![][Link-Create_Sas_Token_5]

### 6. **Management** > **SAS Token**을 눌러 엽니다.

![][Link-Create_Sas_Token_6]

### 7. **TTL(Days)** 값 입력한 후, **Generate**를 눌러 생성합니다.

![][Link-Create_Sas_Token_7]



[Link-Azure-Portal]: https://portal.azure.com/
[Link-Azure-Account-Free]: https://azure.microsoft.com/ko-kr/free/
[Link-Create_Sas_Token_1]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/device_explorer_create_sas_token_1.png
[Link-Create_Sas_Token_2]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/cdevice_explorer_reate_sas_token_2.png
[Link-Create_Sas_Token_3]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/device_explorer_create_sas_token_3.png
[Link-Device_Explorer]:https://github.com/Wiznet/azure-iot-kr/blob/master/tools/Device%20Explorer/SetupDeviceExplorer.msi
[Link-Create_Sas_Token_4]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/device_explorer_create_sas_token_4.png
[Link-Create_Sas_Token_5]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/device_explorer_create_sas_token_5.png
[Link-Create_Sas_Token_6]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/device_explorer_create_sas_token_6.png
[Link-Create_Sas_Token_7]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/device_explorer_create_sas_token_7.png