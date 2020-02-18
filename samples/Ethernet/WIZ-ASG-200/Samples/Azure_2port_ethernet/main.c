/* Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License. */

// This sample C application for Azure Sphere demonstrates Azure IoT SDK C APIs
// The application uses the Azure IoT SDK C APIs to
// 1. Use the buttons to trigger sending telemetry to Azure IoT Hub/Central.
// 2. Use IoT Hub/Device Twin to control an LED.

// You will need to provide four pieces of information to use this application, all of which are set
// in the app_manifest.json.
// 1. The Scope Id for your IoT Central application (set in 'CmdArgs')
// 2. The Tenant Id obtained from 'azsphere tenant show-selected' (set in 'DeviceAuthentication')
// 3. The Azure DPS Global endpoint address 'global.azure-devices-provisioning.net'
//    (set in 'AllowedConnections')
// 4. The IoT Hub Endpoint address for your IoT Central application (set in 'AllowedConnections')

#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <ctype.h>
#include <assert.h>
#include <unistd.h>

// applibs_versions.h defines the API struct versions to use for applibs APIs.
#include "applibs_versions.h"
#include <applibs/log.h>
#include <applibs/wificonfig.h>
#include <applibs/networking.h>
#include <applibs/gpio.h>
#include <applibs/storage.h>

// By default, this sample's CMake build targets hardware that follows the MT3620
// Reference Development Board (RDB) specification, such as the MT3620 Dev Kit from
// Seeed Studios.
//
// To target different hardware, you'll need to update the CMake build. The necessary
// steps to do this vary depending on if you are building in Visual Studio, in Visual
// Studio Code or via the command line.
//
// See https://github.com/Azure/azure-sphere-samples/tree/master/Hardware for more details.
//
// This #include imports the sample_hardware abstraction from that hardware definition.
// #include <hw/sample_hardware.h>

// Azure sphere Avnet starter kit
#include <hw/avnet_mt3620_sk.h>

#include "epoll_timerfd_utilities.h"

// Azure IoT SDK
#include <iothub_client_core_common.h>
#include <iothub_device_client_ll.h>
#include <iothub_client_options.h>
#include <iothubtransportmqtt.h>
#include <iothub.h>
#include <azure_sphere_provisioning.h>

#include "wizchip_conf.h"
#include "W5500/w5500.h"
#include "socket.h"

static volatile sig_atomic_t terminationRequired = false;

#include "parson.h" // used to parse Device Twin messages.

#define sampleData 0    // sample data for simulation

void InitPublicEthernet(void);
void InitPublicWifi(void);

//int interface_status;
Networking_InterfaceConnectionStatus interface_status;
Networking_InterfaceMedium_Type mediumType;
int interface_num;
int interface_enable;

#if 1 // network status
static bool ethernet_connected = false;
static bool net_switch_toggle = true;
static bool azure_connected = false;
static bool wifiConnected = false;
static bool wifi_connected = false;
unsigned int storedNetworkCnt = 0;
#endif

#if 1
// The MT3620 currently handles a maximum of 37 stored wifi networks.
static const unsigned int MAX_NUMBER_STORED_NETWORKS = 37;

// Array used to print the network security type as a string
static const char* securityTypeToString[] = { "Unknown", "Open", "WPA2/PSK" };

static int sampleStoredNetworkId = -1;
static int storedWifiId = 0;

// Available states
static int OutputStoredWifiNetworks(void);
static int WifiRetrieveStoredNetworks(ssize_t* numberOfNetworksStored,
    WifiConfig_StoredNetwork* storedNetworksArray);
static void WifiNetworkConfigureAndAddState(void);
static void WifiNetworkEnableState(void);
static void WifiNetworkDisableState(void);
static void WifiNetworkDeleteState(void);
#endif

// Network
static const char EthernetInterface[] = "eth0";
static const char WifiInterface[] = "wlan0";
static bool isNetworkStackReady = false;

uint8_t gDATABUF[1024];

#define DATA_BUF_SIZE 2048

// Default Static Network Configuration for TCP Server //
wiz_NetInfo gWIZNETINFO = {{0x00, 0x08, 0xdc, 0x01, 0xab, 0xcd},
                           {192, 168, 50, 120},
                           {255, 255, 255, 0},
                           {192, 168, 50, 1},
                           {8, 8, 8, 8},
                           NETINFO_STATIC};

uint16_t w5500_tcps_port = 3000;
// Default Static Network Configuration for TCP Server //

// Azure IoT Hub/Central defines.
#define SCOPEID_LENGTH 20
static char scopeId[SCOPEID_LENGTH]; // ScopeId for the Azure IoT Central application, set in
                                     // app_manifest.json, CmdArgs

static IOTHUB_DEVICE_CLIENT_LL_HANDLE iothubClientHandle = NULL;
static const int keepalivePeriodSeconds = 20;
static bool iothubAuthenticated = false;
static void SendMessageCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void *context);
static void TwinCallback(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char *payload,
                         size_t payloadSize, void *userContextCallback);
static void TwinReportBoolState(const char *propertyName, bool propertyValue);
static void ReportStatusCallback(int result, void *context);
static const char *GetReasonString(IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason);
static const char *getAzureSphereProvisioningResultString(
    AZURE_SPHERE_PROV_RETURN_VALUE provisioningResult);
static void SendTelemetry(const unsigned char *key, const unsigned char *value);
static void SendTelemetryJson(const unsigned char *value);
static void SetupAzureClient(void);

// W5500 Ethernet
static int SendEthernetData(uint8_t sn, uint8_t* buf, uint16_t port);

// Function to generate simulated Temperature data/telemetry
static void SendSimulatedTemperature(void);

// Initialization/Cleanup
static int InitPeripheralsAndHandlers(void);
static void ClosePeripheralsAndHandlers(void);

