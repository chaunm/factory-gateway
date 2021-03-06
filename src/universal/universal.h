/*
 * universal.h
 *
 *  Created on: Feb 13, 2016
 *      Author: root
 */

#ifndef UNIVERSAL_H_
#define UNIVERSAL_H_
#include "typesdef.h"

#define DEVICE_UUID	"7037ba23-20d0-496c-b852-be2d5946dd86"

VOID CopyMemory(PBYTE pDest, PBYTE pSource, BYTE nLength);
char* StrDup(const char* string);
char* IeeeToString(IEEEADDRESS macId);
VOID GetIpAddress(char* ip);
#endif /* UNIVERSAL_H_ */
