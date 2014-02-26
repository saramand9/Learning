#ifndef __SERVER_H__
#define __SERVER_H__


#ifdef WIN32
    #include <WinSock2.h>
    #include <process.h>
#else
    #include <pthread.h>
#endif

#ifdef __GNUC__
    typedef int                 SOCKET;
    #define INVALID_SOCKET      (SOCKET)(~0)
    #define SOCKET_ERROR        -1
    typedef void*				PVOID;
#endif

#define MAX_CLIENT_SOCKET   1024
#define MAX_BUFFER_SIZE     1024

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

#ifdef __GNUC__
    static void* ThreadNewConnectFunc(PVOID pVoid);
    static void* ThreadMsgFunc(PVOID pVoid);
#else
    static unsigned int __stdcall ThreadNewConnectFunc(PVOID pVoid);
    static unsigned int __stdcall ThreadMsgFunc(PVOID pVoid);
#endif

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

    static int     __GetSocketLastError();
    static void    __CloseSocket(SOCKET socket);

};
#endif