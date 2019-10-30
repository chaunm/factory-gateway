
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

#include "universal.h"
#include "typesdef.h"
VOID CopyMemory (PBYTE pDest, PBYTE pSource, BYTE nLength)
{
	BYTE nIndex;
	if ((pDest == NULL) || (pSource == NULL))
			return;
	for (nIndex = 0; nIndex < nLength; nIndex++)
	{
		pDest[nIndex] = pSource[nIndex];
	}
}


char* StrDup(const char* string)
{
	if (string == NULL) return NULL;
	char* pDest = malloc(strlen(string) + 1);
	memset(pDest, 0, strlen(string) + 1);
	memcpy(pDest, string, strlen(string));
	return pDest;
}

char* IeeeToString(IEEEADDRESS macId)
{
	char* macIdString = malloc(17);
	memset(macIdString, 0, 17);
	PWORD macIdArray = (PWORD)(&macId);
	sprintf(macIdString, "%04X%04X%04X%04X", macIdArray[3], macIdArray[2], macIdArray[1], macIdArray[0]);
	return macIdString;
}

VOID GetIpAddress(char* ip)
{
//	if (sizeof(ip) < 16)
//		return;
	int fd;
	struct ifreq ifr;
	fd = socket(AF_INET, SOCK_DGRAM, 0);

	/* I want to get an IPv4 IP address */
	ifr.ifr_addr.sa_family = AF_INET;
	/* I want IP address attached to "eth0" */
	strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);
	ioctl(fd, SIOCGIFADDR, &ifr);
	close(fd);
	/* return result */
	sprintf(ip, "%s", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
}

VOID MakeSensorUUID(char* string, WORD address)
{
	sprintf(string, "sensor%d-%s", address, DEVICE_UUID);
}
