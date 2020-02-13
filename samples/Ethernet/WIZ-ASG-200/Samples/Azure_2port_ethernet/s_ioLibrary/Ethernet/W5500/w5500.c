//*****************************************************************************
//
//! \file w5500.c
//! \brief W5500 HAL Interface.
//! \version 1.0.2
//! \date 2013/10/21
//! \par  Revision history
//!       <2015/02/05> Notice
//!        The version history is not updated after this point.
//!        Download the latest version directly from GitHub. Please visit the our GitHub repository for ioLibrary.
//!        >> https://github.com/Wiznet/ioLibrary_Driver
//!       <2014/05/01> V1.0.2
//!         1. Implicit type casting -> Explicit type casting. Refer to M20140501
//!            Fixed the problem on porting into under 32bit MCU
//!            Issued by Mathias ClauBen, wizwiki forum ID Think01 and bobh
//!            Thank for your interesting and serious advices.
//!       <2013/12/20> V1.0.1
//!         1. Remove warning
//!         2. WIZCHIP_READ_BUF WIZCHIP_WRITE_BUF in case _WIZCHIP_IO_MODE_SPI_FDM_
//!            for loop optimized(removed). refer to M20131220
//!       <2013/10/21> 1st Release
//! \author MidnightCow
//! \copyright
//!
//! Copyright (c)  2013, WIZnet Co., LTD.
//! All rights reserved.
//!
//! Redistribution and use in source and binary forms, with or without
//! modification, are permitted provided that the following conditions
//! are met:
//!
//!     * Redistributions of source code must retain the above copyright
//! notice, this list of conditions and the following disclaimer.
//!     * Redistributions in binary form must reproduce the above copyright
//! notice, this list of conditions and the following disclaimer in the
//! documentation and/or other materials provided with the distribution.
//!     * Neither the name of the <ORGANIZATION> nor the names of its
//! contributors may be used to endorse or promote products derived
//! from this software without specific prior written permission.
//!
//! THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//! AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//! IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//! ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
//! LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//! CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//! SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//! INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//! CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//! ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
//! THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "../../applibs_versions.h"

#include <applibs/log.h>
#include <applibs/spi.h>
#include <hw/avnet_mt3620_sk.h>

#include "w5500.h"
#include "w5500-dbg.h"

#define _W5500_SPI_VDM_OP_ 0x00
#define _W5500_SPI_FDM_OP_LEN1_ 0x01
#define _W5500_SPI_FDM_OP_LEN2_ 0x02
#define _W5500_SPI_FDM_OP_LEN4_ 0x03

static int spiFd = -1;

#if (_WIZCHIP_ == 5500)
////////////////////////////////////////////////////

/*
*   AVNET_MT3620_SK SPI Init
*/
uint8_t Init_SPIMaster(void)
{
    SPIMaster_Config config;
    int ret = SPIMaster_InitConfig(&config);
    if (ret != 0)
    {
        Log_Debug("ERROR: SPIMaster_InitConfig = %d errno = %s (%d)\n", ret, strerror(errno),
                  errno);
        return -1;
    }

    config.csPolarity = SPI_ChipSelectPolarity_ActiveLow;

#if 0   // SPI_1 CS_A enable
    spiFd = SPIMaster_Open(AVNET_MT3620_SK_ISU1_SPI, MT3620_SPI_CS_A, &config);
#else   // SPI_1 CS_B enable
    spiFd = SPIMaster_Open(AVNET_MT3620_SK_ISU1_SPI, MT3620_SPI_CS_B, &config);
#endif
    Log_Debug("SPIMaster_Open() \n");
    if (spiFd < 0)
    {
#if WIZNET_DBG
        Log_Debug("ERROR: SPIMaster_Open: errno=%d (%s)\n", errno, strerror(errno));
#endif
        return -1;
    }

    int result = SPIMaster_SetBusSpeed(spiFd, 8000000); // 8MHz
    if (result != 0)
    {
#if WIZNET_DBG
        Log_Debug("ERROR: SPIMaster_SetBusSpeed: errno=%d (%s)\n", errno, strerror(errno));
#endif
        return -1;
    }

#if 1   // SPI Mode_3 enable
    result = SPIMaster_SetMode(spiFd, SPI_Mode_3);
#else   // SPI Mode_0 enable
    result = SPIMaster_SetMode(spiFd, SPI_Mode_0);
#endif
    
    if (result != 0)
    {
#if WIZNET_DBG
        Log_Debug("ERROR: SPIMaster_SetMode: errno=%d (%s)\n", errno, strerror(errno));
#endif
        return -1;
    }

    return 0;
}


