#include <stdio.h>
#include "loopback.h"
#include "socket.h"
#include "wizchip_conf.h"

#include "../../applibs_versions.h"
#include <applibs/log.h>

#if LOOPBACK_MODE == LOOPBACK_MAIN_NOBLCOK

int32_t tcp_server(uint8_t sn, uint8_t *buf, uint16_t port)
{
    int32_t ret;
    uint16_t size = 0, sentsize = 0;
    uint8_t sock_buf[DATA_BUF_SIZE];

    for (int i = 0; i < DATA_BUF_SIZE; i++)
    {
        sock_buf[i] = NULL;
    }

#ifdef _LOOPBACK_DEBUG_
    uint8_t destip[4];
    uint16_t destport;
#endif

    switch (getSn_SR(sn))
    {
    case SOCK_ESTABLISHED:
        if (getSn_IR(sn) & Sn_IR_CON)
        {
#ifdef _LOOPBACK_DEBUG_
            getSn_DIPR(sn, destip);
            destport = getSn_DPORT(sn);

            Log_Debug("%d:Connected - %d.%d.%d.%d : %d\r\n", sn, destip[0], destip[1], destip[2], destip[3], destport);
#endif
            setSn_IR(sn, Sn_IR_CON);
        }
        if ((size = getSn_RX_RSR(sn)) > 0) // Don't need to check SOCKERR_BUSY because it doesn't not occur.
        {
            if (size > DATA_BUF_SIZE)
                size = DATA_BUF_SIZE;

            ret = sock_recv(sn, sock_buf, size);

            if (ret <= 0)
                return ret; // check SOCKERR_BUSY & SOCKERR_XXX. For showing the occurrence of SOCKERR_BUSY.

                //size = (uint16_t)ret;
                //sentsize = 0;

#if 0
            int j = 0;
            for (j = 0; j < size; j++)
            {
                // address
                if (j % 16 == 0)
                {
                    Log_Debug("0x%08x  ", sock_buf);
                }
                // value
                Log_Debug("0x%02x ", sock_buf[j]);
                // arrange
                if (j % 16 - 15 == 0)
                {
                    int k;
                    Log_Debug("  ");
                    for (k = j - 15; k < j + 1; k++)
                    {
                        Log_Debug("%c", sock_buf[k]);
                    }
                    Log_Debug("\n");
                }
            }

            if (j % 16 != 0)
            {
                int l;
                int spaces = (size - j + 16 - j % 16) * 3 + 2;
                for (l = 0; l < spaces; l++)
                {
                    Log_Debug("  ");
                }
                for (l = j - j % 16; l < size; l++)
                {
                    Log_Debug("%c", sock_buf[l]);
                }
            }
            Log_Debug("\n");
#endif

#if 0
            Log_Debug("loopback.c (%d) -> %s ", size, sock_buf);
#endif

            //while (size != sentsize)
            //{
            //   ret = sock_send(sn, buf + sentsize, size - sentsize);
            //   if (ret < 0)
            //   {
            //      close_socket(sn);
            //      return ret;
            //   }
            //   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
            //}
        }
        break;
    case SOCK_CLOSE_WAIT:
#ifdef _LOOPBACK_DEBUG_
        //Log_Debug("%d:CloseWait\r\n",sn);
#endif
        if ((ret = sock_disconnect(sn)) != SOCK_OK)
            return ret;
#ifdef _LOOPBACK_DEBUG_
        Log_Debug("%d:Socket Closed\r\n", sn);
#endif
        break;
    case SOCK_INIT:
#ifdef _LOOPBACK_DEBUG_
        Log_Debug("%d:Listen, TCP server loopback, port [%d]\r\n", sn, port);
#endif
        if ((ret = sock_listen(sn)) != SOCK_OK)
            return ret;
        break;
    case SOCK_CLOSED:
#ifdef _LOOPBACK_DEBUG_
        //Log_Debug("%d:TCP server loopback start\r\n",sn);
#endif
        if ((ret = wiz_socket(sn, Sn_MR_TCP, port, 0x00)) != sn)
            return ret;
#ifdef _LOOPBACK_DEBUG_
            //Log_Debug("%d:Socket opened\r\n",sn);
#endif
        break;
    default:
        break;
    }
    return 1;
}

