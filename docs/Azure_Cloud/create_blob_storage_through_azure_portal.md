# Azure Portal을 사용하여 Blob Storage 만들기



## 목차

- [시작하기 전에](#Prerequisites)
- [소개](#Introduction)
- [Blob Storage 만들기](#Create_Blob_Storage)
- [더 보기](#Read_More)



<a name="Prerequisites"></a>
## 시작하기 전에

> [**Azure Portal**][Link-Azure-Portal]에 Login을 합니다. 계정이 없는 경우, 계정 생성 후에 Login을 진행합니다.
>
> ※ 본 문서에는 [**체험 계정**][Link-Azure-Account-Free]으로 진행합니다.



<a name="Introduction"></a>
## 소개

본 문서는 Azure Portal을 사용하여 **Blob Storage 생성**하는 방법을 설명합니다.

Blob Storage에 대한 내용은 다음을 참조 바랍니다.

- [Azure Blob Storage 소개][Link-Introduction_To_Azure_Blob_Storage]



<a name="Create_Blob_Storage"></a>
## Blob Storage 만들기

### 1. Azure Portal에서 **리소스 만들기** > **저장소** > **Storage 계정**을 선택합니다.

![][Link-Create_Blob_Storage_1]

![][Link-Create_Blob_Storage_2]

### 2. **기본 사항**을 입력 및 설정합니다. 자신이 만든 **IoT Hub와 동일한 리소스 그룹 및 위치**를 선택합니다.

![][Link-Create_Blob_Storage_4]

### 3. **검토 + 만들기**를 선택합니다.

![][Link-Create_Blob_Storage_5]

### 4. 모든 항목을 입력 및 설정을 하였다면, **만들기**를 눌러 Blob Storage를 생성합니다.

![][Link-Create_Blob_Storage_6]

### 5. Blob Storage 생성 후, **개요** > **컨테이너** > **컨테이너**를 선택합니다.

![][Link-Create_Blob_Storage_10]

![][Link-Create_Blob_Storage_11]

### 6. Container의 **이름**을 입력 후, **확인**을 눌러 Container를 생성합니다.

![][Link-Create_Blob_Storage_12]



<a name="Read_More"></a>
## 더 보기

- [Azure Portal을 사용하여 IoT Hub 만들기][Link-Create_IoT_Hub_Through_Azure_Portal]
- [Azure Portal을 사용하여 Stream Analytics 만들기][Link-Create_Stream_Analytics_Through_Azure_Portal]



[Link-Azure-Portal]: https://portal.azure.com/
[Link-Azure-Account-Free]: https://azure.microsoft.com/ko-kr/free/
[Link-Introduction_To_Azure_Blob_Storage]: https://docs.microsoft.com/ko-kr/azure/storage/blobs/storage-blobs-introduction
[Link-Create_Blob_Storage_1]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/create_blob_storage_1.png
[Link-Create_Blob_Storage_2]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/create_blob_storage_2.png
[Link-Create_Blob_Storage_4]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/create_blob_storage_4.png
[Link-Create_Blob_Storage_5]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/create_blob_storage_5.png
[Link-Create_Blob_Storage_6]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/create_blob_storage_6.png
[Link-Create_Blob_Storage_10]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/create_blob_storage_10.png
[Link-Create_Blob_Storage_11]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/create_blob_storage_11.png
[Link-Create_Blob_Storage_12]: https://github.com/Wiznet/azure-iot-kr/blob/master/images/create_blob_storage_12.png
[Link-Create_IoT_Hub_Through_Azure_Portal]: https://docs.microsoft.com/ko-kr/azure/iot-hub/iot-hub-create-through-portal
[Link-Create_Stream_Analytics_Through_Azure_Portal]: https://github.com/Wiznet/azure-iot-kr/blob/master/docs/Azure_Cloud/create_stream_analytics_through_azure_portal.md