// File descriptors - initialized to invalid value

// Status LEDs
static bool statusLedOn = false;
static int azureStatusLedGpioFd = -1;
static int wifiStatusLedGpioFd = -1;
static int ethStatusLedGpioFd = -1;
static int interfaceStatusLedGpioFd = -1;
//static int deviceTwinStatusLedGpioFd = -1;


// Timer / polling
static int buttonPollTimerFd = -1;
static int azureTimerFd = -1;
static int epollFd = -1;

// Azure IoT poll periods
static const int AzureIoTDefaultPollPeriodSeconds = 10;
//static const int AzureIoTDefaultPollPeriodSeconds = 20;
//static const int AzureIoTMinReconnectPeriodSeconds = 60;
static const int AzureIoTMinReconnectPeriodSeconds = 10;
static const int AzureIoTMaxReconnectPeriodSeconds = 10 * 60;

static int azureIoTPollPeriodSeconds = -1;

// Button state variables
static GPIO_Value_Type sendMessageButtonState = GPIO_Value_High;
static GPIO_Value_Type sendOrientationButtonState = GPIO_Value_High;

static bool deviceIsUp = false; // Orientation
static void AzureTimerEventHandler(EventData *eventData);

/// <summary>
///     Signal handler for termination requests. This handler must be async-signal-safe.
/// </summary>
static void TerminationHandler(int signalNumber)
{
    // Don't use Log_Debug here, as it is not guaranteed to be async-signal-safe.
    terminationRequired = true;
}

/// <summary>
///     Retrieves the stored networks on the device.
/// </summary>
/// <param name="numberOfNetworksStored">Output param used to retrieve the number of stored
/// networks on the device</param>
/// <param name="storedNetworksArray">Output param used to maintain the stored networks on the
/// device.</param>
/// <returns>Returns 0 in case of success, -1 otherwise</returns>
static int WifiRetrieveStoredNetworks(ssize_t* numberOfNetworksStored,
    WifiConfig_StoredNetwork* storedNetworksArray)
{
    ssize_t temporaryNumberOfNetworks = WifiConfig_GetStoredNetworkCount();
    if (temporaryNumberOfNetworks < 0) {
        //Log_Debug("ERROR: WifiConfig_GetStoredNetworkCount failed: %s (%d).\n", strerror(errno), errno);
        Log_Debug("ERROR: WifiConfig_GetStoredNetworkCount failed\n");
        return -1;
    }

    assert(temporaryNumberOfNetworks <= MAX_NUMBER_STORED_NETWORKS);

    if (temporaryNumberOfNetworks == 0) {
        *numberOfNetworksStored = 0;
        return 0;
    }

    WifiConfig_StoredNetwork temporaryStoredNetworksArray[temporaryNumberOfNetworks];
    temporaryNumberOfNetworks = WifiConfig_GetStoredNetworks(temporaryStoredNetworksArray,
        (size_t)temporaryNumberOfNetworks);
    if (temporaryNumberOfNetworks < 0) {
        //Log_Debug("ERROR: WifiConfig_GetStoredNetworks failed: %s (%d).\n", strerror(errno), errno);
        Log_Debug("ERROR: WifiConfig_GetStoredNetworks failed\n");
        return -1;
    }
    for (size_t i = 0; i < temporaryNumberOfNetworks; ++i) {
        storedNetworksArray[i] = temporaryStoredNetworksArray[i];
    }

    memcpy(storedNetworksArray, temporaryStoredNetworksArray, (size_t)temporaryNumberOfNetworks);

    *numberOfNetworksStored = temporaryNumberOfNetworks;

    return 0;
}

/// <summary>
///     Checks if the current Wi-Fi network is enabled, connected and outputs its SSID,
///     RSSI signal and security type.
/// </summary>
/// <returns>0 in case of success, any other value in case of failure</returns>
static int CheckCurrentWifiNetworkStatus(void)
{
    // Check the current Wi-Fi network status
    WifiConfig_ConnectedNetwork connectedNetwork;
    int result = WifiConfig_GetCurrentNetwork(&connectedNetwork);
    if (result != 0 && errno != ENODATA) {
        Log_Debug("\nERROR: WifiConfig_GetCurrentNetwork failed: %s (%d).\n", strerror(errno),errno);
        return result;
    }
    else if (result != 0 && errno == ENODATA) {
        Log_Debug("INFO: The device is not connected to any Wi-Fi networks.\n");
        result = 0;
    }
    else {  // connected any wifi network
        Log_Debug("INFO: The device is connected to: ");
        for (unsigned int i = 0; i < connectedNetwork.ssidLength; ++i) {
            Log_Debug("%c", isprint(connectedNetwork.ssid[i]) ? connectedNetwork.ssid[i] : '.');
        }
        assert(connectedNetwork.security < 3);
        Log_Debug(" : %s : %d dB\n", securityTypeToString[connectedNetwork.security],
            connectedNetwork.signalRssi);
    }
    return result;
}

/// <summary>
///    Outputs the stored Wi-Fi networks.
/// </summary>
/// <returns>0 in case of success, any other value in case of failure</returns>
static int OutputStoredWifiNetworks(void)
{
    unsigned int i, j;
    ssize_t numberOfNetworksStored;
    WifiConfig_StoredNetwork storedNetworksArray[MAX_NUMBER_STORED_NETWORKS];
    int result = WifiRetrieveStoredNetworks(&numberOfNetworksStored, storedNetworksArray);
    if (result != 0) {
        terminationRequired = true;
        return -1;
    }
    if (numberOfNetworksStored == 0) {
        return 0;
    }

    Log_Debug("INFO: Stored Wi-Fi networks:\n");
    for (i = 0; i < numberOfNetworksStored; ++i) {
        for (j = 0; j < storedNetworksArray[i].ssidLength; ++j) {
            Log_Debug("%c", isprint(storedNetworksArray[i].ssid[j]) ? storedNetworksArray[i].ssid[j] : '.');
        }
        assert(storedNetworksArray[i].security < 3);
        Log_Debug(" : %s : %s : %s\n", securityTypeToString[storedNetworksArray[i].security],
            storedNetworksArray[i].isEnabled ? "Enabled" : "Disabled",
            storedNetworksArray[i].isConnected ? "Connected" : "Disconnected");

#if 0   // change wifi id as connected
        if (storedNetworksArray[i].isConnected) {        
            wifi_connected = true;
            Log_Debug("wi-fi[%d] Connected \n", i);
        }
#endif
    }

    storedNetworkCnt = i;

    return 0;
}

