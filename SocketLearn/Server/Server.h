#ifndef __SERVER_H__
#define __SERVER_H__

#include <process.h>
#include <Windows.h>

#define MAX_CLIENT_SOCKET   10240
#define MAX_BUFFER_SIZE     2

class Server
{
public:
    Server();
    ~Server();
public:
    int Init(char szIP[], unsigned short uPort);
    int ProcessMsg();
    int ProcessNewConnect();
    int Listen();
private:
    static unsigned int  __stdcall ThreadNewConnectFunc(PVOID pVoid);
    static unsigned int __stdcall ThreadMsgFunc(PVOID pVoid);
private:
    char           m_szIP[128];
    unsigned short m_uPort;
    int            m_nAcceptFlag;
    int            m_nMsgFlag;

    SOCKET*        m_pListenSocket;
    SOCKET         m_ClientSocket[MAX_CLIENT_SOCKET];
    char           m_szBuff[MAX_BUFFER_SIZE];

    int            AddNewConnect(SOCKET socket);
    int            RemoveConnect(SOCKET socket);

};
#endif