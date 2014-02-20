// Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <WinSock2.h>
#include <iostream>
using namespace std;

#pragma comment(lib, "ws2_32.lib")

int _tmain(int argc, _TCHAR* argv[])
{
    
    WSADATA         wsaData;
    sockaddr_in     ServerAddr;
    SOCKET          ClientSocket;
    int             nRetCode = 0;
    unsigned short  uPort = 6628;
    char            szIP[] = "127.0.0.1";

    nRetCode = WSAStartup(MAKEWORD(1,1), &wsaData); //1)
    if (nRetCode != 0)
    {
        printf("%s occurs errs at line %d, error code %d\n", __FUNCTION__, __LINE__, nRetCode);
        return 0;
    }

    ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //2)
    if (ClientSocket == INVALID_SOCKET)
    {
        printf("%s occurs errs at line %d, error code %d\n", __FUNCTION__, __LINE__, WSAGetLastError());
        goto Exit1;
    }

    ServerAddr.sin_family   = AF_INET;
    ServerAddr.sin_port     = htons(uPort);
    ServerAddr.sin_addr.S_un.S_addr     =  inet_addr(szIP);


    printf("Start connect %s:%u\n", inet_ntoa(ServerAddr.sin_addr), ntohs(ServerAddr.sin_port));
    nRetCode = connect(ClientSocket, (sockaddr*)&ServerAddr, sizeof(ServerAddr)); //3)
    
    if (nRetCode == SOCKET_ERROR)
    {
        printf("%s occurs errs at line %d, error code %d\n", __FUNCTION__, __LINE__, WSAGetLastError());
        goto Exit1;
    }
    
    printf("Connect %s:%u OK\n", inet_ntoa(ServerAddr.sin_addr), ntohs(ServerAddr.sin_port));
    
    const int cnBuffLen = 1024;
    char szMsg[cnBuffLen] = {0};

    while (gets_s(szMsg, cnBuffLen) != NULL)
    {
        nRetCode = send(ClientSocket, szMsg, (int)strlen(szMsg) + 1, 0);
        if (nRetCode == SOCKET_ERROR)
        {
            printf("%s occurs errs at line %d, error code %d\n", __FUNCTION__, __LINE__, WSAGetLastError());
            break;
        }
        szMsg[nRetCode] = 0;
        printf("Client send msg [%s]\n",szMsg);
    }
    
    getchar();
    closesocket(ClientSocket);
Exit1:
    WSACleanup();
	return 0;
}