/// <summary>
///     Checks if the device is connected to any Wi-Fi networks.
/// </summary>
/// <returns>0 in case of success, any other value in case of failure</returns>
static int CheckNetworkReady(void)
{
    bool isNetworkReady;
    int result = Networking_IsNetworkingReady(&isNetworkReady);
    int eth0_status = 0;
    int wlan0_status = 0;
    Networking_InterfaceConnectionStatus status;

    if (result != 0) {
        Log_Debug("\nERROR: Networking_IsNetworkingReady failed: %s (%d).\n", strerror(errno), errno);
        return result;
    }
    if (!isNetworkReady) {
        Log_Debug("INFO: Internet connectivity is not available.\n");
    }
    else {
        Log_Debug("INFO: Internet connectivity is available.\n");
    }

#if 1   // check wifi network 
    if (OutputStoredWifiNetworks() != 0) {
        terminationRequired = true;
        return;
    }
#endif  // check wifi network

    ssize_t count = Networking_GetInterfaceCount();
    if (count == -1) {
        Log_Debug("ERROR: Networking_GetInterfaceCount: errno=%d (%s)\n", errno, strerror(errno));
        return -1;
    }

    // Read current status of all interfaces.
    size_t bytesRequired = ((size_t)count) * sizeof(Networking_NetworkInterface);
    Networking_NetworkInterface* interfaces = malloc(bytesRequired);
    if (!interfaces) {
        abort();
    }

    ssize_t actualCount = Networking_GetInterfaces(interfaces, (size_t)count);
    if (actualCount == -1) {
        Log_Debug("ERROR: Networking_GetInterfaces: errno=%d (%s)\n", errno, strerror(errno));
    }

    for (ssize_t i = 0; i < actualCount; ++i) {
        int result = Networking_GetInterfaceConnectionStatus(interfaces[i].interfaceName, &interface_status);
        if (result != 0) {
            Log_Debug("ERROR: Networking_GetInterfaceConnectionStatus: errno=%d (%s)\n", errno, strerror(errno));
            return -1;
        }

        if (strstr(interfaces[i].interfaceName, "wlan0")) {
            Log_Debug("INFO: %s \n", interfaces[i].interfaceName);
            switch (interface_status) {
            case 0x01:
                wlan0_status = 1;
                wifi_connected = false;
                Log_Debug("INFO: wlan0_status 0x01 \n");
#if 1   // wifi enable
                if (!ethernet_connected) {
                    for (unsigned int i = 0; i < storedNetworkCnt; i++) {
                        result = WifiConfig_SetNetworkEnabled(i, true);
                        if (result < 0) {
                            terminationRequired = true;
                            return;
                        }
                    }
                }
#endif  // wifi enable
                break;

            case 0x03:
                wlan0_status = 2;
                wifi_connected = false;
                Log_Debug("INFO: wlan0_status 0x03 \n");
                break;

            case 0x0f:
                wlan0_status = 3;
                wifi_connected = true;
                Log_Debug("INFO: wlan0_status 0x0F \n");
#if 1   // wifi disable
                if (ethernet_connected) {
                    Log_Debug("INFO: Ethernet linked -> Wi-Fi disable \n");
                    for (unsigned int i = 0; i < storedNetworkCnt; i++) {
                        result = WifiConfig_SetNetworkEnabled(i, false);
                        if (result < 0) {
                            terminationRequired = true;
                            return;
                        }
                    }
                }
#endif  // wifi disable
                break;

            default:
                wifi_connected = false;
                break;
            }
        }

        if (strstr(interfaces[i].interfaceName, "eth0")) {
            switch (interface_status) {
            case 0x01:
                eth0_status = 1;
                ethernet_connected = false;
                Log_Debug("INFO: eth0_status 0x01 \n");
                break;

            case 0x03:
                eth0_status = 2;
                ethernet_connected = false;
                Log_Debug("INFO: eth0_status 0x03 \n");
                break;

            case 0x0f:
                eth0_status = 3;
                ethernet_connected = true;
                Log_Debug("INFO: eth0_status 0x0F \n");
                if (wifi_connected) {
                    for (unsigned int i = 0; i < storedNetworkCnt; i++) {
                        result = WifiConfig_SetNetworkEnabled(i, false);
                        if (result < 0) {
                            terminationRequired = true;
                            return;
                        }
                    }
                }
                break;

            default:
                ethernet_connected = false;
                break;
            }
        }
    }

    // set azure connect trigger
    Log_Debug("azure_connected : %d \n", azure_connected);
    if (!azure_connected) {
        net_switch_toggle = true;
    }
    else {
        if ((wlan0_status == 3) && (eth0_status == 3)) {
            net_switch_toggle = true;
        }
        if ((wlan0_status == 1) && (eth0_status == 1)) {
            net_switch_toggle = true;     // true
        }
    }
    Log_Debug("net_switch_toggle : %d \n", net_switch_toggle);
    // set azure connect trigger


    free(interfaces);
}

