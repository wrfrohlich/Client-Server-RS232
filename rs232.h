//-------------------------------------------------------------------------------
//
//              Name: Eng. William da Rosa Fröhlich
//
//              Project: RS232 Serial
//
//-------------------------------------------------------------------------------

#ifndef rs232_INCLUDED
#define rs232_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <windows.h>

int RS232_OpenComport(int, int, const char *, int);
int RS232_PollComport(int, char *, int);
int RS232_SendByte(int, char);
int RS232_SendBuf(int, char *, int);
void RS232_CloseComport(int);
void RS232_cputs(int, char *);
int RS232_IsDCDEnabled(int);
int RS232_IsRINGEnabled(int);
int RS232_IsCTSEnabled(int);
int RS232_IsDSREnabled(int);
void RS232_enableDTR(int);
void RS232_disableDTR(int);
void RS232_enableRTS(int);
void RS232_disableRTS(int);
void RS232_flushRX(int);
void RS232_flushTX(int);
void RS232_flushRXTX(int);
int RS232_GetPortnr(const char *);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

