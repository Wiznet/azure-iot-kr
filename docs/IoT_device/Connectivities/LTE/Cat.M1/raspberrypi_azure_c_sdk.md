# Raspberry Pi와 Cat.M1 외장형 모뎀을 이용한 Azure IoT Hub 연동

## 목차

-   [소개](#Introduction)
-   [Step 1: 필수 구성 요소](#Step-1-Prerequisites)
-   [Step 2: Raspbian Cross Compile 환경 구축](#Step-2-CrossCompile)
-   [Step 3: 예제 코드 빌드](#Step-3-Build)
-   [Step 4: 에이엠 텔레콤 Cat.M1 외장형 모뎀의 RNDIS 모드 설정](#Step-4-SetRNDIS)
-   [Step 5: 디바이스 준비](#Step-5-PrepareDevice)
-   [Step 6: 예제 실행 결과](#Step-6-Execute)
-   [더 보기](#ReadMore)


<a name="Introduction"></a>
## 소개

본 가이드는 Ubuntu OS에 Docker를 설치하고, Raspbian Cross Compile 환경이 구성되어 있는 Docker Image를 이용하여 Raspbian에서 실행가능한 바이너리를 생성해보도록 하겠습니다.

![][1]

생성된 바이너리를 Raspberry pi에 옮겨 실행하고 에이엠 텔레콤의 Cat.M1 외장형 모뎀을 이용하여 SK Telecom Cat.M1 망을 통해 AZURE IoT Hub로 데이터를 보내는 방법에 대하여 설명합니다.

Raspberry Pi를 이용하면 Cat.M1 모듈의 RNDIS(Remote Network Driver Interface Specification) 기능을 사용할 수 있습니다. RNDIS는 주로 USB 인터페이스를 통해 사용할 수 있으며, 대부분의 Window, Linux, FreeBSD 등 운영체제에 대한 가상 이더넷 링크를 제공합니다.

데스크 탑 PC에서 무선 인터넷을 사용하기 위해 무선 랜카드를 PC USB 포트에 연결하는 것을 예로 들 수 있습니다.이와 같이 Cat.M1 모듈 및 외장형 모뎀을 RNDIS 형태로 사용하면 연결된 기기에서 Cat.M1 망을이용할 수 있습니다.

각 과정에는 다음 내용들이 포함되어 있습니다.
- Ubuntu OS에 Docker image를 이용한 Raspbian Cross Compile 환경 구축
- 에이엠 텔레콤 Cat.M1 외장형 모뎀의 RNDIS 모드 설정
- 에이엠 텔레콤 Cat.M1 외장형 모뎀과 Raspberry pi 연결
- AZURE C SDK 기반으로 Cat.M1을 통해 AZURE IoT Hub로 임의의 온도 데이터 전송


<a name="Step-1-Prerequisites"></a>
## Step 1: 필수 구성 요소
이 문서를 따라하기 전에 다음과 같은 것들이 준비되어야 합니다.

### Hardware
  - Raspberry Pi Model B
  - AMT5920SK(에이엠 텔레콤 Cat.M1 외장형 모뎀, [Openhouse **이용신청**](https://www.sktiot.com/iot/support/openhouse/reservation/openhouseMain) 시, 요청 사항에 AMT5920SK를 구매 요청 부탁드립니다.)
  - SD Card(8G 이상)
  - Ubuntu OS PC

### Software
  - [Docker](https://www.docker.com/)
  - [Raspbian](https://www.raspberrypi.org/downloads/raspbian/)
  - [Raspberry-Pi-IoT-Hub-C-SDK](https://github.com/Azure-Samples/iot-hub-c-raspberrypi-docker)


### Cat.M1 외장형 모뎀 상용개통 
  - Cat.M1 외장형 모뎀으로 통신 기능을 구현하려면 **망 개통 과정**이 선행되어야 합니다.
    - 한국의 경우, 국내 Cat.M1 서비스 사업자인 SK Telecom의 망 개통 과정이 필요합니다.

> 모듈은 개발 단계에 따라 시험망 개통 - 상용망 개통 단계를 거쳐야 하며 외장형 모뎀은 즉시 상용망 개통이 가능합니다.
> - 상용망 개통의 경우 USIM 구매 대리점이나 디바이스 구매처 또는 오픈 하우스에 개통 문의

<a name="Step-2-CrossCompile"></a>
## Step 2: Raspbian Cross Compile 환경 구축
본 가이드에서는 Ubuntu OS 기반에 Docker를 설치하였으며, Raspbian에서 실행할 수 있는 C 기반의 Azure IoT Hub SDK를 받기 위해 다음 명령을 수행합니다.

```cpp
git clone https://github.com/Azure-Samples/iot-hub-c-raspberrypi-docker.git
```
![][2]


다음으로, Ubuntu OS에서 Raspbian Cross Compile 환경이 구축되어 있는 Docker Iamge를 받습니다.
```cpp
sudo docker pull microsoft/iot-hub-c-raspberrypi-build
```
![][3]


<a name="Step-3-Build"></a>
## Step 3: 예제 코드 빌드

### 1) 소스 코드 수정
해당 예제에 센서 데이터를 추가하려면, `iot-hub-c-raspberrypi-docker/samples/azure-iot-hub`폴더로 이동 후, `main.c`소스를 수정 해야합니다.

본 가이드에서는 별도의 센서를 연결하지 않았으므로, 임의의 온도 데이터를 보내도록 수정하겠습니다.

```cpp
static void sendMessageAndBlink(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, char *device_id)
{
    char buffer[256];
    
    
    //snprintf(buffer, sizeof(buffer), "{\"deviceId\":\"%s\",\"messageId\":%d}", device_id, totalBlinkTimes);
    snprintf(buffer, sizeof(buffer), "{\"deviceId\":\"%s\",\"messageId\":%d, \"temperature\":%d}", device_id, totalBlinkTimes, rand() % 30);
    
    // 온도 데이터 추가
    IOTHUB_MESSAGE_HANDLE messageHandle = IoTHubMessage_CreateFromByteArray(buffer, strlen(buffer));
    if (messageHandle == NULL)
    {
        printf("[Device] ERROR: unable to create a new IoTHubMessage\r\n");
    }
    else
    {
        if (IoTHubClient_LL_SendEventAsync(iotHubClientHandle, messageHandle, sendCallback, NULL) != IOTHUB_CLIENT_OK)
        {
            printf("[Device] ERROR: Failed to hand over the message to IoTHubClient\r\n");
        }
        else
        {
            lastMessageSentTime = millis();
            messagePending = true;
            printf("[Device] Sending message #%d: %s\r\n", totalBlinkTimes, buffer);
        }

        IoTHubMessage_Destroy(messageHandle);
    }
}
```

### 2) 프로그램 빌드

Microsoft에서 제공하는 SDK를 Raspbian 환경에서 Cross Compile 하기위해 다음 명령을 수행합니다.

```cpp
sudo docker run --rm -v $PWD/iot-hub-c-raspberrypi-docker/samples:/repo -it microsoft/iot-hub-c-raspberrypi-build /build.sh --source azure-iot-hub
```

정상적으로 빌드가 완료 되면, 다음과 같이 `Build succeeded!` 구문과 실행가능한 바이너리가 생성된것을 확인할 수 있습니다.

![][4]

### 3) 생성된 실행 가능한 바이너리를 SSH를 통해 Raspberry Pi로 이동

SSH를 통해 Ubuntu OS 기반 PC에서 Raspberry pi로 파일을 전송하기 위해서는 Raspberry pi의 SSH 설정을 가능하도록 해야 하며, PC와 Raspberry pi가 같은 네트워크에 있어야 합니다. 

다운로드 받은 SDK 폴더의 `iot-hub-c-raspberrypi-docker/samples`폴더로 이동 후, 다음 명령을 수행합니다.

```cpp
scp -r azure-iot-hub pi@222.98.173.238:/home/pi
scp build/azure-iot-hub/azure-blink pi@222.98.173.238:/home/pi/azure-iot-hub
```

![][5]
<a name="Step-4-SetRNDIS"></a>
## Step 4: 에이엠 텔레콤 Cat.M1 외장형 모뎀의 RNDIS 모드 설정


Cat.M1 모듈을 RNDIS 모드로 사용하기 위해서는 다음과 같은 AT 명령어를 통해 설정이 필요하며, 정상적으로 변경이 되면 모듈은 자동으로 재부팅을 수행합니다.

### 1) RNDIS 모드 설정

**AT Command:** AT@USBCHG=RNDIS

| Type | Syntax | Response | Example
|:--------|:--------|:--------|:--------|
| Write  | AT@USBCHG=(Mode) | @USBCHG:(Mode)<br><br>OK | AT@USBCHG=RNDIS<br>@USBCHG:RNDIS<br><br>OK<br> |

**Defined values:**

| Parameter | Type | Description |
|:--------|:--------|:--------|
| (Mode) | String | ACM : USB 인터페이스를 ACM 모드로 설정<br>RNDIS : USB 인터페이스를 RNDIS 모드로 설정<br>RMNET : USB 인터페이스를 RMNET 모드로 설정 |

### 2) WIoT-AM01 RNDIS 동작 구조 예제

```cpp
/* AMM592SK RNDIS 모드 설정 */

// RNDIS 모드 설정
AT@USBCHG=RNDIS
@USBCHG:RNDIS

OK

// 모듈은 자동으로 재부팅
```

### 3) WIoT-AM01 RNDIS 동작 확인
위와 같이 하드웨어 연결이 정상적으로 되었고 Cat.M1 모듈 또한 RNDIS 모드를 이용하기 위한 설정이 정상적으로 되었다면 다음과 같이 Raspberry Pi에서 Cat.M1 네트워크가 연결된 것을 확인 할 수 있습니다.
![][6]


<a name="Step-5-PrepareDevice"></a>
## Step 5: 디바이스 준비

에이엠 텔레콤 외장형 모뎀과 Raspbarry pi를 아래와 같이 연결합니다.

![][7] 

Raspberry pi와 이 외의 장치는 필요에 맞게 연결하여 사용 합니다.


<a name="Step-6-Execute"></a>
## Step 6: 예제 실행 결과
위의 과정에서 H/W와 S/W가 모두 준비 되었다면, 다음과 같이 실행할 수 있습니다.

```cpp
./azure-blink "<AZURE DEVICE CONNECTION STRING>"
```

![][8]


<a name="ReadMore"></a>
## 더 보기
* [WIZnet IoT Shield(WIoT-QC01)와 STM32L496 Nucleo-144를 이용한 X-CUBE-AZURE SDK 개발 환경 구축][nucleo-stm32l496_azure-st-sdk]
* [Azure Portal을 사용하여 IoT Hub 만들기](https://docs.microsoft.com/ko-kr/azure/iot-hub/iot-hub-create-through-portal)
* [Azure IoT Explorer 사용하기](https://github.com/Azure/azure-iot-explorer#getting-azure-iot-explorer)



[nucleo-stm32l496_azure-st-sdk]: ./nucleo_stm32l496_azure_st_sdk_bg96.md
[st-azure-dashboard]: ../../../../Azure_Cloud/st_azure_dashboard.md

[1]: ../../../../../images/raspberrypi-azure-c-sdk-1.png "크로스 컴파일 환경"
[2]: ../../../../../images/raspberrypi-azure-c-sdk-2.png "깃허브 - iot-hub-c-raspberrypi-docker 다운로드"
[3]: ../../../../../images/raspberrypi-azure-c-sdk-3.png "도커 - Kiot-hub-c-raspberrypi-build 이미지 다운로드"
[4]: ../../../../../images/raspberrypi-azure-c-sdk-4.png "프로그램 빌드"
[5]: ../../../../../images/raspberrypi-azure-c-sdk-5.png "SSH를 통해 파일 전송"
[6]: ../../../../../images/raspberrypi-azure-c-sdk-6.png "RNDIS 동작 확인"
[7]: ../../../../../images/raspberrypi-azure-c-sdk-7.png "디바이스 연결"
[8]: ../../../../../images/raspberrypi-azure-c-sdk-8.png "실행 결과"