// check w5500 network setting
void InitWizEthernet(void) {
    uint8_t tmpstr[6];
    ctlwizchip(CW_GET_ID, (void*)tmpstr);

    if (ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO) < 0) {
        Log_Debug("ERROR: ctlnetwork SET\n");
    }
    
    ctlnetwork(CN_GET_NETINFO, (void*)&gWIZNETINFO);

    Log_Debug("\r\n=== %s NET CONF ===\r\n", (char*)tmpstr);
    Log_Debug("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n", gWIZNETINFO.mac[0], gWIZNETINFO.mac[1], gWIZNETINFO.mac[2],
           gWIZNETINFO.mac[3], gWIZNETINFO.mac[4], gWIZNETINFO.mac[5]);
    Log_Debug("SIP: %d.%d.%d.%d\r\n", gWIZNETINFO.ip[0], gWIZNETINFO.ip[1], gWIZNETINFO.ip[2], gWIZNETINFO.ip[3]);
    Log_Debug("GAR: %d.%d.%d.%d\r\n", gWIZNETINFO.gw[0], gWIZNETINFO.gw[1], gWIZNETINFO.gw[2], gWIZNETINFO.gw[3]);
    Log_Debug("SUB: %d.%d.%d.%d\r\n", gWIZNETINFO.sn[0], gWIZNETINFO.sn[1], gWIZNETINFO.sn[2], gWIZNETINFO.sn[3]);
    Log_Debug("DNS: %d.%d.%d.%d\r\n", gWIZNETINFO.dns[0], gWIZNETINFO.dns[1], gWIZNETINFO.dns[2], gWIZNETINFO.dns[3]);
    Log_Debug("======================\r\n");
}

// Init SPI master interface for W5500.
static int InitPrivateEthernet(void)
{
    if (Init_SPIMaster() != 0) {
        Log_Debug("Init_SPIMaster() error\n");
        return -1;
    }
    if (interfaceStatusLedGpioFd >= 0) {
        GPIO_SetValue(interfaceStatusLedGpioFd, GPIO_Value_Low);
    }
    Log_Debug("SPI Master initialized...\n");
    InitWizEthernet();
}


// Init Azure Sphere Ethernet
void InitPublicEthernet(void)
{
    // Ensure the necessary network interface is enabled.
    int result = Networking_SetInterfaceState(EthernetInterface, true);
    if (result < 0) {
        if (errno == EAGAIN) {
            Log_Debug("INFO: The networking stack isn't ready yet, will try again later.\n");
        } else {
            //Log_Debug("ERROR: Networking_SetInterfaceState for interface '%s' failed: errno=%d (%s)\n",EthernetInterface, errno, strerror(errno));
            Log_Debug("ERROR: Networking_SetInterfaceState for interface '%s' failed\n",EthernetInterface);
        }
    }

    // DHCP enable
    Networking_IpConfig ipConfig;
    Networking_IpConfig_Init(&ipConfig);
    Networking_IpConfig_EnableDynamicIp(&ipConfig);
    result = Networking_IpConfig_Apply(EthernetInterface, &ipConfig);
    Networking_IpConfig_Destroy(&ipConfig);
    if (result != 0) {
        Log_Debug("ERROR: Networking_IpConfig_Apply: %d (%s)\n", errno, strerror(errno));
        return -1;
    }
    Log_Debug("INFO: Networking_IpConfig_EnableDynamicIp: %s.\n", EthernetInterface);
}

// Init Azure Sphere Wi-Fi
void InitPublicWifi(void)
{
    // Ensure the necessary network interface is enabled.
    int result = Networking_SetInterfaceState(WifiInterface, true);
    if (result < 0) {
        if (errno == EAGAIN) {
            Log_Debug("INFO: The networking stack isn't ready yet, will try again later.\n");
        }
        else {
            Log_Debug("ERROR: Networking_SetInterfaceState for interface '%s' failed: errno=%d (%s)\n",WifiInterface, errno, strerror(errno));
        }
    }

#if 0
    if (CheckCurrentWifiNetworkStatus() != 0) {
        terminationRequired = true;
        return;
    }
#endif

    if (OutputStoredWifiNetworks() != 0) {
        terminationRequired = true;
        return;
    }
}

/// <summary>
///     Main entry point for this sample.
/// </summary>
int main(int argc, char *argv[])
{
    uint8_t err = 0;
    uint8_t ret = 0;

    net_switch_toggle = true;

    Log_Debug("IoT Hub/Central Application starting.\n");

    // Init GPIO
    if (InitPeripheralsAndHandlers() != 0) {
        terminationRequired = true;
    }

    // Init SPI interface for W5500
    InitPrivateEthernet();

    // Init Sphere ethernet/wi-fi interface
    InitPublicEthernet();
    InitPublicWifi();

#if 0 // first check network connectivity for network swiching
    if (CheckNetworkReady() != 0) {
        Log_Debug("Failed NetworkReady \n");
        terminationRequired = true;
        return;
    }
#endif

    if (argc == 2) {
        Log_Debug("Setting Azure Scope ID %s\n", argv[1]);
        strncpy(scopeId, argv[1], SCOPEID_LENGTH);
    } else {
        Log_Debug("ScopeId needs to be set in the app_manifest CmdArgs\n");
        return -1;
    }

    // Main loop
    while (!terminationRequired) {
        if (WaitForEventAndCallHandler(epollFd) != 0) {
            terminationRequired = true;
        }
#if 1
        if ((ret = SendEthernetData(0, gDATABUF, w5500_tcps_port)) < 0) {
            Log_Debug("%d:SendEthernetData error:%d\r\n", 0, ret);
            terminationRequired = true;
        }
#endif
    }

    ClosePeripheralsAndHandlers();
    Log_Debug("Application exiting.\n");

    return 0;
}


