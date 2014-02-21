// Main.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <WinSock2.h>
#include <iostream>
#include "Server.h"

using namespace std;

#pragma comment(lib, "ws2_32.lib")

void Active()
{

}

int _tmain(int argc, _TCHAR* argv[])
{   
    int nRetCode = 0;

    Server* pServer = new Server();
    if (pServer == NULL)
    {
        goto Exit0;
    }

    nRetCode = pServer->Init("127.0.0.1", 6628);
    nRetCode = pServer->Listen();
    nRetCode = pServer->ProcessNewConnect();
    nRetCode = pServer->ProcessMsg();

    while (true)
    {
        Active();
    }

Exit0:
    if  (pServer)
    {
        delete pServer;
        pServer = NULL;
    }
	return 0;
}

