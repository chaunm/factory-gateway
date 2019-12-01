/*
 * uart-data.h
 *
 *  Created on: Oct 13, 2019
 *      Author: chaunm
 */

#ifndef UART_DATA_H_
#define UART_DATA_H_
#include "serialcommunication.h"

#define __BUFFER_LENGTH(pBuffer)	(pBuffer[1])
#define __BUFFER_TYPE(pBuffer)		(pBuffer[2])
#define __BUFFER_ADDRESS(pBuffer)	(*(PWORD)(pBuffer + 3))
#define __BUFFER_DATA(pBuffer)		(pBuffer + 5)

// set param package
#define __BUFFER_PARAM_SET(pBuffer) (*(PWORD)(pBuffer + 5))
#define __BUFFER_PARAM_VALUE

#define _UARTPACKAGE(pBuffer)  (PUARTPACKAGE)(pBuffer)

#define PACKAGE_TYPE_DEVICE_REGISTER	0x0E	// 14
#define PACKAGE_TYPE_DEVICE_PARAM	    0x0F	// 15
#define PACKAGE_TYPE_DEVICE_FLASH       0x38

void UartHandleBuffer(PBYTE pBuffer, BYTE size);
void UartRequestSentParamSet(WORD address, WORD param, WORD value);
void UartRequestSentRegisterSet(WORD address, BYTE reg, BYTE value);
void UartSendProcess(PSERIAL pSerial);
#endif /* UART_DATA_H_ */