int32_t loopback_tcps(uint8_t sn, uint8_t *buf, uint16_t port)
{
    int32_t ret;
    uint16_t size = 0, sentsize = 0;

#ifdef _LOOPBACK_DEBUG_
    uint8_t destip[4];
    uint16_t destport;
#endif

    switch (getSn_SR(sn))
    {
    case SOCK_ESTABLISHED:
        if (getSn_IR(sn) & Sn_IR_CON)
        {
#ifdef _LOOPBACK_DEBUG_
            getSn_DIPR(sn, destip);
            destport = getSn_DPORT(sn);

            Log_Debug("%d:Connected - %d.%d.%d.%d : %d\r\n", sn, destip[0], destip[1], destip[2], destip[3], destport);
#endif
            setSn_IR(sn, Sn_IR_CON);
        }
        if ((size = getSn_RX_RSR(sn)) > 0) // Don't need to check SOCKERR_BUSY because it doesn't not occur.
        {
            if (size > DATA_BUF_SIZE)
                size = DATA_BUF_SIZE;
            ret = sock_recv(sn, buf, size);

            if (ret <= 0)
                return ret; // check SOCKERR_BUSY & SOCKERR_XXX. For showing the occurrence of SOCKERR_BUSY.
            size = (uint16_t)ret;
            sentsize = 0;

            Log_Debug("loopback_tcps() Received data from sn %d: (%d) %s\n", sn, size - sentsize, buf + sentsize);

            while (size != sentsize)
            {

                ret = sock_send(sn, buf + sentsize, size - sentsize);
                if (ret < 0)
                {
                    close_socket(sn);
                    return ret;
                }
                sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
            }
        }
        break;
    case SOCK_CLOSE_WAIT:
#ifdef _LOOPBACK_DEBUG_
        //Log_Debug("%d:CloseWait\r\n",sn);
#endif
        if ((ret = sock_disconnect(sn)) != SOCK_OK)
            return ret;
#ifdef _LOOPBACK_DEBUG_
        Log_Debug("%d:Socket Closed\r\n", sn);
#endif
        break;
    case SOCK_INIT:
#ifdef _LOOPBACK_DEBUG_
        Log_Debug("%d:Listen, TCP server loopback, port [%d]\r\n", sn, port);
#endif
        if ((ret = sock_listen(sn)) != SOCK_OK)
            return ret;
        break;
    case SOCK_CLOSED:
#ifdef _LOOPBACK_DEBUG_
        //Log_Debug("%d:TCP server loopback start\r\n",sn);
#endif
        if ((ret = wiz_socket(sn, Sn_MR_TCP, port, 0x00)) != sn)
            return ret;
#ifdef _LOOPBACK_DEBUG_
        Log_Debug("%d:Socket opened\r\n", sn);
#endif
        break;
    default:
        break;
    }
    return 1;
}

