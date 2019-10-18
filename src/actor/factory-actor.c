/*
 * factory-actor.c
 *
 *  Created on: Oct 13, 2019
 *      Author: chaunm
 */

#include <mosquitto.h>
#include <jansson.h>
#include <unistd.h>
#include <pthread.h>
#include "actor.h"

static PACTOR pFactoryActor;
pthread_t factoryThread;

static void FactoryActorCreat(char* guid, char* psw, char* host, WORD port, char* ca, char* clientCrt, char* clientKey)
{
	pFactoryActor = ActorCreate(guid, psw, host, port, ca, clientCrt, clientKey);
	if (pFactoryActor == NULL)
	{
		printf("Couldn't create actor\n");
		return;
	}
}

static void FactoryActorProcess(PACTOROPTION option)
{
	mosquitto_lib_init();
	FactoryActorCreat(option->guid, option->psw, option->host, option->port, option->caCert, option->clientCrt, option->clientKey);
	if (pFactoryActor == NULL)
	{
		mosquitto_lib_cleanup();
		return;
	}
	while(1)
	{
		mosquitto_loop(pFactoryActor->client, 0, 1);
		usleep(10000);
	}
	mosquitto_disconnect(pFactoryActor->client);
	mosquitto_destroy(pFactoryActor->client);
	mosquitto_lib_cleanup();
}

void FactorActorStart(PACTOROPTION option)
{
	pthread_create(&factoryThread, NULL, (void*)&FactoryActorProcess, (void*)option);
	pthread_detach(factoryThread);
}

void FactoryActorSend(char* topic, char* message)
{
	if (pFactoryActor == NULL)
		return;
	ActorSend(pFactoryActor, topic, message);
}