/// <summary>
/// Azure timer event:  Check connection status and send telemetry
/// </summary>
static void AzureTimerEventHandler(EventData *eventData)
{
    int result = 0;

    if (CheckNetworkReady() != 0) {
        Log_Debug("Failed NetworkReady \n");
        terminationRequired = true;
        return;
    }

#if 1
    if (ConsumeTimerFdEvent(azureTimerFd) != 0) {
        terminationRequired = true;
        return;
    }
#endif 

#if 1   // toggle
    if(net_switch_toggle) {             
        net_switch_toggle = false;     // <- false
        iothubAuthenticated = false;
        //Log_Debug("iothuAuthenticated <- false \n");
    }
#endif  // toggle

    Log_Debug("ethernet_connected: %d \n", ethernet_connected);

    if (ethernet_connected){
        if (ethStatusLedGpioFd >= 0) {
            GPIO_SetValue(ethStatusLedGpioFd, GPIO_Value_Low);
        }
    } 
    else {
        if (ethStatusLedGpioFd >= 0) {
            GPIO_SetValue(ethStatusLedGpioFd, GPIO_Value_High);
        }
    }

    if (wifi_connected) {
        if (wifiStatusLedGpioFd >= 0) {
            GPIO_SetValue(wifiStatusLedGpioFd, GPIO_Value_Low);
        }
    }
    else {
        if (wifiStatusLedGpioFd >= 0) {
            GPIO_SetValue(wifiStatusLedGpioFd, GPIO_Value_High);
        }
    }

    if (!iothubAuthenticated) {
        Log_Debug("SetupAzureClient() \n");
        SetupAzureClient();
    }

/// send sample data for simulation
#if sampleData
    SendSimulatedTemperature();
#else
    IoTHubDeviceClient_LL_DoWork(iothubClientHandle);
#endif

}

// event handler data structures. Only the event handler field needs to be populated.
static EventData azureEventData = {.eventHandler = &AzureTimerEventHandler};

/// <summary>
///     Set up SIGTERM termination handler, initialize peripherals, and set up event handlers.
/// </summary>
/// <returns>0 on success, or -1 on failure</returns>
static int InitPeripheralsAndHandlers(void)
{
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = TerminationHandler;
    sigaction(SIGTERM, &action, NULL);

    epollFd = CreateEpollFd();
    if (epollFd < 0) {
        return -1;
    }


#if 1   // init led
    Log_Debug("Opening AVNET_MT3620_SK_APP_STATUS_LED_YELLOW as output\n");
    azureStatusLedGpioFd =
        GPIO_OpenAsOutput(AVNET_MT3620_SK_APP_STATUS_LED_YELLOW, GPIO_OutputMode_PushPull, GPIO_Value_High);
    if (azureStatusLedGpioFd < 0) {
        Log_Debug("ERROR: Could not open LED: %s (%d).\n", strerror(errno), errno);
        return -1;
    }

    Log_Debug("Opening $AVNET_MT3620_SK_USER_LED_BLUE as output\n");
    interfaceStatusLedGpioFd =
        GPIO_OpenAsOutput(AVNET_MT3620_SK_USER_LED_BLUE, GPIO_OutputMode_PushPull, GPIO_Value_High);
    if (interfaceStatusLedGpioFd < 0) {
        Log_Debug("ERROR: Could not open LED: %s (%d).\n", strerror(errno), errno);
        return -1;
    }

    Log_Debug("Opening AVNET_MT3620_SK_USER_LED_GREEN as output\n");
    ethStatusLedGpioFd =
        GPIO_OpenAsOutput(AVNET_MT3620_SK_USER_LED_GREEN, GPIO_OutputMode_PushPull, GPIO_Value_High);
    if (ethStatusLedGpioFd < 0) {
        Log_Debug("ERROR: Could not open LED: %s (%d).\n", strerror(errno), errno);
        return -1;
    }

    Log_Debug("Opening AVNET_MT3620_SK_USER_LED_RED as output\n");
    wifiStatusLedGpioFd =
        GPIO_OpenAsOutput(AVNET_MT3620_SK_USER_LED_RED, GPIO_OutputMode_PushPull, GPIO_Value_High);
    if (wifiStatusLedGpioFd < 0) {
        Log_Debug("ERROR: Could not open LED: %s (%d).\n", strerror(errno), errno);
        return -1;
    }
#endif  // init led

#if 1
    azureIoTPollPeriodSeconds = AzureIoTDefaultPollPeriodSeconds;
    struct timespec azureTelemetryPeriod = {azureIoTPollPeriodSeconds, 0};
    azureTimerFd =
        CreateTimerFdAndAddToEpoll(epollFd, &azureTelemetryPeriod, &azureEventData, EPOLLIN);
    if (azureTimerFd < 0) {
        Log_Debug("azureTimerFd : %d \n", azureTimerFd);
        return -1;
    }
#endif

    return 0;
}

/// <summary>
///     Close peripherals and handlers.
/// </summary>
static void ClosePeripheralsAndHandlers(void)
{
    Log_Debug("Closing file descriptors\n");

    if (azureStatusLedGpioFd >= 0) {
        GPIO_SetValue(azureStatusLedGpioFd, GPIO_Value_High);
    }
    if (ethStatusLedGpioFd >= 0) {
        GPIO_SetValue(ethStatusLedGpioFd, GPIO_Value_High);
    }
    if (interfaceStatusLedGpioFd >= 0) {
        GPIO_SetValue(interfaceStatusLedGpioFd, GPIO_Value_High);
    }
    if (wifiStatusLedGpioFd >= 0) {
        GPIO_SetValue(wifiStatusLedGpioFd, GPIO_Value_High);
    }

    CloseFdAndPrintError(azureTimerFd, "AzureTimer");
    CloseFdAndPrintError(azureStatusLedGpioFd, "StatusLed");
    CloseFdAndPrintError(ethStatusLedGpioFd, "StatusLed");
    CloseFdAndPrintError(interfaceStatusLedGpioFd, "StatusLed");
    CloseFdAndPrintError(wifiStatusLedGpioFd, "StatusLed");
    CloseFdAndPrintError(epollFd, "Epoll");
}

