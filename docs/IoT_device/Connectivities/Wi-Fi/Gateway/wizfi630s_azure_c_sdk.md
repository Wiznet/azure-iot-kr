# azure-iot-sdk-c 이용하여 WizFi630S를 Azure IoT Hub에 연결(OpenWrt용)



## 목차
- [시작하기 전에](#What_To_Do)
- [소개](#Learning_Content)
- [디바이스 준비](#Device_Prep)
- [개발 환경 설정](#Develop_Env)
- [예제 수정](#Edit_Exam)
- [예제 실행](#Run_Exam)



<a name="What_To_Do"></a>
## 시작하기 전에

### Hardware Requirement
-   Host PC (Ubuntu)
-   Target Board (WizFi630S)
-   WizFi630S-EVB

### Software Requirement

- 	MS Azure Account (Azure 구독이 아직 없는 경우 체험 무료[계정]을 만듭니다.)
-   Preferred SSH Terminal
-   Azure IoT Explorer

<a name="Learning_Content"></a>
## 소개

본 문서에서는 WizFi630S를 Azure IoT Hub에 접속하는 방법에 대해 설명합니다.
구체적으로는 Host PC(Ubuntu)에서 WizFi630S의 OS인 OpenWrt SDK와 Azure-IoT-SDK-C를 Cross Compile 하고 예제를 WizFi630S에서 실행하는 단계로 구성되어 있습니다.

- Azure IoT Hub 준비
- IoT 디바이스 등록
- Azure IoT와 연결 및 데이터 통신



<a name="Device_Prep"></a>
## 디바이스 준비

### 하드웨어 설정

WizFi630S를 WizFi630S-EVB에 연결합니다. Host PC와 WizFi630S 간에는 Ethernet Interface를 연결하고 SSH 터미널 프로그램을 사용하여 SSH 통신을 연결합니다.

![](/images/wizfi630s_module.png)
![](/images/wizfi630s_evb.png)

<a name="Develop_Env"></a>
## 개발 환경 설정

### OpenWrt SDK 개발환경 설정

#### 사전 준비

```
sudo apt-get update
sudo apt-get install subversion build-essential libncurses5-dev zlib1g-dev gawk git ccache gettext libssl-dev xsltproc zip
```

#### OpenWrt SDK 다운로드

OpenWrt Cross Compile 환경을 구축하기 위해서 OpenWrt SDK를 다운로드해 컴파일 해야합니다.

```
cd ~
mkdir openwrt
cd openwrt
git clone https://git.openwrt.org/openwrt/openwrt.git sdk
```

#### OpenWrt SDK 컴파일

```
cd sdk
make menuconfig
```

menuconfig 에서 아래 정보를 선택 후 저장하고 menuconfig를 종료한다.

-	Target System: MediaTek Ralink MIPS
-	Subtarget: MT76x8 based boards
-	Target Profile: WIZnet WizFi630S


OpenWrt를 컴파일 합니다. 이 작업은 경우에 따라 수 시간이 소요됩니다.

```
make V=s -j1
```

### azure-iot-sdk-c 개발환경 설정

OpenWrt 컴파일이 완료된 이후에 Azure 개발환경을 설정합니다.

#### 사전준비

```
sudo apt-get update
sudo apt-get install -y git cmake build-essential curl libcurl4-openssl-dev libssl-dev uuid-dev
```

#### Azure IoT SDK C 다운로드

```
cd ~
mkdir Azure
cd Azure
git clone https://github.com/Azure/azure-iot-sdk-c.git
cd azure-iot-sdk-c
git submodule update --init
```

#### Makefile.iot 파일 수정
build_all/arduino/ directory에서 Makefile.iot 파일을 수정합니다.

* define Build/Prepare

```
$(CP) ./src/* $(PKG_BUILD_DIR)/
```

을 아래와 같이 변경합니다.

```
$(CP) ./src/azure-iot-sdk-c/* $(PKG_BUILD_DIR)/
```

* define Build/Configure

```
CC="$(TOOLCHAIN_DIR)/bin/$(TARGET_CC)" \
GCC="$(TOOLCHAIN_DIR)/bin/$(TARGET_CC)" \
CXX="$(TOOLCHAIN_DIR)/bin/$(TARGET_CC)" \
``` 

을 아래와 같이 변경합니다.

```
CC="$(TOOLCHAIN_DIR)/bin/mipsel-openwrt-linux-gcc" \
GCC="$(TOOLCHAIN_DIR)/bin/mipsel-openwrt-linux-gcc" \
CXX="$(TOOLCHAIN_DIR)/bin/mipsel-openwrt-linux-g++" \
```

#### build.sh 파일 수정

build_all/arduino/ directory에서 build.sh 파일에서 OpenWrt SDK의 설치 경로에 따라 sdk_root, openwrt_folder, openwrt_sdk_folder를 수정합니다.


<a name=Edit_Exam></a>
## 예제 수정

SDK에서 제공하는 기본 예제들은 아래 링크에서 확인할 수 있습니다.

<https://github.com/Azure/azure-iot-sdk-c/tree/master/iothub_client/samples>

<https://github.com/Azure/azure-iot-sdk-c/tree/master/serializer/samples>


이 가이드에서는 iothub_convenience_sample 예제로 진행합니다.

### Connection String 수정

테스트하고자 하는 예제에서 Connection String을 수정합니다.
Connection String은 Azure IoT Explorer에서 확인할 수 있습니다.

```
static const char* connectionString = "[device connection string]";
```

### build.sh 로 build

```
cd azure-iot-sdk-c/build_all/arduino
sudo ./build.sh
```

<a name="Run_Exam"></a>
## 예제 실행

### 보드로 파일 복사

Azure IoT 예제의 build가 완료되었다면 scp 명령을 사용해서 실행파일을 WizFi630S로 복사합니다.
만약 WizFi630S의 IP가 192.168.1.134라면 scp 명령은 아래와 같습니다.

```
scp ~openwrt/sdk/build_dir/target-mipsel_24kec+dsp_uClibc-0.9.33.2/azure-iot-sdk-c-1/iothub_client/samples/iothub_convenience_sample/iothub_convenience_sample root@192.168.1.134:/tmp
```

### 실행

WizFi630S 보드에서 실행파일을 실행합니다.

```
./tmp/iothub_convenience_sample
```
