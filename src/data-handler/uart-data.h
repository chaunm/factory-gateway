/*
 * uart-data.h
 *
 *  Created on: Oct 13, 2019
 *      Author: chaunm
 */

#ifndef UART_DATA_H_
#define UART_DATA_H_
#include "serialcommunication.h"
#pragma pack(1)
typedef struct tagUARTPACKAGE {
	BYTE nStart;
	BYTE nLength;
	BYTE nType;
	WORD nAddress;
	BYTE pData[];
} UARTPACKAGE, *PUARTPACKAGE;

#define _UARTPACKAGE(pBuffer)  (PUARTPACKAGE)(pBuffer)

#define PACKAGE_TYPE_DEVICE_REGISTER	0x0E	// 14
#define PACKAGE_TYPE_DEVICE_PARAM	    0x0F	// 15
#define PACKAGE_TYPE_DEVICE_FLASH       0x38

void UartHandleBuffer(PBYTE pBuffer, BYTE size);
void UartSendSensorRequestRegister(PSERIAL pSerial);
#endif /* UART_DATA_H_ */
