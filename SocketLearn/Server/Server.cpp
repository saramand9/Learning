#include "stdafx.h"
#include "Server.h"

#ifdef __GNUC__
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <errno.h>
#endif

Server::Server()
{
    memset(m_szIP, 0, sizeof(m_szIP));
    m_uPort         = 0;
    m_nAcceptFlag   = 0;
    m_nMsgFlag      = 0;

    for (int i = 0; i < MAX_CLIENT_SOCKET; ++i)
    {
        m_ClientSocket[i] = INVALID_SOCKET;
    }
}

Server::~Server()
{

}

int Server::__GetSocketLastError()
{
#ifdef __GNUC__
    return errno;
#else
    return WSAGetLastError();
#endif
}

void Server::__CloseSocket(SOCKET socket)
{
#ifdef __GNUC__
   close(socket);
#else
    closesocket(socket);
#endif
}

int Server::Init(char szIP[], unsigned short uPort)
{
    int nRetCode = 0;

    m_uPort = uPort;
    memcpy(m_szIP, szIP, strlen(szIP));
    m_szIP[strlen(szIP)] = '\0';

    return nRetCode;
}


int Server::Listen()
{
    struct sockaddr_in ServerAddr = {0};
    int nRetCode = 0;
#ifdef WIN32
    WSADATA wsaData = {0};
    nRetCode = WSAStartup(MAKEWORD(-1,-10), &wsaData);// 1)
    if (nRetCode != 0)
    {
        printf("%s occurs errs at line %d, error code is %d\n", __FUNCTION__, __LINE__, nRetCode);
        return 0;
    }
#endif

    m_pListenSocket = new SOCKET;
    if (m_pListenSocket == NULL)
    {
        return 0;
    }

    *m_pListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);// 2)
    if (*m_pListenSocket == INVALID_SOCKET)
    {
        printf("%s occurs errs at line %d, error code is %d\n", __FUNCTION__, __LINE__, __GetSocketLastError());
        return 0;
    }

    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_port = htons(m_uPort);
    #ifdef WIN32
        ServerAddr.sin_addr.S_un.S_addr = inet_addr((const char*)m_szIP);
    #else
        ServerAddr.sin_addr.s_addr = inet_addr(m_szIP);
    #endif
    nRetCode = bind(*m_pListenSocket,(sockaddr*)&ServerAddr, sizeof(ServerAddr)); // 3)
    if (nRetCode == SOCKET_ERROR)
    {
        printf("%s occurs errs at line %d, error code is %d\n", __FUNCTION__, __LINE__, __GetSocketLastError());
        return 0;
    }


    nRetCode = listen(*m_pListenSocket, 5); // 4)
    if (nRetCode == SOCKET_ERROR)
    {
        printf("%s occurs errs at line %d, error code is %d\n", __FUNCTION__, __LINE__, __GetSocketLastError());
        goto Exit1;
    }

    printf("Server listen at %s:%u\n", inet_ntoa(ServerAddr.sin_addr), ntohs(ServerAddr.sin_port));

Exit1:
    return 0;
}


int Server::AddNewConnect(SOCKET socket)
{
    for (int i = 0; i < MAX_CLIENT_SOCKET; ++i)
    {
        if (m_ClientSocket[i] == INVALID_SOCKET)
        {
            m_ClientSocket[i] = socket;
            return i;
        }
    }
    return -1;
}

int Server::RemoveConnect(SOCKET socket)
{
    for (int i = 0; i < MAX_CLIENT_SOCKET; ++i)
    {
        if (m_ClientSocket[i] == socket)
        {
            m_ClientSocket[i] = INVALID_SOCKET;
            return i;
        }
    }
    return -1;
}


#ifdef __GNUC__
void*  Server::ThreadNewConnectFunc(PVOID pVoid)
#else
unsigned int  __stdcall  Server::ThreadNewConnectFunc(PVOID pVoid)
#endif
{
    Server* pServer = (Server*)pVoid;

    sockaddr_in ClientAddr;
    
    #ifdef __GNUC__
        socklen_t nClientAddrLen = sizeof(ClientAddr);
    #else
        int nClientAddrLen = sizeof(ClientAddr);
    #endif
    while (pServer->m_nAcceptFlag)
    {
        SOCKET clientSocket;

        clientSocket = accept(*(pServer->m_pListenSocket), (sockaddr*)&ClientAddr,  &nClientAddrLen);
        if (clientSocket == INVALID_SOCKET)
        {
            printf("%s occurs errs at line %d, error code is %d\n", __FUNCTION__, __LINE__, __GetSocketLastError());
        }
        else if (pServer->AddNewConnect(clientSocket) != -1)
        {
            printf("New connection from %s:%u\n", inet_ntoa(ClientAddr.sin_addr), htons(ClientAddr.sin_port));
        }
        else
        {
            printf("SocketList is full, %s:%d is reject\n", inet_ntoa(ClientAddr.sin_addr), htons(ClientAddr.sin_port));
            __CloseSocket(clientSocket);
        }
    }

    return 0;
}

