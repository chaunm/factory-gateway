/*
 * sensor.c
 *
 *  Created on: Oct 14, 2019
 *      Author: chaunm
 */

#include <stdio.h>
#include <jansson.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include "sensor.h"
#include "factory-actor.h"
#include "universal.h"

SENSOR sensorList[NUMBER_OF_SENSORS];
static VOID SensorCreateEventDate(char* inputString)
{
	time_t currentTime = time(NULL);
	WORD miliseconds;
	int differentMin;
	struct timeval tv;
	struct tm timest = *localtime(&currentTime);
	struct tm reftime;
	char dateString[12];
	char timeString[12];
	memcpy(&reftime, &timest, sizeof(struct tm));
	strftime(timeString, 30, "%T", &timest);
	strftime(dateString, 12, "%F", &timest);
	sprintf(inputString, "%sT%s", dateString, timeString);
	gettimeofday(&tv, NULL);
	miliseconds = (WORD)(tv.tv_usec / 1000);
	sprintf(inputString, "%s.%03d", inputString, miliseconds);
	differentMin = (timegm(&timest) - mktime(&reftime)) / 60;
	if (differentMin >= 0)
		sprintf(inputString, "%s+%02d%02d", inputString, differentMin / 60, differentMin % 60);
	else
		sprintf(inputString, "%s-%02d%02d", inputString, differentMin / 60, differentMin % 60);
}

VOID SensorInit()
{
	int i, j;
	for (i = 0; i < NUMBER_OF_SENSORS; i++)
	{
		sensorList[i].address = i + 2;	// change due to node assignment -chau nguyen
		sensorList[i].Humi = 0;
		sensorList[i].Temp = 0;
		sensorList[i].timeout = FALSE;
		sensorList[i].regs[TEMP_HIGH_REG] = 0;
		sensorList[i].regs[TEMP_LOW_REG] = 0;
		sensorList[i].regs[HUMI_HIGH_REG] = 0;
		sensorList[i].regs[HUMI_LOW_REG] = 0;
		sensorList[i].regs[ALARM_REG] = 0;
		for (j = 0; j < NUMBER_OF_PARAMS; j++)
		{
			sensorList[i].params[j] = 0;
		}
	}
}

VOID SensorUpdateReg(WORD address, BYTE reg, BYTE value)
{
	int i;
	for (i = 0; i < NUMBER_OF_SENSORS; i++)
	{
		if (sensorList[i].address == address)
		{
			sensorList[i].regs[reg] = value;
			break;
		}
	}
}

VOID SensorUpdateParam(WORD address, WORD param, WORD value)
{
	int i;
	for (i = 0; i < NUMBER_OF_SENSORS; i++)
	{
		if (sensorList[i].address == address)
		{
			sensorList[i].params[param] = value;
			break;
		}
	}
}

WORD GetSensorAddress(BYTE nIndex)
{
	return sensorList[nIndex].address;
}


VOID SensorSendStates(WORD address)
{
	BYTE nIndex;
	char timeJsonString[80];
	for (nIndex = 0; nIndex < NUMBER_OF_SENSORS; nIndex++)
		if (sensorList[nIndex].address == address)
			break;
	SensorCreateEventDate(timeJsonString);
	sensorList[nIndex].Humi = (WORD)(sensorList[nIndex].regs[HUMI_HIGH_REG]) << 8;
	sensorList[nIndex].Humi += sensorList[nIndex].regs[HUMI_LOW_REG];
	sensorList[nIndex].Temp = (WORD)(sensorList[nIndex].regs[TEMP_HIGH_REG]) << 8;
	sensorList[nIndex].Temp += sensorList[nIndex].regs[TEMP_LOW_REG];
	float temp = (float)sensorList[nIndex].Temp / 10;
	float humi = (float)sensorList[nIndex].Humi / 10;
	json_t* eventJson = json_object();
	json_t* tempJson = json_object();
	json_t* humiJson = json_object();
	json_t* typeJson = json_string("DeviceMeasurement");
	json_t* addressJson = json_integer(sensorList[nIndex].address);
	json_t* originatorJson = json_string("Factory");
	json_t* timeJson = json_string(timeJsonString);
	json_object_set(eventJson, "type", typeJson);
	json_object_set(eventJson, "originator", originatorJson);
	json_object_set(eventJson, "address", addressJson);
	json_object_set(eventJson, "eventDate", timeJson);
	json_decref(addressJson);
	json_decref(typeJson);
	json_decref(originatorJson);
	json_decref(timeJson);
	json_t* tempValueJson = json_real(temp);
	json_t* tempUnitJson = json_string("celcius");
	json_object_set(tempJson, "value", tempValueJson);
	json_object_set(tempJson, "unit", tempUnitJson);
	json_decref(tempValueJson);
	json_decref(tempUnitJson);
	json_t* humiValueJson = json_real(humi);
	json_t* humiUnitJson = json_string("%");
	json_object_set(humiJson, "value", humiValueJson);
	json_object_set(humiJson, "unit", humiUnitJson);
	json_decref(humiValueJson);
	json_decref(humiUnitJson);
	json_object_set(eventJson, "temp", tempJson);
	json_object_set(eventJson, "humi", humiJson);
	json_decref(tempJson);
	json_decref(humiJson);
	char* eventMessage = json_dumps(eventJson, JSON_INDENT(4) | JSON_REAL_PRECISION(4));
	json_decref(eventJson);
	FactoryActorSend("SiteWhere/factory/input/json", eventMessage);
	free(eventMessage);
}

