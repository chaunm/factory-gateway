/*
 ============================================================================
 Name        : gateway-factory.c
 Author      : Chau Nguyen
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <pthread.h>
#include "sensor.h"
#include "actor.h"
#include "factory-actor.h"
#include "serialcommunication.h"
#include "universal.h"
#include "data-handler.h"

void PrintHelpMenu()
{
	printf("usage: gateway-factory --host <mqtt_broker> --port <mqtt-port> --id <user name> --password <user password>"\
			"--serial <UART port> --ca <ca-file> --cert <client cert> --key <client key>\n");
}

int main(int argc, char* argv[])
{

	pthread_t SerialProcessThread;
	pthread_t SerialOutputThread;
	pthread_t SerialHandleThread;
	ACTOROPTION actorOption = {0};
	PSERIAL	pSerialPort;
	BOOL bResult = FALSE;
	/* get option */
	int opt= 0;
	char *SerialPort = NULL;
	memset(&actorOption, 0, sizeof(actorOption));
	// specific the expected option
	static struct option long_options[] = {
			{"id",      required_argument, 0, 'i' },
			{"password", 	required_argument, 0, 'p' },
			{"serial",    required_argument, 0, 's' },
			{"host", required_argument, 0, 'H'},
			{"port", required_argument, 0, 'P'},
			{"cafile", required_argument, 0, 'c'},
			{"cert", required_argument, 0, 'r'},
			{"key", required_argument, 0, 'k'}
	};
	int long_index;
	/* Process option */
	while ((opt = getopt_long(argc, argv,":hi:p:s:H:P:c:r:k:",
			long_options, &long_index )) != -1) {
		switch (opt) {
		case 'h' :
			PrintHelpMenu();
			return EXIT_SUCCESS;
			break;
		case 's' :
			SerialPort = StrDup(optarg);
			break;
		case 'i':
			actorOption.guid = StrDup(optarg);
			break;
		case 'p':
			actorOption.psw = StrDup(optarg);
			break;
		case 'H':
			actorOption.host = StrDup(optarg);
			break;
		case 'P':
			actorOption.port = atoi(optarg);
			break;
		case 'c':
			actorOption.caCert = StrDup(optarg);
			break;
		case 'r':
			actorOption.clientCrt = StrDup(optarg);
			break;
		case 'k':
			actorOption.clientKey = StrDup(optarg);
			break;
		case ':':
			if ((optopt == 'i') || optopt == 'h')
			{
				printf("invalid option(s), using -h for help\n");
				return EXIT_FAILURE;
			}
			break;
		default:
			break;
		}
	}
	if ((SerialPort == NULL) || (actorOption.guid == NULL))
	{
		printf("invalid options, using -h for help\n");
		return EXIT_FAILURE;
	}
	printf("sizeof sensor_tag %d\n", (int)sizeof(SENSOR));
	// Start MQTT client
//	FactorActorStart(&actorOption);

	/* end options processing */
	/* open serial port and init queue for serial communication */
	char* portName = malloc(strlen("/dev/") + strlen(SerialPort) + 1);
	memset(portName, 0, strlen("/dev/") + strlen(SerialPort) + 1);
	sprintf(portName, "%s%s", "/dev/", SerialPort);
	printf("open port %s\n", portName);
	while (bResult == FALSE)
	{
		pSerialPort = SerialOpen(portName, B9600);
		if (pSerialPort == NULL)
		{
			printf("Can not open serial port %s, try another port\n", portName);
			return EXIT_FAILURE;
		}
		free(portName);
		// Initial Serial port handle process
		SerialRegisterDataProc(UartHandleBuffer);
		pthread_create(&SerialProcessThread, NULL, (void*)&SerialProcessIncomingData, (void*)pSerialPort);
		pthread_create(&SerialOutputThread, NULL, (void*)&SerialOutputDataProcess, (void*)pSerialPort);
		pthread_create(&SerialHandleThread, NULL, (void*)&SerialInputDataProcess, (void*)pSerialPort);
		// init mosquitto client
		bResult = TRUE;
	}
	SensorInit();
	while (1)
	{
//		DeviceProcessTimeout();
		UartSendSensorRequestRegister(pSerialPort);
		sleep(1);
	}
	SerialClose(pSerialPort);
	return EXIT_SUCCESS;
}
