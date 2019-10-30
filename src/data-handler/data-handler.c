/*
 * data-handler.c
 *
 *  Created on: Oct 13, 2019
 *      Author: chaunm
 */
#include "data-handler.h"
#include "universal.h"

void HandleRequestConfig(const char* thingId, const char* param, WORD value)
{
	char* uuidString;
	char addressString[10];
	WORD address;
	int comma;
	if (strstr(thingId, "sensor") != thingId)
		return;
	for (comma = 0; comma < strlen(thingId); comma++)
	{
		if (thingId[comma] == '-')
			break;
	}
	if (comma == strlen(thingId))
		return;
	if (comma < strlen("sensor"))
		return;
	uuidString = (char*)thingId + comma + 1;
	if (strcmp(uuidString, DEVICE_UUID) != 0)
		return;
	memset(addressString, 0, sizeof(addressString));
	memcpy(addressString, thingId + strlen("sensor"), comma - strlen("sensor"));
	printf("Configure %s for sensor %s, value: %d\n", param, addressString, value);
	address = atoi(addressString);
	if (address == 0)
		return;
	if (strcmp(param, "max_temp") == 0)
	{
		UartRequestSentParamSet(address, 2, value);
	}
	if (strcmp(param, "min_temp") == 0)
	{
		UartRequestSentParamSet(address, 1, value);
	}
	if (strcmp(param, "max_humi") == 0)
	{
		UartRequestSentParamSet(address, 4, value);
	}
	if (strcmp(param, "min_humi") == 0)
	{
		UartRequestSentParamSet(address, 3, value);
	}
}

