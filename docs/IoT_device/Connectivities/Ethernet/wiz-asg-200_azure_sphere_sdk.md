# Azure Sphere Gardia(WIZ-AZG-200)을 이용하여 이더넷 or WiFi로 Azure IoT Hub에 연결

## 목차

- [소개](#overview)
  - [Features](#features)
- [Step 1: 필수 구성 요소](#step-1-prerequisites)
- [Step 2: Device 준비](#Step-2-device-configuration)
- [Step 3: Application 설정](#step-3-application-setting)
- [Step 4: Application 실행](#step-4-build-and-run)

- [참고 문서](#reference)


<a name="overview"></a>
## 소개

### WIZ-ASG-200 (Azure Sphere Guardian)

![WIZASG200]

WIZ-ASG-200은 IoT 보안성능을 강화한 2포트 이더넷 및 WiFi Edge 모듈입니다. Microsoft Azure Sphere모듈을 사용하여 Microsoft가 제공하는 강력한 보안 서비스를 받을 수 있습니다. WIZnet W5500 Hardwired TCP/IP 이더넷 칩을 통해 IoT 장치에 연결될 수 있으며 Azure Sphere의 듀얼 밴드 Wi-Fi 또는 이더넷(ENC28J60)으로 장치에 안전하게 연결됩니다. WIZ-ASG-200을 사용하면 기존의 유선네트워크 환경의 장치가 쉽고 저렴하게 강력한 보안 솔루션을 탑제할 수 있습니다.


### Features

- Built-in Azure Sphere MT3620 Module
- Support MS Pluton security system in MT3620
- Support Ethernet or Dual Wi-Fi (2.4/5GHz) interface for Azure connection
- Support Auto Ethernet and Wi-Fi switching for stable Azure connection
- Support USB Interface for debug, service & recovery UARTs, and JTAG
- Available in minimizing the existing wired network to support greenfield + brownfield
- Support variety protocols for brownfield wired network : TCP, UDP, ICMP, IPv4, ARP, IGMP, PPPoE 


본 문서는 WIZ-ASG-200을 이용하여 이더넷 또는 WiFi로 MS Azure IoT Hub에 연동하는 방법에 대한 가이드를 제공합니다. 

<img src="https://github.com/Wiznet/azure-iot-kr/blob/master/images/wiz-asg-200-demo.png?raw=true">


<a name="step-1-prerequisites"></a>
## Step 1: 필수 구성 요소

이 문서를 따라가기 전에 아래 항목들이 준비되어야 합니다.

### Hardware

* WIZ-ASG-200 
* Micro 5pin usb cable
* 5V 2A 전원 케이블

### Software 

[[MS] Azure Sphere: 설정 프로시저 개요](https://docs.microsoft.com/ko-kr/azure-sphere/install/overview)

* Windows 10 1주년 업데이트 이상을 실행하는 PC 또는 Ubuntu 18.04 LTS를 실행하는 Linux 머신
* Visual Studio 2019 Enterprise, Professional, Community 버전 16.04 이상
* Visual Studio용 Azure Sphere SDK Preview 설치
* 구독이 있는 Azure 계정

<a name="step-2-device-configuration"></a>
## Step 2: WIZ-ASG-200 디바이스 설정

### 디바이스 연결 확인

PC와 디바이스를 micro 5 pin USB 케이블로 연결하면 FTDI 드라이버가 자동 설치됩니다. 설치가 완료되면 장치관리자에 3개의 COM 포트가 인식되었는지 확인합니다.

### 초기 디바이스 설정

필수 구성 요소의 `Visual Studio용 Azure Sphere SDK Preview`가 설치되면 `Azure Sphere Developer Command Prompt Preview`를 사용할 수 있습니다.

다음 과정을 통해 디바이스 초기 설정을 진행합니다.

* `azsphere login` command를 이용해 로그인
  * 새 계정으로 로그인 시 `azsphere login --newuser <MS account>` 로 계정 추가
* 기존 tenant 가 없는 경우 `azsphere tenant create -n <tenant name>` 를 이용해 Azure Sphere tenant 생성
* `azsphere tenant select -i <tenant id>` 로 Azure Sphere tenant 선택

* (새 보드만 해당) `azsphere device claim` 을 통해 현재 선택된 tenant에 디바이스를 클레임

* WiFi 설정 및 연결

  `azsphere device wifi add --ssid <yourSSID> --psk <yourNetworkKey>`
   
   * 보안설정이 없는 Wi-Fi 네트워크 연결에서는 --psk 옵션 생략
   * SSID 나 패스워드에 스페이스가 있는 경우 " " 로 처리  e.g. --ssid "My AP"

*  Wi-Fi 상태 확인
   
   `azsphere device wifi show-status`


* `azsphere device recover` 로 최적의 Azure Sphere OS 버전으로 디바이스 복구
  
* Azure Sphere 개발보드를 PC에 연결하고 Azure Sphere utility 에서 디바이스를 디버그 모드로 전환 (OTA는 비활성화됨)    
  `azsphere device enable-development`


### 이더넷 인터페이스 사용 설정

[[MS] Azure Sphere: 이더넷에 연결](https://docs.microsoft.com/ko-kr/azure-sphere/network/connect-ethernet#board-configuration)

이더넷 인터페이스를 사용하기 위한 설정을 진행합니다. 이더넷을 사용하려면 어플리케이션 이미지 외에 보드 구성 이미지가 필요합니다.

1. 보드 구성 이미지 패키지 생성 (Preset 설정 사용)
  
  `azsphere image-package pack-board-config --preset lan-enc28j60-isu0-int5 --output enc28j60-isu0-int5.imagepackage`

2. 이미지 패키지 로드
  
  `azsphere device sideload deploy --imagepackage enc28j60-isu0-int5.imagepackage`


### IoT Hub 설정

아래 링크 페이지 절차를 따라 클라우드 자원을 설정하고, 디바이스 인증 등록을 위한 절차를 진행합니다.

* [[MS] Azure Sphere에 대해 Azure IoT Hub 설정](https://docs.microsoft.com/ko-kr/azure-sphere/app-development/setup-iot-hub) 


<a name="step-3-application-setting"></a>
## Step 3: 예제 코드 설정

### app_manifest.json 수정

app_manifest.json 설정 파일에서 아래 항목들을 입력합니다.

* DeviceAuthentication: Azure sphere tenant ID
  * 연결된 디바이스와 클레임 된 tenant ID
  * `azsphere tenant show-selected` 명령으로 확인
* CmdArgs: Azure device provisioning service(DPS)의 Scope ID
* Capabilities - AllowedConnections: Azure IoT Hub의 endpoint URL
  * Global access link는 그대로 유지 (global.azure-devices.provisioning.net)


<a name="step-4-build-and-run"></a>
## Step 4: 예제 코드 빌드 및 실행

Visual Studio를 사용하여 Application을 빌드하고 실행합니다.

* 상단 메뉴 아래의 '시작 항목 선택' 에서 **GDB Debugger (HLCore)** 선택
* 메뉴 - 빌드 - 모두 빌드 클릭
* 선택된 **GDB Debugger (HLCore)** 버튼을 클릭하거나 키보드의 F5를 눌러 어플리케이션 실행


----

<a name="reference"></a>
## 참고 문서

[Azure Sphere에서 이더넷에 연결](https://docs.microsoft.com/ko-kr/azure-sphere/network/connect-ethernet)

[WIZ-ASG-200를 이용하여 Azure IoT Hub에 연결하기](https://docs.microsoft.com/ko-kr/azure-sphere/app-development/use-azure-iot)

[Azure Sphere Bootcamp](https://github.com/azsphere/Azure-Sphere-Bootcamp)

[Azure Sphere Samples: AzureIoT](https://github.com/Azure/azure-sphere-samples/blob/master/Samples/AzureIoT/IoTHub.md)


[WIZASG200]: ../../../../images/WIZ-ASG-200.png
