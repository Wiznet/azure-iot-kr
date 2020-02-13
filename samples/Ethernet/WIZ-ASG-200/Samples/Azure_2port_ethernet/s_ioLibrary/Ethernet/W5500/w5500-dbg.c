#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include <stdarg.h>

#include <stdio.h>

#include "w5500-dbg.h"


typedef unsigned char U8;
typedef unsigned long DWORD;

#define wizfi_rtos_lock_mutex(a)  
#define wizfi_rtos_unlock_mutex(a)  

#pragma warning(disable : 4996)

void PrintHexLine(U8* payload, int len, int offset)
{
    int i;
    int gap;
    U8* ch;

    W_RSP("%08x   ", (DWORD)(payload));

    ch = payload;
    for (i = 0; i < len; i++)
    {
        W_RSP("%02x ", *ch);

        ch++;
        if (i == 7)                             W_RSP(" ");
    }
    if (len < 8)                 W_RSP(" ");

    if (len < 16)
    {
        gap = 16 - len;
        for (i = 0; i < gap; i++)
            W_RSP("   ");
    }
    W_RSP("   ");

    ch = payload;
    for (i = 0; i < len; i++)
    {
        if (isprint(*ch))             W_RSP("%c", *ch);
        else                                                 W_RSP(".");
        ch++;
    }

    W_RSP("\r\n");
    return;
}

void WDUMP(U8* payload, int len)
{
    int len_rem = len;
    int line_width = 16;
    int line_len;
    int offset = 0;
    U8* ch = payload;

    if (len <= 0)
    {
        return;
    }

    if (len <= line_width)
    {
        PrintHexLine(ch, len, offset);
        return;
    }

    for (;;)
    {
        line_len = line_width % len_rem;
        PrintHexLine(ch, line_len, offset);
        len_rem = len_rem - line_len;
        ch = ch + line_len;
        offset = offset + line_width;
        if (len_rem <= line_width)
        {
            PrintHexLine(ch, len_rem, offset);
            break;
        }
    }
    return;
}

void PrintHexLineExt(U8* payload, int len, DWORD offset, char* pData)
{
    int i;
    int gap;
    U8* ch;
    char szBuff[256];

    sprintf(pData, "%09x   ", (DWORD)(payload));

    ch = payload;
    for (i = 0; i < len; i++)
    {
        sprintf(szBuff, "%02x ", *ch);
        strcat(pData, szBuff);

        ch++;
        if (i == 7)
            strcat(pData, " ");
    }
    if (len < 8)
        strcat(pData, " ");

    if (len < 16)
    {
        gap = 16 - len;
        for (i = 0; i < gap; i++)
            strcat(pData, "   ");
    }
    strcat(pData, "   ");

    ch = payload;
    for (i = 0; i < len; i++)
    {
        if (isprint(*ch))
        {
            sprintf(szBuff, "%c", *ch);
            strcat(pData, szBuff);
        }
        else
            strcat(pData, ".");
        ch++;
    }

    strcat(pData, "\r\n");
    return;
}

void WDUMPEXT(U8* payload, DWORD len)
{
    DWORD len_rem = len;
    DWORD line_width = 16;
    DWORD k = 0;
    int line_len;
    DWORD offset = 0;
    U8* ch = payload;

    char szLine1[100];
    char szPreLine[100];
    U8 preData[100];

    int is_pre_print_dot = -1;
    int is_pre_print_data = -1;
    int pre_bRandomPattern = 0;

    U8 option_print_dot = 0;

    if (len <= 0)
    {
        return;
    }

    if (len <= line_width)
    {
        PrintHexLineExt(ch, len, offset, szLine1);
        W_RSP(szLine1);
        return;
    }

    for (;;)
    {
        line_len = line_width % len_rem;

        PrintHexLineExt(ch, line_len, offset, szLine1);

        option_print_dot = 0;

        if (is_pre_print_data == -1)
        {
            W_RSP(szLine1);
            is_pre_print_data = 1;
            is_pre_print_dot = 0;
        }
        else
        {
            if (memcmp(preData, ch, line_len) != 0)
            {
                U8 bRandomPattern = 0;
                for (k = 1; k < (DWORD)line_len; k++)
                {
                    if (ch[k - 1] != ch[k])
                    {
                        bRandomPattern = 1;
                        break;
                    }
                }

                if (pre_bRandomPattern == bRandomPattern)
                {
                    option_print_dot = 1;
                }

                pre_bRandomPattern = bRandomPattern;
            }
            else
            {
                option_print_dot = 1;
            }

            if (option_print_dot)
            {
                if (!is_pre_print_dot)
                {
                    W_RSP("................................................................................\r\n");
                    W_RSP("................................................................................\r\n");
                    is_pre_print_dot = 1;
                }
                is_pre_print_data = 0;
            }
            else
            {
                if (is_pre_print_data == 0)
                    W_RSP(szPreLine);
                W_RSP(szLine1);
                is_pre_print_data = 1;
                is_pre_print_dot = 0;
            }
        }

        strcpy(szPreLine, szLine1);
        memcpy(preData, ch, line_len);

        len_rem = len_rem - line_len;
        ch = ch + line_len;
        offset = offset + line_width;
        if (len_rem <= line_width)
        {
            PrintHexLineExt(ch, len_rem, offset, szLine1);
            W_RSP(szLine1);

            break;
        }
    }
    return;
}

int WXS2w_VPrintf(const char* format, va_list ap)
{
    static char buf[1024];
    int len;

    // sekim XXX for compile
           //len = vsnprintf((char*)buf, sizeof(buf), (char*)format, (__Va_list)ap);    
           //len = vsnprintf(buf, sizeof(buf), format, *(__Va_list*)(&ap));
    len = vsnprintf(buf, sizeof(buf), format, *(&ap));

    if (len < 0)
    {
        return -1;
    }

    if (len >= sizeof(buf))
    {
        len = sizeof(buf) - 1;
    }

    //wxp_put_uartbytes(buf, len);        
    buf[len] = 0;
    printf(buf);

    return len;
}

// msgLevel ==> 1:Response 2:Event 3:Debug
void W_RSP(const char* format, ...)
{
    va_list args;

    wizfi_rtos_lock_mutex(&g_upart_type1_wizmutex);

    va_start(args, format);
    WXS2w_VPrintf(format, args);
    va_end(args);
    wizfi_rtos_unlock_mutex(&g_upart_type1_wizmutex);
}

void W_EVT(const char* format, ...)
{
    va_list args;
    //if ( 2>msgLevel )         return;

    wizfi_rtos_lock_mutex(&g_upart_type1_wizmutex);
    va_start(args, format);
    WXS2w_VPrintf(format, args);
    va_end(args);
    wizfi_rtos_unlock_mutex(&g_upart_type1_wizmutex);
}

void W_DBG(const char* format, ...)
{
    va_list args;
    //if ( 3>msgLevel )         return;

    wizfi_rtos_lock_mutex(&g_upart_type1_wizmutex);
    va_start(args, format);
    WXS2w_VPrintf(format, args);
    va_end(args);
    W_RSP("\r\n");
    wizfi_rtos_unlock_mutex(&g_upart_type1_wizmutex);
}

void W_DBG2(const char* format, ...)
{
    va_list args;
    //if ( 3>msgLevel )         return;

    wizfi_rtos_lock_mutex(&g_upart_type1_wizmutex);
    va_start(args, format);
    WXS2w_VPrintf(format, args);
    va_end(args);
    wizfi_rtos_unlock_mutex(&g_upart_type1_wizmutex);
}

