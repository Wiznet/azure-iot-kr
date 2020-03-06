# Connect to Azure IoT Hub with using Azure Sphere Guardian(WIZ-ASG-200) via Ethernet or Wi-Fi

## Contents

- [Overview](#overview)
  - [Features](#features)
- [Step 1: Pre Setting](#step-1-prerequisites)
- [Step 2: WIZ-ASG-200 Configuration](#Step-2-device-configuration)
- [Step 3: Application Setting](#step-3-application-setting)
- [Step 4: Application Run](#step-4-build-and-run)

- [Reference Docs](#reference)

<a name="overview"></a>

## Overview

### WIZ-ASG-200 (Azure Sphere Guardian)

![WIZASG200]

WIZ-ASG-200 is 2-port Ethernet and Wi-Fi Edge module with enhanced IoT security. User can take full advantage of Microsoft's powerful security services with Microsoft Azure Sphere on WIZ-ASG-200.

One of 2-port Ethernet, WIZnet W5500 Hardwired TCP/IP Ethernet chip, is for connecting Lagecy devices and On-premise network system which cannot connect to Azure IoT Cloud. Other Ethernet port is for connecting Azure IoT Cloud. WIZ-ASG-200 supports Dual Wi-Fi bandwidth as well for Azure IoT Cloud connection.

### Features

- Built-in Azure Sphere MT3620 Module
- Support MS Pluton security system in MT3620
- Support Ethernet or Dual Wi-Fi (2.4/5GHz) interface for Azure connection
- Support Auto Ethernet and Wi-Fi switching for stable Azure connection
- Support USB Interface for debug, service & recovery UARTs, and JTAG
- Available in minimizing the existing wired network to support greenfield + brownfield
- Support variety protocols for brownfield wired network : TCP, UDP, ICMP, IPv4, ARP, IGMP, PPPoE

In this documents, you can learn how to connect MS Azure IoT Hub with WIZ-ASG-200.

<img src="https://github.com/Wiznet/azure-iot-kr/blob/master/images/wiz-asg-200-demo.png?raw=true">

<a name="step-1-prerequisites"></a>

## Step 1: Pre Setting

Below Hardware and Software should be set for running WIZ-ASG-200.

### Hardware

- WIZ-ASG-200
- Development Computer with Windows 10 or Linux OS
- Micro 5 pin USB cable
- 5V 2A Power Adapter

### Software

- [[MS] Azure Sphere: Overview of set-up procedures](https://docs.microsoft.com/en-us/azure-sphere/install/overview)

- [[MS] Azure Sphere: Set up your Windows PC for app development](https://docs.microsoft.com/en-us/azure-sphere/install/development-environment-windows)

- [[MS] Azure Sphere: Set up your Linux system for app development](https://docs.microsoft.com/en-us/azure-sphere/install/development-environment-linux)

<a name="step-2-device-configuration"></a>

## Step 2: WIZ-ASG-200 Configuration

### Device connection check

Once connect WIZ-ASG-200 to PC with Micro 5 pin USB cable, FTDI driver will be automatically installed. After install done, 3 COM ports will be shown in Device Manager.

<img src="https://github.com/Wiznet/azure-iot-kr/blob/master/images/wiz-asg-200-comport.png?raw=true">

### Device configuration

Follow the below steps on `Azure Sphere Developer Command Prompt Preview`
<br>
(User can use `Azure Sphere Developer Command Prompt Preview` after `Azure Sphere SDK Preview` installation for `Visual Studio`)

- Login with `azsphere login` command
  - Add new account with `azsphere login --newuser <MS account>`
- Create new Azure Sphere tenant with `azsphere tenant create -n <tenant name>`
- Select tenant with `azsphere tenant select -i <tenant id>`
- Claim selected tenant to device with `azsphere device claim`
- Set Wi-Fi network
  <br> `azsphere device wifi add --ssid <yourSSID> --psk <yourNetworkKey>`
  - --psk is not nessesary for week security Wi-Fi network
  - Type " " for blank in SSID or Password e.g. --ssid "My AP"
- Check Wi-Fi status
  <br> `azsphere device wifi show-status`

> - Restore Azure Sphere as recent OS version with `azsphere device recover`
> - Select Debug mode with `azsphere device enable-development` (OTA will be disabled )

### Set Ethernet interface

[[MS] Azure Sphere: Connect Azure Sphere to Ethernet](https://docs.microsoft.com/en-us/azure-sphere/network/connect-ethernet#board-configuration)

To set Ethernet interface, follow the below steps.

1. Board configuration image

`azsphere image-package pack-board-config --preset lan-enc28j60-isu0-int5 --output enc28j60-isu0-int5.imagepackage`

2. Load image package

`azsphere device sideload deploy --imagepackage enc28j60-isu0-int5.imagepackage`

### Set IoT Hub

To use WIZ-ASG-200, Azure IoT Hub should be set up to work with Azure Sphere tenant

- [[MS] Set up an Azure IoT Hub for Azure Sphere](https://docs.microsoft.com/en-us/azure-sphere/app-development/setup-iot-hub)

<a name="step-3-application-setting"></a>

## Step 3: Modified example code

### app_manifest.json

Fullfill the below list in app_manifest.json

- DeviceAuthentication: Azure sphere tenant ID
  - claimed tenant ID for the WIZ-ASG-200
  - check with `azsphere tenant show-selected`
- CmdArgs: `Scope ID` in Azure device provisioning service(DPS)
- Capabilities - AllowedConnections: `endpoint URL` in Azure IoT Hub
  - Global access link : `global.azure-devices.provisioning.net`

<a name="step-4-build-and-run"></a>

## Step 4: Build and running example code

Build Application and run with Visual Studio

- Select **GDB Debugger (HLCore)** in `Select Startup Item` on the top menu
- Menu - build - build solution
- Click **GDB Debugger (HLCore)** or press **F5** to run application

---

<a name="reference"></a>

## Reference Docs

- [Azure Sphere Bootcamp](https://github.com/azsphere/Azure-Sphere-Bootcamp)

- [Azure Sphere Samples: AzureIoT](https://github.com/Azure/azure-sphere-samples/blob/master/Samples/AzureIoT/IoTHub.md)

[wizasg200]: ../../../../images/WIZ-ASG-200.png