uint8_t WIZCHIP_READ(uint32_t AddrSel)
{
    static const size_t transferCount = 2;
    SPIMaster_Transfer transfers[transferCount];
    uint8_t ret;

    if (SPIMaster_InitTransfers(transfers, transferCount) != 0)
    {
        Log_Debug("ERROR: SPIMaster_InitTransfers: errno=%d (%s)\n", errno, strerror(errno));
        return -1;
    }

    AddrSel |= (_W5500_SPI_READ_ | _W5500_SPI_VDM_OP_);
    uint8_t data[] = {(AddrSel & 0x00FF0000) >> 16, (AddrSel & 0x0000FF00) >> 8, (AddrSel & 0x000000FF) >> 0};

    transfers[0].flags = SPI_TransferFlags_Write;
    transfers[0].writeData = data;
    transfers[0].length = sizeof(data);

    transfers[1].flags = SPI_TransferFlags_Read;
    transfers[1].readData = &ret;
    transfers[1].length = sizeof(ret);

    ssize_t transferredBytes = SPIMaster_TransferSequential(spiFd, transfers, transferCount);

    if (transferredBytes < 0)
    {
        Log_Debug("errno=%d (%s)\n", errno, strerror(errno));
        return -1;
    }

    return ret;
}

void WIZCHIP_WRITE(uint32_t AddrSel, uint8_t wb)
{
    static const size_t transferCount = 1;
    SPIMaster_Transfer transfers[transferCount];

    if (SPIMaster_InitTransfers(transfers, transferCount) != 0)
    {
        Log_Debug("ERROR: SPIMaster_InitTransfers: errno=%d (%s)\n", errno, strerror(errno));
        return -1;
    }

    AddrSel |= (_W5500_SPI_WRITE_ | _W5500_SPI_VDM_OP_);
    uint8_t data[] = {
        (AddrSel & 0x00FF0000) >> 16,
        (AddrSel & 0x0000FF00) >> 8,
        (AddrSel & 0x000000FF) >> 0,
        wb,
    };

    transfers[0].flags = SPI_TransferFlags_Write;
    transfers[0].writeData = data;
    transfers[0].length = 4;

    ssize_t transferredBytes = SPIMaster_TransferSequential(spiFd, transfers, transferCount);

    if (transferredBytes < 0)
    {
        Log_Debug("errno=%d (%s)\n", errno, strerror(errno));
        return -1;
    }
}


void WIZCHIP_READ_BUF(uint32_t AddrSel, uint8_t* pBuf, uint16_t len)
{
    static const size_t transferCount = 2;
    SPIMaster_Transfer transfers[transferCount];

    if (SPIMaster_InitTransfers(transfers, transferCount) != 0)
    {
        Log_Debug("ERROR: SPIMaster_InitTransfers: errno=%d (%s)\n", errno, strerror(errno));
        return -1;
    }

    AddrSel |= (_W5500_SPI_READ_ | _W5500_SPI_VDM_OP_);

    uint8_t data[] = {
            (AddrSel & 0x00FF0000) >> 16,
            ((AddrSel & 0x0000FF00) >> 8),
            (AddrSel & 0x000000FF) >> 0 };

    ssize_t transferredBytes = SPIMaster_WriteThenRead(
        spiFd, data, sizeof(data), pBuf, len);

#if WIZNET_DBG  // SPI dump
    printf("\n after Sphere Write XXXXXXXXXXXXXXXXXXXXXXXXXXX \n");
    WDUMP(data, sizeof(data));
    printf("\n after Sphere Read XXXXXXXXXXXXXXXXXXXXXXXXXXX \n");
    WDUMP(pBuf, len);
#endif

}


