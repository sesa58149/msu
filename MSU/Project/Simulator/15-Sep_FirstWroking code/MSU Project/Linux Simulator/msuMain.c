/*
 * msuMain.c
 *
 *  Created on: 25-Aug-2011
 *      Author: root
 */

#include <stdio.h>
#include <string.h>
#include "MsuPlatform.h"
#include "MsuFilePlatform.h"
#include "MsuPdu.h"
#include "MsuSocket.h"
#include "MsuLeech.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "MsuPdu.h"
#include "MsuSocket.h"
#include <errno.h>

extern  BOOL msuSendDiscoveryResponse(MSUDiscoveryRequestPdu *discoveryRequestPdu);
#if 1
int main(int argc, char *argv[])
{
	 BOOL paramStatus = FALSE;

	    memset(clientIP, 0, sizeof(clientIP));

	    /*printf("\nNo of items received = %d\n", argc);
	    for(i = 0; i < argc; i++)
	    {
	    	printf("\n%s\n",argv[i]);
	    }*/

	    if(argc == 0)
	    {
	    	printf("\nNo IP address provided\n");
	    }
	    else if(argc > 2)
	    {
	    	printf("\nToo many parameters\n");
	    }
	    else if(argc == 2)
	    {
	    	//printf("\nReceived IP address %s\n",(char *) argv[1]);

	    	memcpy(clientIP,(char *) argv[1], strlen((char *)argv[1]));

	    	printf("\nClient IP address = %s\n", clientIP);

	    	paramStatus = TRUE;
	    }

	    if(paramStatus == TRUE)
	    {
	    	msuLeechStateMachine();
	    }

		return 0;

}

#endif

#if 0
int main()
{
	MSU_SOCKET sock1, sock2;
	SINT8 buff[10] = {0};
	SINT32 sentStatus =  MSU_SOCKET_SUCCESS;
	struct sockaddr_in unicastAddr = {0};
	struct sockaddr_in sockAddress;

	strcpy(buff, (char *)"test");

	sock1 = sock2 = MSU_INVALID_SOCKET;

	//Create UDP sockets
	sock1 = msuSocketCreate(AF_INET, SOCK_DGRAM, 0);
	if (sock1 == MSU_INVALID_SOCKET)
	{
		printf("\nSocket creation failed \n");
		printf("\nSocketopt error = %d\n",errno);
	}

	sock2 = msuSocketCreate(AF_INET, SOCK_DGRAM, 0);
	if (sock2 == MSU_INVALID_SOCKET)
	{
		printf("\nSocket creation failed \n");
		printf("\nSocketopt error = %d\n",errno);
	}

	memset((char *) &sockAddress, 0, sizeof(sockAddress));
	sockAddress.sin_family = AF_INET;
	sockAddress.sin_port = trHtoNs(567);
	sockAddress.sin_addr.s_addr = trHtoNl(INADDR_ANY);

	//unicastAddr.sin_addr.s_addr = inet_addr("192.168.2.75");//(0xC0A8020A);
	unicastAddr.sin_addr.s_addr = trHtoNl(0xC0A8024B);
	unicastAddr.sin_family = AF_INET;
	unicastAddr.sin_port = trHtoNs(567);

    sentStatus = sendto( sock1, buff,
    		             10, 0,
    		             (struct sockaddr*)&unicastAddr, sizeof(struct sockaddr) );
    if(sentStatus == MSU_SOCKET_ERROR )
	{
    	printf("\nSocketopt error = %d\n",errno);
		printf("\n Failed to send message");
	}

	unicastAddr.sin_addr.s_addr = inet_addr("192.168.2.75");//(0xC0A8020A);
	unicastAddr.sin_family = AF_INET;
	unicastAddr.sin_port = trHtoNs(568);

    sentStatus = sendto( sock2, buff,
    		             10, 0,
    		             (struct sockaddr*)&unicastAddr, sizeof(struct sockaddr) );
    if(sentStatus == MSU_SOCKET_ERROR )
	{
    	printf("\nSocketopt error = %d\n",errno);
		printf("\n Failed to send message");
	}

    close(sock1);
    close(sock2);

    while(1);
}
#endif
