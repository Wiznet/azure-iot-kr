# SAS Token 생성하기



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

본 문서는 Azure Portal과 [Azure IoT Explorer][Link-Azure_IoT_Explorer] 사용하여 **SAS Token 생성**하는 방법을 설명합니다.



<a name="Create_Sas_Token"></a>
## SAS Token 생성하기

### 1. IoT Hub에서 **설정** > **공유 액세스 정책**을 선택합니다.

![][Link-Create_Sas_Token_1]

### 2. **iothubowner**를 선택합니다.

![][Link-Create_Sas_Token_2]

### 3. 공유 엑세스 키에서 **연결 문자열 - 기본 키**를 복사합니다.

![][Link-Create_Sas_Token_3]

### 4. **Azure IoT Explorer**를 실행합니다.

### 5. **연결 문자열 - 기본 키**를 입력하고 **Connent** 눌러 접속합니다.

![][Link-Create_Sas_Token_4]

### 6. IoT Device 목록에서 IoT Device 선택 후, 눌러 엽니다.

![][Link-Create_Sas_Token_5]

### 7. **Device identity** > **Connection String with SAS token**을 눌러 엽니다.

![][Link-Create_Sas_Token_6]

### 8. **Symmetric key**, **Expiration(minutes)** 값 입력 후, **Generate**를 눌러 생성합니다.

![][Link-Create_Sas_Token_8]

![][Link-Create_Sas_Token_9]

![][Link-Create_Sas_Token_10]



[Link-Azure-Portal]: https://portal.azure.com/
[Link-Azure_IoT_Explorer]: https://github.com/Azure/azure-iot-explorer/releases
[Link-Azure-Account-Free]: https://azure.microsoft.com/ko-kr/free/
[Link-Create_Sas_Token_1]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/azure_iot_explorer_create_sas_token_1.png
[Link-Create_Sas_Token_2]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/azure_iot_explorer_create_sas_token_2.png
[Link-Create_Sas_Token_3]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/azure_iot_explorer_create_sas_token_3.png
[Link-Create_Sas_Token_4]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/azure_iot_explorer_create_sas_token_4.png
[Link-Create_Sas_Token_5]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/azure_iot_explorer_create_sas_token_5.png
[Link-Create_Sas_Token_6]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/azure_iot_explorer_create_sas_token_6.png
[Link-Create_Sas_Token_8]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/azure_iot_explorer_create_sas_token_8.png
[Link-Create_Sas_Token_9]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/azure_iot_explorer_create_sas_token_9.png
[Link-Create_Sas_Token_10]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/azure_iot_explorer_create_sas_token_10.png