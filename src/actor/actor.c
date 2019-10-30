/*
 * actor.c
 *
 *  Created on: May 26, 2016
 *      Author: ChauNM
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <jansson_config.h>
#include <jansson.h>
#include <mosquitto.h>
#include "actor.h"
#include "universal.h"
//#include "common/ActorParser.h"
#include "unistd.h"
#include "data-handler.h"


static int ActorConnect(PACTOR pACtor);
static void ActorOnMessage(struct mosquitto* client, void* context, const struct mosquitto_message* message);
static void ActorOnOffline(struct mosquitto* client, void * context, int cause);
static void ActorOnConnect(struct mosquitto* client, void* context, int result);
static void ActorOnDelivered(struct mosquitto* client, void* context, int dt);


char* ActorMakeTopicName()
{
	char* topicName = malloc(strlen("SiteWhere/factory/command/") + strlen(DEVICE_UUID) + 1);
	memset(topicName, 0, strlen("SiteWhere/factory/command/") + strlen(DEVICE_UUID) + 1);
	sprintf(topicName, "SiteWhere/factory/command/%s", DEVICE_UUID);
	return topicName;
}

PACTOR ActorCreate(char* guid, char* user, char* psw, char* host, WORD port, char* ca, char* clientCrt, char* clientKey)
{
	if ((guid == NULL))
		return NULL;
	PACTOR pActor = (PACTOR)malloc(sizeof(ACTOR));
	memset(pActor, 0, sizeof(ACTOR));
	pActor->options.guid = StrDup(guid);
	pActor->options.user = StrDup(user);
	pActor->options.psw = StrDup(psw);
	if (host != NULL)
		pActor->options.host = StrDup(host);
	else
		pActor->options.host = StrDup(HOST);
	if (port != 0)
		pActor->options.port = port;
	else
		pActor->options.port = PORT;
	pActor->options.caCert = StrDup(ca);
	pActor->options.clientCrt = StrDup(clientCrt);
	pActor->options.clientKey = StrDup(clientKey);
	pActor->connected = FALSE;
	while (pActor->client == NULL)
	{
		ActorConnect(pActor);
		sleep(5);
	}

	if (pActor->client != NULL)
	{
		return pActor;
	}
	else
	{
		ActorDelete(pActor);
		return NULL;
	}

}

void ActorDelete(PACTOR pActor)
{

	mosquitto_disconnect(pActor->client);
	mosquitto_destroy(pActor->client);
	free(pActor->options.guid);
	free(pActor->options.host);
	free(pActor->options.user);
	if (pActor->options.psw != NULL)
		free(pActor->options.psw);
	if (pActor->options.caCert != NULL)
		free(pActor->options.caCert);
	if (pActor->options.caCert != NULL)
		free(pActor->options.caCert);
	if (pActor->options.caCert != NULL)
		free(pActor->options.caCert);
	free(pActor);
}

void ActorLogCallback(struct mosquitto *mosq, void *obj, int level, const char *str)
{
    printf("%s\n", str);
}

int ActorConnect(PACTOR pActor)
{
    int rc;
    int status;
    struct mosquitto* client;
    static int mqttProtocol = MQTT_PROTOCOL_V311;
    if (pActor->client == NULL)
    {
    	printf("create mosquitto client with id %s\n", pActor->options.guid);
    	client = mosquitto_new(pActor->options.guid, TRUE, (void*)pActor);
    	pActor->client = client;

    	// Setting callback for connection
    	mosquitto_connect_callback_set(client, ActorOnConnect);
    	mosquitto_disconnect_callback_set(client, ActorOnOffline);
    	mosquitto_message_callback_set(client, ActorOnMessage);
    	mosquitto_publish_callback_set(client, ActorOnDelivered);
    	mosquitto_opts_set(client, MOSQ_OPT_PROTOCOL_VERSION, &mqttProtocol);
    	//logging
//    	mosquitto_log_callback_set(client, ActorLogCallback);

    	// set tls option
    	if ((pActor->options.caCert != NULL) && (pActor->options.clientCrt != NULL) && (pActor->options.clientKey != NULL))
    	{
    		status = mosquitto_tls_set(client, pActor->options.caCert, NULL, pActor->options.clientCrt, pActor->options.clientKey, NULL);
    		printf("%s mosquitto_tls_set status: %d\n", pActor->options.guid, status);
//    		status = mosquitto_tls_insecure_set(pActor->client, true);
//    		printf("tsl set insecure status  %d\n", status);
    		status = mosquitto_tls_opts_set(client, 0, "tlsv1.1", NULL);
    		printf("%s set tsl opt %d\n", pActor->options.guid, status);
    	}
    	// set user and password if needed
    	if ((pActor->options.user != NULL ) && (pActor->options.psw != NULL))
    	{
    		printf("set username: %s, password: %s\n", pActor->options.user, pActor->options.psw);
    		mosquitto_username_pw_set(client, pActor->options.user, pActor->options.psw);
    	}
    }
    else
    	client = pActor->client;
    if (client == NULL)
    	return -1;

    //connect to broker
    printf("%s connected to %s at port %d\n", pActor->options.guid, pActor->options.host, pActor->options.port);
    printf("user: %s, password: %s\n", pActor->options.user, pActor->options.psw);
    pActor->connected = 0;
    rc = mosquitto_loop_start(client);
    rc = mosquitto_connect_async(client, pActor->options.host, pActor->options.port, 60);
    printf("%s connect to %s:%d , status %d\n", pActor->options.guid, pActor->options.host,
    				pActor->options.port, rc);
    if (rc != MOSQ_ERR_SUCCESS)
    {
        mosquitto_destroy(client);
        pActor->client = NULL;
        printf("%s Failed to connect, return code %d\n", pActor->options.guid, rc);
    }
//    else
//    	ActorOnConnect(client, (void*)pActor, 0);

    return rc;
}
// chau nguyen: this function is used to send message
void ActorSend(PACTOR pActor, char* topicName, char* message)
{
	if (pActor->connected == FALSE)
		return;
	mosquitto_publish(pActor->client, &pActor->DeliveredToken, topicName, strlen(message),
			(void*)message, QOS, 0);
}

char* ActorGetGuid(PACTOR pActor)
{
	char* guid = StrDup(pActor->options.guid);
	return guid;
}

// local function

// chau nguyen: change below function for processing
// requeset template
//{"request":{"name":"config","param":"co2","value":"80", "thing_id" : "xxxx"}}
static void ActorReceive(PACTOR pActor, char* topicName, char* payload)
{
	// chau nguyen: any processing add here.
	json_t* messageJson;
	json_t* requestJson ;
	json_t* nameJson;
	json_t* thingJson;
	json_t* paramJson;
	json_t* valueJson;

	messageJson = json_loads(payload, JSON_DECODE_ANY, NULL);
	if (messageJson == NULL)
	{
		return;
	}
	requestJson = json_object_get(messageJson, "request");
	if (requestJson == NULL)
	{
		json_decref(messageJson);
		return;
	}
	nameJson = json_object_get(requestJson, "name");
	if (nameJson == NULL)
	{
		json_decref(requestJson);
		json_decref(messageJson);
		return;
	}
	if (!json_is_string(nameJson))
	{
		json_decref(nameJson);
		json_decref(requestJson);
		json_decref(messageJson);
		return;
	}
	if (strcmp(json_string_value(nameJson), "config") != 0)
	{
		json_decref(nameJson);
		json_decref(requestJson);
		json_decref(messageJson);
		return;
	}
	thingJson = json_object_get(requestJson, "thing_id");
	if (thingJson == NULL)
	{
		json_decref(nameJson);
		json_decref(requestJson);
		json_decref(messageJson);
		return;
	}
	if (!json_is_string(thingJson))
	{
		json_decref(thingJson);
		json_decref(nameJson);
		json_decref(requestJson);
		json_decref(messageJson);
		return;
	}
	paramJson = json_object_get(requestJson, "param");
	if (paramJson == NULL)
	{
		json_decref(thingJson);
		json_decref(nameJson);
		json_decref(requestJson);
		json_decref(messageJson);
		return;
	}
	if (!json_is_string(paramJson))
	{
		json_decref(thingJson);
		json_decref(nameJson);
		json_decref(paramJson);
		json_decref(requestJson);
		json_decref(messageJson);
		return;
	}
	valueJson = json_object_get(requestJson, "value");
	if (valueJson == NULL)
	{
		json_decref(thingJson);
		json_decref(nameJson);
		json_decref(paramJson);
		json_decref(requestJson);
		json_decref(messageJson);
		return;
	}
	if (json_is_number(valueJson))
	{
		WORD value = (WORD)(json_real_value(valueJson) * 10);
		HandleRequestConfig(json_string_value(thingJson), json_string_value(paramJson), value);
	}
	json_decref(thingJson);
	json_decref(nameJson);
	json_decref(paramJson);
	json_decref(valueJson);
	json_decref(requestJson);
	json_decref(messageJson);

}

// callback function for MQTTClient event
static void ActorOnMessage(struct mosquitto* client, void* context, const struct mosquitto_message* message)
{
	printf("%s received data on topic %s\n", ((PACTOR)context)->options.guid, message->topic);
	char *messageContent = malloc(message->payloadlen + 1);
	memset(messageContent, 0, message->payloadlen + 1);
	memcpy(messageContent, message->payload, message->payloadlen);
	printf("%s\n", messageContent);
	//Process on message
	ActorReceive((PACTOR)context, message->topic, messageContent);
	free(messageContent);

}


static void ActorOnOffline(struct mosquitto* client, void * context, int cause)
{
	printf("\n*** %s Connection lost***\n", ((PACTOR)context)->options.guid);
	printf("     cause: %d\n", cause);
	//retry connect
	int rc = -1;
	PACTOR pActor = (PACTOR)context;
	mosquitto_destroy(pActor->client);
	pActor->client = NULL;
	pActor->connected = FALSE;
	while (rc != MOSQ_ERR_SUCCESS)
	{
		rc = ActorConnect(pActor);
		printf("%s connect to %s:%d , status %d\n", ((PACTOR)context)->options.guid, ((PACTOR)context)->options.host,
				((PACTOR)context)->options.port, rc);
		sleep(5);
	}
}

static void ActorOnConnect(struct mosquitto* client, void* context, int result)
{
	PACTOR pActor = (PACTOR)context;
	char* topicName;
	//char* guid = pActor->guid;
	printf("%s actor connected %d\n", pActor->options.guid, result);
	if (result == 0)
	{
		pActor->connected = 1;
		// chau nguyen: defaul subcription topic here
		// topic: action/<name>/#
		topicName = ActorMakeTopicName();
		printf("subscribe to topic %s\n", topicName);
		mosquitto_subscribe(client, &pActor->DeliveredToken, topicName, QOS);
		free(topicName);

		//publish to the system about online status
		json_t* startJson = json_object();
		json_t* statusJson = json_string("status.online");
		json_object_set(startJson, "status", statusJson);
		char* startMessage = json_dumps(startJson, JSON_INDENT(4) | JSON_REAL_PRECISION(4));
		ActorSend(pActor, "SiteWhere/factory/input/json", startMessage);
		free(startMessage);
		json_decref(statusJson);
		json_decref(startJson);
	}
	else
		pActor->connected = 0;
}

static void ActorOnDelivered(struct mosquitto* client, void* context, int dt)
{
	PACTOR pActor = (PACTOR)context;
	printf("%s send message with token value %d delivery confirmed\n", pActor->options.guid, dt);
}

