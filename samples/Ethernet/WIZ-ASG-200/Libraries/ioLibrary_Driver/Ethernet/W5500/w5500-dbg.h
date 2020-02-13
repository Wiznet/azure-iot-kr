#ifndef  _W5500_DBG_H_
#define  _W5500_DBG_H_

typedef unsigned char U8;
typedef unsigned long DWORD;

void PrintHexLine(U8* payload, int len, int offset);
void WDUMP(U8* payload, int len);
void PrintHexLineExt(U8* payload, int len, DWORD offset, char* pData);
void WDUMPEXT(U8* payload, DWORD len);
int WXS2w_VPrintf(const char* format, va_list ap);
void W_RSP(const char* format, ...);
void W_EVT(const char* format, ...);
void W_DBG(const char* format, ...);
void W_DBG2(const char* format, ...);


#endif   // _W5500_H_
