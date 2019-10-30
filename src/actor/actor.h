/*
 * Actor.h
 *
 *  Created on: May 26, 2016
 *      Author: ChauNM
 */

#ifndef ACTOR_H_
#define ACTOR_H_

#include <mosquitto.h>
#include "typesdef.h"
#define HOST 		"iot.eclipse.org"
#define PORT		1883
#define QOS         1
#define TIMEOUT     10000L

#define CALLBACK_ONCE		0
#define CALLBACK_RETAIN		1

typedef void (*ACTORCALLBACKFN)(void*);

typedef void* CallbackParam;
//#pragma pack (1)
typedef struct tagACTOROPTION {
	char* guid;
	char* user; 
	char* psw;
	char* host;
	WORD port;
	char* caCert;
	char* clientCrt;
	char* clientKey;
} ACTOROPTION, *PACTOROPTION;

//#pragma pack(1)
typedef struct tagACTOR {
	struct mosquitto* client;
	int DeliveredToken;
	char connected;
	ACTOROPTION options;
}ACTOR, *PACTOR;

PACTOR ActorCreate(char* guid, char* user, char* psw, char* host, WORD port, char* ca, char* clientCrt, char* clientKey);
/* Delete an actor */
void ActorDelete(PACTOR pActor);
/* Actor publish a message to topicName and trigger a callback for response handling */
void ActorSend(PACTOR pActor, char* topicName, char* message);
/* Get uuid name of an actor */
char* ActorGetGuid(PACTOR pActor);

#endif /* ACTOR_H_ */
