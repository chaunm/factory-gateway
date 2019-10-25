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

/* make topic name for actor to public or subcribe
 * A topic name has form of actor's guid/topic
 * guid: Actor's guid
 * topic: topic to subscribe or publish, topic can be
 * 		/:request/...
 * 		/:response
 * 		/:event
 */
char* ActorMakeTopicName(const char* messageType, const char* guid, char* topic);
/* register callback for actor
 * event: event to trigger callback
 * callback:
 * 		fncallback to call when event occur (void fncallback(PVOID* pParam)
 * 			pParam is a pointer to a data that user can pass when callback occur
 * 			this param must be allocated to pass to callback and will be free after callback has been processed by actor
 * callbackType:
 * 		CALLBACK_ONCE: callback is only triggered one time and will be free.
 * 		CALLBACK_RETAIN: callback is triggered every time event occurs
 */

PACTOR ActorCreate(char* guid, char* user, char* psw, char* host, WORD port, char* ca, char* clientCrt, char* clientKey);
/* Delete an actor */
void ActorDelete(PACTOR pActor);
/* Actor publish a message to topicName and trigger a callback for response handling */
void ActorSend(PACTOR pActor, char* topicName, char* message);
/* Get uuid name of an actor */
char* ActorGetGuid(PACTOR pActor);

#endif /* ACTOR_H_ */
