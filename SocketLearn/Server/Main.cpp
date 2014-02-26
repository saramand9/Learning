// Main.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <iostream>
#include "Server.h"

#ifdef WIN32
    #pragma comment(lib, "ws2_32.lib")
#endif

using namespace std;


void Active()
{

}

int main(int argc, char* argv[])
{   
    int     nRetCode = 0;
    char    szIP[] = "192.168.1.105";

    Server* pServer = new Server();

    if (pServer == NULL)
    {
        goto Exit0;
    }

    nRetCode = pServer->Init(szIP, 6628);
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