/// <summary>
///     Sets the IoT Hub authentication state for the app
///     The SAS Token expires which will set the authentication state
/// </summary>
static void HubConnectionStatusCallback(IOTHUB_CLIENT_CONNECTION_STATUS result,
                                        IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason,
                                        void *userContextCallback)
{
    iothubAuthenticated = (result == IOTHUB_CLIENT_CONNECTION_AUTHENTICATED);
    Log_Debug("IoT Hub Authenticated: %s\n", GetReasonString(reason));

#if 1 // azure connected
    if(strstr(GetReasonString(reason), "IOTHUB_CLIENT_CONNECTION_OK")) {
        if (azureStatusLedGpioFd >= 0) {
            GPIO_SetValue(azureStatusLedGpioFd, GPIO_Value_Low);
        }    
    }    
#endif
}

/// <summary>
///     Sets up the Azure IoT Hub connection (creates the iothubClientHandle)
///     When the SAS Token for a device expires the connection needs to be recreated
///     which is why this is not simply a one time call.
/// </summary>
static void SetupAzureClient(void)
{
    if (iothubClientHandle != NULL)
        IoTHubDeviceClient_LL_Destroy(iothubClientHandle);

    AZURE_SPHERE_PROV_RETURN_VALUE provResult =
        IoTHubDeviceClient_LL_CreateWithAzureSphereDeviceAuthProvisioning(scopeId, 10000,
                                                                          &iothubClientHandle);
    Log_Debug("IoTHubDeviceClient_LL_CreateWithAzureSphereDeviceAuthProvisioning returned '%s'.\n",
              getAzureSphereProvisioningResultString(provResult));

    if (provResult.result != AZURE_SPHERE_PROV_RESULT_OK) {       
#if 1   // azure no connected
        azure_connected = false;
        if (azureStatusLedGpioFd >= 0) {
            GPIO_SetValue(azureStatusLedGpioFd, GPIO_Value_High);
        }
#endif

        // If we fail to connect, reduce the polling frequency, starting at
        // AzureIoTMinReconnectPeriodSeconds and with a backoff up to
        // AzureIoTMaxReconnectPeriodSeconds
        if (azureIoTPollPeriodSeconds == AzureIoTDefaultPollPeriodSeconds) {
            azureIoTPollPeriodSeconds = AzureIoTMinReconnectPeriodSeconds;
        } else {
#if 0       // *2 times for re-connetion
            azureIoTPollPeriodSeconds *= 2;
#endif
            if (azureIoTPollPeriodSeconds > AzureIoTMaxReconnectPeriodSeconds) {
                azureIoTPollPeriodSeconds = AzureIoTMaxReconnectPeriodSeconds;
            }
        }

        struct timespec azureTelemetryPeriod = {azureIoTPollPeriodSeconds, 0};
        SetTimerFdToPeriod(azureTimerFd, &azureTelemetryPeriod);

        Log_Debug("ERROR: failure to create IoTHub Handle - will retry in %i seconds.\n",
                  azureIoTPollPeriodSeconds);
        return;
    }

    // Successfully connected, so make sure the polling frequency is back to the default
    azureIoTPollPeriodSeconds = AzureIoTDefaultPollPeriodSeconds;
    struct timespec azureTelemetryPeriod = {azureIoTPollPeriodSeconds, 0};
    SetTimerFdToPeriod(azureTimerFd, &azureTelemetryPeriod);

    iothubAuthenticated = true;
    azure_connected = true;

    if (IoTHubDeviceClient_LL_SetOption(iothubClientHandle, OPTION_KEEP_ALIVE,
                                        &keepalivePeriodSeconds) != IOTHUB_CLIENT_OK) {
        Log_Debug("ERROR: failure setting option \"%s\"\n", OPTION_KEEP_ALIVE);
        return;
    }

    IoTHubDeviceClient_LL_SetDeviceTwinCallback(iothubClientHandle, TwinCallback, NULL);
    IoTHubDeviceClient_LL_SetConnectionStatusCallback(iothubClientHandle,
                                                      HubConnectionStatusCallback, NULL);
}

/// <summary>
///     Callback invoked when a Device Twin update is received from IoT Hub.
///     Updates local state for 'showEvents' (bool).
/// </summary>
/// <param name="payload">contains the Device Twin JSON document (desired and reported)</param>
/// <param name="payloadSize">size of the Device Twin JSON document</param>
static void TwinCallback(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char *payload,
                         size_t payloadSize, void *userContextCallback)
{
    size_t nullTerminatedJsonSize = payloadSize + 1;
    char *nullTerminatedJsonString = (char *)malloc(nullTerminatedJsonSize);
    if (nullTerminatedJsonString == NULL) {
        Log_Debug("ERROR: Could not allocate buffer for twin update payload.\n");
        abort();
    }

    // Copy the provided buffer to a null terminated buffer.
    memcpy(nullTerminatedJsonString, payload, payloadSize);
    // Add the null terminator at the end.
    nullTerminatedJsonString[nullTerminatedJsonSize - 1] = 0;

    JSON_Value *rootProperties = NULL;
    rootProperties = json_parse_string(nullTerminatedJsonString);
    if (rootProperties == NULL) {
        Log_Debug("WARNING: Cannot parse the string as JSON content.\n");
        goto cleanup;
    }

    JSON_Object *rootObject = json_value_get_object(rootProperties);
    JSON_Object *desiredProperties = json_object_dotget_object(rootObject, "desired");
    if (desiredProperties == NULL) {
        desiredProperties = rootObject;
    }

    // Handle the Device Twin Desired Properties here.
    JSON_Object *LEDState = json_object_dotget_object(desiredProperties, "StatusLED");
    if (LEDState != NULL) {
        statusLedOn = (bool)json_object_get_boolean(LEDState, "value");
#if 0   // for azure demo test
        GPIO_SetValue(deviceTwinStatusLedGpioFd,
                      (statusLedOn == true ? GPIO_Value_Low : GPIO_Value_High));
#endif
        TwinReportBoolState("StatusLED", statusLedOn);
    }

cleanup:
    // Release the allocated memory.
    json_value_free(rootProperties);
    free(nullTerminatedJsonString);
}

