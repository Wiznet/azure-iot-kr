# 개발 환경

## [필수 요소: ](https://docs.microsoft.com/ko-kr/azure-sphere/install/overview)

* Windows 10 1주년 업데이트 이상을 실행하는 PC 또는 Ubuntu 18.04 LTS를 실행하는 Linux 머신
* Visual Studio 2019 Enterprise, Professional, Community 버전 16.04 이상
  * Visual Studio용 Azure Sphere SDK Preview 설치

## Checklist:

### 환경 설정
- FTDI 드라이버가 설치되고, 장치관리자에 3개의 COM 포트가 인식되었는지 확인
- 최신 버전의 [Visual Studio](https://www.visualstudio.com/) 와 [Visual Studio용 Azure Sphere SDK](https://docs.microsoft.com/ko-kr/azure-sphere/install/install-sdk) 설치
- 구독이 있는 Azure 계정 필요 
  
### Azure Sphere CLI

`Azure Sphere Developer Command Prompt Preview` 실행 후, 아래 절차 진행

- `azsphere login` command를 이용해 로그인
  - 새 계정으로 로그인 시 `azsphere login --newuser <MS account>` 로 계정 추가
- 기존 tenant 가 없는 경우 `azsphere tenant create -n <tenant name>` 를 이용해 Azure Sphere tenant 생성
- `azsphere tenant select -i <tenant id>` 로 Azure Sphere tenant 선택

- (새 보드만 해당) `azsphere device claim` 을 통해 현재 선택된 tenant에 디바이스를 클레임

- WiFi 설정 및 연결

  `azsphere device wifi add --ssid <yourSSID> --psk <yourNetworkKey>`
   
   - 보안설정이 없는 Wi-Fi 네트워크 연결에서는 --psk 옵션 생략
   - SSID 나 패스워드에 스페이스가 있는 경우 " " 로 처리  e.g. --ssid "My AP"

-  Wi-Fi 상태 확인
   
   `azsphere device wifi show-status`


- `azsphere device recover` 로 최적의 Azure Sphere OS 버전으로 디바이스 복구
  
- Azure Sphere 개발보드를 PC에 연결하고 Azure Sphere utility 에서 디바이스를 디버그 모드로 전환 (OTA는 비활성화됨)    
  `azsphere device enable-development`



## 이더넷 인터페이스 사용 설정

이더넷을 사용하려면 애플리케이션 이미지 외에 보드 구성 이미지가 필요

- Preset 설정으로부터 보드 구성 이미지 패키지 생성
  
  `azsphere image-package pack-board-config --preset lan-enc28j60-isu0-int5 --output enc28j60-isu0-int5.imagepackage`

- 이미지 패키지 로드
  
  `azsphere device sideload deploy --imagepackage enc28j60-isu0-int5.imagepackage`

----

# 2 Port Application (with IoT Hub)

## Azure IoT Hub 설정

* [Azure Sphere에 대해 Azure IoT Hub 설정](https://docs.microsoft.com/ko-kr/azure-sphere/app-development/setup-iot-hub) 페이지 절차를 따라 클라우드 자원 설정


## app_manifest.json 설정 수정

* DeviceAuthentication: Tenant ID
  * 연결된 디바이스와 클레임 된 tenant ID
  * `azsphere tenant show-selected` 명령으로 확인
* CmdArgs: Azure device provisioning service(DPS)의 Scope ID
* Capabilities - AllowedConnections: Azure IoT Hub의 endpoint URL
  * Global access link는 그대로 유지 (global.azure-devices.provisioning.net)

## Application 실행

* 상단 메뉴 아래의 '시작 항목 선택' 에서 **GDB Debugger (HLCore)** 선택
* 메뉴 - 빌드 - 모두 빌드
* 선택된 **GDB Debugger (HLCore)** 버튼을 누르거나, 키보드의 F5를 눌러 어플리케이션 실행


## 실행 Log
...

----
----

# Azure Sphere Samples
This repository contains samples for the [Azure Sphere platform](https://www.microsoft.com/azure-sphere/).

## Using the samples
See the [Azure Sphere Getting Started](https://www.microsoft.com/en-us/azure-sphere/get-started/) page for details on getting an [Azure Sphere Development kit](https://aka.ms/AzureSphereHardware) and setting up your PC for development. You should complete the Azure Sphere [Installation](https://docs.microsoft.com/azure-sphere/install/overview) and [Quickstarts](https://docs.microsoft.com/azure-sphere/quickstarts/qs-overview) to validate that your environment is configured properly before using the samples here. 

Clone this entire repository locally. The repository includes the [hardware definition files](./Hardware/) that are required to run the samples on a range of Azure Sphere hardware.

Each folder within the samples subdirectory contains a README.md file that describes the samples therein. Follow the instructions for each individual sample to build and deploy it to your Azure Sphere hardware to learn about the features that the sample demonstrates.

## Contributing
This project welcomes contributions and suggestions. Most contributions require you to agree to a Contributor License Agreement (CLA) declaring that you have the right to, and actually do, grant us the rights to use your contribution. For details, visit https://cla.microsoft.com.

When you submit a pull request, a CLA-bot will automatically determine whether you need to provide a CLA and decorate the PR appropriately (e.g., label, comment). Simply follow the instructions provided by the bot. You will only need to do this once across all repos using our CLA.

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/).
For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or
contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.

## Licenses

For information about the licenses that apply to a particular sample, see the License and README.md files in each subdirectory. 