int32_t loopback_tcpc(uint8_t sn, uint8_t *buf, uint8_t *destip, uint16_t destport)
{
    int32_t ret; // return value for SOCK_ERRORs
    uint16_t size = 0, sentsize = 0;

    // Destination (TCP Server) IP info (will be connected)
    // >> loopback_tcpc() function parameter
    // >> Ex)
    //	uint8_t destip[4] = 	{192, 168, 0, 214};
    //	uint16_t destport = 	5000;

    // Port number for TCP client (will be increased)
    static uint16_t any_port = 50000;

    // Socket Status Transitions
    // Check the W5500 Socket n status register (Sn_SR, The 'Sn_SR' controlled by Sn_CR command or Packet send/recv status)
    switch (getSn_SR(sn))
    {
    case SOCK_ESTABLISHED:
        if (getSn_IR(sn) & Sn_IR_CON) // Socket n interrupt register mask; TCP CON interrupt = connection with peer is successful
        {
#ifdef _LOOPBACK_DEBUG_
            Log_Debug("%d:Connected to - %d.%d.%d.%d : %d\r\n", sn, destip[0], destip[1], destip[2], destip[3], destport);
#endif
            setSn_IR(sn, Sn_IR_CON); // this interrupt should be write the bit cleared to '1'
        }

        //////////////////////////////////////////////////////////////////////////////////////////////
        // Data Transaction Parts; Handle the [data receive and send] process
        //////////////////////////////////////////////////////////////////////////////////////////////
        if ((size = getSn_RX_RSR(sn)) > 0) // Sn_RX_RSR: Socket n Received Size Register, Receiving data length
        {
            if (size > DATA_BUF_SIZE)
                size = DATA_BUF_SIZE;       // DATA_BUF_SIZE means user defined buffer size (array)
            ret = sock_recv(sn, buf, size); // Data Receive process (H/W Rx socket buffer -> User's buffer)

            if (ret <= 0)
                return ret; // If the received data length <= 0, receive failed and process end
            size = (uint16_t)ret;
            sentsize = 0;

            // Data sentsize control
            while (size != sentsize)
            {
                ret = sock_send(sn, buf + sentsize, size - sentsize); // Data send process (User's buffer -> Destination through H/W Tx socket buffer)
                if (ret < 0)                                          // Send Error occurred (sent data length < 0)
                {
                    close_socket(sn); // socket close
                    return ret;
                }
                sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
            }
        }
        //////////////////////////////////////////////////////////////////////////////////////////////
        break;

    case SOCK_CLOSE_WAIT:
#ifdef _LOOPBACK_DEBUG_
        //Log_Debug("%d:CloseWait\r\n",sn);
#endif
        if ((ret = sock_disconnect(sn)) != SOCK_OK)
            return ret;
#ifdef _LOOPBACK_DEBUG_
        Log_Debug("%d:Socket Closed\r\n", sn);
#endif
        break;

    case SOCK_INIT:
#ifdef _LOOPBACK_DEBUG_
        Log_Debug("%d:Try to connect to the %d.%d.%d.%d : %d\r\n", sn, destip[0], destip[1], destip[2], destip[3], destport);
#endif
        if ((ret = sock_connect(sn, destip, destport)) != SOCK_OK)
            return ret; //	Try to TCP connect to the TCP server (destination)
        break;

    case SOCK_CLOSED:
        close_socket(sn);
        if ((ret = wiz_socket(sn, Sn_MR_TCP, any_port++, 0x00)) != sn)
        {
            if (any_port == 0xffff)
                any_port = 50000;
            return ret; // TCP socket open with 'any_port' port number
        }
#ifdef _LOOPBACK_DEBUG_
        //Log_Debug("%d:TCP client loopback start\r\n",sn);
        //Log_Debug("%d:Socket opened\r\n",sn);
#endif
        break;
    default:
        break;
    }
    return 1;
}

int32_t loopback_udps(uint8_t sn, uint8_t *buf, uint16_t port)
{
    int32_t ret;
    uint16_t size, sentsize;
    uint8_t destip[4];
    uint16_t destport;

    switch (getSn_SR(sn))
    {
    case SOCK_UDP:
        if ((size = getSn_RX_RSR(sn)) > 0)
        {
            if (size > DATA_BUF_SIZE)
                size = DATA_BUF_SIZE;
            ret = sock_recvfrom(sn, buf, size, destip, (uint16_t *)&destport);
            if (ret <= 0)
            {
#ifdef _LOOPBACK_DEBUG_
                Log_Debug("%d: recvfrom error. %ld\r\n", sn, ret);
#endif
                return ret;
            }
            size = (uint16_t)ret;
            sentsize = 0;
            while (sentsize != size)
            {
                ret = sock_sendto(sn, buf + sentsize, size - sentsize, destip, destport);
                if (ret < 0)
                {
#ifdef _LOOPBACK_DEBUG_
                    Log_Debug("%d: sock_sendto error. %ld\r\n", sn, ret);
#endif
                    return ret;
                }
                sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
            }
        }
        break;
    case SOCK_CLOSED:
#ifdef _LOOPBACK_DEBUG_
        //Log_Debug("%d:UDP loopback start\r\n",sn);
#endif
        if ((ret = wiz_socket(sn, Sn_MR_UDP, port, 0x00)) != sn)
            return ret;
#ifdef _LOOPBACK_DEBUG_
        Log_Debug("%d:Opened, UDP loopback, port [%d]\r\n", sn, port);
#endif
        break;
    default:
        break;
    }
    return 1;
}

#endif