/// <summary>
///     Converts the IoT Hub connection status reason to a string.
/// </summary>
static const char *GetReasonString(IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason)
{
    static char *reasonString = "unknown reason";
    switch (reason) {
    case IOTHUB_CLIENT_CONNECTION_EXPIRED_SAS_TOKEN:
        reasonString = "IOTHUB_CLIENT_CONNECTION_EXPIRED_SAS_TOKEN";
        break;
    case IOTHUB_CLIENT_CONNECTION_DEVICE_DISABLED:
        reasonString = "IOTHUB_CLIENT_CONNECTION_DEVICE_DISABLED";
        break;
    case IOTHUB_CLIENT_CONNECTION_BAD_CREDENTIAL:
        reasonString = "IOTHUB_CLIENT_CONNECTION_BAD_CREDENTIAL";
        break;
    case IOTHUB_CLIENT_CONNECTION_RETRY_EXPIRED:
        reasonString = "IOTHUB_CLIENT_CONNECTION_RETRY_EXPIRED";
        break;
    case IOTHUB_CLIENT_CONNECTION_NO_NETWORK:
        reasonString = "IOTHUB_CLIENT_CONNECTION_NO_NETWORK";
        break;
    case IOTHUB_CLIENT_CONNECTION_COMMUNICATION_ERROR:
        reasonString = "IOTHUB_CLIENT_CONNECTION_COMMUNICATION_ERROR";
        break;
    case IOTHUB_CLIENT_CONNECTION_OK:
        reasonString = "IOTHUB_CLIENT_CONNECTION_OK";
        break;
    }
    return reasonString;
}

/// <summary>
///     Converts AZURE_SPHERE_PROV_RETURN_VALUE to a string.
/// </summary>
static const char *getAzureSphereProvisioningResultString(
    AZURE_SPHERE_PROV_RETURN_VALUE provisioningResult)
{
    switch (provisioningResult.result) {
    case AZURE_SPHERE_PROV_RESULT_OK:
        return "AZURE_SPHERE_PROV_RESULT_OK";
    case AZURE_SPHERE_PROV_RESULT_INVALID_PARAM:
        return "AZURE_SPHERE_PROV_RESULT_INVALID_PARAM";
    case AZURE_SPHERE_PROV_RESULT_NETWORK_NOT_READY:
        return "AZURE_SPHERE_PROV_RESULT_NETWORK_NOT_READY";
    case AZURE_SPHERE_PROV_RESULT_DEVICEAUTH_NOT_READY:
        return "AZURE_SPHERE_PROV_RESULT_DEVICEAUTH_NOT_READY";
    case AZURE_SPHERE_PROV_RESULT_PROV_DEVICE_ERROR:
        return "AZURE_SPHERE_PROV_RESULT_PROV_DEVICE_ERROR";
    case AZURE_SPHERE_PROV_RESULT_GENERIC_ERROR:
        return "AZURE_SPHERE_PROV_RESULT_GENERIC_ERROR";
    default:
        return "UNKNOWN_RETURN_VALUE";
    }
}

/// <summary>
///     Sends telemetry to IoT Hub
/// </summary>
/// <param name="key">The telemetry item to update</param>
/// <param name="value">new telemetry value</param>
static void SendTelemetry(const unsigned char *key, const unsigned char *value)
{
    static char eventBuffer[100] = {0};
    static const char *EventMsgTemplate = "{ \"%s\": \"%s\" }";
    int len = snprintf(eventBuffer, sizeof(eventBuffer), EventMsgTemplate, key, value);
    if (len < 0)
        return;

    Log_Debug("Sending IoT Hub Message: %s\n", eventBuffer);

    IOTHUB_MESSAGE_HANDLE messageHandle = IoTHubMessage_CreateFromString(eventBuffer);

    if (messageHandle == 0) {
        Log_Debug("WARNING: unable to create a new IoTHubMessage\n");
        return;
    }

	// rena 20191118
	// Set system properties
	(void)IoTHubMessage_SetContentTypeSystemProperty(messageHandle, "application%2fjson");
	(void)IoTHubMessage_SetContentEncodingSystemProperty(messageHandle, "utf-8");

    if (IoTHubDeviceClient_LL_SendEventAsync(iothubClientHandle, messageHandle, SendMessageCallback,
                                             /*&callback_param*/ 0) != IOTHUB_CLIENT_OK) {
        Log_Debug("WARNING: failed to hand over the message to IoTHubClient: %s (%d)\n", strerror(errno), errno);
    } else {
        Log_Debug("INFO: IoTHubClient accepted the message for delivery\n");
    }

    IoTHubMessage_Destroy(messageHandle);
}


