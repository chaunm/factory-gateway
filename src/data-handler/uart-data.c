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

static BYTE reqRegs[8] = {  170, 5, 14, 31, 0, 1, 3, 85  };
static BYTE reqParams[8] = { 170, 5, 14, 31, 0, 1, 4, 85  };
static BYTE setParams[10] = { 170, 7, 15, 31, 0, 0, 0, 0, 0, 85 };
static BYTE sensorIndex = 25;
static BOOL setParam = FALSE;
static PSERIAL serialPort = NULL;

static void UartHandleRegisterPackage(PBYTE pBuffer)
{
	BYTE regCount;
	BYTE index;
	WORD address = __BUFFER_ADDRESS(pBuffer);
	PREGISTER pReg;
	regCount = __BUFFER_LENGTH(pBuffer) - 3;
	regCount = regCount / sizeof(REGISTER);
	printf("received register report from device %d\n", address);
	pReg = (PREGISTER)(__BUFFER_DATA(pBuffer));
	for (index = 0; index < regCount; index++)
	{
		printf("Register: %d, value: %d\n", pReg->reg, pReg->value);
		SensorUpdateReg(address, pReg->reg, pReg->value);
		pReg++;
	}
	printf("temp = %d\n", ((WORD)SensorGetReg(address, TEMP_HIGH_REG) << 8) +
			SensorGetReg(address, TEMP_LOW_REG));
	printf("humi = %d\n", ((WORD)SensorGetReg(address, HUMI_HIGH_REG) << 8) +
				SensorGetReg(address, HUMI_LOW_REG));
//	SensorSendStates(address);
	SensorSendSingleState(address, TYPE_TEMP);
	SensorSendSingleState(address, TYPE_HUMI);
	if (SensorGetReg(address, HUMI_ALARM_REG) == 1)
	{
		SensorSendAlert(address, TYPE_HUMI, 1);
	}
	if (SensorGetReg(address, TEMP_ALARM_REG) == 1)
	{
		SensorSendAlert(address, TYPE_TEMP, 1);
	}
}

static void UartHandleParameterPackage(PBYTE pBuffer)
{
	BYTE paramCount;
	BYTE index;
	PPARAMETER pParam;
	WORD address = __BUFFER_ADDRESS(pBuffer);
	paramCount = __BUFFER_LENGTH(pBuffer) - 3;
	paramCount = paramCount / sizeof(PARAMETER);
	printf("received parameter report from device %d\n", address);
	pParam = (PPARAMETER)(__BUFFER_DATA(pBuffer));
	for (index = 0; index < paramCount; index++)
	{
		printf("Parameter: %d, value: %d\n", pParam->param, pParam->value);
		SensorUpdateParam(address, pParam->param, pParam->value);
		pParam++;
	}
}

void UartHandleBuffer(PBYTE pBuffer, BYTE size)
{
	switch (__BUFFER_TYPE(pBuffer))
	{
	case PACKAGE_TYPE_DEVICE_REGISTER:
		UartHandleRegisterPackage(pBuffer);
		break;
	case PACKAGE_TYPE_DEVICE_PARAM:
		UartHandleParameterPackage(pBuffer);
		break;
	default:
		break;
	}
}

void UartSendParamSet(PSERIAL pSerial, WORD address, WORD param, WORD value)
{
	PBYTE pBuffer = setParams;
	PPARAMETER pParam;
	printf("send param_set param: %d, value: %d\n", param, value);
	__BUFFER_ADDRESS(pBuffer) = address;
	pParam = (PPARAMETER)(__BUFFER_DATA(pBuffer));
	pParam->param = param;
	pParam->value = value;
	SerialOutput(pSerial, setParams, sizeof(setParams));
}

void UartRequestSentParamSet(WORD address, WORD param, WORD value)
{
	if (serialPort == NULL)
		return;
	UartSendParamSet(serialPort, address, param, value);
	setParam = TRUE;
}

void UartSendSensorRequestRegister(PSERIAL pSerial)
{
	PBYTE pBuffer = reqRegs;
	printf("request sensor %d\n", sensorIndex);
	__BUFFER_ADDRESS(pBuffer) = GetSensorAddress(sensorIndex);
	SerialOutput(pSerial, reqRegs, sizeof(reqRegs));
	pBuffer = reqParams;
	__BUFFER_ADDRESS(pBuffer) = GetSensorAddress(sensorIndex);
	SerialOutput(pSerial, reqParams, sizeof(reqParams));
	sensorIndex++;
	if(sensorIndex == NUMBER_OF_SENSORS)
		sensorIndex = 0;

}

void UartSendProcess(PSERIAL pSerial)
{
	serialPort = pSerial;
	if (setParam == TRUE)
	{
		sleep(1);
		setParam = FALSE;
	}
	else
	{
		UartSendSensorRequestRegister(serialPort);
	}
}


