# STM32L496 Nucleo-144와 WIZnet IoT Shield(WIoT-QC01)를 이용한 Azure IoT Hub 연동

## 목차

-   [소개](#Introduction)
-   [Step 1: 필수 구성 요소](#Step-1-Prerequisites)
-   [Step 2: 디바이스 준비](#Step-2-PrepareDevice)
-   [Step 3: 주요 소스 설명](#Step-3-SoueceExplanation)
-   [Step 4: 예제 코드 빌드](#Step-4-Build)
-   [Step 5: 예제 실행 결과](#Step-5-Execute)
-   [더 보기](#ReadMore)


<a name="Introduction"></a>
## 소개


이 문서는 [Keil Compiler](https://www.keil.com/)를 기반으로 WIZnet IoT Shield(WIoT-QC01)와 STM32L496 Nucleo-144를 이용하여 X-CUBE-AZURE SDK 개발 환경을 구축하는 방법에 대하여 설명하며, 다음 그림에서 Iot Device 부분을 설명합니다.

![][st-azure-sdk-outline]

각 과정에는 다음 내용들이 포함되어 있습니다.
- WIZnet IoT Shield(WIoT-QC01)와 STM32L496 Nucleo-144 하드웨어 설정
- STM32L496 Nucleo-144와 앰투앰넷 Cat.M1 모듈을 이용하여 X-CUBE-AZURE SDK를 사용하기 위한 수정 사항
- ST X-CUBE AZURE SDK를 기반으로 Cat.M1을 통해 ST AZURE Dashboard로 온도 데이터 전송


<a name="Step-1-Prerequisites"></a>
## Step 1: 필수 구성 요소
이 문서를 따라하기 전에 다음과 같은 것들이 준비되어야 합니다.

### Hardware
  - Keil Compiler를 사용할 수 있는 PC
  - [STM32L496 Nucleo-144](https://www.st.com/content/st_com/en/products/evaluation-tools/product-evaluation-tools/mcu-mpu-eval-tools/stm32-mcu-mpu-eval-tools/stm32-nucleo-boards/nucleo-l496zg.html)
  - WIZnet IoT Shield ([Openhouse **이용신청**](https://www.sktiot.com/iot/support/openhouse/reservation/openhouseMain) 시, 무상으로 3개월간 WIZnet IoT Shield를 대여할 수 있습니다.)
    - WIoT-QC01 (앰투앰넷 BG96)

### Software
  - 디버깅을 위한 시리얼 터미널 프로그램 ([Token2Shell](https://choung.net/token2shell), [PuTTY](https://www.putty.org), [TeraTerm](https://ttssh2.osdn.jp) 등)
  - Keil Compiler(Version: MDK-ARM Plus 5.21a)
  - [X-CUBE-AZURE SDK](https://github.com/Wiznet/azure-iot-kr/)


### Cat.M1 모듈의 (시험 망)개통 
  - Cat.M1 모듈로 통신 기능을 구현하려면 **망 개통 과정**이 선행되어야 합니다.
    - 한국의 경우, 국내 Cat.M1 서비스 사업자인 SK Telecom의 망 개통 과정이 필요합니다.

> 모듈은 개발 단계에 따라 시험망 개통 - 상용망 개통 단계를 거쳐야 하며 외장형 모뎀은 즉시 상용망 개통이 가능합니다.
> - 개발 중인 제품의 시험망 개통인 경우 [SKT IoT OpenHouse](https://www.sktiot.com/iot/support/openhouse/reservation/openhouseMain)에 기술 지원 문의
> - 상용망 개통의 경우 USIM 구매 대리점이나 디바이스 구매처에 개통 문의

<a name="Step-2-PrepareDevice"></a>
## Step 2: 디바이스 준비

### 1) 하드웨어 설정

WIZnet IoT Shield를 STM32L496 Nucleo-144 보드와 결합합니다.
- 두 장치 모두 Arduino UNO Rev3 호환 핀 커넥터를 지원하므로 손쉽게 결합(Stacking) 할 수 있습니다.

![][hw-stack-nucleo] 

WIZnet IoT Shield는 다양한 밴더의 Cat.M1 모듈을 활용 할 수 있도록 하드웨어 설정을 제공합니다. 따라서 선택한 Cat.M1 인터페이스 보드를 확인하여 장치 설정이 필요합니다.

- 각각 밴더의 모듈은 동작 전압, PWRKEY 동작 등에 차이가 있습니다.
- 따라서 Jumper 설정을 통해 인터페이스 보드에 적합한 하드웨어 설정이 선행되어야 합니다.

| :heavy_check_mark: WIoT-QC01 Jumper settings<bR> | WIoT-WM01 Jumper settings | WIoT-AM01 Jumper settings |
|:--------------------------:|:--------------------------:|:--------------------------:|
|![][hw-settings-nucleo-qc01]|![][hw-settings-nucleo-wm01]|![][hw-settings-nucleo-am01]|

> 해당 설정은 각 모듈 별로 Nucleo 보드의 `D2`, `D8` 핀에서 지원하는 UART와 인터페이스 하기 위한 설정입니다. `D0`, `D1` 핀을 UART로 활용하거나 MCU 보드 없이 Standalone 모드로 활용하려는 경우 하드웨어 저장소의 **Settings**를 참고하시기 바랍니다.
> - 본 가이드에서는 WIoT-QC01을 사용하며, `D0`, `D1`핀을 UART로 활용할 예정입니다.


### 2) 디바이스 연결

하드웨어 설정 후 USB 커넥터를 이용하여 STM32L496 Nucleo-144 보드와 PC를 연결합니다. PC 운영체제에서 보드와 연결된 COM 포트를 확인할 수 있습니다.
> 윈도우 운영체제의 경우, 장치 관리자(Device Manager)에서 COM 포트를 확인할 수 있습니다.

![][1]


> 장치 관리자에서 COM 포트를 확인할 수 없는 경우, 다음 링크에서 드라이버를 다운로드하여 설치하시기 바랍니다.
>  * [ST Link USB driver for Windows 7, 8, 10][st-usb-driver]



<a name="Step-3-SoueceExplanation"></a>
## Step 3: 주요 소스 설명
ST에서 제공하는 X-CUBE-AZURE SDK의 32L496GDISCOVERY 프로젝트를 기반으로 다음과 같이 수정 및 추가하였습니다.

- **32L496GDISCOVERY(STM32L496AG) => STM32L496-Nucleo(STM32L496ZG)**
  - Virtual COM port UART: UART2 => LPUART1
  - Cat.M1 모듈과 통신하는 UART: USART1 => USART3
  - Cat.M1 모듈 PWR 핀: PD3 => PD15
  - Cat.M1 모듈 RST 핀: PB2 => PF13

- **Sequance Cat.M1 모듈 => WIoT-QC01(앰투앰넷 Cat.M1 모듈)**
  - APN 설정
  - IPv4 기반 주소 처리

- **HAL 기반 ADC를 이용하여 WIZnet IoT Shield의 온도 센서 사용**


### Virtual COM port UART
```cpp
static void Console_UART_Init(void)
{
  /* 중략 */
  console_uart.Instance = LPUART1;
  console_uart.Init.BaudRate = 115200;
  console_uart.Init.WordLength = UART_WORDLENGTH_8B;
  console_uart.Init.StopBits = UART_STOPBITS_1;
  console_uart.Init.Parity = UART_PARITY_NONE;
  console_uart.Init.Mode = UART_MODE_TX_RX;
  console_uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  //console_uart.Init.OverSampling = UART_OVERSAMPLING_16;
  /* 중략 */
}

// Virtual Port를 사용하기 위해 LPUART1 uartHandler 추가 및 Pheriperal 초기화
void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{
  /* 중략 */
  else if(uartHandle->Instance==LPUART1)
  {
  /* USER CODE BEGIN LPUART1_MspInit 0 */
	__HAL_RCC_GPIOG_CLK_ENABLE();
  /* USER CODE END LPUART1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_LPUART1_CLK_ENABLE();
  
    /*LPUART1 GPIO Configuration    
    PG7     ------> LPUART1_TX
    PG8     ------> LPUART1_RX 
    */
    
    GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_LPUART1;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
  /* USER CODE BEGIN LPUART1_MspInit 1 */
  /* USER CODE END LPUART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{
 /* 중략 */
  else if(uartHandle->Instance==LPUART1)
  {
    /* Peripheral clock disable */
    __HAL_RCC_LPUART1_CLK_DISABLE();
  
    /**LPUART1 GPIO Configuration    
    PG7     ------> LPUART1_TX
    PG8     ------> LPUART1_RX 
    */
    HAL_GPIO_DeInit(GPIOG, GPIO_PIN_7|GPIO_PIN_8);
  }
}
```

### Cat.M1 모듈과 통신하는 UART
```cpp
void MX_USART3_UART_Init(void)
{
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200 ;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_RTS_CTS;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
}

// USART3를 사용하기 위해 USART3 uartHandler 추가 및 Pheriperal 초기화
void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* 중략 */
  else if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */
  /* USER CODE END USART1_MspInit 0 */
  
    /* USART1 clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART3_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
    /* USER CODE BEGIN USART1_MspInit 1 */
    /* disable IRQ to avoid problems with IPC - will be reactivated later */
    HAL_NVIC_DisableIRQ(USART3_IRQn);
  /* USER CODE END USART1_MspInit 1 */
  }
  /* 중략 */
}
void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{
  /* 중략 */
  else if(uartHandle->Instance==USART3)
  {
    /* Peripheral clock disable */
    __HAL_RCC_USART3_CLK_DISABLE();

    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_8|GPIO_PIN_9);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART3_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */
	HAL_NVIC_DisableIRQ(USART3_IRQn);
  /* USER CODE END USART1_MspDeInit 1 */
  }
  /* 중략 */
}
```

### Cat.M1 모듈 PWR/RST 핀 수정
```cpp
#define MDM_PWR_EN_Pin GPIO_PIN_15
#define MDM_PWR_EN_GPIO_Port GPIOD

#define MDM_RST_Pin GPIO_PIN_13
#define MDM_RST_GPIO_Port GPIOF
```

### APN(Access Point Name) 및 PDP(Packet Data Protocol) 설정
```cpp


 //memcpy(cellular_params.sim_slot[1].apn, "EM", (size_t)sizeof("EM"));
 memcpy(cellular_params.sim_slot[1].apn, "", (size_t)sizeof("")); //SKT Telecom 망을 사용하기 위해서는 APN을 ""으로 설정
 
 /* Initialize all other fields */
 //cellular_params.set_pdn_mode = 1U;
 cellular_params.set_pdn_mode = 3U; //Context Type을 "IPv4v6"로 설정
 cellular_params.target_state = DC_TARGET_STATE_FULL;
 cellular_params.nfmc_active  = 0U;
```

### IPv4 기반 주소 처리
```cpp
int tcpsocketconnection_connect(TCPSOCKETCONNECTION_HANDLE tcpSocketConnectionHandle, const char* host, const int port)
{
  TCPSOCKETCONNECTION_CONNECTIVITY *psock = (TCPSOCKETCONNECTION_CONNECTIVITY *)(tcpSocketConnectionHandle);
  sockaddr_in_t addr;
  int rc = NET_OK;
  addr.sin_len    = sizeof(sockaddr_in_t);
  
  /* 중략 */
  // 기존 ST AZURE SDK의 경우 DNS Query응답을 IPv4로 처리하지만, WIoT-QC01는 DNS Query를 수행하면 IPv6를 return 하기 때문에,
  // 도메인 주소에 맞는 IPv4 기반의 IP주소를 static하게 return 하도록 처리하였음
  
  if( strstr(host, "global") != 0)   //  "global.azure-devices-provisioning.net"
	addr.sin_addr = net_aton_r((const char_t*)"23.100.8.130");
  else if(strstr(host, "stm32") != 0) // "stm32-iothub-prod.azure-devices.net"
	addr.sin_addr = net_aton_r((const char_t*)"13.69.192.43");
    
  addr.sin_port = NET_HTONS(port);
  rc = net_connect(psock->socket, (sockaddr_t *)&addr, sizeof(addr));
  if (rc != NET_OK)
  {
    msg_error("Could not connect to %s:%d (%d)\n", host, port, rc);
  }
  return rc;
}
```

### HAL 기반 ADC를 이용한 온도센서 사용
```cpp
static void MX_ADC1_Init(void)
{
  ADC_MultiModeTypeDef multimode = {0};
  ADC_ChannelConfTypeDef sConfig = {0};

  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the ADC multi-mode 
  */
  multimode.Mode = ADC_MODE_INDEPENDENT;
  if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel 
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
}
/* Private sensor variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
uint16_t value;
uint16_t vout;
float temp1;

/* 중략 */
#else /* SENSOR */
          /* Serialize the device data. */
          if (SERIALIZE(&destination, &destinationSize,
                mdl->mac, mdl->TEMPERATURE, //사용할 데이터를 Serialization
#if defined(AZURE_DPS_PROV)
                mdl->deviceId,
#endif /* AZURE_DPS_PROV */
                mdl->ts) != CODEFIRST_OK)
#endif /* SENSOR */
          {
            msg_error("Failed to serialize.\n");
          }
          else
          {
            IOTHUB_MESSAGE_HANDLE msgHnd = NULL;
            /* Create the message. */
            /* Note: The message is destroyed by the confirmation callback. */
					
		    HAL_ADC_Start(&hadc1);
            HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
            value = HAL_ADC_GetValue(&hadc1);
            HAL_ADC_Stop(&hadc1);
            
            //ADC로 측정한 값을 온도 데이터로 변환
            vout = 3100 * value / 4096;
            temp1 = (float)vout / 10.0;
			
			mdl->TEMPERATURE=temp1;
            if ((msgHnd = IoTHubMessage_CreateFromByteArray(destination, destinationSize)) == NULL)
            {
              msg_error("Failed allocating an iotHubMessage.\n");
            }
            else
            {
              /* Send the message. */
              if (IoTHubClient_LL_SendEventAsync(device->iotHubClientHandle, msgHnd, SendConfirmationCallback, msgHnd) != IOTHUB_CLIENT_OK)
              {
                msg_error("IoTHubClient_LL_SendEventAsync failed.\n");
              }
/* 중략 */
```


<a name="Step-4-Build"></a>
## Step 4: 예제 코드 빌드 및 실행

### 1) Keil 프로젝트 열기

[Step 1: 필수 구성 요소](#Step-1-Prerequisites)의 Software란의 [X-CUBE-AZURE SDK](https://github.com/Wiznet/azure-iot-kr)를 Local 저장소에 저장하고, 다음 경로 `azure-iot-kr-master\samples\LTE\Cat.M1\en.x-cube-azure_v2\AZURE_V1.2.0\Projects\STM32L496ZG-Nucleo\Applications\Cloud\Azure\MDK-ARM`에 위치한 Keil 프로젝트를 실행합니다.

정상적으로 실행되면 다음과 같은 Workspace를 확인 할 수 있으며, 좌측에서 Project Window를 통해 해당 프로젝트에 포함된 소스를 트리형식으로 확인할 수 있습니다.

![][2]

### 2) 프로그램 빌드

상단 메뉴의 `Build` 및 `ReBuild` 버튼을 클릭합니다.

![][build]

예제 프로젝트가 성공적으로 빌드 되면 `STM32L496G-Discovery_Azure.bin`이 생성됩니다.
> 본 예제는 기존의 `STM32L496G-Discovery_Azure`프로젝트를 기반으로 수정하였기 때문에 해당 이름의 bin 파일이 생성되지만, 위에서 수정한 소스가 반영되어 있으므로 STM32L496 Nucleo-144 보드에서 정상 동작합니다.

![][3]

이 파일을 `Drag & Drop`하여 연결된 MBED target 보드에 삽입하면 firmware write가 완료됩니다.

> MBED 플랫폼 보드는 `NODE_L496ZG (E:)`와 같은 이름의 디스크 드라이브로 할당되어 있습니다. 이 곳에 생성된 펌웨어 바이너리 파일을 복사하면 됩니다.

### 3) 시리얼 터미널 연결

시리얼 터미널 프로그램을 실행하여 **디바이스 연결** 단계에서 확인한 보드의 COM 포트와 Baudrate 115200을 선택하여 시리얼 포트를 연결합니다.

> 디버그 메시지 출력용 시리얼 포트 설정 정보: 115200-8-N-1, None


<a name="Step-5-Execute"></a>
## Step 5: 예제 실행 결과

펌웨어가 정상적으로 업로드되면, ST AZURE Dashboard에 디바이스를 등록할 때 사용되는 `DeviceID`를 확인해야 합니다.

![][4]


디바이스가 ST AZURE Dashboard로 데이터를 보낼 준비가 되었다면 다음과 같은 디버그 메시지를 확인 할 수 있습니다.

![][5]

`STM32L496 Nucleo-144`보드 하단 좌측의 `User button`을 누르면, ST AZURE Dashboard로 현재의 온도를 측정하여 온도 데이터를 전송합니다.
데이터를 정상 적으로 보내면 다음과 같은 디버그 메시지를 확인할 수 있습니다.

![][6]


`User button`을 2번 연속으로 누르면, 5초마다 온도 데이터를 주기적으로 전송하며 정상적으로 실행되면 다음과 같이 `Enter the sensor values publication loop.` 메시지를 확인할 수 있습니다.

![][7]

또한, 주기적으로 전송하는 것을 종료하기 위해서도 동일하게 `User button`을 2번 연속으로 누르면되며, 정상적으로 종료되면 다음과 같이 `Exit the sensor values publication loop.`메시지를 확인할 수 있습니다.

![][8]

이와 같이 X-CUBE-AZURE SDK 기반으로 WIZnet IoT Shield(WIoT-QC01)와 STM32L496 Nucleo-144를 이용하여 ST AZURE Dashboard에 데이터 보내는 것을 확인 해보았습니다.


<a name="ReadMore"></a>
## 더 보기
* [ST AZURE Dashboard 가이드][st-azure-dashboard]
* [Raspberry Pi를 이용한 X-CUBE-AZURE SDK 개발 환경 구축][raspberrypi-azure-c-sdk]




[st-usb-driver]: https://www.st.com/en/development-tools/stsw-link009.html

[st-azure-dashboard]: ../../../../Azure_Cloud/st_azure_dashboard.md
[raspberrypi-azure-c-sdk]: ./Azure_Cloud/raspberrypi_azure_c_sdk.md

[hw-stack-nucleo]: ../../../../../images/wiot-shield-qc01-nucleo-144.png "Nucleo144와 WIZnet IoT Shield 결합"

[hw-settings-nucleo-qc01]: ../../../../../images/WIoT-QC01_JUMP_Arduino_serialD2_D8.png "hw-settings-nucleo-qc01"
[hw-settings-nucleo-wm01]: ../../../../../images/WIoT-WM01_JUMP_Arduino_serialD2_D8.png "hw-settings-nucleo-wm01"
[hw-settings-nucleo-am01]: ../../../../../images/WIoT-AM01_JUMP_Arduino_serialD2_D8.png "hw-settings-nucleo-am01"

[st-azure-sdk-outline]: ../../../../../images/st-azure-sdk-outline.PNG "ST AZURE SDK 개요도"

[build]: ../../../../../images/keil-build.png "Keil 빌드"
[1]: ../../../../../images/nucleo-stm32l496-azure-st-sdk-1.png "장치 관리자"
[2]: ../../../../../images/nucleo-stm32l496-azure-st-sdk-2.png "Keil 프로젝트 화면"
[3]: ../../../../../images/nucleo-stm32l496-azure-st-sdk-3.png "Keil 프로젝트 빌드 결과"
[4]: ../../../../../images/nucleo-stm32l496-azure-st-sdk-4.png "실행 결과_DeviceId 확인"
[5]: ../../../../../images/nucleo-stm32l496-azure-st-sdk-5.png "실행 결과_보낼 준비 완료"
[6]: ../../../../../images/nucleo-stm32l496-azure-st-sdk-6.png "실행 결과_데이터 보내기"
[7]: ../../../../../images/nucleo-stm32l496-azure-st-sdk-7.png "실행 결과_데이터 주기적으로 보내기"
[8]: ../../../../../images/nucleo-stm32l496-azure-st-sdk-8.png "실행 결과_데이터 주기적으로 보내기 종료"