void WIZCHIP_WRITE_BUF(uint32_t AddrSel, uint8_t *pBuf, uint16_t len)
{
    uint16_t i;

    static const size_t transferCount = 1;
    SPIMaster_Transfer transfers[transferCount];

    if (SPIMaster_InitTransfers(transfers, transferCount) != 0)
    {
        Log_Debug("ERROR: SPIMaster_InitTransfers: errno=%d (%s)\n", errno, strerror(errno));
        return -1;
    }

    // AddrSel |= (_W5500_SPI_WRITE_ | _W5500_SPI_VDM_OP_);
    AddrSel |= (_W5500_SPI_WRITE_ | _W5500_SPI_FDM_OP_LEN1_);

    for (i = 0; i < len; i++)
    {
        uint8_t data[] = {
            (AddrSel & 0x00FF0000) >> 16,
            ((AddrSel & 0x0000FF00) >> 8) + i,
            (AddrSel & 0x000000FF) >> 0,
            pBuf[i]};

        transfers[0].flags = SPI_TransferFlags_Write;
        transfers[0].writeData = data;
        transfers[0].length = sizeof(data);

        ssize_t transferredBytes = SPIMaster_TransferSequential(spiFd, transfers, transferCount);

        if (transferredBytes < 0)
        {
            Log_Debug("errno=%d (%s)\n", errno, strerror(errno));
            return -1;
        }
    }
}

uint16_t getSn_TX_FSR(uint8_t sn)
{
    uint16_t val = 0, val1 = 0;

    do
    {
        val1 = WIZCHIP_READ(Sn_TX_FSR(sn));
        val1 = (val1 << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_TX_FSR(sn), 1));
        if (val1 != 0)
        {
            val = WIZCHIP_READ(Sn_TX_FSR(sn));
            val = (val << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_TX_FSR(sn), 1));
        }
    } while (val != val1);
    return val;
}

uint16_t getSn_RX_RSR(uint8_t sn)
{
    uint16_t val = 0, val1 = 0;

    do
    {
        val1 = WIZCHIP_READ(Sn_RX_RSR(sn));
        val1 = (val1 << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_RX_RSR(sn), 1));
        if (val1 != 0)
        {
            val = WIZCHIP_READ(Sn_RX_RSR(sn));
            val = (val << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_RX_RSR(sn), 1));
        }
    } while (val != val1);
    return val;
}

void wiz_send_data(uint8_t sn, uint8_t *wizdata, uint16_t len)
{
    uint16_t ptr = 0;
    uint32_t addrsel = 0;

    if (len == 0)
        return;
    ptr = getSn_TX_WR(sn);
    //M20140501 : implict type casting -> explict type casting
    //addrsel = (ptr << 8) + (WIZCHIP_TXBUF_BLOCK(sn) << 3);
    addrsel = ((uint32_t)ptr << 8) + (WIZCHIP_TXBUF_BLOCK(sn) << 3);
    //
    WIZCHIP_WRITE_BUF(addrsel, wizdata, len);

    ptr += len;
    setSn_TX_WR(sn, ptr);
}

void wiz_recv_data(uint8_t sn, uint8_t *wizdata, uint16_t len)
{
    uint16_t ptr = 0;
    uint32_t addrsel = 0;

    if (len == 0)
        return;
    ptr = getSn_RX_RD(sn);
    //M20140501 : implict type casting -> explict type casting
    //addrsel = ((ptr << 8) + (WIZCHIP_RXBUF_BLOCK(sn) << 3);
    addrsel = ((uint32_t)ptr << 8) + (WIZCHIP_RXBUF_BLOCK(sn) << 3);
    //
    WIZCHIP_READ_BUF(addrsel, wizdata, len);
    ptr += len;

    setSn_RX_RD(sn, ptr);
}

void wiz_recv_ignore(uint8_t sn, uint16_t len)
{
    uint16_t ptr = 0;

    ptr = getSn_RX_RD(sn);
    ptr += len;
    setSn_RX_RD(sn, ptr);
}

#endif
