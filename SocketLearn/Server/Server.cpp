// Server.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <WinSock2.h>

#include <iostream>
using namespace std;

#pragma comment(lib, "ws2_32.lib")


int _tmain(int argc, _TCHAR* argv[])
{   
    
    sockaddr_in     ServerAddr;
    sockaddr_in     ClientAddr;
    SOCKET          ListenSocket;
    SOCKET          ClientSocket;
    WSADATA         wsaData = {0};
    unsigned short  uPort   = 6628;
    char            szIP[]  = "127.0.0.1";

    int nRetCode = WSAStartup(MAKEWORD(-1,-10), &wsaData);// 1)
    if (nRetCode != 0)
    {
        printf("%s occurs errs at line %d, error code is %d\n", __FUNCTION__, __LINE__, nRetCode);
        return 0;
    }

    
    ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);// 2)
    if (ListenSocket == INVALID_SOCKET)
    {
        printf("%s occurs errs at line %d, error code is %d\n", __FUNCTION__, __LINE__, WSAGetLastError());
        return 0;
    }
    
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_port = htons(uPort);
    ServerAddr.sin_addr.S_un.S_addr = inet_addr((const char*)szIP);
    
    nRetCode = bind(ListenSocket,(sockaddr*)&ServerAddr, sizeof(ServerAddr)); // 3)
    if (nRetCode == SOCKET_ERROR)
    {
        printf("%s occurs errs at line %d, error code is %d\n", __FUNCTION__, __LINE__, WSAGetLastError());
        return 0;
    }


    nRetCode = listen(ListenSocket, 5); // 4)
    if (nRetCode == SOCKET_ERROR)
    {
        printf("%s occurs errs at line %d, error code is %d\n", __FUNCTION__, __LINE__, WSAGetLastError());
        goto Exit1;
    }

    int nSocketAddrLen = sizeof(ClientAddr);
    memset(&ClientAddr, 0, nSocketAddrLen);


    printf("Server listen at %s:%u\n", inet_ntoa(ServerAddr.sin_addr), ntohs(ServerAddr.sin_port));

    ClientSocket = accept(ListenSocket, (sockaddr*)&ClientAddr, &nSocketAddrLen); // 5)

    if (ClientSocket == INVALID_SOCKET)
    {
        printf("%s occurs errs at line %d, error code is %d\n", __FUNCTION__, __LINE__, WSAGetLastError());
        goto Exit1;
    }

    
    printf("ip %s port %u connected!\n",inet_ntoa(ClientAddr.sin_addr), ntohs(ClientAddr.sin_port));
    
    
    getchar();

Exit0:
    closesocket(ListenSocket);
    closesocket(ClientSocket);
Exit1:
    WSACleanup();
	return 0;
}