#ifdef __GNUC__
void* Server::ThreadMsgFunc(PVOID pVoid)
#else
unsigned int __stdcall Server::ThreadMsgFunc(PVOID pVoid) 
#endif
{
    fd_set      fdSets;
    int         nfds        = 0;
    TIMEVAL     timeOut     = {0,0};
    int         nRetCode    = 0;
    Server*     pServer     = (Server*)pVoid;
    SOCKET      maxSocketfd = 0;

    FD_ZERO(&fdSets);

    while(pServer->m_nMsgFlag)
    {
        FD_ZERO(&fdSets);

        for (int i = 0; i < MAX_CLIENT_SOCKET; ++i)
        {
            if (pServer->m_ClientSocket[i] != INVALID_SOCKET)
            {
                FD_SET(pServer->m_ClientSocket[i], &fdSets);
                maxSocketfd = max(maxSocketfd, pServer->m_ClientSocket[i]);
            }
        }

        if (maxSocketfd == 0)
        {
            continue;;
        }
    
        nRetCode = select((int)maxSocketfd + 1, &fdSets, NULL, NULL, &timeOut);
        
        switch (nRetCode)
        {
        case 0: 
            #ifdef __GNUC__
                sleep(1);
            #else
                Sleep(1);
            #endif

            continue;
            break;
        case SOCKET_ERROR: 
            printf("%s occurs errs at line %d, error code is %d\n", __FUNCTION__, __LINE__, __GetSocketLastError());
            break;
        default:
            
            for (int i = 0; i < MAX_CLIENT_SOCKET; ++i)
            {
                if (FD_ISSET(pServer->m_ClientSocket[i], &fdSets))
                {
                    nRetCode = recv(pServer->m_ClientSocket[i], pServer->m_szBuff, sizeof(pServer->m_szBuff), 0);
                    if (nRetCode == 0)
                    {
                        __CloseSocket(pServer->m_ClientSocket[i]);
                        pServer->RemoveConnect(pServer->m_ClientSocket[i]);
                        printf("Remote client disconnec\n");
                        break;
                    }
                    if (nRetCode == SOCKET_ERROR)
                    {
                        __CloseSocket(pServer->m_ClientSocket[i]);
                        pServer->RemoveConnect(pServer->m_ClientSocket[i]);
                        printf("%s occurs errs at line %d, error code is %d\n", __FUNCTION__, __LINE__, __GetSocketLastError());
                        break;
                    }

                    pServer->m_szBuff[nRetCode] = '\0';
                    printf("Server recive msg [%s]\n", pServer->m_szBuff);

                    if (strcmp(pServer->m_szBuff, "bye") == 0)
                    {
                        __CloseSocket(pServer->m_ClientSocket[i]);
                        pServer->RemoveConnect(pServer->m_ClientSocket[i]);
                    }
                }
            }
            break;
        }
    }
    return 0;
}


int Server::ProcessMsg()
{
    int     nRetCode    = 0;
    
    assert(m_nMsgFlag == 0);

#ifdef __GNUC__
    pthread_t handle;
    nRetCode = pthread_create(&handle, NULL, ThreadMsgFunc, this);
#else
    HANDLE handle;
    handle = (HANDLE)_beginthreadex(NULL, 0, ThreadMsgFunc, this, 0, NULL);
#endif

    m_nMsgFlag = 1;
    return 0;
}

int Server::ProcessNewConnect()
{
    int     nRetCode    = 0;
    assert(m_nAcceptFlag == 0);

#ifdef __GNUC__
    pthread_t handle;
    nRetCode = pthread_create(&handle, NULL, ThreadNewConnectFunc, this);
#else
    HANDLE handle;
    handle = (HANDLE)_beginthreadex(NULL, 0, ThreadNewConnectFunc, this, 0, NULL);
#endif
    m_nAcceptFlag = 1;

    return 0;
}