/*
 * uart-data.c
 *
 *  Created on: Oct 13, 2019
 *      Author: chaunm
 */
#include <stdio.h>
#include <jansson.h>
#include "uart-data.h"
#include "sensor.h"
#include "factory-actor.h"

static BYTE reqRegs[8] = { 170, 5, 14, 31, 0, 1, 3, 85  };
static BYTE reqParams[8] = { 170, 5, 14, 31, 0, 1, 4, 85  };
static BYTE sensorIndex = 0;

static void UartHandleRegisterPackage(PUARTPACKAGE pBuffer)
{
	BYTE regCount;
	BYTE index;
	PREGISTER pReg;
	regCount = pBuffer->nLength - 3;
	regCount = regCount / sizeof(REGISTER);
	printf("received register report from device %d\n", pBuffer->nAddress);
	pReg = (PREGISTER)(pBuffer->pData);
	for (index = 0; index < regCount; index++)
	{
		printf("Register: %d, value: %d\n", pReg->reg, pReg->value);
		SensorUpdateReg(pBuffer->nAddress, pReg->reg, pReg->value);
		pReg++;
	}
//	SensorSendStates(pBuffer->nAddress);
	SensorSendSingleState(pBuffer->nAddress, TYPE_TEMP);
	SensorSendSingleState(pBuffer->nAddress, TYPE_HUMI);
}

static void UartHandleParameterPackage(PUARTPACKAGE pBuffer)
{
	BYTE paramCount;
	BYTE index;
	PPARAMETER pParam;
	paramCount = pBuffer->nLength - 3;
	paramCount = paramCount / sizeof(PARAMETER);
	printf("received parameter report from device %d\n", pBuffer->nAddress);
	pParam = (PPARAMETER)(pBuffer->pData);
	for (index = 0; index < paramCount; index++)
	{
		printf("Parameter: %d, value: %d\n", pParam->param, pParam->value);
		SensorUpdateParam(pBuffer->nAddress, pParam->param, pParam->value);
		pParam++;
	}
}

void UartHandleBuffer(PBYTE pBuffer, BYTE size)
{
	PUARTPACKAGE uartPackage = _UARTPACKAGE(pBuffer);
	switch (uartPackage->nType)
	{
	case PACKAGE_TYPE_DEVICE_REGISTER:
		UartHandleRegisterPackage(uartPackage);
		break;
	case PACKAGE_TYPE_DEVICE_PARAM:
		UartHandleParameterPackage(uartPackage);
		break;
	default:
		break;
	}
}

void UartSendSensorRequestRegister(PSERIAL pSerial)
{
	PUARTPACKAGE pPackage = (PUARTPACKAGE)reqRegs;
	printf("request sensor %d\n", sensorIndex);
	pPackage->nAddress = GetSensorAddress(sensorIndex);
	SerialOutput(pSerial, reqRegs, sizeof(reqRegs));
	pPackage = (PUARTPACKAGE)reqParams;
	pPackage->nAddress = GetSensorAddress(sensorIndex);
	SerialOutput(pSerial, reqParams, sizeof(reqParams));
	sensorIndex++;
	if(sensorIndex == NUMBER_OF_SENSORS)
		sensorIndex = 0;

}