VOID SensorSendSingleState(WORD address, sensor_t type)
{
	BYTE nIndex;
	float value;
	char timeJsonString[80];
	char ipString[16];
	for (nIndex = 0; nIndex < NUMBER_OF_SENSORS; nIndex++)
		if (sensorList[nIndex].address == address)
			break;
	memset(ipString, 0, sizeof(ipString));
	SensorCreateEventDate(timeJsonString);
	GetIpAddress(ipString);
	if (type == TYPE_TEMP)
	{
		sensorList[nIndex].Temp = (WORD)(sensorList[nIndex].regs[TEMP_HIGH_REG]) << 8;
		sensorList[nIndex].Temp += sensorList[nIndex].regs[TEMP_LOW_REG];
		value = (float)sensorList[nIndex].Temp / 10;
	}
	else
	{
		sensorList[nIndex].Humi = (WORD)(sensorList[nIndex].regs[HUMI_HIGH_REG]) << 8;
		sensorList[nIndex].Humi += sensorList[nIndex].regs[HUMI_LOW_REG];
		value = (float)sensorList[nIndex].Humi / 10;
	}
	json_t* eventJson = json_object();
	json_t* typeJson = json_string("DeviceMeasurement");
	json_t* addressJson = json_integer(sensorList[nIndex].address);
	json_t* originatorJson = json_string("Factory");
	json_t* tokenJson = json_string(ipString);
	json_object_set(eventJson, "type", typeJson);
	json_object_set(eventJson, "originator", originatorJson);
	json_object_set(eventJson, "deviceToken", tokenJson);
	json_object_set(eventJson, "address", addressJson);
	json_decref(addressJson);
	json_decref(typeJson);
	json_decref(originatorJson);
	json_decref(tokenJson);
	json_t* requestJson = json_object();
	json_t* nameJson;
	json_t* unitJson;
	json_t* minJson;
	json_t* maxJson;
	json_t* valueJson = json_real(value);
	json_t* metaJson = json_object();
	json_t* timeJson = json_string(timeJsonString);
	if (type == TYPE_TEMP)
	{
		nameJson = json_string("temp");
		unitJson = json_string("celcius");
		// need to finalize - chau nguy
		minJson = json_real((float)sensorList[nIndex].params[MIN_TEMP_PARAM] / 10);
		maxJson = json_real((float)sensorList[nIndex].params[MAX_TEMP_PARAM] / 10);
	}
	else
	{
		nameJson = json_string("humi");
		unitJson = json_string("%");
		// need to finalize - chau nguy
		minJson = json_real((float)sensorList[nIndex].params[MIN_HUMI_PARAM] / 10);
		maxJson = json_real((float)sensorList[nIndex].params[MAX_HUMI_PARAM] / 10);
	}
	json_object_set(requestJson, "name", nameJson);
	json_object_set(requestJson, "value", valueJson);
	json_object_set(requestJson, "eventDate", timeJson);
	json_decref(nameJson);
	json_decref(valueJson);
	json_decref(timeJson);
	json_object_set(metaJson, "unit", unitJson);
	json_object_set(metaJson, "min", minJson);
	json_object_set(metaJson, "max", maxJson);
	json_decref(unitJson);
	json_decref(minJson);
	json_decref(maxJson);
	json_object_set(requestJson, "meta", metaJson);
	json_decref(metaJson);
	json_object_set(eventJson, "request", requestJson);
	json_decref(requestJson);
	char* eventMessage = json_dumps(eventJson, JSON_INDENT(4) | JSON_REAL_PRECISION(4));
	json_decref(eventJson);
	FactoryActorSend("SiteWhere/factory/input/json", eventMessage);
	free(eventMessage);
}