// JSON String value
static void SendTelemetryJson(const unsigned char *value)
{
    static char eventBuffer[1024] = {0};
    // value data must be JSON format
    // static const char *EventMsgTemplate = "{ \"%s\": \"%s\" }";
    // static const char *EventMsgTemplate = "\"{%s}\"";
    static const char *EventMsgTemplate = "%s";
    int len = snprintf(eventBuffer, sizeof(eventBuffer), EventMsgTemplate, value);
    if (len < 0)
        return;

    Log_Debug("Sending IoT Hub Message: %s\n", eventBuffer);

    IOTHUB_MESSAGE_HANDLE messageHandle = IoTHubMessage_CreateFromString(eventBuffer);
    if (messageHandle == 0) {
        Log_Debug("WARNING: unable to create a new IoTHubMessage\n");
        return;
    }

	// rena 20191118
	// Set system properties
	(void)IoTHubMessage_SetContentTypeSystemProperty(messageHandle, "application%2fjson");
	(void)IoTHubMessage_SetContentEncodingSystemProperty(messageHandle, "utf-8");

    if (IoTHubDeviceClient_LL_SendEventAsync(iothubClientHandle, messageHandle, SendMessageCallback,
                                             /*&callback_param*/ 0) != IOTHUB_CLIENT_OK) {
        Log_Debug("WARNING: failed to hand over the message to IoTHubClient: %s (%d)\n", strerror(errno), errno);
    } else {
        Log_Debug("INFO: IoTHubClient accepted the message for delivery\n");
    }

    IoTHubMessage_Destroy(messageHandle);
}


/// <summary>
///     Callback confirming message delivered to IoT Hub.
/// </summary>
/// <param name="result">Message delivery status</param>
/// <param name="context">User specified context</param>
static void SendMessageCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void *context)
{
    Log_Debug("INFO: Message received by IoT Hub. Result is: %d\n", result);
}

/// <summary>
///     Creates and enqueues a report containing the name and value pair of a Device Twin reported
///     property. The report is not sent immediately, but it is sent on the next invocation of
///     IoTHubDeviceClient_LL_DoWork().
/// </summary>
/// <param name="propertyName">the IoT Hub Device Twin property name</param>
/// <param name="propertyValue">the IoT Hub Device Twin property value</param>
static void TwinReportBoolState(const char *propertyName, bool propertyValue)
{
    if (iothubClientHandle == NULL) {
        Log_Debug("ERROR: client not initialized\n");
    } else {
        static char reportedPropertiesString[30] = {0};
        int len = snprintf(reportedPropertiesString, 30, "{\"%s\":%s}", propertyName,
                           (propertyValue == true ? "true" : "false"));
        if (len < 0)
            return;

        if (IoTHubDeviceClient_LL_SendReportedState(
                iothubClientHandle, (unsigned char *)reportedPropertiesString,
                strlen(reportedPropertiesString), ReportStatusCallback, 0) != IOTHUB_CLIENT_OK) {
            Log_Debug("ERROR: failed to set reported state for '%s'.\n", propertyName);
        } else {
            Log_Debug("INFO: Reported state for '%s' to value '%s'.\n", propertyName,
                      (propertyValue == true ? "true" : "false"));
        }
    }
}

/// <summary>
///     Callback invoked when the Device Twin reported properties are accepted by IoT Hub.
/// </summary>
static void ReportStatusCallback(int result, void *context)
{
    Log_Debug("INFO: Device Twin reported properties update result: HTTP status code %d\n", result);
}

#if sampleData
/// <summary>
///     Generates a simulated Temperature and sends to IoT Hub.
/// </summary>
void SendSimulatedTemperature(void)
{
    static float temperature = 30.0;
    float deltaTemp = (float)(rand() % 20) / 20.0f;
    if (rand() % 2 == 0) {
        temperature += deltaTemp;
    } else {
        temperature -= deltaTemp;
    }

    char tempBuffer[20];
    int len = snprintf(tempBuffer, 20, "%3.2f", temperature);
    if (len > 0)
        SendTelemetry("temperature", tempBuffer);
}
#endif

static int SendEthernetData(uint8_t sn, uint8_t* buf, uint16_t port)
{
    int32_t ret;
    uint16_t size = 0, sentsize = 0;
    uint8_t sock_buf[DATA_BUF_SIZE];

    for (int i = 0; i < DATA_BUF_SIZE; i++) {
        sock_buf[i] = NULL;
    }

    uint8_t destip[4];
    uint16_t destport;

    switch (getSn_SR(sn)) {
        case SOCK_ESTABLISHED:
            if (getSn_IR(sn) & Sn_IR_CON) {
                getSn_DIPR(sn, destip);
                destport = getSn_DPORT(sn);

                Log_Debug("%d:Connected - %d.%d.%d.%d : %d\r\n", sn, destip[0], destip[1], destip[2], destip[3], destport);
                setSn_IR(sn, Sn_IR_CON);
            }
            if ((size = getSn_RX_RSR(sn)) > 0)  // Don't need to check SOCKERR_BUSY because it doesn't not occur.
            {
                if (size > DATA_BUF_SIZE) size = DATA_BUF_SIZE;
                ret = sock_recv(sn, sock_buf, size);

                if (ret <= 0) return ret;  // check SOCKERR_BUSY & SOCKERR_XXX. For showing the occurrence of SOCKERR_BUSY.
                
                Log_Debug("Received data from socket %d: %s\n", sn, sock_buf);
                //! Send received data from ethernet to IoT Hub
                SendTelemetryJson(sock_buf);
                IoTHubDeviceClient_LL_DoWork(iothubClientHandle);

                for (int i = 0; i < size; i++) {
                    sock_buf[i] = NULL;
                }
            }
            break;

        case SOCK_CLOSE_WAIT:
            if ((ret = sock_disconnect(sn)) != SOCK_OK) return ret;
            Log_Debug("%d:Socket Closed\r\n", sn);
            break;

        case SOCK_INIT:
            if ((ret = sock_listen(sn)) != SOCK_OK) return ret;
            Log_Debug("%d:Listen, TCP server, port [%d]\r\n", sn, port);
            break;

        case SOCK_CLOSED:
            if ((ret = wiz_socket(sn, Sn_MR_TCP, port, 0x00)) != sn) return ret;
            Log_Debug("%d:TCP Socket opened\r\n", sn);
            break;

        default:
            break;
    }
    return 1;
}
